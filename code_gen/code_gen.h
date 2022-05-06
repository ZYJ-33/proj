#include <stdlib.h>
#include <sys/types.h>

struct instr_data
{
    u_int64_t config;

    int has_conv_size;
    u_int64_t conv_size;

    int has_accu_b_size;
    u_int64_t accu_b_size;

    int has_accu_c_size;
    u_int64_t accu_c_size;

    int has_wb_size;
    u_int64_t wb_size;

    int has_base_addr0;
    u_int64_t base_addr0;

    int has_base_addr1;
    u_int64_t base_addr1;

    int has_acti;
    u_int64_t acti;

    int has_quanti;
    u_int64_t quanti;
};

struct instr_data* new_instr(); 

// convmod convsign padding_left padding_right padding_top padding_bottom channel_num
void set_conv(struct instr_data* instr, int mod, int sign, int pl, int pr, int pt, int pb, int chan_num);

void set_input_addr(struct instr_data* instr, u_int32_t addr, int tw, int tl, int bpl, int bpc);

void set_another_input_addr(struct instr_data* instr, u_int32_t addr, int tw, int tl, int bpl, int bpc);

void set_write_back(struct instr_data* instr, u_int32_t addr, int bpl, int bpc);

void set_quantize(struct instr_data* instr, int sign, int pos_slope, int neg_slope);

void set_activate(struct instr_data* instr, )





struct instrs
{
    struct instr_data* data;
    struct instrs* next;
};

