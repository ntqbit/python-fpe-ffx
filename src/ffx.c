#include "ffx.h"

PyObject *FFX_new(PyTypeObject *type, PyObject *args, PyObject *kwargs)
{
    return type->tp_alloc(type, 0);
}

int FFX_init(FFX *self, PyObject *args, PyObject *kwargs)
{
    PyObject *round_function, *maxval, *radix;
    int length, rounds;

    static const char *kwlist[] = {"round_function", "maxval", "length", "rounds", "radix", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OOiiO", kwlist,
                                     &round_function, &maxval, &length, &rounds, &radix))
        return -1;

    Py_INCREF(round_function);
    Py_INCREF(maxval);

    self->round_function = round_function;
    self->maxval = maxval;
    self->rounds = rounds;

    int half_length_2 = length / 2;
    int half_length_1 = length - half_length_2;

    PyObject *half_length_1_obj = PyLong_FromLong(half_length_1);
    PyObject *half_length_2_obj = PyLong_FromLong(half_length_2);

    self->modulos[0] = PyNumber_Power(radix, half_length_1_obj, Py_None);
    self->modulos[1] = PyNumber_Power(radix, half_length_2_obj, Py_None);
    self->half_length_byte = (ceil(half_length_1 * log2(PyLong_AsDouble(radix))) + 7) / 8;

    Py_DECREF(half_length_1_obj);
    Py_DECREF(half_length_2_obj);
    return 0;
}

void FFX_dealloc(FFX *self)
{
    Py_DECREF(self->maxval);
    Py_DECREF(self->round_function);

    Py_DECREF(self->modulos[0]);
    Py_DECREF(self->modulos[1]);

    Py_TYPE(self)->tp_free((PyObject *)self);
}

static void split(FFX *self, PyObject *val, PyObject **vals)
{
    // a = value % self._modulos[0]
    // b = (value // self._modulos[0]) % self._modulos[1]

    vals[0] = PyNumber_Remainder(val, self->modulos[0]);
    PyObject *tmp = PyNumber_FloorDivide(val, self->modulos[0]);
    vals[1] = PyNumber_Remainder(tmp, self->modulos[1]);
    Py_DECREF(tmp);
}

static PyObject *join(FFX *self, PyObject *a, PyObject *b)
{
    PyObject *mul = PyNumber_Multiply(b, self->modulos[0]);
    PyObject *result = PyNumber_Add(a, mul);
    Py_DECREF(mul);
    return result;
}

static PyObject *apply_round_function(FFX *self, PyObject *val, int round, PyObject *round_function_input_bytes)
{
    // b = val.to_bytes(self._half_byte, 'big') + round_.to_bytes(1, 'big') + tweak
    // enc = self._round_function.apply(b)
    // return int.from_bytes(enc[:self._half_byte], 'big')

    unsigned char *round_function_input_bytes_ptr = (unsigned char *)PyBytes_AS_STRING(round_function_input_bytes);
    _PyLong_AsByteArray((PyLongObject *)val, round_function_input_bytes_ptr, self->half_length_byte, 0, 0);
    round_function_input_bytes_ptr[self->half_length_byte] = (unsigned char)round;

    PyObject *round_function_result = PyObject_CallMethod(self->round_function, "apply", "O", round_function_input_bytes);

    if (round_function_result == NULL)
        return NULL;

    if (PyBytes_GET_SIZE(round_function_result) < self->half_length_byte)
    {
        // TODO: throw an exception
        Py_DECREF(round_function_result);
        return NULL;
    }

    PyObject *enc_val = _PyLong_FromByteArray((unsigned char *)PyBytes_AS_STRING(round_function_result), self->half_length_byte, 0, 0);
    Py_DECREF(round_function_result);
    return enc_val;
}

static PyObject *encrypt(FFX *self, PyObject *plaintext, PyObject *tweak)
{
    PyObject *vals[2];
    split(self, plaintext, vals);

    Py_ssize_t tweak_size = PyBytes_GET_SIZE(tweak);
    Py_ssize_t round_function_input_size = self->half_length_byte + 1 + tweak_size;
    PyObject *round_function_input_bytes = PyBytes_FromStringAndSize(NULL, round_function_input_size);

    int idx_from = 0, idx_to = 1;

    for (int round = 0; round < self->rounds; round++)
    {
        PyObject *enc_val = apply_round_function(self, vals[idx_from], round, round_function_input_bytes);
        if (enc_val == NULL)
        {
            Py_DECREF(vals[0]);
            Py_DECREF(vals[1]);
            Py_DECREF(round_function_input_bytes);
            return NULL;
        }

        PyObject *op_val = PyNumber_Add(vals[idx_to], enc_val);
        PyObject *new_val = PyNumber_Remainder(op_val, self->modulos[idx_to]);

        Py_DECREF(enc_val);
        Py_DECREF(op_val);
        Py_DECREF(vals[idx_to]);
        vals[idx_to] = new_val;

        // Swap
        int tmp = idx_from;
        idx_from = idx_to;
        idx_to = tmp;
    }

    PyObject *output_value = join(self, vals[0], vals[1]);

    Py_DECREF(vals[0]);
    Py_DECREF(vals[1]);
    Py_DECREF(round_function_input_bytes);

    if (self->maxval != Py_None)
    {
        // Check if satisfies length
        int cmp = PyObject_RichCompareBool(output_value, self->maxval, Py_LT);
        assert(cmp != -1);

        if (cmp == 0)
        {
            // Length of the output value is not satistied.
            // Do cycle walking.
            PyObject *result = encrypt(self, output_value, tweak);
            Py_DECREF(output_value);
            return result;
        }
    }

    return output_value;
}

static PyObject *decrypt(FFX *self, PyObject *ciphertext, PyObject *tweak)
{
    PyObject *vals[2];
    split(self, ciphertext, vals);

    Py_ssize_t tweak_size = PyBytes_GET_SIZE(tweak);
    Py_ssize_t round_function_input_size = self->half_length_byte + 1 + tweak_size;
    PyObject *round_function_input_bytes = PyBytes_FromStringAndSize(NULL, round_function_input_size);

    int idx_from = (self->rounds - 1) % 2;
    int idx_to = self->rounds % 2;

    for (int round = self->rounds - 1; round >= 0; round--)
    {
        PyObject *enc_val = apply_round_function(self, vals[idx_from], round, round_function_input_bytes);
        if (enc_val == NULL)
        {
            Py_DECREF(vals[0]);
            Py_DECREF(vals[1]);
            Py_DECREF(round_function_input_bytes);
            return NULL;
        }

        PyObject *op_val = PyNumber_Subtract(vals[idx_to], enc_val);
        PyObject *new_val = PyNumber_Remainder(op_val, self->modulos[idx_to]);

        Py_DECREF(enc_val);
        Py_DECREF(op_val);
        Py_DECREF(vals[idx_to]);
        vals[idx_to] = new_val;

        // Swap
        int tmp = idx_from;
        idx_from = idx_to;
        idx_to = tmp;
    }

    PyObject *output_value = join(self, vals[0], vals[1]);

    Py_DECREF(vals[0]);
    Py_DECREF(vals[1]);
    Py_DECREF(round_function_input_bytes);

    if (self->maxval != Py_None)
    {
        // Check if satisfies length
        int cmp = PyObject_RichCompareBool(output_value, self->maxval, Py_LT);
        assert(cmp != -1);

        if (cmp == 0)
        {
            // Length of the output value is not satistied.
            // Do cycle walking.
            PyObject *result = decrypt(self, output_value, tweak);
            Py_DECREF(output_value);
            return result;
        }
    }

    return output_value;
}

PyObject *FFX_cipher(FFX *self, PyObject *args)
{
    PyObject *input_value, *tweak;
    int encryption;

    if (!PyArg_ParseTuple(args, "OpS", &input_value, &encryption, &tweak))
        return NULL;

    PyObject *result;

    if (encryption)
        result = encrypt(self, input_value, tweak);
    else
        result = decrypt(self, input_value, tweak);

    return result;
}