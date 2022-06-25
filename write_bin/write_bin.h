#include <sys/types.h>

#ifndef WRITE_BIN
#define WRITE_BIN 1
void write_bin_64(u_int64_t data);
void write_bin_32(u_int32_t data);
void write_bin_init(char* buf, u_int32_t len);
void output_res();
#endif
