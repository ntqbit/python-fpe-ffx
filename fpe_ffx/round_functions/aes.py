import _fpe_ffx


class AesRoundFunction(_fpe_ffx.AesRoundFunction):
    def __init__(self, key: bytes):
        super().__init__(key)
