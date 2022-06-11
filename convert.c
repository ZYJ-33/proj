#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "convert.h"

#define EXPO 0x7F800000
#define SIGN 0x80000000
#define FRAC 0x007FFFFF

#define RES_FRAC 0x0000007F

BF16 fp32_to_bf16(float* f)
{
     BF16 res = 0;
     u_int32_t* ptr = (u_int32_t*)f;
     u_int32_t sign_bit = (*ptr & SIGN);
     u_int32_t expo = ((*ptr & EXPO) >> 23);
     u_int32_t frac = (*ptr & FRAC);

     if(expo == 255) // nan
     {
        printf("warning: exist an NAN in float conversion\n");
        if(sign_bit == 0)
                return 0x7f80;
        else
                return 0xff80;
     }
     else if(expo == 0)
     {
         u_int16_t frac_res =(u_int16_t) ((frac >> 16) & RES_FRAC);
         if(sign_bit == 0)
                 return res | frac_res;
         else
                 return res | 0x8000 | frac_res;
     }
     u_int32_t small_expo_bit = (expo - 16) << 23;
     u_int32_t big_expo = sign_bit | (expo) << 23;
     u_int32_t small_expo = sign_bit | small_expo_bit | frac;
     float a = *((float*) &(big_expo));
     float b = *((float*) &(small_expo));
     float c = a+b;
     
     u_int32_t* float_res_ptr = (u_int32_t*) &c;
     u_int32_t res_sign = ((*float_res_ptr) & SIGN) >> 31;

     short res_expo = ((short)(((*float_res_ptr) & EXPO) >> 23)<<7);
     short res_frac = (short)((*float_res_ptr) & RES_FRAC);

     if(res_sign == 1)
            res |= 0x8000;
     res |= res_expo;
     res |= res_frac;
     return res;
}

void output_bf16_bin(BF16* val_p)
{
    short val = *val_p;
    int stack[16];
    for(int i=0;i<16;i++)
            stack[i] = 0;
    int cur = 15;
    while (val > 0)
    {
        if(val % 2 == 1)
             stack[cur] = 1;
        else
             stack[cur] = 0;
        cur -= 1;
        val = val >> 1;
    }

    printf("0b");
    for(int i=0;i<16;i++)
    {
        printf("%d", stack[i]);
        if(i == 0 || i == 8)
            printf(" ");
    }
    printf("\n");
}

void output_float_bin(u_int32_t* val_p)
{
    u_int32_t val = *val_p;
    int stack[32];
    for(int i=0;i<32;i++)
            stack[i] = 0;
    int cur = 31;

    while(val > 0)
    {
        if (val % 2 == 1)
            stack[cur] = 1;
        else
            stack[cur] = 0;
        cur -= 1;
        val = val >> 1;
    }

    printf("0b");
    for(int i=0;i<32;i++)
    {
        printf("%d", stack[i]);
        if(i == 0 || i == 8)
               printf(" ");
    }
    printf("\n");
}

int test()
{
    float a = 0.5;
    BF16 res = fp32_to_bf16(&a);
    output_bf16_bin(&res);

    float b = 0;
    res = fp32_to_bf16(&b);
    output_bf16_bin(&res);

    int c = 0x7fffffff;
    res = fp32_to_bf16(&c);
    output_bf16_bin(&res);
}
