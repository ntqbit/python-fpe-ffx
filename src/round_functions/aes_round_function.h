#pragma once

#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include "../lib/aes/aes.h"

typedef struct
{
    PyObject_HEAD;

    struct AES_ctx aes_ctx;
} AesRoundFunction;

PyObject *AesRoundFunction_new(PyTypeObject *type, PyObject *args, PyObject *kwargs);

int AesRoundFunction_init(AesRoundFunction *self, PyObject *args, PyObject *kwargs);

PyObject *AesRoundFunction_apply(AesRoundFunction *self, PyObject *args);

static PyMethodDef AesRoundFunction_methods[] = {
    {"apply", (PyCFunction)AesRoundFunction_apply, METH_VARARGS},
    {NULL}};

static PyTypeObject AesRoundFunction_type = {
    PyVarObject_HEAD_INIT(NULL, 0)

        .tp_name = "_fpe_ffx.AesRoundFunction",
    .tp_basicsize = sizeof(AesRoundFunction),
    .tp_base = &PyBaseObject_Type,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_new = AesRoundFunction_new,
    .tp_init = (initproc)AesRoundFunction_init,
    .tp_methods = AesRoundFunction_methods};