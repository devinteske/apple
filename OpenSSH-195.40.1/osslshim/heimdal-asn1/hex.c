/*
 * Copyright (c) 2011-12 Apple Inc. All Rights Reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 *
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 *
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 *
 * @APPLE_LICENSE_HEADER_END@
 */
/*
 * Copyright (c) 2004-2005 Kungliga Tekniska Högskolan
 * (Royal Institute of Technology, Stockholm, Sweden).
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */


#include "ossl-config.h"

/* #include "rk-roken.h" */

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "hex.h"

static const char hexchar[16] = "0123456789ABCDEF";

static int
pos(char c)
{
    const char *p;
    c = toupper((unsigned char)c);
    for (p = hexchar; *p; p++)
	if (*p == c)
	    return p - hexchar;
    return -1;
}

ROKEN_LIB_FUNCTION ssize_t ROKEN_LIB_CALL
hex_encode(const void *data, size_t size, char **str)
{
    const unsigned char *q = data;
    size_t i;
    char *p;

    /* check for overflow */
    if (size * 2 < size) {
        *str = NULL;
	return -1;
    }

    p = malloc(size * 2 + 1);
    if (p == NULL) {
        *str = NULL;
	return -1;
    }

    for (i = 0; i < size; i++) {
	p[i * 2] = hexchar[(*q >> 4) & 0xf];
	p[i * 2 + 1] = hexchar[*q & 0xf];
	q++;
    }
    p[i * 2] = '\0';
    *str = p;

    return i * 2;
}

ROKEN_LIB_FUNCTION ssize_t ROKEN_LIB_CALL
hex_decode(const char *str, void *data, size_t len)
{
    size_t l;
    unsigned char *p = data;
    size_t i;

    l = strlen(str);

    /* check for overflow, same as (l+1)/2 but overflow safe */
    if ((l/2) + (l&1) > len)
	return -1;

    if (l & 1) {
	p[0] = pos(str[0]);
	str++;
	p++;
    }
    for (i = 0; i < l / 2; i++)
	p[i] = pos(str[i * 2]) << 4 | pos(str[(i * 2) + 1]);
    return i + (l & 1);
}
