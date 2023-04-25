import math
import operator


class ECBCipher:
    def encrypt(self, data):
        raise NotImplementedError


def tweak_to_bytes(tweak):
    if tweak is None:
        return bytes()

    if isinstance(tweak, bytes):
        return tweak

    if isinstance(tweak, str):
        return tweak.encode()

    if isinstance(tweak, int):
        return tweak.to_bytes((tweak.bit_length() + 7) // 8, 'big')

    raise ValueError('Unsupported tweak type: ' + str(tweak_type))


class FFX:
    def __init__(self, length, ecb_cipher: ECBCipher, rounds=10, radix=2):
        self._length = length
        self._ecb = ecb_cipher
        self._rounds = rounds

        total_bits = int(math.ceil(math.log(length, radix)))
        half_bits = [(total_bits + 1) // 2, total_bits // 2]
        self._modulos = [radix ** half for half in half_bits]
        self._half_byte = (half_bits[0] + 7) // 8
        self._encryption_rounds = list(range(self._rounds))
        self._decryption_rounds = list(reversed(range(self._rounds)))

    def encrypt(self, plain, tweak=None):
        return self._cipher(
            input_value=plain,
            next_func=self.encrypt,
            rounds=self._encryption_rounds,
            operation=operator.add,
            tweak=tweak
        )

    def decrypt(self, cipher, tweak=None):
        return self._cipher(
            input_value=cipher,
            next_func=self.decrypt,
            rounds=self._decryption_rounds,
            operation=operator.sub,
            tweak=tweak
        )

    def _cipher(self, input_value, next_func, rounds, operation, tweak):
        rounds = list(rounds)
        tweak = tweak_to_bytes(tweak)

        val = list(self._split(input_value))

        start_round = rounds[0]
        idx_from, idx_to = start_round % 2, (start_round + 1) % 2

        for round_ in rounds:
            out = self._encrypt_value(round_, val[idx_from], tweak)
            val[idx_to] = operation(val[idx_to], out) % self._modulos[idx_to]
            idx_from, idx_to = idx_to, idx_from

        output_value = self._join(*val)

        if not self._satisfies_length(output_value):
            return next_func(output_value, tweak)

        return output_value

    def _satisfies_length(self, value):
        return value < self._length

    def _split(self, value: int):
        assert type(value) is int
        a = value % self._modulos[0]
        b = (value // self._modulos[0]) % self._modulos[1]
        return a, b

    def _join(self, a, b):
        return b * self._modulos[0] + a

    def _encrypt_value(self, round: int, val: int, tweak: bytes):
        b = val.to_bytes(self._half_byte, 'big') + round.to_bytes(1, 'big') + tweak
        enc = self._ecb.encrypt(b)
        return int.from_bytes(enc[:self._half_byte], 'big')
