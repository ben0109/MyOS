#ifndef __FILEDESCR_H
#define __FILEDESCR_H

#include "types.h"

struct __file_struct;
struct __fd_list_struct;

typedef struct __file_struct file_t;
typedef struct __fd_list_struct fd_list_t

file_t* get_file(int fd);

int close(int);
int dup(int);
uint32_t read(int, void *, uint32);
uint32_t write(int fildes, const void *buf, uint32 nbyte);

#endif
