#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include "ffx.h"
#include "round_functions/aes_round_function.h"

// MODULE
static PyMethodDef module_methods[] = {
    {NULL}};

static struct PyModuleDef module = {
    PyModuleDef_HEAD_INIT,
    "fpe_ffx",
    "",
    -1,
    module_methods};

PyMODINIT_FUNC PyInit__fpe_ffx(void)
{
    PyObject *m = PyModule_Create(&module);

    if (PyType_Ready(&FFX_type) < 0)
        return NULL;

    if (PyType_Ready(&AesRoundFunction_type) < 0)
        return NULL;

    PyModule_AddObject(m, "FFX", (PyObject *)&FFX_type);
    PyModule_AddObject(m, "AesRoundFunction", (PyObject *)&AesRoundFunction_type);

    return m;
}
