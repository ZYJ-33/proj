#include "write_bin.h"
#include <sys/types.h>
#include <stdio.h>

char* write_bin_buf;
u_int32_t write_bin_total_len;
u_int32_t write_bin_cur;

char to_char(u_int8_t byte)
{
    if(byte > 15)
            panic("in to char byte > 15\n");
    switch (byte) 
    {
        case 0: return '0';
        case 1: return '1';
        case 2: return '2';
        case 3: return '3';
        case 4: return '4';
        case 5: return '5';
        case 6: return '6';
        case 7: return '7';
        case 8: return '8';
        case 9: return '9';
        case 10: return 'A';
        case 11: return 'B';
        case 12: return 'C';
        case 13: return 'D';
        case 14: return 'E';
        case 15: return 'F';
    }
    panic("in to_char");
}

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

void __print(u_int8_t n)
{
    if(0 <= n && n <= 9)
    {
        printf("%d", n);
    }
    switch (n) {
        case 10: 
            printf("A");
            break;
        case 11:
            printf("B");
            break;
        case 12:
            printf("C");
            break;
        case 13:
            printf("D");
            break;
        case 14:
            printf("E");
            break;
        case 15:
            printf("F");
            break;
        default:
            break;
    }
}

void print_hex(u_int8_t n)
{
    u_int8_t high = (n & 0xF0) >> 4;
    u_int8_t low = n & 0x0F;
    char _low = to_char(low);
    char _high = to_char(high);
    printf(" %c%c ", _high, _low); 
}

void output_res()
{
    printf("========== below are final binary ============\n");
    printf("bin buf total len %d\n", write_bin_total_len);
    for(u_int32_t i = 0; i<write_bin_cur; i++)
    {
            print_hex(write_bin_buf[i]);
    }
}

void write_byte(FILE* byte_per_line_file, u_int8_t byte)
{
    char low = to_char(byte & 0x0F);
    char high = to_char((byte & 0xF0) >> 4);
    fputc(high, byte_per_line_file);
    fputc(low, byte_per_line_file);
    fputc('\n', byte_per_line_file);
}

void write_final_bin_to_file(char* output)
{
    FILE* byte_per_line_file = fopen("byte_per_line", "w");
    FILE* output_file = fopen(output, "w");
    for(int i=0; i<write_bin_cur; i++)
    {
       write_byte(byte_per_line_file, write_bin_buf[i]);
    }
    fwrite((void*) write_bin_buf, 1, write_bin_cur, output_file);
}
