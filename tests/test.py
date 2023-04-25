import random
import unittest

from fpe_ffx.fpe_ffx import FFX, RoundFunction
from fpe_ffx.round_functions import AesRoundFunction


class NoRoundFunction(RoundFunction):
    def apply(self, data):
        return data


class FpeFfxTest(unittest.TestCase):
    def test_no_round_function(self):
        self._test_round_function(NoRoundFunction())

    def test_aes(self):
        self._test_round_function(AesRoundFunction(b'0' * 16))

    def _test_round_function(self, round_function):
        self._test_ffx_small(round_function)
        self._test_ffx_medium(round_function)

    def _test_ffx_small(self, round_function):
        self._test_ffx(100, round_function)

    def _test_ffx_medium(self, round_function):
        self._test_ffx(10000, round_function)

    def _test_ffx(self, length, round_function):
        inputs = list(range(length))
        ffx = FFX(length, round_function)
        sa = set()

        for input_ in inputs:
            encrypted = ffx.encrypt(input_)
            self.assertNotIn(encrypted, sa, 'Encrypted numbers should be unique if inputs are unique')
            sa.add(encrypted)
            self.assertIn(encrypted, inputs, 'Encrypted value is not within the allowed set')
            self.assertEqual(input_, ffx.decrypt(encrypted), 'Decrypted value does not equal to the plain value')

        self.assertEqual(sorted(ffx.encrypt(input_) for input_ in inputs), inputs)


if __name__ == '__main__':
    unittest.main()
