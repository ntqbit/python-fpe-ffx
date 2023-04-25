from Crypto.Cipher import AES
from Crypto.Util.Padding import pad

from fpe_ffx.fpe_ffx import BlockCipher


class AESECBCipher(BlockCipher):
    def __init__(self, key):
        self._aes_ecb = AES.new(key, AES.MODE_ECB)

    def encrypt(self, data):
        return self._aes_ecb.encrypt(pad(data, 16))
