#include "write_bin.h"
#include <sys/types.h>
char* write_bin_buf;
u_int32_t write_bin_total_len;
u_int32_t write_bin_cur;

void write_bin_init(char* buf, u_int32_t len)
{
    write_bin_total_len = len;
    write_bin_cur = 0;
    write_bin_buf = buf;
}

void write_bin_32(u_int32_t data)
{
    u_int32_t* ptr = (u_int32_t*)(write_bin_buf+write_bin_cur);
    *ptr = data;
    write_bin_cur += 4;
}

void write_bin_64(u_int64_t data)
{
    u_int64_t* ptr = (u_int64_t*)(write_bin_buf+write_bin_cur);
    *ptr = data;
    write_bin_cur += 8;
}

void output_res()
{
    printf("bin buf total len %d\n", write_bin_total_len);
    for(u_int32_t i = 0; i<write_bin_total_len; i++)
            printf("%d", write_bin_buf[i]);

}
