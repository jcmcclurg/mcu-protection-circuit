/*
 * itoa.c
 *
 *  Created on: Sep 29, 2012
 *      Original Author: mdst on Nov 14, 2008 10:29 PM
 *      URL: http://e2e.ti.com/support/microcontrollers/stellaris_arm_cortex-m3_microcontroller/f/473/t/44708.aspx
 *      Modified: jcmcclu2 on Sep 29, 2012 (added support for arbitrary base [up to base 36])
 */

#include <string.h>

char* num_chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

/* reverse:  reverse string s in place */
void reverse(char s[])
{
    int c, i, j;

    for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

/* itoa:  convert n to characters in s */
void itoa(int n, char s[], int base)
{
    int i, sign;

    if ((sign = n) < 0)  /* record sign */
        n = -n;          /* make n positive */
    i = 0;
    do {       /* generate digits in reverse order */
        s[i++] = *(num_chars + n % base);   /* get next digit */
    } while ((n /= base) > 0);     /* delete it */
    if (sign < 0)
        s[i++] = '-';
    s[i] = 0;
    reverse(s);
}
