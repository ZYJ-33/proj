#include <stdlib.h>
#include <sys/types.h>
#include "../convert.h"
#include "../weight.h"
#define HARD 1
#define SOFT 2
#define CLEAN_HIGH_BIT(WORD, LEN) (WORD & (((u_int64_t)1 << (64 - LEN))-1))
#define CLEAN_LOW_BIT(WORD, LEN) (WORD & ~(((u_int64_t)1 << LEN) - 1))
#define ALL_ONE(X) (((u_int64_t)1 << X) - 1)
#define GET_BIT_BETWEEN(WORD, LEFT, RIGHT) (WORD & ((ALL_ONE(RIGHT - LEFT + 1)) << LEFT))
#define CLEAN_BIT_BETWEEN(WORD, LEFT, RIGHT) (WORD & ~(((ALL_ONE(RIGHT - LEFT + 1)) << LEFT)))
#define WBADDR(WORD) (CLEAN_LOW_BIT(WORD, 32) >> 32)
#define ACCUB_ADDR(WORD) (CLEAN_LOW_BIT(WORD, 32) >> 32)
#define ACCUC_ADDR(WORD) (CLEAN_HIGH_BIT(WORD, 32))
#define WRITE_BACK_ADDR(WORD) (CLEAN_LOW_BIT(WORD, 32) >> 32)
#define QUANTI_ACCUB_FACTOR(WORD) (CLEAN_LOW_BIT(WORD, 48) >> 48)
#define QUANTI_ACCUC_VALUE(WORD) (GET_BIT_BETWEEN(WORD, 32, 47))

struct instr_data
{
    int type;  // indicate whether is a soft_operator or not
    
    //soft operator config
    u_int32_t soft_first_word;
    u_int32_t soft_second_word;
    u_int32_t input_addr;
    u_int32_t output_addr;
    u_int32_t para_offset;
    u_int32_t para_byte_len;

    //below are hard_operator config
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

    struct weight_list* weights;
};

struct instr_data* new_instr(int type); 

struct instr_header
{
    u_int32_t word;
    u_int32_t magic;
};

struct instr_header* new_header(u_int32_t id, u_int32_t type, u_int32_t len);
struct instr_header* calculate_header(struct instr_data* data);

struct instrs
{
    struct instr_header* hdr;
    struct instr_data* data;
    struct instrs* next;
};
struct instrs* new_instrs();

void set_conv_en(struct instr_data* instr);
void set_conv_mod(struct instr_data* instr, int is_conv3);
void set_conv_sign(struct instr_data* instr, int is_uint8);
void set_accuC_mode(struct instr_data* instr, int from_bias);
void set_conv_size(struct instr_data* instr);
void set_baseaddr0(struct instr_data* instr);
void set_has_weight(struct instr_data* instr);

void set_conv_size_word(struct instr_data* instr, u_int64_t tw, u_int64_t th, u_int64_t bpl, u_int64_t bpc);
void set_wb_size_word(struct instr_data* instr, u_int64_t bpl, u_int64_t bpc);
void set_baseaddr0_word(struct instr_data *instr, u_int64_t conv_addr, u_int64_t accub_addr);
void set_baseaddr1_word(struct instr_data *instr, u_int64_t accuc_addr, u_int64_t wb_addr);
void set_quantize_word(struct instr_data* instr, BF16 pos_slope, BF16 neg_slope, u_int64_t bias, u_int64_t factor);
void set_acti_word(struct instr_data* instr, BF16 pos_slope, BF16 neg_slope, BF16 pos_thd, BF16 neg_thd);
void set_conv_channum(struct instr_data* instr, u_int64_t channum);
void set_post_mode(struct instr_data* instr, u_int64_t post);
void set_accuc_size_word(struct instr_data* instr, u_int64_t tw, u_int64_t th, u_int64_t bpl, u_int64_t bpc);
void set_last_hard(struct instr_data* instr);
void set_weights(struct instr_data* instr, u_int32_t channum, u_int32_t ker_size);

void set_padtop(struct instr_data* instr);
void set_padbot(struct instr_data* instr);
void set_padleft(struct instr_data* instr);
void set_padright(struct instr_data* instr);

void code_gen(struct instrs* prog);
