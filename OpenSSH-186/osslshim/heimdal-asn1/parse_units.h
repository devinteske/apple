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
 * Copyright (c) 1997 - 2001 Kungliga Tekniska Högskolan
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

/* $Id$ */

#ifndef __RK_PARSE_UNITS_H__
#define __RK_PARSE_UNITS_H__

#include <stdio.h>
#include <stddef.h>

/* symbol rewrite */
#define	parse_units		ossl_parse_units
#define print_units_table	ossl_print_units_table
#define	parse_flags		ossl_parse_flags
#define	unparse_units		ossl_unparse_units
#define	unparse_units_approx	ossl_unparse_units_approx
#define	unparse_flags		ossl_unparse_flags
#define	print_flags_table	ossl_print_flags_table

#ifndef ROKEN_LIB_FUNCTION
#ifdef _WIN32
#define ROKEN_LIB_FUNCTION
#define ROKEN_LIB_CALL     __cdecl
#else
#define ROKEN_LIB_FUNCTION
#define ROKEN_LIB_CALL
#endif
#endif

struct units {
    const char *name;
    unsigned mult;
};

ROKEN_LIB_FUNCTION int ROKEN_LIB_CALL
parse_units (const char *s, const struct units *units,
	     const char *def_unit);

ROKEN_LIB_FUNCTION void ROKEN_LIB_CALL
print_units_table (const struct units *units, FILE *f);

ROKEN_LIB_FUNCTION int ROKEN_LIB_CALL
parse_flags (const char *s, const struct units *units,
	     int orig);

ROKEN_LIB_FUNCTION int ROKEN_LIB_CALL
unparse_units (int num, const struct units *units, char *s, size_t len);

ROKEN_LIB_FUNCTION int ROKEN_LIB_CALL
unparse_units_approx (int num, const struct units *units, char *s,
		      size_t len);

ROKEN_LIB_FUNCTION int ROKEN_LIB_CALL
unparse_flags (int num, const struct units *units, char *s, size_t len);

ROKEN_LIB_FUNCTION void ROKEN_LIB_CALL
print_flags_table (const struct units *units, FILE *f);

#endif /* __RK_PARSE_UNITS_H__ */
