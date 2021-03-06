/*
 * Copyright 2009 Paul J. Davis <paul.joseph.davis@gmail.com>
 *
 * This file is part of the python-spidermonkey package released
 * under the MIT license.
 *
 */

#include "spidermonkey.h"
#include "frameobject.h" // Python
#include "traceback.h" // Python

void
add_frame(const char* srcfile, const char* funcname, int linenum)
{
    PyObject* src = NULL;
    PyObject* func = NULL;
    PyObject* glbl = NULL;
    PyObject* tpl = NULL;
    PyObject* str = NULL;
    PyCodeObject* code = NULL;
    PyFrameObject* frame = NULL;

    src = PyBytes_FromString(srcfile);
    if(src == NULL) goto error;

    func = PyBytes_FromString(funcname);
    if(func == NULL) goto error;
    
    glbl = PyModule_GetDict(SpidermonkeyModule);
    if(glbl == NULL) goto error;

    tpl = PyTuple_New(0);
    if(tpl == NULL) goto error;

    str = PyBytes_FromString("");
    if(str == NULL) goto error;

    code = PyCode_NewEmpty(
        src,                    /*co_filename*/
        func,                   /*co_name*/
        linenum                 /*co_firstlineno*/
    );
    if(code == NULL) goto error;
   
    frame = PyFrame_New(PyThreadState_Get(), code, glbl, NULL);
    if(frame == NULL) goto error;
    
    frame->f_lineno = linenum;
    PyTraceBack_Here(frame);

    goto success;
    
error:
success:
    Py_XDECREF(func);
    Py_XDECREF(src);
    Py_XDECREF(tpl);
    Py_XDECREF(str);
    Py_XDECREF(code);
    Py_XDECREF(frame);
}

void
report_error_cb(JSContext* cx, const char* message, JSErrorReport* report)
{
    /* Subtle note about JSREPORT_EXCEPTION: it triggers whenever exceptions
     * are raised, even if they're caught and the Mozilla docs say you can
     * ignore it.
     */
    /* TODO What should we do about warnings? A callback somehow? */
    if (report->flags & JSREPORT_WARNING)
        return;

    const char* srcfile = report->filename;
    const char* mesg = message;

    if(srcfile == NULL) srcfile = "<JavaScript>";
    if(mesg == NULL) mesg = "Unknown JavaScript execution error";

    if(!PyErr_Occurred())
    {
        PyErr_SetString(JSError, mesg);
    }

    add_frame(srcfile, "JavaScript code", report->lineno);
}
