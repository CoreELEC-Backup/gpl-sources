#ifndef TOOLBOX_TOOLS_H
#define TOOLBOX_TOOLS_H

//#include <stdio.h>
//#include <iostream>
#include <sys/types.h>

//#define KILOBYTE(x) ((x)*1024)
//#define MEGABYTE(x) (KILOBYTE(x)*1024)

//typedef unsigned int  uint;
//typedef unsigned long ulong;
typedef unsigned char uchar;
//typedef unsigned short ushort;

// Special constructor for CreateElements
void *operator new(size_t, void*) throw ();

#ifdef TOOLBOX_DEBUG
#	define ASSERT(x) if ((x)) cerr << "Warning: ASSERT failed At " << __FILE__ << ":" << __LINE__ << " ["#x"]" << endl
#	define CHECK_PTR(x) if (!(x)) cerr << "Warning: Pointer is NULL At " << __FILE__ << ":" << __LINE__ << endl;
#	define CHECK_NEXT_ALLOC() _checkNextAlloc()
#	define DPRINT(x...) LOGi(x)
#else
#	define ASSERT(x)
#	define CHECK_PTR(x)
#	define CHECK_NEXT_ALLOC()
#	define DPRINT(x...)
#endif

#define ERRNUL(e) {errno=e;return 0;}
#define ERRSYS(e) {errno=e;return -1;}

/* RETURNS() and RETURN() are macros that can be used if a class object is
   being returned. They make use of the GNU C-Compiler's named return value
   feature, if available. In this case, the class object isn't returned and 
   copied, but the result itself is filled. 

   RETURNS(ReturnType, FunctionDeclaration, Result)
     ... function-body working on Result ...
   RETURN(Result)
   
   A function like this (cXYZ is a class type):
   
   cXYZ myfunction(int a, char *b) {
     cXYZ result;
     ... something happens with result ...
     return result;
   }

   can be written like this:

   RETURNS(cXYZ, myfunction(int a, char *b), result)
     ... something happens with result ...
   RETURN(result)

	 DISABLED SINCE GCC 3.x
*/
//#ifdef __GNUC__
//#	define RETURNS(t,x,r) t x return r {
//#	define RETURN(x) }
//#else
#	define RETURNS(t,x,r) t x { t r;
#	define RETURN(x) return x; }
//#endif

#endif // TOOLBOX_TOOLS_H
