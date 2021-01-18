import random


class XOR(object):
    def __init__(self):
        super(XOR, self).__init__()
        self.gen_key()

    def gen_key(self):
        self.key = random.randint(10, 100)


    def crypt_file(self, crypt, key, infile=None, data=None, data_length=None):
        bytes = ""
        if (infile != None) and (data == None):
            with open(infile, "rb") as file:
                data = file.read()
                data_len = len(data)
        else:
            data_len = data_length
        iter = 0
        for num, byte in enumerate(data):
            byte = hex(byte)
            if crypt:
                byte = hex(int(byte, 16) ^ int(key, 16))
            else:
                if len(str(byte)) == 3:
                    byte = str(byte).replace("0x", '')
                    byte = f"0x0{byte}"
            iter += 1
            if num == data_len - 1:
                bytes += f"{str(byte)}"
                return bytes, data_len, key
            if iter == 16:
                bytes += f"{str(byte)},\n"
                iter = 0
                continue
            bytes += f"{str(byte)}, "
