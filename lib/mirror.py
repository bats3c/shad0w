# Mirror a website

import re
import requests

def get_base_domain(site):
    return site.replace("https://", "").replace("http://", "").replace("/", "")

def get_base_page(shad0w, site, dynamic=False, htmlonly=False, method=None, headers=None, data=None, cookies=None):
    try:
        if dynamic:
            req_headers = {}

            for header in headers:
                req_headers[header[0]] = header[1]

            req_headers["Host"] = get_base_domain(shad0w.mirror)
            req_headers["Accept-Encoding"] = ""

            req = requests.request(url=site, method=method, headers=req_headers, data=data, cookies=cookies)
        elif not dynamic:
            req = requests.get(site)
    except requests.exceptions.MissingSchema:
        shad0w.debug.error(f"Need a correctly formatted url e.g https://example.com/")
        exit(-1)

    if not htmlonly:
        excluded_headers = ['content-encoding', 'content-length', 'transfer-encoding', 'connection']
        headers = [(name, value) for (name, value) in req.raw.headers.items() if name.lower() not in excluded_headers]
        return req.content, req.status_code, headers
    if htmlonly:
        return req.text

def fix_internal_links(shad0w, html, site):
    # add us to paths
    if shad0w.endpoint is None:
        print(f"\033[1;33mMirror mode: Parameter -e (endpoint) not set. Internal links cannot be patched.\033[0m")
    else:

        if type(html) != bytes:
            endpoint = shad0w.endpoint
            if not shad0w.endpoint.startswith("https://"):
                endpoint = "https://" + shad0w.endpoint

            html = re.sub(r'=\"/', '="%s/' % endpoint, html)

            # make sure we cover http and https
            html = re.sub(r'http://' + get_base_domain(shad0w.mirror), endpoint, html)
            html = re.sub(r'https://' + get_base_domain(shad0w.mirror), endpoint, html)

    return html

def mirror_site(shad0w, site, dynamic=False, method=None, headers=None, data=None, cookies=None):

    if not dynamic:
        shad0w.debug.log(f"Connecting to {site}...")        
        html = get_base_page(shad0w, site, htmlonly=True)
        html = fix_internal_links(shad0w, html, site)
        shad0w.page_data = html
    
    if dynamic:
        data, headers, status_code = get_base_page(shad0w, site, dynamic=dynamic, method=method, headers=headers, data=data, cookies=cookies)
        data = fix_internal_links(shad0w, data, site)
        return data, status_code, headers