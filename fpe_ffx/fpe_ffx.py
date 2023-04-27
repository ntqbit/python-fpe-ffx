import math
import operator

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


class FFX:
    def __init__(self, length, round_function: RoundFunction, rounds=10, radix=2):
        self._length = length

        self._ffx = _fpe_ffx.FFX(length, round_function, rounds, radix)

    def encrypt(self, plain, tweak=None):
        self._ensure_valid_input(plain)
        return self._ffx.cipher(plain, True, _tweak_to_bytes(tweak))

    def decrypt(self, plain, tweak=None):
        self._ensure_valid_input(plain)
        return self._ffx.cipher(plain, False, _tweak_to_bytes(tweak))

    def _ensure_valid_input(self, value: int):
        if not isinstance(value, int):
            raise ValueError(f'FFX cipher input must be an int, but got {type(value)}')

        if value >= self._length:
            raise ValueError(f'FFX cipher input `{value}` does not satisfy length: {self._length}')
