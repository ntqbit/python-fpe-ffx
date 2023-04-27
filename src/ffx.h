#pragma once

#define PY_SSIZE_T_CLEAN

#include <Python.h>

typedef struct
{
    PyObject_HEAD;

    PyObject *length;
    PyObject *round_function;
    int rounds;

    PyObject *modulos[2];
    int half_byte;
} FFX;

PyObject *FFX_new(PyTypeObject *type, PyObject *args, PyObject *kwargs);

PyObject *FFX_cipher(FFX *self, PyObject *args);

static PyMethodDef FFX_methods[] = {
    {"cipher", (PyCFunction)FFX_cipher, METH_VARARGS},
    {NULL}};

static PyTypeObject FFX_type = {
    PyVarObject_HEAD_INIT(NULL, 0)

        .tp_name = "_fpe_ffx.FFX",
    .tp_basicsize = sizeof(FFX),
    .tp_base = &PyBaseObject_Type,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = FFX_new,
    .tp_methods = FFX_methods};