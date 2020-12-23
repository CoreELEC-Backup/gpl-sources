#include <compiler.h>
# This file was created automatically by SWIG.
# Don't modify this file, modify the SWIG interface instead.
# This file is compatible with both classic and new-style classes.

import _hid

def _swig_setattr(self,class_type,name,value):
    if (name == "this"):
        if isinstance(value, class_type):
            self.__dict__[name] = value.this
            if hasattr(value,"thisown"): self.__dict__["thisown"] = value.thisown
            del value.thisown
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    self.__dict__[name] = value

def _swig_getattr(self,class_type,name):
    method = class_type.__swig_getmethods__.get(name,None)
    if method: return method(self)
    raise AttributeError,name

import types
try:
    _object = types.ObjectType
    _newclass = 1
except AttributeError:
    class _object : pass
    _newclass = 0
del types



wrap_hid_interrupt_read = _hid.wrap_hid_interrupt_read

wrap_hid_get_input_report = _hid.wrap_hid_get_input_report

wrap_hid_get_feature_report = _hid.wrap_hid_get_feature_report
true = _hid.true
false = _hid.false
HID_RET_SUCCESS = _hid.HID_RET_SUCCESS
HID_RET_INVALID_PARAMETER = _hid.HID_RET_INVALID_PARAMETER
HID_RET_NOT_INITIALISED = _hid.HID_RET_NOT_INITIALISED
HID_RET_ALREADY_INITIALISED = _hid.HID_RET_ALREADY_INITIALISED
HID_RET_FAIL_FIND_BUSSES = _hid.HID_RET_FAIL_FIND_BUSSES
HID_RET_FAIL_FIND_DEVICES = _hid.HID_RET_FAIL_FIND_DEVICES
HID_RET_FAIL_OPEN_DEVICE = _hid.HID_RET_FAIL_OPEN_DEVICE
HID_RET_DEVICE_NOT_FOUND = _hid.HID_RET_DEVICE_NOT_FOUND
HID_RET_DEVICE_NOT_OPENED = _hid.HID_RET_DEVICE_NOT_OPENED
HID_RET_DEVICE_ALREADY_OPENED = _hid.HID_RET_DEVICE_ALREADY_OPENED
HID_RET_FAIL_CLOSE_DEVICE = _hid.HID_RET_FAIL_CLOSE_DEVICE
HID_RET_FAIL_CLAIM_IFACE = _hid.HID_RET_FAIL_CLAIM_IFACE
HID_RET_FAIL_DETACH_DRIVER = _hid.HID_RET_FAIL_DETACH_DRIVER
HID_RET_NOT_HID_DEVICE = _hid.HID_RET_NOT_HID_DEVICE
HID_RET_HID_DESC_SHORT = _hid.HID_RET_HID_DESC_SHORT
HID_RET_REPORT_DESC_SHORT = _hid.HID_RET_REPORT_DESC_SHORT
HID_RET_REPORT_DESC_LONG = _hid.HID_RET_REPORT_DESC_LONG
HID_RET_FAIL_ALLOC = _hid.HID_RET_FAIL_ALLOC
HID_RET_OUT_OF_SPACE = _hid.HID_RET_OUT_OF_SPACE
HID_RET_FAIL_SET_REPORT = _hid.HID_RET_FAIL_SET_REPORT
HID_RET_FAIL_GET_REPORT = _hid.HID_RET_FAIL_GET_REPORT
HID_RET_FAIL_INT_READ = _hid.HID_RET_FAIL_INT_READ
HID_RET_NOT_FOUND = _hid.HID_RET_NOT_FOUND
HID_RET_TIMEOUT = _hid.HID_RET_TIMEOUT
class HIDInterface(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, HIDInterface, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, HIDInterface, name)
    def __repr__(self):
        return "<C HIDInterface instance at %s>" % (self.this,)
    __swig_getmethods__["device"] = _hid.HIDInterface_device_get
    if _newclass:device = property(_hid.HIDInterface_device_get)
    __swig_getmethods__["interface"] = _hid.HIDInterface_interface_get
    if _newclass:interface = property(_hid.HIDInterface_interface_get)
    __swig_getmethods__["id"] = _hid.HIDInterface_id_get
    if _newclass:id = property(_hid.HIDInterface_id_get)
    def __init__(self, *args):
        _swig_setattr(self, HIDInterface, 'this', _hid.new_HIDInterface(*args))
        _swig_setattr(self, HIDInterface, 'thisown', 1)
    def __del__(self, destroy=_hid.delete_HIDInterface):
        try:
            if self.thisown: destroy(self)
        except: pass

class HIDInterfacePtr(HIDInterface):
    def __init__(self, this):
        _swig_setattr(self, HIDInterface, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, HIDInterface, 'thisown', 0)
        _swig_setattr(self, HIDInterface,self.__class__,HIDInterface)
_hid.HIDInterface_swigregister(HIDInterfacePtr)

class HIDInterfaceMatcher(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, HIDInterfaceMatcher, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, HIDInterfaceMatcher, name)
    def __repr__(self):
        return "<C HIDInterfaceMatcher instance at %s>" % (self.this,)
    __swig_setmethods__["vendor_id"] = _hid.HIDInterfaceMatcher_vendor_id_set
    __swig_getmethods__["vendor_id"] = _hid.HIDInterfaceMatcher_vendor_id_get
    if _newclass:vendor_id = property(_hid.HIDInterfaceMatcher_vendor_id_get, _hid.HIDInterfaceMatcher_vendor_id_set)
    __swig_setmethods__["product_id"] = _hid.HIDInterfaceMatcher_product_id_set
    __swig_getmethods__["product_id"] = _hid.HIDInterfaceMatcher_product_id_get
    if _newclass:product_id = property(_hid.HIDInterfaceMatcher_product_id_get, _hid.HIDInterfaceMatcher_product_id_set)
    def __init__(self, *args):
        _swig_setattr(self, HIDInterfaceMatcher, 'this', _hid.new_HIDInterfaceMatcher(*args))
        _swig_setattr(self, HIDInterfaceMatcher, 'thisown', 1)
    def __del__(self, destroy=_hid.delete_HIDInterfaceMatcher):
        try:
            if self.thisown: destroy(self)
        except: pass

class HIDInterfaceMatcherPtr(HIDInterfaceMatcher):
    def __init__(self, this):
        _swig_setattr(self, HIDInterfaceMatcher, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, HIDInterfaceMatcher, 'thisown', 0)
        _swig_setattr(self, HIDInterfaceMatcher,self.__class__,HIDInterfaceMatcher)
_hid.HIDInterfaceMatcher_swigregister(HIDInterfaceMatcherPtr)

HID_ID_MATCH_ANY = _hid.HID_ID_MATCH_ANY
HID_DEBUG_NONE = _hid.HID_DEBUG_NONE
HID_DEBUG_ERRORS = _hid.HID_DEBUG_ERRORS
HID_DEBUG_WARNINGS = _hid.HID_DEBUG_WARNINGS
HID_DEBUG_NOTICES = _hid.HID_DEBUG_NOTICES
HID_DEBUG_TRACES = _hid.HID_DEBUG_TRACES
HID_DEBUG_ASSERTS = _hid.HID_DEBUG_ASSERTS
HID_DEBUG_NOTRACES = _hid.HID_DEBUG_NOTRACES
HID_DEBUG_ALL = _hid.HID_DEBUG_ALL

hid_set_debug = _hid.hid_set_debug

hid_set_debug_stream = _hid.hid_set_debug_stream

hid_set_usb_debug = _hid.hid_set_usb_debug

hid_new_HIDInterface = _hid.hid_new_HIDInterface

hid_delete_HIDInterface = _hid.hid_delete_HIDInterface

hid_reset_HIDInterface = _hid.hid_reset_HIDInterface

hid_init = _hid.hid_init

hid_cleanup = _hid.hid_cleanup

hid_is_initialised = _hid.hid_is_initialised

hid_open = _hid.hid_open

hid_force_open = _hid.hid_force_open

hid_close = _hid.hid_close

hid_is_opened = _hid.hid_is_opened

hid_strerror = _hid.hid_strerror

hid_get_input_report = _hid.hid_get_input_report

hid_set_output_report = _hid.hid_set_output_report

hid_get_feature_report = _hid.hid_get_feature_report

hid_set_feature_report = _hid.hid_set_feature_report

hid_get_item_value = _hid.hid_get_item_value

hid_write_identification = _hid.hid_write_identification

hid_dump_tree = _hid.hid_dump_tree

hid_interrupt_read = _hid.hid_interrupt_read

hid_interrupt_write = _hid.hid_interrupt_write

hid_set_idle = _hid.hid_set_idle
_doc = hid_interrupt_read.__doc__
hid_interrupt_read = wrap_hid_interrupt_read
hid_interrupt_read.__doc__ = _doc

_doc = hid_get_input_report.__doc__
hid_get_input_report = wrap_hid_get_input_report
hid_get_input_report.__doc__ = _doc

_doc = hid_get_feature_report.__doc__
hid_get_feature_report = wrap_hid_get_feature_report
hid_get_feature_report.__doc__ = _doc

import sys
hid_return = {}
for sym in dir(sys.modules[__name__]):
    if sym.startswith('HID_RET_'):
        hid_return[eval(sym)] = sym


) ptr;
  int i;
  for (i = 0; i < sz; i++, u++) {
    d = *(c++);
    if ((d >= '0') && (d <= '9'))
      uu = ((d - '0') << 4);
    else if ((d >= 'a') && (d <= 'f'))
      uu = ((d - ('a'-10)) << 4);
    d = *(c++);
    if ((d >= '0') && (d <= '9'))
      uu |= (d - '0');
    else if ((d >= 'a') && (d <= 'f'))
      uu |= (d - ('a'-10));
    *u = uu;
  }
  return c;
}

#endif

#ifdef __cplusplus
}
#endif

/***********************************************************************
 * python.swg
 *
 *     This file contains the runtime support for Python modules
 *     and includes code for managing global variables and pointer
 *     type checking.
 *
 * Author : David Beazley (beazley@cs.uchicago.edu)
 ************************************************************************/

#include "Python.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SWIG_PY_INT     1
#define SWIG_PY_FLOAT   2
#define SWIG_PY_STRING  3
#define SWIG_PY_POINTER 4
#define SWIG_PY_BINARY  5

/* Flags for pointer conversion */

#define SWIG_POINTER_EXCEPTION     0x1
#define SWIG_POINTER_DISOWN        0x2

/* Exception handling in wrappers */
#define SWIG_fail   goto fail

/* Constant information structure */
typedef struct swig_const_info {
    int type;
    char *name;
    long lvalue;
    double dvalue;
    void   *pvalue;
    swig_type_info **ptype;
} swig_const_info;

/* Common SWIG API */
#define SWIG_ConvertPtr(obj, pp, type, flags) \
  SWIG_Python_ConvertPtr(obj, pp, type, flags)
#define SWIG_NewPointerObj(p, type, flags) \
  SWIG_Python_NewPointerObj(p, type, flags)
#define SWIG_MustGetPtr(p, type, argnum, flags) \
  SWIG_Python_MustGetPtr(p, type, argnum, flags)

/* Python-specific SWIG API */
#define SWIG_newvarlink() \
  SWIG_Python_newvarlink()
#define SWIG_addvarlink(p, name, get_attr, set_attr) \
  SWIG_Python_addvarlink(p, name, get_attr, set_attr)
#define SWIG_ConvertPacked(obj, ptr, sz, ty, flags) \
  SWIG_Python_ConvertPacked(obj, ptr, sz, ty, flags)
#define SWIG_NewPackedObj(ptr, sz, type) \
  SWIG_Python_NewPackedObj(ptr, sz, type)
#define SWIG_InstallConstants(d, constants) \
  SWIG_Python_InstallConstants(d, constants)

#ifdef SWIG_NOINCLUDE

SWIGIMPORT(int)               SWIG_Python_ConvertPtr(PyObject *, void **, swig_type_info *, int);
SWIGIMPORT(PyObject *)        SWIG_Python_NewPointerObj(void *, swig_type_info *,int own);
SWIGIMPORT(void *)            SWIG_Python_MustGetPtr(PyObject *, swig_type_info *, int, int);
SWIGIMPORT(PyObject *)        SWIG_Python_newvarlink(void);
SWIGIMPORT(void)              SWIG_Python_addvarlink(PyObject *, char *, PyObject *(*)(void), int (*)(PyObject *));
SWIGIMPORT(int)               SWIG_Python_ConvertPacked(PyObject *, void *, int sz, swig_type_info *, int);
SWIGIMPORT(PyObject *)        SWIG_Python_NewPackedObj(void *, int sz, swig_type_info *);
SWIGIMPORT(void)              SWIG_Python_InstallConstants(PyObject *d, swig_const_info constants[]);

#else

/* -----------------------------------------------------------------------------
 * global variable support code.
 * ----------------------------------------------------------------------------- */

typedef struct swig_globalvar {   
  char       *name;                  /* Name of global variable */
  PyObject *(*get_attr)(void);       /* Return the current value */
  int       (*set_attr)(PyObject *); /* Set the value */
  struct swig_globalvar *next;
} swig_globalvar;

typedef struct swig_varlinkobject {
  PyObject_HEAD
  swig_globalvar *vars;
} swig_varlinkobject;

static PyObject *
swig_varlink_repr(swig_varlinkobject *v) {
  v = v;
  return PyString_FromString("<Global variables>");
}

static int
swig_varlink_print(swig_varlinkobject *v, FILE *fp, int flags UNUSED) {
  swig_globalvar  *var;
  flags = flags;
  fprintf(fp,"Global variables { ");
  for (var = v->vars; var; var=var->next) {
    fprintf(fp,"%s", var->name);
    if (var->next) fprintf(fp,", ");
  }
  fprintf(fp," }\n");
  return 0;
}

static PyObject *
swig_varlink_getattr(swig_varlinkobject *v, char *n) {
  swig_globalvar *var = v->vars;
  while (var) {
    if (strcmp(var->name,n) == 0) {
      return (*var->get_attr)();
    }
    var = var->next;
  }
  PyErr_SetString(PyExc_NameError,"Unknown C global variable");
  return NULL;
}

static int
swig_varlink_setattr(swig_varlinkobject *v, char *n, PyObject *p) {
  swig_globalvar *var = v->vars;
  while (var) {
    if (strcmp(var->name,n) == 0) {
      return (*var->set_attr)(p);
    }
    var = var->next;
  }
  PyErr_SetString(PyExc_NameError,"Unknown C global variable");
  return 1;
}

statichere PyTypeObject varlinktype = {
  PyObject_HEAD_INIT(0)              
  0,
  (char *)"swigvarlink",              /* Type name    */
  sizeof(swig_varlinkobject),         /* Basic size   */
  0,                                  /* Itemsize     */
  0,                                  /* Deallocator  */ 
  (printfunc) swig_varlink_print,     /* Print        */
  (getattrfunc) swig_varlink_getattr, /* get attr     */
  (setattrfunc) swig_varlink_setattr, /* Set attr     */
  0,                                  /* tp_compare   */
  (reprfunc) swig_varlink_repr,       /* tp_repr      */    
  0,                                  /* tp_as_number */
  0,                                  /* tp_as_mapping*/
  0,                                  /* tp_hash      */
};

/* Create a variable linking object for use later */
SWIGRUNTIME(PyObject *)
SWIG_Python_newvarlink(void) {
  swig_varlinkobject *result = 0;
  result = PyMem_NEW(swig_varlinkobject,1);
  varlinktype.ob_type = &PyType_Type;    /* Patch varlinktype into a PyType */
  result->ob_type = &varlinktype;
  result->vars = 0;
  result->ob_refcnt = 0;
  Py_XINCREF((PyObject *) result);
  return ((PyObject*) result);
}

SWIGRUNTIME(void)
SWIG_Python_addvarlink(PyObject *p, char *name, PyObject *(*get_attr)(void), int (*set_attr)(PyObject *p)) {
  swig_varlinkobject *v;
  swig_globalvar *gv;
  v= (swig_varlinkobject *) p;
  gv = (swig_globalvar *) malloc(sizeof(swig_globalvar));
  gv->name = (char *) malloc(strlen(name)+1);
  strcpy(gv->name,name);
  gv->get_attr = get_attr;
  gv->set_attr = set_attr;
  gv->next = v->vars;
  v->vars = gv;
}

/* Convert a pointer value */
SWIGRUNTIME(int)
SWIG_Python_ConvertPtr(PyObject *obj, void **ptr, swig_type_info *ty, int flags UNUSED) {
  swig_type_info *tc;
  char  *c = 0;
  static PyObject *SWIG_this = 0;
  int    newref = 0;
  PyObject  *pyobj = 0;

  if (!obj) return 0;
  if (obj == Py_None) {
    *ptr = 0;
    return 0;
  }
#ifdef SWIG_COBJECT_TYPES
  if (!(PyCObject_Check(obj))) {
    if (!SWIG_this)
      SWIG_this = PyString_FromString("this");
    pyobj = obj;
    obj = PyObject_GetAttr(obj,SWIG_this);
    newref = 1;
    if (!obj) goto type_error;
    if (!PyCObject_Check(obj)) {
      Py_DECREF(obj);
      goto type_error;
    }
  }  
  *ptr = PyCObject_AsVoidPtr(obj);
  c = (char *) PyCObject_GetDesc(obj);
  if (newref) Py_DECREF(obj);
  goto cobject;
#else
  if (!(PyString_Check(obj))) {
    if (!SWIG_this)
      SWIG_this = PyString_FromString("this");
    pyobj = obj;
    obj = PyObject_GetAttr(obj,SWIG_this);
    newref = 1;
    if (!obj) goto type_error;
    if (!PyString_Check(obj)) {
      Py_DECREF(obj);
      goto type_error;
    }
  } 
  c = PyString_AsString(obj);
  /* Pointer values must start with leading underscore */
  if (*c != '_') {
    *ptr = (void *) 0;
    if (strcmp(c,"NULL") == 0) {
      if (newref) { Py_DECREF(obj); }
      return 0;
    } else {
      if (newref) { Py_DECREF(obj); }
      goto type_error;
    }
  }
  c++;
  c = SWIG_UnpackData(c,ptr,sizeof(void *));
  if (newref) { Py_DECREF(obj); }
#endif

#ifdef SWIG_COBJECT_TYPES
cobject:
#endif

  if (ty) {
    tc = SWIG_TypeCheck(c,ty);
    if (!tc) goto type_error;
    *ptr = SWIG_TypeCast(tc,(void*) *ptr);
  }

  if ((pyobj) && (flags & SWIG_POINTER_DISOWN)) {
    PyObject *zero = PyInt_FromLong(0);
    PyObject_SetAttrString(pyobj,(char*)"thisown",zero);
    Py_DECREF(zero);
  }
  return 0;

type_error:
  if (flags & SWIG_POINTER_EXCEPTION) {
    if (ty && c) {
      char *temp = (char *) malloc(64+strlen(ty->name)+strlen(c));
      sprintf(temp,"Type error. Got %s, expected %s", c, ty->name);
      PyErr_SetString(PyExc_TypeError, temp);
      free((char *) temp);
    } else {
      PyErr_SetString(PyExc_TypeError,"Expected a pointer");
    }
  }
  return -1;
}

/* Convert a pointer value, signal an exception on a type mismatch */
SWIGRUNTIME(void *)
SWIG_Python_MustGetPtr(PyObject *obj, swig_type_info *ty, int argnum, int flags UNUSED) {
  void *result;
  SWIG_Python_ConvertPtr(obj, &result, ty, flags | SWIG_POINTER_EXCEPTION);
  return result;
}

/* Convert a packed value value */
SWIGRUNTIME(int)
SWIG_Python_ConvertPacked(PyObject *obj, void *ptr, int sz, swig_type_info *ty, int flags UNUSED) {
  swig_type_info *tc;
  char  *c = 0;

  if ((!obj) || (!PyString_Check(obj))) goto type_error;
  c = PyString_AsString(obj);
  /* Pointer values must start with leading underscore */
  if (*c != '_') goto type_error;
  c++;
  c = SWIG_UnpackData(c,ptr,sz);
  if (ty) {
    tc = SWIG_TypeCheck(c,ty);
    if (!tc) goto type_error;
  }
  return 0;

type_error:

  if (flags) {
    if (ty && c) {
      char *temp = (char *) malloc(64+strlen(ty->name)+strlen(c));
      sprintf(temp,"Type error. Got %s, expected %s", c, ty->name);
      PyErr_SetString(PyExc_TypeError, temp);
      free((char *) temp);
    } else {
      PyErr_SetString(PyExc_TypeError,"Expected a pointer");
    }
  }
  return -1;
}

/* Create a new pointer object */
SWIGRUNTIME(PyObject *)
SWIG_Python_NewPointerObj(void *ptr, swig_type_info *type, int own) {
  PyObject *robj;
  if (!ptr) {
    Py_INCREF(Py_None);
    return Py_None;
  }
#ifdef SWIG_COBJECT_TYPES
  robj = PyCObject_FromVoidPtrAndDesc((void *) ptr, (char *) type->name, NULL);
#else
  {
    char result[1024];
    char *r = result;
    *(r++) = '_';
    r = SWIG_PackData(r,&ptr,sizeof(void *));
    strcpy(r,type->name);
    robj = PyString_FromString(result);
  }
#endif
  if (!robj || (robj == Py_None)) return robj;
  if (type->clientdata) {
    PyObject *inst;
    PyObject *args = Py_BuildValue((char*)"(O)", robj);
    Py_DECREF(robj);
    inst = PyObject_CallObject((PyObject *) type->clientdata, args);
    Py_DECREF(args);
    if (inst) {
      if (own) {
        PyObject *n = PyInt_FromLong(1);
        PyObject_SetAttrString(inst,(char*)"thisown",n);
        Py_DECREF(n);
      }
      robj = inst;
    }
  }
  return robj;
}

SWIGRUNTIME(PyObject *)
SWIG_Python_NewPackedObj(void *ptr, int sz, swig_type_info *type) {
  char result[1024];
  char *r = result;
  if ((2*sz + 1 + strlen(type->name)) > 1000) return 0;
  *(r++) = '_';
  r = SWIG_PackData(r,ptr,sz);
  strcpy(r,type->name);
  return PyString_FromString(result);
}

/* Install Constants */
SWIGRUNTIME(void)
SWIG_Python_InstallConstants(PyObject *d, swig_const_info constants[]) {
  int i;
  PyObject *obj;
  for (i = 0; constants[i].type; i++) {
    switch(constants[i].type) {
    case SWIG_PY_INT:
      obj = PyInt_FromLong(constants[i].lvalue);
      break;
    case SWIG_PY_FLOAT:
      obj = PyFloat_FromDouble(constants[i].dvalue);
      break;
    case SWIG_PY_STRING:
      obj = PyString_FromString((char *) constants[i].pvalue);
      break;
    case SWIG_PY_POINTER:
      obj = SWIG_NewPointerObj(constants[i].pvalue, *(constants[i]).ptype,0);
      break;
    case SWIG_PY_BINARY:
      obj = SWIG_NewPackedObj(constants[i].pvalue, constants[i].lvalue, *(constants[i].ptype));
      break;
    default:
      obj = 0;
      break;
    }
    if (obj) {
      PyDict_SetItemString(d,constants[i].name,obj);
      Py_DECREF(obj);
    }
  }
}

#endif

/* Contract support */

#define SWIG_contract_assert(expr, msg) if (!(expr)) { PyErr_SetString(PyExc_RuntimeError, (char *) msg ); goto fail; } else

#ifdef __cplusplus
}
#endif


/* -------- TYPES TABLE (BEGIN) -------- */

#define  SWIGTYPE_p_HIDInterface swig_types[0] 
#define  SWIGTYPE_p_FILE swig_types[1] 
#define  SWIGTYPE_p_p_HIDInterface swig_types[2] 
#define  SWIGTYPE_p__Bool swig_types[3] 
#define  SWIGTYPE_p_HIDInterfaceMatcher swig_types[4] 
#define  SWIGTYPE_p_double swig_types[5] 
#define  SWIGTYPE_p_unsigned_int swig_types[6] 
#define  SWIGTYPE_p_usb_device swig_types[7] 
#define  SWIGTYPE_p_int swig_types[8] 
static swig_type_info *swig_types[10];

/* -------- TYPES TABLE (END) -------- */


/*-----------------------------------------------
              @(target):= _hid.so
  ------------------------------------------------*/
#define SWIG_init    init_hid

#define SWIG_name    "_hid"

#include <compiler.h>
#include <hid.h>


#define  SWIG_MemoryError    1
#define  SWIG_IOError        2
#define  SWIG_RuntimeError   3
#define  SWIG_IndexError     4
#define  SWIG_TypeError      5
#define  SWIG_DivisionByZero 6
#define  SWIG_OverflowError  7
#define  SWIG_SyntaxError    8
#define  SWIG_ValueError     9
#define  SWIG_SystemError   10
#define  SWIG_UnknownError  99


static void SWIG_exception_(int code, const char *msg) {
  switch(code) {
  case SWIG_MemoryError:
    PyErr_SetString(PyExc_MemoryError,msg);
    break;
  case SWIG_IOError:
    PyErr_SetString(PyExc_IOError,msg);
    break;
  case SWIG_RuntimeError:
    PyErr_SetString(PyExc_RuntimeError,msg);
    break;
  case SWIG_IndexError:
    PyErr_SetString(PyExc_IndexError,msg);
    break;
  case SWIG_TypeError:
    PyErr_SetString(PyExc_TypeError,msg);
    break;
  case SWIG_DivisionByZero:
    PyErr_SetString(PyExc_ZeroDivisionError,msg);
    break;
  case SWIG_OverflowError:
    PyErr_SetString(PyExc_OverflowError,msg);
    break;
  case SWIG_SyntaxError:
    PyErr_SetString(PyExc_SyntaxError,msg);
    break;
  case SWIG_ValueError:
    PyErr_SetString(PyExc_ValueError,msg);
    break;
  case SWIG_SystemError:
    PyErr_SetString(PyExc_SystemError,msg);
    break;
  default:
    PyErr_SetString(PyExc_RuntimeError,msg);
    break;
  }
}

#define SWIG_exception(a,b) { SWIG_exception_(a,b); SWIG_fail; }


hid_return wrap_hid_interrupt_read(HIDInterface* const hidif, unsigned int const ep,
    char* const bytes_out, unsigned int* const size_out, unsigned int const timeout)
{
   int res;

   res=hid_interrupt_read(hidif, ep, bytes_out, *size_out, timeout);
   if (res != HID_RET_SUCCESS) {
      *size_out = 0;
   }
   return res;
}


static PyObject* t_output_helper(PyObject* target, PyObject* o) {
    PyObject*   o2;
    PyObject*   o3;

    if (!target) {                   
        target = o;
    } else if (target == Py_None) {  
        Py_DECREF(Py_None);
        target = o;
    } else {                         
        if (!PyTuple_Check(target)) {
            o2 = target;
            target = PyTuple_New(1);
            PyTuple_SetItem(target, 0, o2);
        }
        o3 = PyTuple_New(1);            
        PyTuple_SetItem(o3, 0, o);      

        o2 = target;
        target = PySequence_Concat(o2, o3); 
        Py_DECREF(o2);                      
        Py_DECREF(o3);
    }
    return target;
}


hid_return wrap_hid_get_input_report(HIDInterface* const hidif, 
    int const path[], unsigned int const depth,
    char* const bytes_out, unsigned int* const size_out)
{
   int res;

   res=hid_get_input_report(hidif, path, depth, bytes_out, *size_out);
   if (res != HID_RET_SUCCESS) {
      *size_out = 0;
   }
   return res;
}


hid_return wrap_hid_get_feature_report(HIDInterface* const hidif, 
    int const path[], unsigned int const depth,
    char* const bytes_out, unsigned int* const size_out)
{
   int res;

   res=hid_get_feature_report(hidif, path, depth, bytes_out, *size_out);
   if (res != HID_RET_SUCCESS) {
      *size_out = 0;
   }
   return res;
}

#ifdef __cplusplus
extern "C" {
#endif
static PyObject *_wrap_wrap_hid_interrupt_read(PyObject *self UNUSED, PyObject *args) {
    PyObject *resultobj;
    HIDInterface *arg1 = (HIDInterface *) (HIDInterface *)0 ;
    unsigned int arg2 ;
    char *arg3 ;
    unsigned int *arg4 = (unsigned int *) (unsigned int *)0 ;
    unsigned int arg5 ;
    int result;
    PyObject * obj0 = 0 ;
    PyObject * obj1 = 0 ;
    PyObject * obj2 = 0 ;
    PyObject * obj3 = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"OOOO:wrap_hid_interrupt_read",&obj0,&obj1,&obj2,&obj3)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_HIDInterface,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    arg2 = (unsigned int) PyInt_AsLong(obj1);
    if (PyErr_Occurred()) SWIG_fail;
    {
        int n = PyInt_AsLong(obj2);
        if (PyErr_Occurred()) SWIG_fail;
        
        
        
        
        arg3 = (char *) malloc(n+1);
        arg4 = (unsigned int *) malloc(sizeof(unsigned int));
        
        *arg4 = n;
    }
    arg5 = (unsigned int) PyInt_AsLong(obj3);
    if (PyErr_Occurred()) SWIG_fail;
    result = (int)wrap_hid_interrupt_read(arg1,arg2,arg3,arg4,arg5);
    
    resultobj = PyInt_FromLong((long)result);
    {
        PyObject *o;
        o = PyString_FromStringAndSize(arg3,*arg4);
        resultobj = t_output_helper(resultobj,o);
        
        
        
        
        free(arg3);
        free(arg4);
        
    }
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_wrap_hid_get_input_report(PyObject *self UNUSED, PyObject *args) {
    PyObject *resultobj;
    HIDInterface *arg1 = (HIDInterface *) (HIDInterface *)0 ;
    int *arg2 ;
    unsigned int arg3 ;
    char *arg4 ;
    unsigned int *arg5 = (unsigned int *) (unsigned int *)0 ;
    int result;
    PyObject * obj0 = 0 ;
    PyObject * obj1 = 0 ;
    PyObject * obj2 = 0 ;
    
    {
        arg2 = NULL;
    }
    if(!PyArg_ParseTuple(args,(char *)"OOO:wrap_hid_get_input_report",&obj0,&obj1,&obj2)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_HIDInterface,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    {
        int i, size;
        int *temp = NULL;
        
        if (!PySequence_Check(obj1)) {
            PyErr_SetString(PyExc_TypeError,"Expecting a sequence");
            return NULL;
        }
        
        size = PySequence_Size(obj1);
        temp = (int *)calloc(size, sizeof(int));
        
        for (i =0; i < size; i++) {
            PyObject *o = PySequence_GetItem(obj1,i);
            if (PyInt_Check(o)) {
                temp[i] = (int)PyInt_AsLong(o);
            }
            else if (PyLong_Check(o)) {
                temp[i] = (int)PyLong_AsUnsignedLongMask(o);
            }
            else {
                PyErr_SetString(PyExc_ValueError,"Expecting a sequence of integers");
                return NULL;
            }
        }
        
        arg2 = temp;
        arg3 = size;
    }
    {
        int n = PyInt_AsLong(obj2);
        if (PyErr_Occurred()) SWIG_fail;
        
        
        
        
        arg4 = (char *) malloc(n+1);
        arg5 = (unsigned int *) malloc(sizeof(unsigned int));
        
        *arg5 = n;
    }
    result = (int)wrap_hid_get_input_report(arg1,(int const (*))arg2,arg3,arg4,arg5);
    
    resultobj = PyInt_FromLong((long)result);
    {
        PyObject *o;
        o = PyString_FromStringAndSize(arg4,*arg5);
        resultobj = t_output_helper(resultobj,o);
        
        
        
        
        free(arg4);
        free(arg5);
        
    }
    {
        if(arg2) free((char *) arg2);
    }
    return resultobj;
    fail:
    {
        if(arg2) free((char *) arg2);
    }
    return NULL;
}


static PyObject *_wrap_wrap_hid_get_feature_report(PyObject *self UNUSED, PyObject *args) {
    PyObject *resultobj;
    HIDInterface *arg1 = (HIDInterface *) (HIDInterface *)0 ;
    int *arg2 ;
    unsigned int arg3 ;
    char *arg4 ;
    unsigned int *arg5 = (unsigned int *) (unsigned int *)0 ;
    int result;
    PyObject * obj0 = 0 ;
    PyObject * obj1 = 0 ;
    PyObject * obj2 = 0 ;
    
    {
        arg2 = NULL;
    }
    if(!PyArg_ParseTuple(args,(char *)"OOO:wrap_hid_get_feature_report",&obj0,&obj1,&obj2)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_HIDInterface,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    {
        int i, size;
        int *temp = NULL;
        
        if (!PySequence_Check(obj1)) {
            PyErr_SetString(PyExc_TypeError,"Expecting a sequence");
            return NULL;
        }
        
        size = PySequence_Size(obj1);
        temp = (int *)calloc(size, sizeof(int));
        
        for (i =0; i < size; i++) {
            PyObject *o = PySequence_GetItem(obj1,i);
            if (PyInt_Check(o)) {
                temp[i] = (int)PyInt_AsLong(o);
            }
            else if (PyLong_Check(o)) {
                temp[i] = (int)PyLong_AsUnsignedLongMask(o);
            }
            else {
                PyErr_SetString(PyExc_ValueError,"Expecting a sequence of integers");
                return NULL;
            }
        }
        
        arg2 = temp;
        arg3 = size;
    }
    {
        int n = PyInt_AsLong(obj2);
        if (PyErr_Occurred()) SWIG_fail;
        
        
        
        
        arg4 = (char *) malloc(n+1);
        arg5 = (unsigned int *) malloc(sizeof(unsigned int));
        
        *arg5 = n;
    }
    result = (int)wrap_hid_get_feature_report(arg1,(int const (*))arg2,arg3,arg4,arg5);
    
    resultobj = PyInt_FromLong((long)result);
    {
        PyObject *o;
        o = PyString_FromStringAndSize(arg4,*arg5);
        resultobj = t_output_helper(resultobj,o);
        
        
        
        
        free(arg4);
        free(arg5);
        
    }
    {
        if(arg2) free((char *) arg2);
    }
    return resultobj;
    fail:
    {
        if(arg2) free((char *) arg2);
    }
    return NULL;
}


static PyObject *_wrap_HIDInterface_device_get(PyObject *self UNUSED, PyObject *args) {
    PyObject *resultobj;
    HIDInterface *arg1 = (HIDInterface *) 0 ;
    struct usb_device *result;
    PyObject * obj0 = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"O:HIDInterface_device_get",&obj0)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_HIDInterface,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    result = (struct usb_device *) ((arg1)->device);
    
    resultobj = SWIG_NewPointerObj((void *) result, SWIGTYPE_p_usb_device, 0);
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_HIDInterface_interface_get(PyObject *self UNUSED, PyObject *args) {
    PyObject *resultobj;
    HIDInterface *arg1 = (HIDInterface *) 0 ;
    int result;
    PyObject * obj0 = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"O:HIDInterface_interface_get",&obj0)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_HIDInterface,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    result = (int) ((arg1)->interface);
    
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_HIDInterface_id_get(PyObject *self UNUSED, PyObject *args) {
    PyObject *resultobj;
    HIDInterface *arg1 = (HIDInterface *) 0 ;
    char *result;
    PyObject * obj0 = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"O:HIDInterface_id_get",&obj0)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_HIDInterface,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    result = (char *)(char *) ((arg1)->id);
    
    {
        resultobj = PyString_FromString(result);
    }
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_new_HIDInterface(PyObject *self UNUSED, PyObject *args) {
    PyObject *resultobj;
    HIDInterface *result;
    
    if(!PyArg_ParseTuple(args,(char *)":new_HIDInterface")) goto fail;
    result = (HIDInterface *)(HIDInterface *) calloc(1, sizeof(HIDInterface));
    
    resultobj = SWIG_NewPointerObj((void *) result, SWIGTYPE_p_HIDInterface, 1);
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_delete_HIDInterface(PyObject *self UNUSED, PyObject *args) {
    PyObject *resultobj;
    HIDInterface *arg1 = (HIDInterface *) 0 ;
    PyObject * obj0 = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"O:delete_HIDInterface",&obj0)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_HIDInterface,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    free((char *) arg1);
    
    Py_INCREF(Py_None); resultobj = Py_None;
    return resultobj;
    fail:
    return NULL;
}


static PyObject * HIDInterface_swigregister(PyObject *self UNUSED, PyObject *args) {
    PyObject *obj;
    if (!PyArg_ParseTuple(args,(char*)"O", &obj)) return NULL;
    SWIG_TypeClientData(SWIGTYPE_p_HIDInterface, obj);
    Py_INCREF(obj);
    return Py_BuildValue((char *)"");
}
static PyObject *_wrap_HIDInterfaceMatcher_vendor_id_set(PyObject *self UNUSED, PyObject *args) {
    PyObject *resultobj;
    HIDInterfaceMatcher *arg1 = (HIDInterfaceMatcher *) 0 ;
    unsigned short arg2 ;
    PyObject * obj0 = 0 ;
    PyObject * obj1 = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"OO:HIDInterfaceMatcher_vendor_id_set",&obj0,&obj1)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_HIDInterfaceMatcher,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    arg2 = (unsigned short) PyInt_AsLong(obj1);
    if (PyErr_Occurred()) SWIG_fail;
    if (arg1) (arg1)->vendor_id = arg2;
    
    Py_INCREF(Py_None); resultobj = Py_None;
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_HIDInterfaceMatcher_vendor_id_get(PyObject *self UNUSED, PyObject *args) {
    PyObject *resultobj;
    HIDInterfaceMatcher *arg1 = (HIDInterfaceMatcher *) 0 ;
    unsigned short result;
    PyObject * obj0 = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"O:HIDInterfaceMatcher_vendor_id_get",&obj0)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_HIDInterfaceMatcher,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    result = (unsigned short) ((arg1)->vendor_id);
    
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_HIDInterfaceMatcher_product_id_set(PyObject *self UNUSED, PyObject *args) {
    PyObject *resultobj;
    HIDInterfaceMatcher *arg1 = (HIDInterfaceMatcher *) 0 ;
    unsigned short arg2 ;
    PyObject * obj0 = 0 ;
    PyObject * obj1 = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"OO:HIDInterfaceMatcher_product_id_set",&obj0,&obj1)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_HIDInterfaceMatcher,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    arg2 = (unsigned short) PyInt_AsLong(obj1);
    if (PyErr_Occurred()) SWIG_fail;
    if (arg1) (arg1)->product_id = arg2;
    
    Py_INCREF(Py_None); resultobj = Py_None;
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_HIDInterfaceMatcher_product_id_get(PyObject *self UNUSED, PyObject *args) {
    PyObject *resultobj;
    HIDInterfaceMatcher *arg1 = (HIDInterfaceMatcher *) 0 ;
    unsigned short result;
    PyObject * obj0 = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"O:HIDInterfaceMatcher_product_id_get",&obj0)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_HIDInterfaceMatcher,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    result = (unsigned short) ((arg1)->product_id);
    
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_new_HIDInterfaceMatcher(PyObject *self UNUSED, PyObject *args) {
    PyObject *resultobj;
    HIDInterfaceMatcher *result;
    
    if(!PyArg_ParseTuple(args,(char *)":new_HIDInterfaceMatcher")) goto fail;
    result = (HIDInterfaceMatcher *)(HIDInterfaceMatcher *) calloc(1, sizeof(HIDInterfaceMatcher));
    
    resultobj = SWIG_NewPointerObj((void *) result, SWIGTYPE_p_HIDInterfaceMatcher, 1);
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_delete_HIDInterfaceMatcher(PyObject *self UNUSED, PyObject *args) {
    PyObject *resultobj;
    HIDInterfaceMatcher *arg1 = (HIDInterfaceMatcher *) 0 ;
    PyObject * obj0 = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"O:delete_HIDInterfaceMatcher",&obj0)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_HIDInterfaceMatcher,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    free((char *) arg1);
    
    Py_INCREF(Py_None); resultobj = Py_None;
    return resultobj;
    fail:
    return NULL;
}


static PyObject * HIDInterfaceMatcher_swigregister(PyObject *self UNUSED, PyObject *args) {
    PyObject *obj;
    if (!PyArg_ParseTuple(args,(char*)"O", &obj)) return NULL;
    SWIG_TypeClientData(SWIGTYPE_p_HIDInterfaceMatcher, obj);
    Py_INCREF(obj);
    return Py_BuildValue((char *)"");
}
static PyObject *_wrap_hid_set_debug(PyObject *self UNUSED, PyObject *args) {
    PyObject *resultobj;
    int arg1 ;
    
    if(!PyArg_ParseTuple(args,(char *)"i:hid_set_debug",&arg1)) goto fail;
    hid_set_debug((HIDDebugLevel )arg1);
    
    Py_INCREF(Py_None); resultobj = Py_None;
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_hid_set_debug_stream(PyObject *self UNUSED, PyObject *args) {
    PyObject *resultobj;
    FILE *arg1 = (FILE *) (FILE *)0 ;
    PyObject * obj0 = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"O:hid_set_debug_stream",&obj0)) goto fail;
    {
        if (PyFile_Check(obj0)) {
            arg1 = PyFile_AsFile(obj0);
        } else {
            SWIG_exception(SWIG_TypeError, "file expected");
        }
    }
    hid_set_debug_stream(arg1);
    
    Py_INCREF(Py_None); resultobj = Py_None;
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_hid_set_usb_debug(PyObject *self UNUSED, PyObject *args) {
    PyObject *resultobj;
    int arg1 ;
    
    if(!PyArg_ParseTuple(args,(char *)"i:hid_set_usb_debug",&arg1)) goto fail;
    hid_set_usb_debug(arg1);
    
    Py_INCREF(Py_None); resultobj = Py_None;
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_hid_new_HIDInterface(PyObject *self UNUSED, PyObject *args) {
    PyObject *resultobj;
    HIDInterface *result;
    
    if(!PyArg_ParseTuple(args,(char *)":hid_new_HIDInterface")) goto fail;
    result = (HIDInterface *)hid_new_HIDInterface();
    
    resultobj = SWIG_NewPointerObj((void *) result, SWIGTYPE_p_HIDInterface, 0);
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_hid_delete_HIDInterface(PyObject *self UNUSED, PyObject *args) {
    PyObject *resultobj;
    HIDInterface **arg1 = (HIDInterface **) (HIDInterface **)0 ;
    PyObject * obj0 = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"O:hid_delete_HIDInterface",&obj0)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_p_HIDInterface,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    hid_delete_HIDInterface(arg1);
    
    Py_INCREF(Py_None); resultobj = Py_None;
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_hid_reset_HIDInterface(PyObject *self UNUSED, PyObject *args) {
    PyObject *resultobj;
    HIDInterface *arg1 = (HIDInterface *) (HIDInterface *)0 ;
    PyObject * obj0 = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"O:hid_reset_HIDInterface",&obj0)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_HIDInterface,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    hid_reset_HIDInterface(arg1);
    
    Py_INCREF(Py_None); resultobj = Py_None;
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_hid_init(PyObject *self UNUSED, PyObject *args) {
    PyObject *resultobj;
    int result;
    
    if(!PyArg_ParseTuple(args,(char *)":hid_init")) goto fail;
    result = (int)hid_init();
    
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_hid_cleanup(PyObject *self UNUSED, PyObject *args) {
    PyObject *resultobj;
    int result;
    
    if(!PyArg_ParseTuple(args,(char *)":hid_cleanup")) goto fail;
    result = (int)hid_cleanup();
    
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_hid_is_initialised(PyObject *self UNUSED, PyObject *args) {
    PyObject *resultobj;
    _Bool result;
    
    if(!PyArg_ParseTuple(args,(char *)":hid_is_initialised")) goto fail;
    result = hid_is_initialised();
    
    {
        _Bool * resultptr;
        resultptr = (_Bool *) malloc(sizeof(_Bool));
        memmove(resultptr, &result, sizeof(_Bool));
        resultobj = SWIG_NewPointerObj((void *) resultptr, SWIGTYPE_p__Bool, 1);
    }
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_hid_open(PyObject *self UNUSED, PyObject *args) {
    PyObject *resultobj;
    HIDInterface *arg1 = (HIDInterface *) (HIDInterface *)0 ;
    int arg2 ;
    HIDInterfaceMatcher *arg3 = (HIDInterfaceMatcher *) (HIDInterfaceMatcher *)0 ;
    int result;
    PyObject * obj0 = 0 ;
    PyObject * obj2 = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"OiO:hid_open",&obj0,&arg2,&obj2)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_HIDInterface,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    if ((SWIG_ConvertPtr(obj2,(void **) &arg3, SWIGTYPE_p_HIDInterfaceMatcher,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    result = (int)hid_open(arg1,arg2,(HIDInterfaceMatcher const *)arg3);
    
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_hid_force_open(PyObject *self UNUSED, PyObject *args) {
    PyObject *resultobj;
    HIDInterface *arg1 = (HIDInterface *) (HIDInterface *)0 ;
    int arg2 ;
    HIDInterfaceMatcher *arg3 = (HIDInterfaceMatcher *) (HIDInterfaceMatcher *)0 ;
    unsigned short arg4 ;
    int result;
    PyObject * obj0 = 0 ;
    PyObject * obj2 = 0 ;
    PyObject * obj3 = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"OiOO:hid_force_open",&obj0,&arg2,&obj2,&obj3)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_HIDInterface,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    if ((SWIG_ConvertPtr(obj2,(void **) &arg3, SWIGTYPE_p_HIDInterfaceMatcher,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    arg4 = (unsigned short) PyInt_AsLong(obj3);
    if (PyErr_Occurred()) SWIG_fail;
    result = (int)hid_force_open(arg1,arg2,(HIDInterfaceMatcher const *)arg3,arg4);
    
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_hid_close(PyObject *self UNUSED, PyObject *args) {
    PyObject *resultobj;
    HIDInterface *arg1 = (HIDInterface *) (HIDInterface *)0 ;
    int result;
    PyObject * obj0 = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"O:hid_close",&obj0)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_HIDInterface,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    result = (int)hid_close(arg1);
    
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_hid_is_opened(PyObject *self UNUSED, PyObject *args) {
    PyObject *resultobj;
    HIDInterface *arg1 = (HIDInterface *) (HIDInterface *)0 ;
    _Bool result;
    PyObject * obj0 = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"O:hid_is_opened",&obj0)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_HIDInterface,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    result = hid_is_opened((HIDInterface const *)arg1);
    
    {
        _Bool * resultptr;
        resultptr = (_Bool *) malloc(sizeof(_Bool));
        memmove(resultptr, &result, sizeof(_Bool));
        resultobj = SWIG_NewPointerObj((void *) resultptr, SWIGTYPE_p__Bool, 1);
    }
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_hid_strerror(PyObject *self UNUSED, PyObject *args) {
    PyObject *resultobj;
    int arg1 ;
    char *result;
    
    if(!PyArg_ParseTuple(args,(char *)"i:hid_strerror",&arg1)) goto fail;
    result = (char *)hid_strerror((hid_return )arg1);
    
    resultobj = result ? PyString_FromString(result) : Py_BuildValue((char*)"");
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_hid_get_input_report(PyObject *self UNUSED, PyObject *args) {
    PyObject *resultobj;
    HIDInterface *arg1 = (HIDInterface *) (HIDInterface *)0 ;
    int *arg2 ;
    unsigned int arg3 ;
    char *arg4 ;
    unsigned int arg5 ;
    int result;
    PyObject * obj0 = 0 ;
    PyObject * obj1 = 0 ;
    PyObject * obj3 = 0 ;
    
    {
        arg2 = NULL;
    }
    if(!PyArg_ParseTuple(args,(char *)"OOsO:hid_get_input_report",&obj0,&obj1,&arg4,&obj3)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_HIDInterface,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    {
        int i, size;
        int *temp = NULL;
        
        if (!PySequence_Check(obj1)) {
            PyErr_SetString(PyExc_TypeError,"Expecting a sequence");
            return NULL;
        }
        
        size = PySequence_Size(obj1);
        temp = (int *)calloc(size, sizeof(int));
        
        for (i =0; i < size; i++) {
            PyObject *o = PySequence_GetItem(obj1,i);
            if (PyInt_Check(o)) {
                temp[i] = (int)PyInt_AsLong(o);
            }
            else if (PyLong_Check(o)) {
                temp[i] = (int)PyLong_AsUnsignedLongMask(o);
            }
            else {
                PyErr_SetString(PyExc_ValueError,"Expecting a sequence of integers");
                return NULL;
            }
        }
        
        arg2 = temp;
        arg3 = size;
    }
    arg5 = (unsigned int) PyInt_AsLong(obj3);
    if (PyErr_Occurred()) SWIG_fail;
    result = (int)hid_get_input_report(arg1,(int const (*))arg2,arg3,arg4,arg5);
    
    resultobj = PyInt_FromLong((long)result);
    {
        if(arg2) free((char *) arg2);
    }
    return resultobj;
    fail:
    {
        if(arg2) free((char *) arg2);
    }
    return NULL;
}


static PyObject *_wrap_hid_set_output_report(PyObject *self UNUSED, PyObject *args) {
    PyObject *resultobj;
    HIDInterface *arg1 = (HIDInterface *) (HIDInterface *)0 ;
    int *arg2 ;
    unsigned int arg3 ;
    char *arg4 ;
    unsigned int arg5 ;
    int result;
    PyObject * obj0 = 0 ;
    PyObject * obj1 = 0 ;
    PyObject * obj2 = 0 ;
    
    {
        arg2 = NULL;
    }
    if(!PyArg_ParseTuple(args,(char *)"OOO:hid_set_output_report",&obj0,&obj1,&obj2)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_HIDInterface,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    {
        int i, size;
        int *temp = NULL;
        
        if (!PySequence_Check(obj1)) {
            PyErr_SetString(PyExc_TypeError,"Expecting a sequence");
            return NULL;
        }
        
        size = PySequence_Size(obj1);
        temp = (int *)calloc(size, sizeof(int));
        
        for (i =0; i < size; i++) {
            PyObject *o = PySequence_GetItem(obj1,i);
            if (PyInt_Check(o)) {
                temp[i] = (int)PyInt_AsLong(o);
            }
            else if (PyLong_Check(o)) {
                temp[i] = (int)PyLong_AsUnsignedLongMask(o);
            }
            else {
                PyErr_SetString(PyExc_ValueError,"Expecting a sequence of integers");
                return NULL;
            }
        }
        
        arg2 = temp;
        arg3 = size;
    }
    {
        arg4 = (char *) PyString_AsString(obj2);
        arg5 = (unsigned int) PyString_Size(obj2);
    }
    result = (int)hid_set_output_report(arg1,(int const (*))arg2,arg3,(char const *)arg4,arg5);
    
    resultobj = PyInt_FromLong((long)result);
    {
        if(arg2) free((char *) arg2);
    }
    return resultobj;
    fail:
    {
        if(arg2) free((char *) arg2);
    }
    return NULL;
}


static PyObject *_wrap_hid_get_feature_report(PyObject *self UNUSED, PyObject *args) {
    PyObject *resultobj;
    HIDInterface *arg1 = (HIDInterface *) (HIDInterface *)0 ;
    int *arg2 ;
    unsigned int arg3 ;
    char *arg4 ;
    unsigned int arg5 ;
    int result;
    PyObject * obj0 = 0 ;
    PyObject * obj1 = 0 ;
    PyObject * obj3 = 0 ;
    
    {
        arg2 = NULL;
    }
    if(!PyArg_ParseTuple(args,(char *)"OOsO:hid_get_feature_report",&obj0,&obj1,&arg4,&obj3)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_HIDInterface,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    {
        int i, size;
        int *temp = NULL;
        
        if (!PySequence_Check(obj1)) {
            PyErr_SetString(PyExc_TypeError,"Expecting a sequence");
            return NULL;
        }
        
        size = PySequence_Size(obj1);
        temp = (int *)calloc(size, sizeof(int));
        
        for (i =0; i < size; i++) {
            PyObject *o = PySequence_GetItem(obj1,i);
            if (PyInt_Check(o)) {
                temp[i] = (int)PyInt_AsLong(o);
            }
            else if (PyLong_Check(o)) {
                temp[i] = (int)PyLong_AsUnsignedLongMask(o);
            }
            else {
                PyErr_SetString(PyExc_ValueError,"Expecting a sequence of integers");
                return NULL;
            }
        }
        
        arg2 = temp;
        arg3 = size;
    }
    arg5 = (unsigned int) PyInt_AsLong(obj3);
    if (PyErr_Occurred()) SWIG_fail;
    result = (int)hid_get_feature_report(arg1,(int const (*))arg2,arg3,arg4,arg5);
    
    resultobj = PyInt_FromLong((long)result);
    {
        if(arg2) free((char *) arg2);
    }
    return resultobj;
    fail:
    {
        if(arg2) free((char *) arg2);
    }
    return NULL;
}


static PyObject *_wrap_hid_set_feature_report(PyObject *self UNUSED, PyObject *args) {
    PyObject *resultobj;
    HIDInterface *arg1 = (HIDInterface *) (HIDInterface *)0 ;
    int *arg2 ;
    unsigned int arg3 ;
    char *arg4 ;
    unsigned int arg5 ;
    int result;
    PyObject * obj0 = 0 ;
    PyObject * obj1 = 0 ;
    PyObject * obj2 = 0 ;
    
    {
        arg2 = NULL;
    }
    if(!PyArg_ParseTuple(args,(char *)"OOO:hid_set_feature_report",&obj0,&obj1,&obj2)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_HIDInterface,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    {
        int i, size;
        int *temp = NULL;
        
        if (!PySequence_Check(obj1)) {
            PyErr_SetString(PyExc_TypeError,"Expecting a sequence");
            return NULL;
        }
        
        size = PySequence_Size(obj1);
        temp = (int *)calloc(size, sizeof(int));
        
        for (i =0; i < size; i++) {
            PyObject *o = PySequence_GetItem(obj1,i);
            if (PyInt_Check(o)) {
                temp[i] = (int)PyInt_AsLong(o);
            }
            else if (PyLong_Check(o)) {
                temp[i] = (int)PyLong_AsUnsignedLongMask(o);
            }
            else {
                PyErr_SetString(PyExc_ValueError,"Expecting a sequence of integers");
                return NULL;
            }
        }
        
        arg2 = temp;
        arg3 = size;
    }
    {
        arg4 = (char *) PyString_AsString(obj2);
        arg5 = (unsigned int) PyString_Size(obj2);
    }
    result = (int)hid_set_feature_report(arg1,(int const (*))arg2,arg3,(char const *)arg4,arg5);
    
    resultobj = PyInt_FromLong((long)result);
    {
        if(arg2) free((char *) arg2);
    }
    return resultobj;
    fail:
    {
        if(arg2) free((char *) arg2);
    }
    return NULL;
}


static PyObject *_wrap_hid_get_item_value(PyObject *self UNUSED, PyObject *args) {
    PyObject *resultobj;
    HIDInterface *arg1 = (HIDInterface *) (HIDInterface *)0 ;
    int *arg2 ;
    unsigned int arg3 ;
    double *arg4 = (double *) (double *)0 ;
    int result;
    PyObject * obj0 = 0 ;
    PyObject * obj1 = 0 ;
    PyObject * obj2 = 0 ;
    
    {
        arg2 = NULL;
    }
    if(!PyArg_ParseTuple(args,(char *)"OOO:hid_get_item_value",&obj0,&obj1,&obj2)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_HIDInterface,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    {
        int i, size;
        int *temp = NULL;
        
        if (!PySequence_Check(obj1)) {
            PyErr_SetString(PyExc_TypeError,"Expecting a sequence");
            return NULL;
        }
        
        size = PySequence_Size(obj1);
        temp = (int *)calloc(size, sizeof(int));
        
        for (i =0; i < size; i++) {
            PyObject *o = PySequence_GetItem(obj1,i);
            if (PyInt_Check(o)) {
                temp[i] = (int)PyInt_AsLong(o);
            }
            else if (PyLong_Check(o)) {
                temp[i] = (int)PyLong_AsUnsignedLongMask(o);
            }
            else {
                PyErr_SetString(PyExc_ValueError,"Expecting a sequence of integers");
                return NULL;
            }
        }
        
        arg2 = temp;
        arg3 = size;
    }
    if ((SWIG_ConvertPtr(obj2,(void **) &arg4, SWIGTYPE_p_double,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    result = (int)hid_get_item_value(arg1,(int const (*))arg2,arg3,arg4);
    
    resultobj = PyInt_FromLong((long)result);
    {
        if(arg2) free((char *) arg2);
    }
    return resultobj;
    fail:
    {
        if(arg2) free((char *) arg2);
    }
    return NULL;
}


static PyObject *_wrap_hid_write_identification(PyObject *self UNUSED, PyObject *args) {
    PyObject *resultobj;
    FILE *arg1 = (FILE *) (FILE *)0 ;
    HIDInterface *arg2 = (HIDInterface *) (HIDInterface *)0 ;
    int result;
    PyObject * obj0 = 0 ;
    PyObject * obj1 = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"OO:hid_write_identification",&obj0,&obj1)) goto fail;
    {
        if (PyFile_Check(obj0)) {
            arg1 = PyFile_AsFile(obj0);
        } else {
            SWIG_exception(SWIG_TypeError, "file expected");
        }
    }
    if ((SWIG_ConvertPtr(obj1,(void **) &arg2, SWIGTYPE_p_HIDInterface,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    result = (int)hid_write_identification(arg1,(HIDInterface const *)arg2);
    
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_hid_dump_tree(PyObject *self UNUSED, PyObject *args) {
    PyObject *resultobj;
    FILE *arg1 = (FILE *) (FILE *)0 ;
    HIDInterface *arg2 = (HIDInterface *) (HIDInterface *)0 ;
    int result;
    PyObject * obj0 = 0 ;
    PyObject * obj1 = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"OO:hid_dump_tree",&obj0,&obj1)) goto fail;
    {
        if (PyFile_Check(obj0)) {
            arg1 = PyFile_AsFile(obj0);
        } else {
            SWIG_exception(SWIG_TypeError, "file expected");
        }
    }
    if ((SWIG_ConvertPtr(obj1,(void **) &arg2, SWIGTYPE_p_HIDInterface,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    result = (int)hid_dump_tree(arg1,arg2);
    
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_hid_interrupt_read(PyObject *self UNUSED, PyObject *args) {
    PyObject *resultobj;
    HIDInterface *arg1 = (HIDInterface *) (HIDInterface *)0 ;
    unsigned int arg2 ;
    char *arg3 ;
    unsigned int arg4 ;
    unsigned int arg5 ;
    int result;
    PyObject * obj0 = 0 ;
    PyObject * obj1 = 0 ;
    PyObject * obj3 = 0 ;
    PyObject * obj4 = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"OOsOO:hid_interrupt_read",&obj0,&obj1,&arg3,&obj3,&obj4)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_HIDInterface,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    arg2 = (unsigned int) PyInt_AsLong(obj1);
    if (PyErr_Occurred()) SWIG_fail;
    arg4 = (unsigned int) PyInt_AsLong(obj3);
    if (PyErr_Occurred()) SWIG_fail;
    arg5 = (unsigned int) PyInt_AsLong(obj4);
    if (PyErr_Occurred()) SWIG_fail;
    result = (int)hid_interrupt_read(arg1,arg2,arg3,arg4,arg5);
    
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_hid_interrupt_write(PyObject *self UNUSED, PyObject *args) {
    PyObject *resultobj;
    HIDInterface *arg1 = (HIDInterface *) (HIDInterface *)0 ;
    unsigned int arg2 ;
    char *arg3 ;
    unsigned int arg4 ;
    unsigned int arg5 ;
    int result;
    PyObject * obj0 = 0 ;
    PyObject * obj1 = 0 ;
    PyObject * obj2 = 0 ;
    PyObject * obj3 = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"OOOO:hid_interrupt_write",&obj0,&obj1,&obj2,&obj3)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_HIDInterface,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    arg2 = (unsigned int) PyInt_AsLong(obj1);
    if (PyErr_Occurred()) SWIG_fail;
    {
        arg3 = (char *) PyString_AsString(obj2);
        arg4 = (unsigned int) PyString_Size(obj2);
    }
    arg5 = (unsigned int) PyInt_AsLong(obj3);
    if (PyErr_Occurred()) SWIG_fail;
    result = (int)hid_interrupt_write(arg1,arg2,(char const *)arg3,arg4,arg5);
    
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
    fail:
    return NULL;
}


static PyObject *_wrap_hid_set_idle(PyObject *self UNUSED, PyObject *args) {
    PyObject *resultobj;
    HIDInterface *arg1 = (HIDInterface *) (HIDInterface *)0 ;
    unsigned int arg2 ;
    unsigned int arg3 ;
    int result;
    PyObject * obj0 = 0 ;
    PyObject * obj1 = 0 ;
    PyObject * obj2 = 0 ;
    
    if(!PyArg_ParseTuple(args,(char *)"OOO:hid_set_idle",&obj0,&obj1,&obj2)) goto fail;
    if ((SWIG_ConvertPtr(obj0,(void **) &arg1, SWIGTYPE_p_HIDInterface,SWIG_POINTER_EXCEPTION | 0 )) == -1) SWIG_fail;
    arg2 = (unsigned int) PyInt_AsLong(obj1);
    if (PyErr_Occurred()) SWIG_fail;
    arg3 = (unsigned int) PyInt_AsLong(obj2);
    if (PyErr_Occurred()) SWIG_fail;
    result = (int)hid_set_idle(arg1,arg2,arg3);
    
    resultobj = PyInt_FromLong((long)result);
    return resultobj;
    fail:
    return NULL;
}


static PyMethodDef SwigMethods[] = {
	 { (char *)"wrap_hid_interrupt_read", _wrap_wrap_hid_interrupt_read, METH_VARARGS },
	 { (char *)"wrap_hid_get_input_report", _wrap_wrap_hid_get_input_report, METH_VARARGS },
	 { (char *)"wrap_hid_get_feature_report", _wrap_wrap_hid_get_feature_report, METH_VARARGS },
	 { (char *)"HIDInterface_device_get", _wrap_HIDInterface_device_get, METH_VARARGS },
	 { (char *)"HIDInterface_interface_get", _wrap_HIDInterface_interface_get, METH_VARARGS },
	 { (char *)"HIDInterface_id_get", _wrap_HIDInterface_id_get, METH_VARARGS },
	 { (char *)"new_HIDInterface", _wrap_new_HIDInterface, METH_VARARGS },
	 { (char *)"delete_HIDInterface", _wrap_delete_HIDInterface, METH_VARARGS },
	 { (char *)"HIDInterface_swigregister", HIDInterface_swigregister, METH_VARARGS },
	 { (char *)"HIDInterfaceMatcher_vendor_id_set", _wrap_HIDInterfaceMatcher_vendor_id_set, METH_VARARGS },
	 { (char *)"HIDInterfaceMatcher_vendor_id_get", _wrap_HIDInterfaceMatcher_vendor_id_get, METH_VARARGS },
	 { (char *)"HIDInterfaceMatcher_product_id_set", _wrap_HIDInterfaceMatcher_product_id_set, METH_VARARGS },
	 { (char *)"HIDInterfaceMatcher_product_id_get", _wrap_HIDInterfaceMatcher_product_id_get, METH_VARARGS },
	 { (char *)"new_HIDInterfaceMatcher", _wrap_new_HIDInterfaceMatcher, METH_VARARGS },
	 { (char *)"delete_HIDInterfaceMatcher", _wrap_delete_HIDInterfaceMatcher, METH_VARARGS },
	 { (char *)"HIDInterfaceMatcher_swigregister", HIDInterfaceMatcher_swigregister, METH_VARARGS },
	 { (char *)"hid_set_debug", _wrap_hid_set_debug, METH_VARARGS },
	 { (char *)"hid_set_debug_stream", _wrap_hid_set_debug_stream, METH_VARARGS },
	 { (char *)"hid_set_usb_debug", _wrap_hid_set_usb_debug, METH_VARARGS },
	 { (char *)"hid_new_HIDInterface", _wrap_hid_new_HIDInterface, METH_VARARGS },
	 { (char *)"hid_delete_HIDInterface", _wrap_hid_delete_HIDInterface, METH_VARARGS },
	 { (char *)"hid_reset_HIDInterface", _wrap_hid_reset_HIDInterface, METH_VARARGS },
	 { (char *)"hid_init", _wrap_hid_init, METH_VARARGS },
	 { (char *)"hid_cleanup", _wrap_hid_cleanup, METH_VARARGS },
	 { (char *)"hid_is_initialised", _wrap_hid_is_initialised, METH_VARARGS },
	 { (char *)"hid_open", _wrap_hid_open, METH_VARARGS },
	 { (char *)"hid_force_open", _wrap_hid_force_open, METH_VARARGS },
	 { (char *)"hid_close", _wrap_hid_close, METH_VARARGS },
	 { (char *)"hid_is_opened", _wrap_hid_is_opened, METH_VARARGS },
	 { (char *)"hid_strerror", _wrap_hid_strerror, METH_VARARGS },
	 { (char *)"hid_get_input_report", _wrap_hid_get_input_report, METH_VARARGS },
	 { (char *)"hid_set_output_report", _wrap_hid_set_output_report, METH_VARARGS },
	 { (char *)"hid_get_feature_report", _wrap_hid_get_feature_report, METH_VARARGS },
	 { (char *)"hid_set_feature_report", _wrap_hid_set_feature_report, METH_VARARGS },
	 { (char *)"hid_get_item_value", _wrap_hid_get_item_value, METH_VARARGS },
	 { (char *)"hid_write_identification", _wrap_hid_write_identification, METH_VARARGS },
	 { (char *)"hid_dump_tree", _wrap_hid_dump_tree, METH_VARARGS },
	 { (char *)"hid_interrupt_read", _wrap_hid_interrupt_read, METH_VARARGS },
	 { (char *)"hid_interrupt_write", _wrap_hid_interrupt_write, METH_VARARGS },
	 { (char *)"hid_set_idle", _wrap_hid_set_idle, METH_VARARGS },
	 { NULL, NULL }
};


/* -------- TYPE CONVERSION AND EQUIVALENCE RULES (BEGIN) -------- */

static swig_type_info _swigt__p_HIDInterface[] = {{"_p_HIDInterface", 0, "HIDInterface *const", 0},{"_p_HIDInterface"},{0}};
static swig_type_info _swigt__p_FILE[] = {{"_p_FILE", 0, "FILE *", 0},{"_p_FILE"},{0}};
static swig_type_info _swigt__p_p_HIDInterface[] = {{"_p_p_HIDInterface", 0, "HIDInterface **const", 0},{"_p_p_HIDInterface"},{0}};
static swig_type_info _swigt__p__Bool[] = {{"_p__Bool", 0, "_Bool *", 0},{"_p__Bool"},{0}};
static swig_type_info _swigt__p_HIDInterfaceMatcher[] = {{"_p_HIDInterfaceMatcher", 0, "HIDInterfaceMatcher const *const", 0},{"_p_HIDInterfaceMatcher"},{0}};
static swig_type_info _swigt__p_double[] = {{"_p_double", 0, "double *", 0},{"_p_double"},{0}};
static swig_type_info _swigt__p_unsigned_int[] = {{"_p_unsigned_int", 0, "unsigned int *", 0},{"_p_unsigned_int"},{0}};
static swig_type_info _swigt__p_usb_device[] = {{"_p_usb_device", 0, "struct usb_device *", 0},{"_p_usb_device"},{0}};
static swig_type_info _swigt__p_int[] = {{"_p_int", 0, "int *", 0},{"_p_int"},{0}};

static swig_type_info *swig_types_initial[] = {
_swigt__p_HIDInterface, 
_swigt__p_FILE, 
_swigt__p_p_HIDInterface, 
_swigt__p__Bool, 
_swigt__p_HIDInterfaceMatcher, 
_swigt__p_double, 
_swigt__p_unsigned_int, 
_swigt__p_usb_device, 
_swigt__p_int, 
0
};


/* -------- TYPE CONVERSION AND EQUIVALENCE RULES (END) -------- */

static swig_const_info swig_const_table[] = {
{ SWIG_PY_INT,     (char *)"true", (long) 1, 0, 0, 0},
{ SWIG_PY_INT,     (char *)"false", (long) 0, 0, 0, 0},
{ SWIG_PY_INT,     (char *)"HID_RET_SUCCESS", (long) HID_RET_SUCCESS, 0, 0, 0},
{ SWIG_PY_INT,     (char *)"HID_RET_INVALID_PARAMETER", (long) HID_RET_INVALID_PARAMETER, 0, 0, 0},
{ SWIG_PY_INT,     (char *)"HID_RET_NOT_INITIALISED", (long) HID_RET_NOT_INITIALISED, 0, 0, 0},
{ SWIG_PY_INT,     (char *)"HID_RET_ALREADY_INITIALISED", (long) HID_RET_ALREADY_INITIALISED, 0, 0, 0},
{ SWIG_PY_INT,     (char *)"HID_RET_FAIL_FIND_BUSSES", (long) HID_RET_FAIL_FIND_BUSSES, 0, 0, 0},
{ SWIG_PY_INT,     (char *)"HID_RET_FAIL_FIND_DEVICES", (long) HID_RET_FAIL_FIND_DEVICES, 0, 0, 0},
{ SWIG_PY_INT,     (char *)"HID_RET_FAIL_OPEN_DEVICE", (long) HID_RET_FAIL_OPEN_DEVICE, 0, 0, 0},
{ SWIG_PY_INT,     (char *)"HID_RET_DEVICE_NOT_FOUND", (long) HID_RET_DEVICE_NOT_FOUND, 0, 0, 0},
{ SWIG_PY_INT,     (char *)"HID_RET_DEVICE_NOT_OPENED", (long) HID_RET_DEVICE_NOT_OPENED, 0, 0, 0},
{ SWIG_PY_INT,     (char *)"HID_RET_DEVICE_ALREADY_OPENED", (long) HID_RET_DEVICE_ALREADY_OPENED, 0, 0, 0},
{ SWIG_PY_INT,     (char *)"HID_RET_FAIL_CLOSE_DEVICE", (long) HID_RET_FAIL_CLOSE_DEVICE, 0, 0, 0},
{ SWIG_PY_INT,     (char *)"HID_RET_FAIL_CLAIM_IFACE", (long) HID_RET_FAIL_CLAIM_IFACE, 0, 0, 0},
{ SWIG_PY_INT,     (char *)"HID_RET_FAIL_DETACH_DRIVER", (long) HID_RET_FAIL_DETACH_DRIVER, 0, 0, 0},
{ SWIG_PY_INT,     (char *)"HID_RET_NOT_HID_DEVICE", (long) HID_RET_NOT_HID_DEVICE, 0, 0, 0},
{ SWIG_PY_INT,     (char *)"HID_RET_HID_DESC_SHORT", (long) HID_RET_HID_DESC_SHORT, 0, 0, 0},
{ SWIG_PY_INT,     (char *)"HID_RET_REPORT_DESC_SHORT", (long) HID_RET_REPORT_DESC_SHORT, 0, 0, 0},
{ SWIG_PY_INT,     (char *)"HID_RET_REPORT_DESC_LONG", (long) HID_RET_REPORT_DESC_LONG, 0, 0, 0},
{ SWIG_PY_INT,     (char *)"HID_RET_FAIL_ALLOC", (long) HID_RET_FAIL_ALLOC, 0, 0, 0},
{ SWIG_PY_INT,     (char *)"HID_RET_OUT_OF_SPACE", (long) HID_RET_OUT_OF_SPACE, 0, 0, 0},
{ SWIG_PY_INT,     (char *)"HID_RET_FAIL_SET_REPORT", (long) HID_RET_FAIL_SET_REPORT, 0, 0, 0},
{ SWIG_PY_INT,     (char *)"HID_RET_FAIL_GET_REPORT", (long) HID_RET_FAIL_GET_REPORT, 0, 0, 0},
{ SWIG_PY_INT,     (char *)"HID_RET_FAIL_INT_READ", (long) HID_RET_FAIL_INT_READ, 0, 0, 0},
{ SWIG_PY_INT,     (char *)"HID_RET_NOT_FOUND", (long) HID_RET_NOT_FOUND, 0, 0, 0},
{ SWIG_PY_INT,     (char *)"HID_RET_TIMEOUT", (long) HID_RET_TIMEOUT, 0, 0, 0},
{ SWIG_PY_INT,     (char *)"HID_ID_MATCH_ANY", (long) 0x0000, 0, 0, 0},
{ SWIG_PY_INT,     (char *)"HID_DEBUG_NONE", (long) HID_DEBUG_NONE, 0, 0, 0},
{ SWIG_PY_INT,     (char *)"HID_DEBUG_ERRORS", (long) HID_DEBUG_ERRORS, 0, 0, 0},
{ SWIG_PY_INT,     (char *)"HID_DEBUG_WARNINGS", (long) HID_DEBUG_WARNINGS, 0, 0, 0},
{ SWIG_PY_INT,     (char *)"HID_DEBUG_NOTICES", (long) HID_DEBUG_NOTICES, 0, 0, 0},
{ SWIG_PY_INT,     (char *)"HID_DEBUG_TRACES", (long) HID_DEBUG_TRACES, 0, 0, 0},
{ SWIG_PY_INT,     (char *)"HID_DEBUG_ASSERTS", (long) HID_DEBUG_ASSERTS, 0, 0, 0},
{ SWIG_PY_INT,     (char *)"HID_DEBUG_NOTRACES", (long) HID_DEBUG_NOTRACES, 0, 0, 0},
{ SWIG_PY_INT,     (char *)"HID_DEBUG_ALL", (long) HID_DEBUG_ALL, 0, 0, 0},
{0}};

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C"
#endif
SWIGEXPORT(void) SWIG_init(void) {
    static PyObject *SWIG_globals = 0; 
    static int       typeinit = 0;
    PyObject *m, *d;
    int       i;
    if (!SWIG_globals) SWIG_globals = SWIG_newvarlink();
    m = Py_InitModule((char *) SWIG_name, SwigMethods);
    d = PyModule_GetDict(m);
    
    if (!typeinit) {
        for (i = 0; swig_types_initial[i]; i++) {
            swig_types[i] = SWIG_TypeRegister(swig_types_initial[i]);
        }
        typeinit = 1;
    }
    SWIG_InstallConstants(d,swig_const_table);
    
}

