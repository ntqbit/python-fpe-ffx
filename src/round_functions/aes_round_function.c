#include "aes_round_function.h"

PyObject *AesRoundFunction_new(PyTypeObject *type, PyObject *args, PyObject *kwargs)
{
    return type->tp_alloc(type, 0);
}

int AesRoundFunction_init(AesRoundFunction *self, PyObject *args, PyObject *kwargs)
{
    PyObject *key;

    if (!PyArg_ParseTuple(args, "S", &key))
    {
        Py_DECREF(self);
        return -1;
    }

    if (PyBytes_GET_SIZE(key) != 16)
    {
        Py_DECREF(self);
        return -1;
    }

    AES_init_ctx(&self->aes_ctx, (const unsigned char *)PyBytes_AS_STRING(key));
    return 0;
}

PyObject *AesRoundFunction_apply(AesRoundFunction *self, PyObject *args)
{
    PyObject *data;

    if (!PyArg_ParseTuple(args, "S", &data))
        return NULL;

    Py_ssize_t data_size = PyBytes_GET_SIZE(data);
    int padding_size = AES_BLOCKLEN - data_size % AES_BLOCKLEN;
    int buffer_size = data_size + padding_size;

    PyObject *result = PyBytes_FromStringAndSize(NULL, buffer_size);
    unsigned char *buffer = PyBytes_AS_STRING(result);

    memcpy(buffer, PyBytes_AS_STRING(data), data_size);
    memset(buffer + data_size, padding_size, padding_size);

    for (int i = 0; i < buffer_size; i += AES_BLOCKLEN)
        AES_ECB_encrypt(&self->aes_ctx, buffer + i);

    return result;
}