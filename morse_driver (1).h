#ifndef _MORSE_DRIVER_H_
#define _MORSE_DRIVER_H_

#include <linux/ioctl.h>

#define MORSE_IOC_MAGIC  'M'
#define MORSE_IOC_RESET  _IO(MORSE_IOC_MAGIC, 1)
#define MORSE_IOC_SEND   _IOW(MORSE_IOC_MAGIC, 2, char *)

#define DOT_DURATION    200
#define DASH_DURATION   600

#endif // _MORSE_DRIVER_H_
