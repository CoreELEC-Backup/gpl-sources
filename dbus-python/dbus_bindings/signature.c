/* Implementation of Signature type for D-Bus bindings.
 *
 * Copyright (C) 2006 Collabora Ltd. <http://www.collabora.co.uk/>
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "dbus_bindings-internal.h"

#include <Python.h>
#include <structmember.h>

#include "types-internal.h"

PyDoc_STRVAR(Signature_tp_doc,
"Signature(value: str or unicode[, variant_level: int])\n"
"\n"
"A string subclass whose values are restricted to valid D-Bus\n"
"signatures. When iterated over, instead of individual characters it\n"
"produces Signature instances representing single complete types.\n"
"\n"
"``value`` must be a valid D-Bus signature (zero or more single complete\n"
"types).\n"
"\n"
":py:attr:`variant_level` must be non-negative; the default is 0.\n"
"\n"
".. py:attribute:: variant_level\n"
"\n"
"    Indicates how many nested Variant containers this object\n"
"    is contained in: if a message's wire format has a variant containing a\n"
"    variant containing a signature, this is represented in Python by a\n"
"    Signature with variant_level==2.\n"
);

typedef struct {
    PyObject_HEAD
    PyObject *bytes;
    DBusSignatureIter iter;
} SignatureIter;

static void
SignatureIter_tp_dealloc (SignatureIter *self)
{
    Py_CLEAR(self->bytes);
    PyObject_Del(self);
}

static PyObject *
SignatureIter_tp_iternext (SignatureIter *self)
{
    char *sig;
    PyObject *obj;

    /* Stop immediately if finished or not correctly initialized */
    if (!self->bytes) return NULL;

    sig = dbus_signature_iter_get_signature(&(self->iter));
    if (!sig) return PyErr_NoMemory();
    obj = PyObject_CallFunction((PyObject *)&DBusPySignature_Type, "s", sig);
    dbus_free(sig);
    if (!obj) return NULL;

    if (!dbus_signature_iter_next(&(self->iter))) {
        /* mark object as having been finished with */
        Py_CLEAR(self->bytes);
    }

    return obj;
}

static PyObject *
SignatureIter_tp_iter(PyObject *self)
{
    Py_INCREF(self);
    return self;
}

static PyTypeObject SignatureIterType = {
    PyVarObject_HEAD_INIT(DEFERRED_ADDRESS(&PyType_Type), 0)
    "_dbus_bindings._SignatureIter",
    sizeof(SignatureIter),
    0,
    (destructor)SignatureIter_tp_dealloc,   /* tp_dealloc */
    0,                                      /* tp_print */
    0,                                      /* tp_getattr */
    0,                                      /* tp_setattr */
    0,                                      /* tp_compare */
    0,                                      /* tp_repr */
    0,                                      /* tp_as_number */
    0,                                      /* tp_as_sequence */
    0,                                      /* tp_as_mapping */
    0,                                      /* tp_hash */
    0,                                      /* tp_call */
    0,                                      /* tp_str */
    0,                                      /* tp_getattro */
    0,                                      /* tp_setattro */
    0,                                      /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,                     /* tp_flags */
    0,                                      /* tp_doc */
    0,                                      /* tp_traverse */
    0,                                      /* tp_clear */
    0,                                      /* tp_richcompare */
    0,                                      /* tp_weaklistoffset */
    SignatureIter_tp_iter,                  /* tp_iter */
    (iternextfunc)SignatureIter_tp_iternext,  /* tp_iternext */
    0,                                      /* tp_methods */
    0,                                      /* tp_members */
    0,                                      /* tp_getset */
    0,                                      /* tp_base */
    0,                                      /* tp_dict */
    0,                                      /* tp_descr_get */
    0,                                      /* tp_descr_set */
    0,                                      /* tp_dictoffset */
    0,                                      /* tp_init */
    0,                                      /* tp_alloc */
    /* deliberately not callable! Use iter(Signature) instead */
    0,                                      /* tp_new */
    0,                                      /* tp_free */
};

static PyObject *
Signature_tp_iter(PyObject *self)
{
    SignatureIter *iter = PyObject_New(SignatureIter, &SignatureIterType);
    PyObject *self_as_bytes;

    if (!iter) return NULL;

#ifdef PY3
    self_as_bytes = PyUnicode_AsUTF8String(self);
    if (!self_as_bytes) {
        Py_CLEAR(iter);
        return NULL;
    }
#else
    self_as_bytes = self;
    Py_INCREF(self_as_bytes);
#endif

    if (PyBytes_GET_SIZE(self_as_bytes) > 0) {
        iter->bytes = self_as_bytes;
        dbus_signature_iter_init(&(iter->iter),
                                 PyBytes_AS_STRING(self_as_bytes));
    }
    else {
        /* this is a null string, make a null iterator */
        iter->bytes = NULL;
        Py_CLEAR(self_as_bytes);
    }
    return (PyObject *)iter;
}

static PyObject *
Signature_tp_new (PyTypeObject *cls, PyObject *args, PyObject *kwargs)
{
    const char *str = NULL;
    PyObject *ignored;
    static char *argnames[] = {"object_path", "variant_level", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s|O:__new__", argnames,
                                     &str, &ignored)) return NULL;
    if (!dbus_signature_validate(str, NULL)) {
        PyErr_SetString(PyExc_ValueError, "Corrupt type signature");
        return NULL;
    }
    return (DBusPyStrBase_Type.tp_new)(cls, args, kwargs);
}

PyTypeObject DBusPySignature_Type = {
    PyVarObject_HEAD_INIT(DEFERRED_ADDRESS(&PyType_Type), 0)
    "dbus.Signature",
    0,
    0,
    0,                                      /* tp_dealloc */
    0,                                      /* tp_print */
    0,                                      /* tp_getattr */
    0,                                      /* tp_setattr */
    0,                                      /* tp_compare */
    0,                                      /* tp_repr */
    0,                                      /* tp_as_number */
    0,                                      /* tp_as_sequence */
    0,                                      /* tp_as_mapping */
    0,                                      /* tp_hash */
    0,                                      /* tp_call */
    0,                                      /* tp_str */
    0,                                      /* tp_getattro */
    0,                                      /* tp_setattro */
    0,                                      /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */
    Signature_tp_doc,                       /* tp_doc */
    0,                                      /* tp_traverse */
    0,                                      /* tp_clear */
    0,                                      /* tp_richcompare */
    0,                                      /* tp_weaklistoffset */
    Signature_tp_iter,                      /* tp_iter */
    0,                                      /* tp_iternext */
    0,                                      /* tp_methods */
    0,                                      /* tp_members */
    0,                                      /* tp_getset */
    DEFERRED_ADDRESS(&DBusPyStrBase_Type),  /* tp_base */
    0,                                      /* tp_dict */
    0,                                      /* tp_descr_get */
    0,                                      /* tp_descr_set */
    0,                                      /* tp_dictoffset */
    0,                                      /* tp_init */
    0,                                      /* tp_alloc */
    Signature_tp_new,                       /* tp_new */
    0,                                      /* tp_free */
};

dbus_bool_t
dbus_py_init_signature(void)
{
    if (PyType_Ready(&SignatureIterType) < 0) return 0;

    DBusPySignature_Type.tp_base = &DBusPyStrBase_Type;
    if (PyType_Ready(&DBusPySignature_Type) < 0) return 0;
#ifndef PY3
    DBusPySignature_Type.tp_print = NULL;
#endif

    return 1;
}

dbus_bool_t
dbus_py_insert_signature(PyObject *this_module)
{
    /* PyModule_AddObject steals a ref */
    Py_INCREF(&DBusPySignature_Type);
    if (PyModule_AddObject(this_module, "Signature",
                           (PyObject *)&DBusPySignature_Type) < 0) return 0;
    Py_INCREF(&SignatureIterType);
    if (PyModule_AddObject(this_module, "_SignatureIter",
                           (PyObject *)&SignatureIterType) < 0) return 0;

    return 1;
}

/* vim:set ft=c cino< sw=4 sts=4 et: */
