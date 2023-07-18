//
// convert a 16-bit value in millivolts (0-9999)
// to a string as "d.ddd"
// avoid sprintf() etc
//
// s points to a 5-character buffer

#include "format_mv.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char tmp[6];		/* temporary buffer */

void format_mv( char *s, uint16_t v) {
  memset( s, '0', 5);
  s[1] = '.';
  s[5] = '\0';
  utoa( v, tmp, 10);		/* initial conversion, results in string from 1-4 chars long */
  int n = strlen( tmp);		/* got length 1-4 */
  if( n > 4)
    return;
  if( n < 4) {			/* only mV */
    memcpy( s+5-n, tmp, n);
  } else {
    memcpy( s+2, tmp+1, n);
    s[0] = tmp[0];
  }
}
