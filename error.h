// error.h - Error numbers
//
// Copyright (c) 2024-2025 University of Illinois
// SPDX-License-identifier: NCSA
//

#ifndef _ERROR_H_
#define _ERROR_H_

#define EINVAL      1
#define EBUSY       2
#define ENOTSUP     3
#define ENODEV      4
#define EIO         5

extern const char * error_name(int code);

#endif // _ERROR_H_
