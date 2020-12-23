#include "syshdrs.h"
#ifdef PRAGMA_HDRSTOP
#	pragma hdrstop
#endif

/*
 * Copy src to dst, truncating.
 * Return dst.
 */
char *
Strncpy_NoZeroPad(char *const dst, const char *const src, const size_t n)
{
	register char *d;
	register const char *s;
	register size_t i;

	d = dst;
	*d = 0;
	if (n != 0) {
		s = src;
		/* If they specified a maximum of n characters, use n - 1 chars to
		 * hold the copy, and the last character in the array as a NUL.
		 * This is the difference between the regular strncpy routine.
		 * strncpy doesn't guarantee that your new string will have a
		 * NUL terminator, but this routine does.
		 */
		for (i=1; i<n; i++) {
			if ((*d++ = *s++) == 0) {
				return dst;
			}
		}
		/* If we get here, then we have a full string, with n - 1 characters,
		 * so now we NUL terminate it and go home.
		 */
		*d = 0;
	}
	return (dst);
}	/* Strncpy_NoZeroPad */
