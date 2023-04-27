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

    def _test_ffx_small(self, round_function):
        self._test_ffx(100, round_function)

    def _test_ffx_medium(self, round_function):
        self._test_ffx(100000, round_function)

    def _test_ffx(self, length, round_function):
        inputs = list(range(length))
        ffx = FFX(length, round_function)
        encrypted = [ffx.encrypt(input_) for input_ in inputs]

        self.assertTrue(all(0 <= e <= length for e in encrypted), 'Encrypted value not in the allowed range')
        self.assertEqual(len(encrypted), len(set(encrypted)), 'Encrypted values must not contain duplicates')
        self.assertEqual(sorted(encrypted), inputs)

        decrypted = [ffx.decrypt(e) for e in encrypted]
        self.assertEqual(inputs, decrypted, 'Decrypted value does not equal to the plain value')

    def test_ffx_value(self):
        ffx = FFX(100000, AesRoundFunction(b'0' * 16))
        self.assertEqual(ffx.encrypt(9498), 74817)
        self.assertEqual(ffx.decrypt(48915), 22523)


if __name__ == '__main__':
    unittest.main()
