/* libtup
 * Copyright (C) 2017 Actronika SAS
 *     Author: Aurélien Zanelli <aurelien.zanelli@actronika.com>
 */

#ifndef LIBTUP_PRIVATE_H
#define LIBTUP_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__AVR)
#define ENOENT      2       /* No such file or directory */
#define E2BIG       7       /* Argument list too long */
#define EBADF       9       /* Bad file number */
#define EAGAIN      11      /* Try again */
#define ENOMEM      12      /* Out of memory */
#define EFAULT      14      /* Bad address */
#define EBUSY       16      /* Device or resource busy */
#define EINVAL      22      /* Invalid argument */
#define ENOSYS      38      /* Invalid system call number */
#define EWOULDBLOCK EAGAIN  /* Operation would block */
#define EBADMSG     74      /* Not a data message */
#endif

#ifdef __cplusplus
}
#endif

#endif

