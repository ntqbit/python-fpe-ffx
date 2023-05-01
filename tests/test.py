import random
import unittest

from fpe_ffx.fpe_ffx import FFX, RoundFunction
from fpe_ffx.round_functions import AesRoundFunction, XteaRoundFunction


class NoRoundFunction(RoundFunction):
    def apply(self, data):
        return data


class FpeFfxTest(unittest.TestCase):
    def test_no_round_function(self):
        self._test_round_function(NoRoundFunction())

    def test_aes(self):
        self._test_round_function(AesRoundFunction(b'0' * 16))

    def test_xtea(self):
        self._test_round_function(XteaRoundFunction(b'0' * 16))

    def _test_round_function(self, round_function):
        self._test_ffx_small(round_function)
        self._test_ffx_medium(round_function)
        self._test_large_length(round_function)

    def _test_ffx_small(self, round_function):
        self._test_small_length(100, round_function)

    def _test_ffx_medium(self, round_function):
        self._test_small_length(100000, round_function)

    def _test_small_length(self, length, round_function):
        inputs = list(range(length))
        ffx = FFX(round_function, length)
        encrypted = [ffx.encrypt(input_) for input_ in inputs]

        self.assertTrue(all(0 <= e <= length for e in encrypted), 'Encrypted value not in the allowed range')
        self.assertEqual(len(encrypted), len(set(encrypted)), 'Encrypted values must not contain duplicates')
        self.assertEqual(sorted(encrypted), inputs)

        decrypted = [ffx.decrypt(e) for e in encrypted]
        self.assertEqual(inputs, decrypted, 'Decrypted value does not equal to the plain value')

    def _test_large_length(self, round_function):
        length = 2**256
        inputs = [random.randint(0, length) for _ in range(1000)]
        ffx = FFX(round_function, length)
        encrypted = [ffx.encrypt(input_) for input_ in inputs]

        self.assertTrue(all(0 <= e <= length for e in encrypted), 'Encrypted value not in the allowed range')

        decrypted = [ffx.decrypt(e) for e in encrypted]
        self.assertEqual(inputs, decrypted, 'Decrypted value does not equal to the plain value')

    def test_ffx_tweak(self):
        length = 2**32
        inputs = [random.randint(0, length) for _ in range(1000)]
        ffx = FFX(XteaRoundFunction(b'0' * 16), length)
        tweak = random.randbytes(6)
        encrypted = [ffx.encrypt(input_, tweak) for input_ in inputs]

        self.assertTrue(all(0 <= e <= length for e in encrypted), 'Encrypted value not in the allowed range')

        decrypted = [ffx.decrypt(e, tweak) for e in encrypted]
        self.assertEqual(inputs, decrypted, 'Decrypted value does not equal to the plain value')

    def test_non_default_rounds(self):
        length = 2**32
        inputs = [random.randint(0, length) for _ in range(1000)]
        ffx = FFX(XteaRoundFunction(b'0' * 16), length, rounds=50)
        encrypted = [ffx.encrypt(input_) for input_ in inputs]

        self.assertTrue(all(0 <= e <= length for e in encrypted), 'Encrypted value not in the allowed range')

        decrypted = [ffx.decrypt(e) for e in encrypted]
        self.assertEqual(inputs, decrypted, 'Decrypted value does not equal to the plain value')

    def test_non_default_radix(self):
        length = 10**16
        inputs = [random.randint(0, length) for _ in range(1000)]
        ffx = FFX(AesRoundFunction(b'0' * 16), length, rounds=50, radix=10)
        encrypted = [ffx.encrypt(input_) for input_ in inputs]

        self.assertTrue(all(0 <= e <= length for e in encrypted), 'Encrypted value not in the allowed range')

        decrypted = [ffx.decrypt(e) for e in encrypted]
        self.assertEqual(inputs, decrypted, 'Decrypted value does not equal to the plain value')

    def test_ffx_expected_value(self):
        ffx = FFX(AesRoundFunction(b'0' * 16), 100000)
        self.assertEqual(ffx.encrypt(9498), 74817)
        self.assertEqual(ffx.decrypt(48915), 22523)


if __name__ == '__main__':
    unittest.main()
