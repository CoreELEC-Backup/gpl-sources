/* D-Bus container types: Array, Dict and Struct.
 *
 * Copyright (C) 2006-2007 Collabora Ltd. <http://www.collabora.co.uk/>
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

/* Array ============================================================ */

PyDoc_STRVAR(Array_tp_doc,
"dbus.Array([iterable][, signature][, variant_level])\n"
"\n"
"An array of similar items, implemented as a subtype of list.\n"
"\n"
"As currently implemented, an Array behaves just like a list, but\n"
"with the addition of a ``signature`` property set by the constructor;\n"
"conversion of its items to D-Bus types is only done when it's sent in\n"
"a Message. This might change in future so validation is done earlier.\n"
"\n"
":py:attr:`variant_level` must be non-negative; the default is 0.\n"
"\n"
"``signature`` is the D-Bus signature string for a single element of the\n"
"array, or None. If not None it must represent a single complete type, the\n"
"type of a single array item; the signature of the whole Array may be\n"
"obtained by prepending ``a`` to the given signature.\n"
"\n"
"If None (the default), when the Array is sent over\n"
"D-Bus, the item signature will be guessed from the first element.\n"
);

static struct PyMemberDef Array_tp_members[] = {
    {"signature", T_OBJECT, offsetof(DBusPyArray, signature), READONLY,
     "The D-Bus signature of each element of this Array (a Signature "
     "instance)"},
    {"variant_level", T_LONG, offsetof(DBusPyArray, variant_level),
     READONLY,
    "Indicates how many nested Variant containers this object\n"
    "is contained in: if a message's wire format has a variant containing a\n"
    "variant containing an array, this is represented in Python by an\n"
    "Array with variant_level==2.\n"
    },
    {NULL},
};

static void
Array_tp_dealloc (DBusPyArray *self)
{
    Py_CLEAR(self->signature);
    (PyList_Type.tp_dealloc)((PyObject *)self);
}

static PyObject *
Array_tp_repr(DBusPyArray *self)
{
    PyObject *parent_repr = (PyList_Type.tp_repr)((PyObject *)self);
    PyObject *sig_repr = PyObject_Repr(self->signature);
    PyObject *my_repr = NULL;
    long variant_level = self->variant_level;

    if (!parent_repr) goto finally;
    if (!sig_repr) goto finally;
    if (variant_level > 0) {
        my_repr = PyUnicode_FromFormat("%s(%V, signature=%V, "
                                       "variant_level=%ld)",
                                       Py_TYPE(&self->super)->tp_name,
                                       REPRV(parent_repr),
                                       REPRV(sig_repr),
                                       variant_level);
    }
    else {
        my_repr = PyUnicode_FromFormat("%s(%V, signature=%V)",
                                       Py_TYPE(&self->super)->tp_name,
                                       REPRV(parent_repr),
                                       REPRV(sig_repr));
    }
finally:
    Py_CLEAR(parent_repr);
    Py_CLEAR(sig_repr);
    return my_repr;
}

static PyObject *
Array_tp_new (PyTypeObject *cls, PyObject *args, PyObject *kwargs)
{
    PyObject *variant_level = NULL;
    DBusPyArray *self = (DBusPyArray *)(PyList_Type.tp_new)(cls, args, kwargs);

    /* variant_level is immutable, so handle it in __new__ rather than
    __init__ */
    if (!self) return NULL;
    Py_INCREF(Py_None);
    self->signature = Py_None;
    self->variant_level = 0;
    if (kwargs) {
        variant_level = PyDict_GetItem(kwargs, dbus_py_variant_level_const);
    }
    if (variant_level) {
        long new_variant_level = PyLong_AsLong(variant_level);
        if (new_variant_level == -1 && PyErr_Occurred()) {
            Py_CLEAR(self);
            return NULL;
        }
        self->variant_level = new_variant_level;
    }
    return (PyObject *)self;
}

static int
Array_tp_init (DBusPyArray *self, PyObject *args, PyObject *kwargs)
{
    PyObject *obj = dbus_py_empty_tuple;
    PyObject *signature = NULL;
    PyObject *tuple;
    PyObject *variant_level;
    /* variant_level is accepted but ignored - it's immutable, so
     * __new__ handles it */
    static char *argnames[] = {"iterable", "signature", "variant_level", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|OOO:__init__", argnames,
                                     &obj, &signature, &variant_level)) {
        return -1;
    }

  /* convert signature from a borrowed ref of unknown type to an owned ref
  of type Signature (or None) */
    if (!signature) signature = Py_None;
    if (signature == Py_None
        || PyObject_IsInstance(signature, (PyObject *)&DBusPySignature_Type)) {
        Py_INCREF(signature);
    }
    else {
        signature = PyObject_CallFunction((PyObject *)&DBusPySignature_Type,
                                          "(O)", signature);
        if (!signature) return -1;
    }

    if (signature != Py_None) {
        const char *c_str;
        PyObject *signature_as_bytes;

        if (
#ifdef PY3
            !PyUnicode_Check(signature)
#else
            !PyBytes_Check(signature)
#endif
            )
        {
            PyErr_SetString(PyExc_TypeError, "str expected");
            Py_CLEAR(signature);
            return -1;
        }
#ifdef PY3
        if (!(signature_as_bytes = PyUnicode_AsUTF8String(signature))) {
            Py_CLEAR(signature);
            return -1;
        }
#else
        signature_as_bytes = signature;
        Py_INCREF(signature_as_bytes);
#endif

        c_str = PyBytes_AS_STRING(signature_as_bytes);

        if (!dbus_signature_validate_single(c_str, NULL)) {
            Py_CLEAR(signature);
            Py_CLEAR(signature_as_bytes);
            PyErr_SetString(PyExc_ValueError,
                            "There must be exactly one complete type in "
                            "an Array's signature parameter");
            return -1;
        }
        Py_CLEAR(signature_as_bytes);
    }

    tuple = Py_BuildValue("(O)", obj);
    if (!tuple) {
        Py_CLEAR(signature);
        return -1;
    }
    if ((PyList_Type.tp_init)((PyObject *)self, tuple, NULL) < 0) {
        Py_CLEAR(tuple);
        Py_CLEAR(signature);
        return -1;
    }
    Py_CLEAR(tuple);

    Py_CLEAR(self->signature);
    self->signature = signature;
    return 0;
}

PyTypeObject DBusPyArray_Type = {
    PyVarObject_HEAD_INIT(DEFERRED_ADDRESS(&PyType_Type), 0)
    "dbus.Array",
    sizeof(DBusPyArray),
    0,
    (destructor)Array_tp_dealloc,           /* tp_dealloc */
    0,                                      /* tp_print */
    0,                                      /* tp_getattr */
    0,                                      /* tp_setattr */
    0,                                      /* tp_compare */
    (reprfunc)Array_tp_repr,                /* tp_repr */
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
    Array_tp_doc,                           /* tp_doc */
    0,                                      /* tp_traverse */
    0,                                      /* tp_clear */
    0,                                      /* tp_richcompare */
    0,                                      /* tp_weaklistoffset */
    0,                                      /* tp_iter */
    0,                                      /* tp_iternext */
    0,                                      /* tp_methods */
    Array_tp_members,                       /* tp_members */
    0,                                      /* tp_getset */
    0,                                      /* tp_base */
    0,                                      /* tp_dict */
    0,                                      /* tp_descr_get */
    0,                                      /* tp_descr_set */
    0,                                      /* tp_dictoffset */
    (initproc)Array_tp_init,                /* tp_init */
    0,                                      /* tp_alloc */
    Array_tp_new,                           /* tp_new */
};

/* Dict ============================================================= */

PyDoc_STRVAR(Dict_tp_doc,
"dbus.Dictionary(mapping_or_iterable=(), signature=None, variant_level=0)\n"
"\n"
"An mapping whose keys are similar and whose values are similar,\n"
"implemented as a subtype of dict.\n"
"\n"
"As currently implemented, a Dictionary behaves just like a dict, but\n"
"with the addition of a ``signature`` property set by the constructor;\n"
"conversion of its items to D-Bus types is only done when it's sent in\n"
"a Message. This may change in future so validation is done earlier.\n"
"\n"
":py:attr:`variant_level` must be non-negative; the default is 0.\n"
"\n"
"``signature`` is either a string or None. If a string, it must consist\n"
"of exactly two complete type signatures, representing the 'key' type\n"
"(which must be a primitive type, i.e. one of \"bdginoqstuxy\")\n"
"and the 'value' type. The signature of the whole Dictionary will be\n"
"``a{xx}`` where ``xx`` is replaced by the given signature.\n"
"\n"
"If it is None (the default), when the Dictionary is sent over\n"
"D-Bus, the key and value signatures will be guessed from an arbitrary\n"
"element of the Dictionary.\n"
);

static struct PyMemberDef Dict_tp_members[] = {
    {"signature", T_OBJECT, offsetof(DBusPyDict, signature), READONLY,
     "The D-Bus signature of each key in this Dictionary, followed by "
     "that of each value in this Dictionary, as a Signature instance."},
    {"variant_level", T_LONG, offsetof(DBusPyDict, variant_level),
     READONLY,
    "Indicates how many nested Variant containers this object\n"
    "is contained in: if a message's wire format has a variant containing a\n"
    "variant containing a dictionary, this is represented in Python by a\n"
    "Dictionary with variant_level==2.\n"
    },
    {NULL},
};

static void
Dict_tp_dealloc (DBusPyDict *self)
{
    Py_CLEAR(self->signature);
    (PyDict_Type.tp_dealloc)((PyObject *)self);
}

static PyObject *
Dict_tp_repr(DBusPyDict *self)
{
    PyObject *parent_repr = (PyDict_Type.tp_repr)((PyObject *)self);
    PyObject *sig_repr = PyObject_Repr(self->signature);
    PyObject *my_repr = NULL;
    long variant_level = self->variant_level;

    if (!parent_repr) goto finally;
    if (!sig_repr) goto finally;
    if (variant_level > 0) {
        my_repr = PyUnicode_FromFormat("%s(%V, signature=%V, "
                                       "variant_level=%ld)",
                                       Py_TYPE(&self->super)->tp_name,
                                       REPRV(parent_repr),
                                       REPRV(sig_repr),
                                       variant_level);
    }
    else {
        my_repr = PyUnicode_FromFormat("%s(%V, signature=%V)",
                                       Py_TYPE(&self->super)->tp_name,
                                       REPRV(parent_repr),
                                       REPRV(sig_repr));
    }
finally:
    Py_CLEAR(parent_repr);
    Py_CLEAR(sig_repr);
    return my_repr;
}

static PyObject *
Dict_tp_new(PyTypeObject *cls, PyObject *args, PyObject *kwargs)
{
    DBusPyDict *self = (DBusPyDict *)(PyDict_Type.tp_new)(cls, args, kwargs);
    PyObject *variant_level = NULL;

    /* variant_level is immutable, so handle it in __new__ rather than
    __init__ */
    if (!self) return NULL;
    Py_INCREF(Py_None);
    self->signature = Py_None;
    self->variant_level = 0;
    if (kwargs) {
        variant_level = PyDict_GetItem(kwargs, dbus_py_variant_level_const);
    }
    if (variant_level) {
        long new_variant_level = PyLong_AsLong(variant_level);

        if (new_variant_level == -1 && PyErr_Occurred()) {
            Py_CLEAR(self);
            return NULL;
        }
        self->variant_level = new_variant_level;
    }
    return (PyObject *)self;
}

static int
Dict_tp_init(DBusPyDict *self, PyObject *args, PyObject *kwargs)
{
    PyObject *obj = dbus_py_empty_tuple;
    PyObject *signature = NULL;
    PyObject *tuple;
    PyObject *variant_level;    /* ignored here - __new__ uses it */
    static char *argnames[] = {"mapping_or_iterable", "signature",
                               "variant_level", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|OOO:__init__", argnames,
                                     &obj, &signature, &variant_level)) {
        return -1;
    }

  /* convert signature from a borrowed ref of unknown type to an owned ref
  of type Signature (or None) */
    if (!signature) signature = Py_None;
    if (signature == Py_None
        || PyObject_IsInstance(signature, (PyObject *)&DBusPySignature_Type)) {
        Py_INCREF(signature);
    }
    else {
        signature = PyObject_CallFunction((PyObject *)&DBusPySignature_Type,
                                          "(O)", signature);
        if (!signature) return -1;
    }

    if (signature != Py_None) {
        const char *c_str;
        PyObject *signature_as_bytes;

        if (!NATIVESTR_CHECK(signature)) {
            PyErr_SetString(PyExc_TypeError, "str expected");
            Py_CLEAR(signature);
            return -1;
        }
#ifdef PY3
        if (!(signature_as_bytes = PyUnicode_AsUTF8String(signature))) {
            Py_CLEAR(signature);
            return -1;
        }
#else
        signature_as_bytes = signature;
        Py_INCREF(signature_as_bytes);
#endif

        c_str = PyBytes_AS_STRING(signature_as_bytes);
        switch (c_str[0]) {
            case DBUS_TYPE_BYTE:
            case DBUS_TYPE_BOOLEAN:
            case DBUS_TYPE_INT16:
            case DBUS_TYPE_UINT16:
            case DBUS_TYPE_INT32:
            case DBUS_TYPE_UINT32:
            case DBUS_TYPE_INT64:
            case DBUS_TYPE_UINT64:
            case DBUS_TYPE_DOUBLE:
#ifdef WITH_DBUS_FLOAT32
            case DBUS_TYPE_FLOAT:
#endif
#ifdef DBUS_TYPE_UNIX_FD
            case DBUS_TYPE_UNIX_FD:
#endif
            case DBUS_TYPE_STRING:
            case DBUS_TYPE_OBJECT_PATH:
            case DBUS_TYPE_SIGNATURE:
                break;
            default:
                Py_CLEAR(signature);
                Py_CLEAR(signature_as_bytes);
                PyErr_SetString(PyExc_ValueError,
                                "The key type in a Dictionary's signature "
                                "must be a primitive type");
                return -1;
        }

        if (!dbus_signature_validate_single(c_str + 1, NULL)) {
            Py_CLEAR(signature);
            Py_CLEAR(signature_as_bytes);
            PyErr_SetString(PyExc_ValueError,
                            "There must be exactly two complete types in "
                            "a Dictionary's signature parameter");
            return -1;
        }
        Py_CLEAR(signature_as_bytes);
    }

    tuple = Py_BuildValue("(O)", obj);
    if (!tuple) {
        Py_CLEAR(signature);
        return -1;
    }

    if ((PyDict_Type.tp_init((PyObject *)self, tuple, NULL)) < 0) {
        Py_CLEAR(tuple);
        Py_CLEAR(signature);
        return -1;
    }
    Py_CLEAR(tuple);

    Py_CLEAR(self->signature);
    self->signature = signature;
    return 0;
}

PyTypeObject DBusPyDict_Type = {
    PyVarObject_HEAD_INIT(DEFERRED_ADDRESS(&PyType_Type), 0)
    "dbus.Dictionary",
    sizeof(DBusPyDict),
    0,
    (destructor)Dict_tp_dealloc,            /* tp_dealloc */
    0,                                      /* tp_print */
    0,                                      /* tp_getattr */
    0,                                      /* tp_setattr */
    0,                                      /* tp_compare */
    (reprfunc)Dict_tp_repr,                 /* tp_repr */
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
    Dict_tp_doc,                            /* tp_doc */
    0,                                      /* tp_traverse */
    0,                                      /* tp_clear */
    0,                                      /* tp_richcompare */
    0,                                      /* tp_weaklistoffset */
    0,                                      /* tp_iter */
    0,                                      /* tp_iternext */
    0,                                      /* tp_methods */
    Dict_tp_members,                        /* tp_members */
    0,                                      /* tp_getset */
    0,                                      /* tp_base */
    0,                                      /* tp_dict */
    0,                                      /* tp_descr_get */
    0,                                      /* tp_descr_set */
    0,                                      /* tp_dictoffset */
    (initproc)Dict_tp_init,                 /* tp_init */
    0,                                      /* tp_alloc */
    Dict_tp_new,                            /* tp_new */
};

/* Struct =========================================================== */

static PyObject *struct_signatures;

PyDoc_STRVAR(Struct_tp_doc,
"dbus.Struct(iterable, signature=None, variant_level=0)\n"
"\n"
"An structure containing items of possibly distinct types.\n"
"\n"
"D-Bus structs may not be empty, so the iterable argument is required and\n"
"may not be an empty iterable.\n"
"\n"
"``signature`` is either None, or a string representing the contents of the\n"
"struct as one or more complete type signatures. The overall signature of\n"
"the struct will be the given signature enclosed in parentheses, ``()``.\n"
"\n"
"If the signature is None (default) it will be guessed\n"
"from the types of the items during construction.\n"
"\n"
":py:attr:`variant_level` must be non-negative; the default is 0.\n"
"\n"
".. py:attribute:: variant_level\n"
"\n"
"    Indicates how many nested Variant containers this object\n"
"    is contained in: if a message's wire format has a variant containing a\n"
"    variant containing a struct, this is represented in Python by a\n"
"    Struct with variant_level==2.\n"
);

static PyObject *
Struct_tp_repr(PyObject *self)
{
    PyObject *parent_repr = (PyTuple_Type.tp_repr)((PyObject *)self);
    PyObject *sig;
    PyObject *sig_repr = NULL;
    PyObject *key;
    long variant_level;
    PyObject *my_repr = NULL;

    if (!parent_repr) goto finally;
    key = PyLong_FromVoidPtr(self);
    if (!key) goto finally;
    sig = PyDict_GetItem(struct_signatures, key);
    Py_CLEAR(key);
    if (!sig) sig = Py_None;
    sig_repr = PyObject_Repr(sig);
    if (!sig_repr) goto finally;

    variant_level = dbus_py_variant_level_get(self);
    if (variant_level < 0)
        goto finally;

    if (variant_level > 0) {
        my_repr = PyUnicode_FromFormat("%s(%V, signature=%V, "
                                       "variant_level=%ld)",
                                       Py_TYPE(self)->tp_name,
                                       REPRV(parent_repr),
                                       REPRV(sig_repr),
                                       variant_level);
    }
    else {
        my_repr = PyUnicode_FromFormat("%s(%V, signature=%V)",
                                       Py_TYPE(self)->tp_name,
                                       REPRV(parent_repr),
                                       REPRV(sig_repr));
    }

finally:
    Py_CLEAR(parent_repr);
    Py_CLEAR(sig_repr);
    return my_repr;
}

static PyObject *
Struct_tp_new (PyTypeObject *cls, PyObject *args, PyObject *kwargs)
{
    PyObject *signature = NULL;
    long variantness = 0;
    PyObject *self, *key;
    static char *argnames[] = {"signature", "variant_level", NULL};

    if (PyTuple_Size(args) != 1) {
        PyErr_SetString(PyExc_TypeError,
                        "__new__ takes exactly one positional parameter");
        return NULL;
    }
    if (!PyArg_ParseTupleAndKeywords(dbus_py_empty_tuple, kwargs,
                                     "|Ol:__new__", argnames,
                                     &signature, &variantness)) {
        return NULL;
    }
    if (variantness < 0) {
        PyErr_SetString(PyExc_ValueError,
                        "variant_level must be non-negative");
        return NULL;
    }

    self = (PyTuple_Type.tp_new)(cls, args, NULL);
    if (!self)
        return NULL;
    if (PyTuple_Size(self) < 1) {
        PyErr_SetString(PyExc_ValueError, "D-Bus structs may not be empty");
        Py_CLEAR(self);
        return NULL;
    }

    if (!dbus_py_variant_level_set(self, variantness)) {
        Py_CLEAR(self);
        return NULL;
    }

    /* convert signature from a borrowed ref of unknown type to an owned ref
    of type Signature (or None) */
    if (!signature) signature = Py_None;
    if (signature == Py_None
        || PyObject_IsInstance(signature, (PyObject *)&DBusPySignature_Type)) {
        Py_INCREF(signature);
    }
    else {
        signature = PyObject_CallFunction((PyObject *)&DBusPySignature_Type,
                                          "(O)", signature);
        if (!signature) {
            Py_CLEAR(self);
            return NULL;
        }
    }

    key = PyLong_FromVoidPtr(self);
    if (!key) {
        Py_CLEAR(self);
        Py_CLEAR(signature);
        return NULL;
    }
    if (PyDict_SetItem(struct_signatures, key, signature) < 0) {
        Py_CLEAR(key);
        Py_CLEAR(self);
        Py_CLEAR(signature);
        return NULL;
    }

    Py_CLEAR(key);
    Py_CLEAR(signature);
    return self;
}

static void
Struct_tp_dealloc(PyObject *self)
{
    PyObject *et, *ev, *etb, *key;

    dbus_py_variant_level_clear(self);
    PyErr_Fetch(&et, &ev, &etb);

    key = PyLong_FromVoidPtr(self);
    if (key) {
        if (PyDict_GetItem(struct_signatures, key)) {
            if (PyDict_DelItem(struct_signatures, key) < 0) {
                /* should never happen */
                PyErr_WriteUnraisable(self);
            }
        }
        Py_CLEAR(key);
    }
    else {
        /* not enough memory to free all the memory... leak the signature,
         * there's not much else we could do here */
        PyErr_WriteUnraisable(self);
    }

    PyErr_Restore(et, ev, etb);
    (PyTuple_Type.tp_dealloc)(self);
}

static PyObject *
Struct_tp_getattro(PyObject *obj, PyObject *name)
{
    PyObject *key, *value;

#ifdef PY3
    if (PyUnicode_CompareWithASCIIString(name, "signature"))
        return dbus_py_variant_level_getattro(obj, name);
#else
    if (PyBytes_Check(name)) {
        Py_INCREF(name);
    }
    else if (PyUnicode_Check(name)) {
        name = PyUnicode_AsEncodedString(name, NULL, NULL);
        if (!name) {
            return NULL;
        }
    }
    else {
        PyErr_SetString(PyExc_TypeError, "attribute name must be string");
        return NULL;
    }

    if (strcmp(PyBytes_AS_STRING(name), "signature")) {
        value = dbus_py_variant_level_getattro(obj, name);
        Py_CLEAR(name);
        return value;
    }
    Py_CLEAR(name);
#endif  /* PY3 */

    key = PyLong_FromVoidPtr(obj);

    if (!key) {
        return NULL;
    }

    value = PyDict_GetItem(struct_signatures, key);
    Py_CLEAR(key);

    if (!value)
        value = Py_None;
    Py_INCREF(value);
    return value;
}

PyTypeObject DBusPyStruct_Type = {
    PyVarObject_HEAD_INIT(DEFERRED_ADDRESS(&PyType_Type), 0)
    "dbus.Struct",
    0,
    0,
    Struct_tp_dealloc,                      /* tp_dealloc */
    0,                                      /* tp_print */
    0,                                      /* tp_getattr */
    0,                                      /* tp_setattr */
    0,                                      /* tp_compare */
    (reprfunc)Struct_tp_repr,               /* tp_repr */
    0,                                      /* tp_as_number */
    0,                                      /* tp_as_sequence */
    0,                                      /* tp_as_mapping */
    0,                                      /* tp_hash */
    0,                                      /* tp_call */
    0,                                      /* tp_str */
    Struct_tp_getattro,                     /* tp_getattro */
    dbus_py_immutable_setattro,             /* tp_setattro */
    0,                                      /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */
    Struct_tp_doc,                          /* tp_doc */
    0,                                      /* tp_traverse */
    0,                                      /* tp_clear */
    0,                                      /* tp_richcompare */
    0,                                      /* tp_weaklistoffset */
    0,                                      /* tp_iter */
    0,                                      /* tp_iternext */
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
    Struct_tp_new,                          /* tp_new */
};

dbus_bool_t
dbus_py_init_container_types(void)
{
    struct_signatures = PyDict_New();
    if (!struct_signatures) return 0;

    DBusPyArray_Type.tp_base = &PyList_Type;
    if (PyType_Ready(&DBusPyArray_Type) < 0) return 0;
#ifndef PY3
    DBusPyArray_Type.tp_print = NULL;
#endif

    DBusPyDict_Type.tp_base = &PyDict_Type;
    if (PyType_Ready(&DBusPyDict_Type) < 0) return 0;
#ifndef PY3
    DBusPyDict_Type.tp_print = NULL;
#endif

    DBusPyStruct_Type.tp_base = &PyTuple_Type;
    if (PyType_Ready(&DBusPyStruct_Type) < 0) return 0;
#ifndef PY3
    DBusPyStruct_Type.tp_print = NULL;
#endif

    return 1;
}

dbus_bool_t
dbus_py_insert_container_types(PyObject *this_module)
{
    /* PyModule_AddObject steals a ref */
    Py_INCREF(&DBusPyArray_Type);
    if (PyModule_AddObject(this_module, "Array",
                           (PyObject *)&DBusPyArray_Type) < 0) return 0;

    Py_INCREF(&DBusPyDict_Type);
    if (PyModule_AddObject(this_module, "Dictionary",
                           (PyObject *)&DBusPyDict_Type) < 0) return 0;

    Py_INCREF(&DBusPyStruct_Type);
    if (PyModule_AddObject(this_module, "Struct",
                           (PyObject *)&DBusPyStruct_Type) < 0) return 0;

    return 1;
}

/* vim:set ft=c cino< sw=4 sts=4 et: */

