// assert.c - Assertion statements and panic function
//
// Copyright (c) 2024-2025 University of Illinois
// SPDX-License-identifier: NCSA
//

#include "assert.h"

void panic_actual(const char * srcfile, int srcline, const char * msg) {
    if (msg != NULL && *msg != '\0')
        printf("PANIC: file=%s, line=%d, msg=%s", srcfile, srcline,  msg);
    else
        printf("PANIC: file=%s, line=%d\n", srcfile, srcline);

    exit(-1);
}

void assert_failed(const char * srcfile, int srcline, const char * stmt) {
    printf("ASSERT: file=%s, line=%d, failed(%s)\n", srcfile, srcline, stmt);
    exit(-1);
}
