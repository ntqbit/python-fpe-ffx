from Crypto.Cipher import AES
from Crypto.Util.Padding import pad

from fpe_ffx.fpe_ffx import RoundFunction


class AESECBCipher(RoundFunction):
    def __init__(self, key):
        self._aes_ecb = AES.new(key, AES.MODE_ECB)

    def apply(self, data: bytes):
        return self._aes_ecb.encrypt(pad(data, 16))
