%module(docstring="libhid is a user-space USB HID access library built on libusb.", "threads"=1) hid 
%{
#include <compiler.h>
#include <hid.h>
%}

%feature("autodoc","0");

%include "exception.i"

%typemap(in) FILE* {
  if (PyFile_Check($input)) {
      $1 = PyFile_AsFile($input);
  } else {
      SWIG_exception(SWIG_TypeError, "file expected");
  }
}

// Convert tuples or lists to paths (and depth)
// Ref: http://www.swig.org/Doc1.3/Python.html
%typemap(in) (int const path[], unsigned int const depth) {
  int i, size;
  int *temp = NULL;

  if (!PySequence_Check($input)) {
    PyErr_SetString(PyExc_TypeError,"Expecting a sequence");
    return NULL;
  }

  size = PySequence_Size($input);
  temp = (int *)calloc(size, sizeof(int));

  for (i =0; i < size; i++) {
    PyObject *o = PySequence_GetItem($input,i);
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

  $1 = temp;
  $2 = size;
}
// Ref: http://www.swig.org/Doc1.3/Typemaps.html#Typemaps_nn33
%typemap(freearg) (int const path[], unsigned int const depth) {
  if($1) free((char *) $1);
}
// Set argument to NULL before any conversion occurs (apparently we have an
// ordering issue where certain failure cases can result in free()ing memory
// before it has been allocated)
%typemap(arginit) (int const path[], unsigned int const depth) {
   $1 = NULL;
}

%include "cstring.i"

// hid_interrupt_write()
%apply (char *STRING, int LENGTH) { (const char* const bytes, unsigned int const size) }

// hid_set_output_report(), etc.
%apply (char *STRING, int LENGTH) { (const char* const buffer, unsigned int const size) }

// hid_get_item_value()
%apply double *OUTPUT { double *const value };

// hid_interrupt_read()
%cstring_output_withsize(char* const bytes_out, unsigned int* const size_out);
%inline %{
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
%}  

// hid_get_input_report()
%inline %{
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
%}  

// hid_get_feature_report()
%inline %{
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
%}  


// HIDInterface:
%ignore dev_handle;	// Internal to libhid
%immutable device;	// provided for identification purposes
%immutable interface;
%ignore hid_data;	// Nothing to see here...
%ignore hid_parser;	// (The HID parser API is hidden)

%immutable id;
%typemap(out) char id[32] {
    $result = PyString_FromString($1);
}

%feature("autodoc","hid_interrupt_read(hidif, ep, size, timeout) -> hid_return,bytes") hid_interrupt_read;
%feature("autodoc","hid_get_input_report(hidif, path, size) -> hid_return,bytes") hid_get_input_report;
%feature("autodoc","hid_get_feature_report(hidif, path, size) -> hid_return,bytes") hid_get_feature_report;
%include "hid.h"

%pythoncode %{
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
%}

/* COPYRIGHT --
 *
 * This file is part of libhid, a user-space HID access library.
 * libhid is (c) 2003-2007
 *   Martin F. Krafft <libhid@pobox.madduck.net>
 *   Charles Lepple <clepple+libhid@ghz.cc>
 *   Arnaud Quette <arnaud.quette@free.fr> && <arnaud.quette@mgeups.com>
 * and distributed under the terms of the GNU General Public License.
 * See the file ./COPYING in the source distribution for more information.
 *
 * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES
 * OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */
