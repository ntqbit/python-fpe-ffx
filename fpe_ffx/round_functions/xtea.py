import struct

import _fpe_ffx


class XteaRoundFunction(_fpe_ffx.XteaRoundFunction):
    def __init__(self, key: bytes, num_rounds=32):
        super().__init__(*struct.unpack('>4L', key), num_rounds)
