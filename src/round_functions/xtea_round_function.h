#pragma once

#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include "../lib/xtea/xtea.h"

typedef struct
{
    PyObject_HEAD;

    uint32_t key[4];
    int num_rounds;
} XteaRoundFunction;

PyObject *XteaRoundFunction_new(PyTypeObject *type, PyObject *args, PyObject *kwargs);

int XteaRoundFunction_init(XteaRoundFunction *self, PyObject *args, PyObject *kwargs);

PyObject *XteaRoundFunction_apply(XteaRoundFunction *self, PyObject *args);

static PyMethodDef XteaRoundFunction_methods[] = {
    {"apply", (PyCFunction)XteaRoundFunction_apply, METH_VARARGS},
    {NULL}};

static PyTypeObject XteaRoundFunction_type = {
    PyVarObject_HEAD_INIT(NULL, 0)

        .tp_name = "_fpe_ffx.XteaRoundFunction",
    .tp_basicsize = sizeof(XteaRoundFunction),
    .tp_base = &PyBaseObject_Type,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_new = XteaRoundFunction_new,
    .tp_init = (initproc)XteaRoundFunction_init,
    .tp_methods = XteaRoundFunction_methods};