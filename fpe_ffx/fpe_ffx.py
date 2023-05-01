import math

import _fpe_ffx


class RoundFunction:
    def apply(self, data: bytes) -> bytes:
        raise NotImplementedError


def _tweak_to_bytes(tweak):
    if isinstance(tweak, bytes):
        return tweak

    if tweak is None:
        return bytes()

    if isinstance(tweak, str):
        return tweak.encode()

    if isinstance(tweak, int):
        return tweak.to_bytes((tweak.bit_length() + 7) // 8, 'big')

    raise ValueError('Unsupported tweak type: ' + str(tweak_type))


class FFX(_fpe_ffx.FFX):
    def __init__(self, round_function: RoundFunction, maxval=None, length=None, rounds=10, radix=2):
        if maxval is None and length is None:
            raise ValueError('You must specify either maxval or length')

        if maxval is None:
            maxval = radix ** length
        if length is None:
            length = int(math.ceil(math.log(maxval, radix)))

        super().__init__(round_function=round_function, maxval=maxval, length=length, rounds=rounds, radix=radix)

        self._maxval = maxval

    def encrypt(self, plain, tweak=None):
        self._ensure_valid_input(plain)
        return self.cipher(plain, True, _tweak_to_bytes(tweak))

    def decrypt(self, plain, tweak=None):
        self._ensure_valid_input(plain)
        return self.cipher(plain, False, _tweak_to_bytes(tweak))

    def _ensure_valid_input(self, value: int):
        if not isinstance(value, int):
            raise ValueError(f'FFX cipher input must be an int, but {type(value)} was passed')

        if value >= self._maxval:
            raise ValueError(f'FFX cipher input `{value}` does not satisfy maximum value: {self._maxval}')
