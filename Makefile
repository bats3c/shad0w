all: gencerts teamcerts

gencerts:
	$(shell openssl req -x509 -newkey rsa:4096 -keyout certs/key.pem -out certs/cert.pem -days 365 -nodes)

teamcerts:
	$(shell openssl req -x509 -newkey rsa:4096 -keyout certs/teamsrv_key.pem -out certs/teamsrv_cert.pem -days 365 -nodes)

install_libs:
	$(shell pip3 install -r requirements.txt)