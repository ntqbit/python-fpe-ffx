from Crypto.Cipher import ARC4

from fpe_ffx.fpe_ffx import RoundFunction


class RC4_RoundFunction(RoundFunction):
    def __init__(self, key: bytes):
        self._cipher = ARC4.new(key)

    def apply(self, data):
        return cipher.encrypt(data)
