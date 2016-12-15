/*
 * Copyright 2009 Paul J. Davis <paul.joseph.davis@gmail.com>
 *
 * This file is part of the python-spidermonkey package released
 * under the MIT license.
 *
 */

#include "spidermonkey.h"

#ifndef PyMODINIT_FUNC
#define PyMODINIT_FUNC void
#endif

PyObject* SpidermonkeyModule = NULL;
PyTypeObject* RuntimeType = NULL;
PyTypeObject* ContextType = NULL;
PyTypeObject* ObjectType = NULL;
PyTypeObject* ArrayType = NULL;
PyTypeObject* FunctionType = NULL;
PyTypeObject* IteratorType = NULL;
PyTypeObject* HashCObjType = NULL;
PyObject* JSError = NULL;

struct module_state {
    PyObject *error;
};

#if PY_MAJOR_VERSION >= 3
#define GETSTATE(m) ((struct module_state*)PyModule_GetState(m))
#else
#define GETSTATE(m) (&_state)
static struct module_state _state;
#endif

static PyMethodDef spidermonkey_methods[] = {
    {NULL}
};


#if PY_MAJOR_VERSION >= 3

static int spidermonkey_traverse(PyObject *m, visitproc visit, void *arg) {
    Py_VISIT(GETSTATE(m)->error);
    return 0;
}

static int spidermonkey_clear(PyObject *m) {
    Py_CLEAR(GETSTATE(m)->error);
    return 0;
}


static struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        "spidermonkey",
        "The Python-Spidermonkey bridge.",
        sizeof(struct module_state),
        spidermonkey_methods,
        NULL,
        spidermonkey_traverse,
        spidermonkey_clear,
        NULL
};

#define INITERROR return NULL

PyMODINIT_FUNC
PyInit_spidermonkey(void)

#else // PY_MAJOR_VERSION == 2
#define INITERROR return

PyMODINIT_FUNC
initspidermonkey(void)
#endif
{
    PyObject* m;
    
    if(PyType_Ready(&_RuntimeType) < 0) INITERROR;
    if(PyType_Ready(&_ContextType) < 0) INITERROR;
    if(PyType_Ready(&_ObjectType) < 0) INITERROR;

    _ArrayType.tp_base = &_ObjectType;
    if(PyType_Ready(&_ArrayType) < 0) INITERROR;

    _FunctionType.tp_base = &_ObjectType;
    if(PyType_Ready(&_FunctionType) < 0) INITERROR;

    if(PyType_Ready(&_IteratorType) < 0) INITERROR;

    if(PyType_Ready(&_HashCObjType) < 0) INITERROR;
    
    #if PY_MAJOR_VERSION >= 3
    m = PyModule_Create(&moduledef);
    #else

    m = Py_InitModule3("spidermonkey", spidermonkey_methods,
            "The Python-Spidermonkey bridge.");
    #endif

    if(m == NULL)
    {
        INITERROR;
    }

    struct module_state *st = GETSTATE(m);
    st->error = PyErr_NewException("spidermonkey.Error", NULL, NULL);
    if (st->error == NULL) {
        Py_DECREF(m);
        INITERROR;
    }

    RuntimeType = &_RuntimeType;
    Py_INCREF(RuntimeType);
    PyModule_AddObject(m, "Runtime", (PyObject*) RuntimeType);

    ContextType = &_ContextType;
    Py_INCREF(ContextType);
    PyModule_AddObject(m, "Context", (PyObject*) ContextType);

    ObjectType = &_ObjectType;
    Py_INCREF(ObjectType);
    PyModule_AddObject(m, "Object", (PyObject*) ObjectType);

    ArrayType = &_ArrayType;
    Py_INCREF(ArrayType);
    PyModule_AddObject(m, "Array", (PyObject*) ArrayType);

    FunctionType = &_FunctionType;
    Py_INCREF(FunctionType);
    PyModule_AddObject(m, "Function", (PyObject*) FunctionType);

    IteratorType = &_IteratorType;
    Py_INCREF(IteratorType);
    // No module access on purpose.

    HashCObjType = &_HashCObjType;
    Py_INCREF(HashCObjType);
    // Don't add access from the module on purpose.

    JSError = PyErr_NewException("spidermonkey.JSError", NULL, NULL);
    PyModule_AddObject(m, "JSError", JSError);
    
    SpidermonkeyModule = m;

#if PY_MAJOR_VERSION >= 3
    return m;
#endif
}
