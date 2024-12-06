#ifndef MAIN_LOGTOCONSOLE_H
#define MAIN_LOGTOCONSOLE_H

#include "./Constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>

void open_console(void);
void log_msg(const char *format, ...) __attribute__((format(printf, 1, 2)));

#endif    // MAIN_LOGTOCONSOLE_H
