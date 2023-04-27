#include "xtea_round_function.h"

PyObject *XteaRoundFunction_new(PyTypeObject *type, PyObject *args, PyObject *kwargs)
{
    return type->tp_alloc(type, 0);
}

int XteaRoundFunction_init(XteaRoundFunction *self, PyObject *args, PyObject *kwargs)
{
    if (!PyArg_ParseTuple(args, "IIIII",
                          &self->key[0], &self->key[1], &self->key[2], &self->key[3],
                          &self->num_rounds))
        return -1;

    return 0;
}

PyObject *XteaRoundFunction_apply(XteaRoundFunction *self, PyObject *args)
{
    PyObject *data;

    if (!PyArg_ParseTuple(args, "S", &data))
        return NULL;

    Py_ssize_t data_size = PyBytes_GET_SIZE(data);
    int padding_size = XTEA_BLOCKLEN - data_size % XTEA_BLOCKLEN;
    int buffer_size = data_size + padding_size;

    PyObject *result = PyBytes_FromStringAndSize(NULL, buffer_size);
    unsigned char *buffer = PyBytes_AS_STRING(result);

    memcpy(buffer, PyBytes_AS_STRING(data), data_size);
    memset(buffer + data_size, padding_size, padding_size);

    for (int i = 0; i < buffer_size; i += XTEA_BLOCKLEN)
        xtea_encrypt(self->key, (uint32_t *)(buffer + i), self->num_rounds);

    return result;
}