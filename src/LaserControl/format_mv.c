//
// format_mv()
//   convert a 16-bit value in millivolts (0-9999)
//   to a string as "d.ddd"
//   avoid sprintf() etc
//   
//   s points to a 5-character buffer
//
// format_degc()
//   convert a 16-bit value in millivolts (0-9999)
//   to a string as "ddd.dC" using the formula for a MCP9700 sensor
//   degC = (mv - 500) / 10
//

#include "format_mv.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char tmp[8];		/* temporary buffer */

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


void format_degc( char *s, uint16_t v) {
  // expect temps in the range 0-200C
  // so voltage in the range 500mV - 2500mV
  memset( s, ' ', 5);
  s[3] = '.';
  s[5] = '\0';
  if( v < 500 || v > 2500)
    return;
  // now we have a value in tenth degrees (0-1000); format as 000.0 to 100.0
  utoa( v - 500, tmp, 10);	/* initial conversion, results in string from 1-4 chars long */
  int n = strlen( tmp);		/* length better be 1-4 */
  s[4] = tmp[n-1];		/* copy tenths */
  if( n == 1)
    return;			/* no whole degrees */
  memcpy( s+4-n, tmp, n-1);
}
