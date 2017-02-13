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
 * Copyright (c) 1997 - 2000, 2005 Kungliga Tekniska Högskolan
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

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#ifdef HAVE_TERMIOS_H
#include <termios.h>
#endif

#include "ossl-ui.h"

#ifdef HAVE_CONIO_H
#include <conio.h>
#endif

static sig_atomic_t intr_flag;

static void
intr(int sig)
{
	intr_flag++;
}


#ifdef HAVE_CONIO_H

/*
 * Windows does console slightly different then then unix case.
 */
static int
read_string(const char *preprompt, const char *prompt,
    char *buf, size_t len, int echo)
{
	int of = 0;
	int c;
	char *p;

	void (*oldsigintr)(int);

	_cprintf("%s%s", preprompt, prompt);

	oldsigintr = signal(SIGINT, intr);

	p = buf;
	while (intr_flag == 0) {
		c = ((echo) ? _getche() : _getch());
		if ((c == '\n') || (c == '\r')) {
			break;
		}
		if (of == 0) {
			*p++ = c;
		}
		of = (p == buf + len);
	}
	if (of) {
		p--;
	}
	*p = 0;

	if (echo == 0) {
		printf("\n");
	}

	signal(SIGINT, oldsigintr);

	if (intr_flag) {
		return (-2);
	}
	if (of) {
		return (-1);
	}
	return (0);
}


#else /* !HAVE_CONIO_H */

#ifndef NSIG
#define NSIG			47
#endif

#define FLAG_ECHO		1
#define FLAG_USE_STDIO		2


static int
read_string(const char *preprompt, const char *prompt,
    char *buf, size_t len, int flags)
{
	struct sigaction sigs[NSIG];
	int oksigs[NSIG];
	struct sigaction sa;
	FILE *tty;
	int ret = 0;
	int of = 0;
	int i;
	int c;
	char *p;

	struct termios t_new, t_old;

	memset(&oksigs, 0, sizeof(oksigs));

	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = intr;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	for (i = 1; i < sizeof(sigs) / sizeof(sigs[0]); i++) {
		if (i != SIGALRM) {
			if (sigaction(i, &sa, &sigs[i]) == 0) {
				oksigs[i] = 1;
			}
		}
	}

	/*
	 * Don't use /dev/tty for now since server tools want to to
	 * read/write from stdio when setting up and interacting with the
	 * Kerberos subsystem.
	 *
	 * When <rdar://problem/7308846> is in we can remove this, this is
	 * to make transiation easier for server folks.
	 */
	if ((flags & FLAG_USE_STDIO) != 0) {
		tty = stdin;
	} else if ((tty = fopen("/dev/tty", "r")) != NULL) {
		int flg, fd = fileno(tty);
		/* rk_cloexec_file(tty); */
		if ((flg = fcntl(fd, F_GETFD)) != -1) {
			fcntl(fd, F_SETFD, flg | FD_CLOEXEC);
		}
	} else{
		tty = stdin;
	}

	fprintf(stderr, "%s%s", preprompt, prompt);
	fflush(stderr);

	if ((flags & FLAG_ECHO) == 0) {
		tcgetattr(fileno(tty), &t_old);
		memcpy(&t_new, &t_old, sizeof(t_new));
		t_new.c_lflag &= ~ECHO;
		tcsetattr(fileno(tty), TCSANOW, &t_new);
	}
	intr_flag = 0;
	p = buf;
	while (intr_flag == 0) {
		c = getc(tty);
		if (c == EOF) {
			if (!ferror(tty)) {
				ret = 1;
			}
			break;
		}
		if (c == '\n') {
			break;
		}
		if (of == 0) {
			*p++ = c;
		}
		of = (p == buf + len);
	}
	if (of) {
		p--;
	}
	*p = 0;

	if ((flags & FLAG_ECHO) == 0) {
		fprintf(stderr, "\n");
		tcsetattr(fileno(tty), TCSANOW, &t_old);
	}

	if (tty != stdin) {
		fclose(tty);
	}

	for (i = 1; i < sizeof(sigs) / sizeof(sigs[0]); i++) {
		if (oksigs[i]) {
			sigaction(i, &sigs[i], NULL);
		}
	}

	if (ret) {
		return (-3);
	}
	if (intr_flag) {
		return (-2);
	}
	if (of) {
		return (-1);
	}
	return (0);
}


#endif /* HAVE_CONIO_H */

int
UI_UTIL_read_pw_string(char *buf, int length, const char *prompt, int verify)
{
	int ret;

	ret = read_string("", prompt, buf, length, 0);
	if (ret) {
		return (ret);
	}

	if (verify) {
		char *buf2;
		buf2 = malloc(length);
		if (buf2 == NULL) {
			return (1);
		}

		ret = read_string("Verify password - ", prompt, buf2, length, 0);
		if (ret) {
			free(buf2);
			return (ret);
		}
		if (strcmp(buf2, buf) != 0) {
			ret = 1;
		}
		memset(buf2, 0, length);
		free(buf2);
	}
	return (ret);
}


int
UI_UTIL_read_pw_string_stdio(char *buf, int length, const char *prompt, int verify)
{
	int ret;

	ret = read_string("", prompt, buf, length, FLAG_USE_STDIO);
	if (ret) {
		return (ret);
	}

	if (verify) {
		char *buf2;
		buf2 = malloc(length);
		if (buf2 == NULL) {
			return (1);
		}

		ret = read_string("Verify password - ", prompt, buf2, length, FLAG_USE_STDIO);
		if (ret) {
			free(buf2);
			return (ret);
		}
		if (strcmp(buf2, buf) != 0) {
			ret = 1;
		}
		memset(buf2, 0, length);
		free(buf2);
	}
	return (ret);
}
