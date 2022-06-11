#include "code_gen.h"
#include "config_offset.h"
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include "../weight.h"
#include "../convert.h"

#define SET_BIT(WORD, WHICH) (WORD | (1 << WHICH))
#define CLEAN_BIT(WORD, WHICH) (WORD & ~(1 << WHICH))
#define POWER_OF_TWO(i) ((u_int64_t)1<<i)

#define TRUE 1
#define FALSE 0
extern char* weights;
extern u_int32_t weight_cur_index;
extern u_int32_t weight_len;


void set_hard_instr_default(struct instr_data* instr)
{
    set_accuC_mode(instr, 1);
}

void set_soft_instr_default(struct instr_data* instr)
{

}

void set_weights(struct instr_data* instr, u_int32_t channum, u_int32_t ker_size)
{
    set_has_weight(instr);
    if(channum >= 256)
    {
        printf("channum: %d too much", channum);
        panic("");
    }
    if(ker_size != 1 && ker_size != 9)
    {
        printf("unsupport kernel size %d", ker_size);
        panic("");
    }
    if(weight_cur_index + ker_size*channum > weight_len)
    {
        panic("no enought weights");
    }
    int total = ker_size * channum;
    printf("TODO: find out what happen when weight are not 64 byte align\n");
    
    u_int64_t* ptr = (u_int64_t*)(weights+weight_cur_index);
    while(total - 8 >= 0)
    {
        add_weight(instr->weights, *ptr);
        total -= 8;
        ptr += 1;
    }
    if(total != 0)
    {
        u_int64_t res = (*ptr);
        res = CLEAN_LOW_BIT(res, (8-total)*8);
        add_weight(instr->weights, res);
    }
}

void set_padtop(struct instr_data* instr)
{
    u_int64_t word = instr->config;
    instr->config = SET_BIT(word, 10);
}

void set_padbot(struct instr_data* instr)
{
    u_int64_t word = instr->config;
    instr->config = SET_BIT(word, 11);
}

void set_padleft(struct instr_data* instr)
{
    u_int64_t word = instr->config;
    instr->config = SET_BIT(word, 8);
}

void set_padright(struct instr_data* instr)
{
    u_int64_t word = instr->config;
    instr->config = SET_BIT(word, 9);
}

void set_accuc_size_word(struct instr_data* data, u_int64_t tw, u_int64_t th, u_int64_t bpl, u_int64_t bpc)
{
    printf("set_accuc_size_word tw:%ld, th:%ld, bpl:%ld, bpc:%ld\n", tw, th, bpl, bpc);
    if(tw >= POWER_OF_TWO(10))
            panic("tile width too large");
    else if(th >= POWER_OF_TWO(10))
            panic("tile height too large");
    else if(bpl >= POWER_OF_TWO(12))
            panic("byte per line too large");
 
    data->has_accu_c_size = 1;
    u_int64_t word = data->accu_c_size;

    word |= tw;
    word |= (th << 10);
    word |= (bpl << 20);
    word |= (bpc << 32);

    data->accu_c_size = word;
}

void config_setup(struct instrs* instr)
{
    struct instr_data* data = instr->data;
    u_int64_t word = data->config;
    if(data->has_base_addr0)
    {
        word = SET_BIT(word, HAS_BASE0);
    }
    if(data->has_base_addr1)
    {
        word = SET_BIT(word, HAS_BASE1);
    }
    if(data->has_conv_size)
    {
        word = SET_BIT(word, HAS_CONV_SIZE);
    }
    if(data->has_accu_b_size)
    {
        word = SET_BIT(word, HAS_ACCU_B);
    }
    if(data->has_accu_c_size)
    {
        word = SET_BIT(word, HAS_ACCU_C);
    }
    if(data->has_wb_size)
    {
        word = SET_BIT(word, HAS_WB);
    }
    if(data->has_quanti)
    {
        word = SET_BIT(word, HAS_QUANTI);
    }
    if(data->has_acti)
    {
        word = SET_BIT(word, HAS_ACTI);
    }
    data->config = word;
}

void output_binary(u_int64_t num, u_int32_t count)
{
   int* bits = (int*)malloc(sizeof(int)*count);
   memset(bits, 0, sizeof(int)*count);
   
   int cur = count-1;
   while(cur >= 0)
   {
      bits[cur] = num%2;
      num = num >> 1;     
      cur -= 1;
   }

   for(int i=0;i<count;i++)
          printf(" %d:%d ", count-1-i, bits[i]);
   free((void*)bits);
   printf("\n");
}

void panic(const char* s)
{
    printf("panic: %s\n", s);
    exit(1);
}

void travel_hdr(struct instr_header* hdr)
{
    printf("hdr word\n");
    output_binary(hdr->word, 32);
    printf("hdr magic\n");
    output_binary(hdr->magic, 32);
}

void travel_data(struct instr_data* data)
{
    printf("Config: %lx\n", data->config);
    output_binary(data->config, 64);
    if(data->has_conv_size)
    {
            printf("Conv size: %lx\n", data->conv_size);
            output_binary(data->conv_size, 64);
    }
    if(data->has_accu_b_size)
    {
            printf("Accu B Size: %lx\n", data->accu_b_size);
            output_binary(data->accu_b_size, 64);
    }
    if(data->has_accu_c_size)
    {
            printf("Accu C Size: %lx\n", data->accu_c_size);
            output_binary(data->accu_c_size, 64);
    }
    if(data->has_wb_size)
    {
            printf("Wb Size: %lx\n", data->wb_size);
            output_binary(data->wb_size, 64);
    }
    if(data->has_base_addr0)
    {
            printf("base_addr0: %lx\n", data->base_addr0);
            output_binary(data->base_addr0, 64);
    }
    if(data->has_base_addr1)
    {
            printf("base_addr1: %lx\n", data->base_addr1);
            output_binary(data->base_addr1, 64);
    }
    if(data->has_quanti)
    {
            printf("quanti: %lx\n", data->quanti);
            output_binary(data->quanti, 64);
    }
    if(data->has_acti)
    {
            printf("acti: %lx\n", data->acti);
            output_binary(data->acti, 64);
    }
}

void travel_weight(struct weight_list* l)
{
    struct weight_node* cur = l->header;
    while(cur)
    {
        printf("%x ", cur->weight);
        cur = cur->next;
    }
}

void code_gen(struct instrs* instrs)
{
    struct instrs* instr = instrs;
    while(instr->next != 0)
    {
        travel_hdr(instr->hdr);
        travel_data(instr->data);
        travel_weight(instr->data->weights); 
        instr = instr->next;
    }
}

struct instrs* new_instrs()
{
    struct instrs* instrs = (struct instrs*) malloc(sizeof(*instrs));
    memset((void*)instrs, 0, sizeof(*instrs));
    return instrs;
}

struct instr_header* new_header(u_int32_t id, u_int32_t type, u_int32_t len)
{
    if(id >= POWER_OF_TWO(8))
            panic("hdr id too large");
    else if(type >= POWER_OF_TWO(8))
            panic("hdr type too large");
    else if(len >= POWER_OF_TWO(16))
            panic("hdr len too large");

    struct instr_header* hdr = malloc(sizeof(*hdr));
    memset(hdr, 0, sizeof(*hdr));

    hdr->magic = HEADER_MAGIC;
    hdr->word |= len;
    hdr->word |= (type << 16);
    hdr->word |= (id << 24);
}

static u_int32_t glb_count = 0;
#define WORD_LEN 32

struct instr_header* calculate_header(struct instr_data* data)
{
    u_int32_t id = glb_count++;
    u_int32_t len = 64/WORD_LEN;
    if(data->has_accu_b_size)
            len += 64/WORD_LEN;
    if(data->has_accu_c_size)
            len += 64/WORD_LEN;
    if(data->has_conv_size)
            len += 64/WORD_LEN;
    if(data->has_wb_size)
            len += 64/WORD_LEN;
    if(data->has_acti)
            len += 64/WORD_LEN;
    if(data->has_base_addr0)
            len += 64/WORD_LEN;
    if(data->has_base_addr1)
            len += 64/WORD_LEN;
    if(data->has_quanti)
            len += 64/WORD_LEN;
    if(data->type == HARD)
        return new_header(id, HARD, len);
    else if(data->type == SOFT)
        return new_header(id, SOFT, len);
    else
        panic("unknown type in calculate_header");
}

struct instr_data* new_instr(int type)
{
    struct instr_data* instr = (struct instr_data*) malloc(sizeof(*instr));
    memset((void*)instr, 0, sizeof(*instr));
    if(type == HARD)
        instr->type = HARD;
    else if(type == SOFT)
        instr->type = SOFT;
    else
        panic("unknown type in new_instr");
    instr->weights = new_list();
    return instr;
        
}

void set_post_mode(struct instr_data* instr, u_int64_t postmode)
{
    if(postmode >= 7)
          panic("no supported post mode");
    u_int64_t word = CLEAN_BIT_BETWEEN(instr->config, 16, 18);
    word |= postmode<<16;
    instr->config = word; 
}

void set_conv_channum(struct instr_data* instr, u_int64_t chan_num)
{
    if(chan_num >= POWER_OF_TWO(8))
            panic("chan_num too big");
    u_int64_t word = instr->config;
    word = CLEAN_BIT_BETWEEN(word, 32, 39);
    word |= (chan_num << 32);
    instr->config = word;
}

void set_conv_en(struct instr_data* instr) // set conv enable in config
{
    if(instr->type != HARD)
        panic("not a hard_instr in set_conv_en");
    
    u_int64_t word = instr->config;
    word = SET_BIT(word, CONV_EN);
    instr->config = word;    
}

void set_conv_mod(struct instr_data* instr, int is_conv3)
{
    if(instr->type != HARD)
        panic("not a hard_instr in set_conv_mod");
    
    u_int64_t word = instr->config;
    if(is_conv3 == 1)
        word = SET_BIT(word, CONV_MODE);
    instr->config = word;
}

void set_conv_sign(struct instr_data* instr, int is_uint8)
{
    if(instr->type != HARD)
        panic("not a hard_instr in set_conv_sign");
    
    u_int64_t word = instr->config;
    if(is_uint8 == 1)
        word = SET_BIT(word, CONV_SIGN);
    instr->config = word;
}

// TODO: ADD FOUR PADDING

void set_accuC_mode(struct instr_data* instr, int is_bias)
{
    if(instr->type != HARD)
         panic("not a hard_instr in set_accuC_mode");
    
    u_int64_t word = instr->config;
    if(is_bias)
    {
         word = SET_BIT(word, ACCU_C_MODE);
         word = SET_BIT(word, HAS_QUANTI);
         instr->has_quanti = 1;
    }
    else
    {
        word = CLEAN_BIT(word, ACCU_C_MODE);
    }
    instr->config = word;
}


//TODO: add post mode

void set_conv_size(struct instr_data* instr)
{
    if(instr->type != HARD)
          panic("no a hard_instr in set_conv_size");

    u_int64_t word = instr->config;
    word = SET_BIT(word, HAS_CONV_SIZE);
    instr->config = word;
}

void set_baseaddr0(struct instr_data* instr)
{
    if(instr->type != HARD)
            panic("no a hard_instr in set_baseaddr0");
    
    u_int64_t word = instr->config;
    word = SET_BIT(word, HAS_BASE0);
    instr->config = word;
}

//TODO: ADD baseaddr1 activation quantize 

void set_has_weight(struct instr_data* instr)
{
    if(instr->type != HARD)
            panic("no a hard_instr in set_has_weight");
    
    u_int64_t word = instr->config;
    word = SET_BIT(word, HAS_WEIGHT);
    instr->config = word;
}
// TODO: ADD CONV ACCUB ACCUC WB ADDR OFFSET EN

void set_conv_size_word(struct instr_data* instr, u_int64_t tw, u_int64_t th, u_int64_t bpl, u_int64_t bpc)
{
    printf("set_conv_size_word tw:%ld, th:%ld, bpl:%ld, bpc:%ld\n", tw, th, bpl, bpc);
    if(tw >= POWER_OF_TWO(10))
            panic("tile width too large");
    else if(th >= POWER_OF_TWO(10))
            panic("tile height too large");
    else if(bpl >= POWER_OF_TWO(12))
            panic("byte per line too large");
    
    instr->has_conv_size = 1;
    u_int64_t word = instr->conv_size;
    
    word |= tw;
    word |= (th << 10);
    word |= (bpl << 20);
    word |= (bpc << 32);

    instr->conv_size = word;
}

void set_wb_size_word(struct instr_data* instr, u_int64_t bpl, u_int64_t bpc)
{
    printf("set_wb_size_word bpl: %ld bpc: %ld\n", bpl, bpc);
    if(bpl >= POWER_OF_TWO(12))
            panic("byte per line too large");

    instr->has_wb_size = 1;
    u_int64_t word = instr->wb_size;
    
    word |= (bpl << 20);
    word |= (bpc << 32);
    
    instr->wb_size = word;
}

void set_baseaddr0_word(struct instr_data* instr, u_int64_t conv_addr, u_int64_t accub_addr)
{
    printf("set_baseaddr0_word conv_addr: %lx accub_addr: %lx\n", conv_addr, accub_addr);
    if(conv_addr >= POWER_OF_TWO(32))
            panic("conv addr too large");
    else if(accub_addr >= POWER_OF_TWO(32))
            panic("accu b addr too large");

    instr->has_base_addr0 = 1;

    u_int64_t word = instr->base_addr0;
    word |= conv_addr;
    word |= (accub_addr<<32);
    instr->base_addr0 = word;
}

void set_baseaddr1_word(struct instr_data* instr, u_int64_t accuc_addr, u_int64_t wb_addr)
{
    printf("set_baseadd1_word accuc_addr: %lx wb_addr: %lx\n", accuc_addr, wb_addr);
    if(accuc_addr >= POWER_OF_TWO(32))
            panic("accu c addr too large");
    else if(wb_addr >= POWER_OF_TWO(32))
            panic("write back addr too large");
    
    instr->has_base_addr1 = 1;
    
    u_int64_t word = instr->base_addr1;
    word |= accuc_addr;
    word |= (wb_addr << 32);
    instr->base_addr1 = word;
}

void set_quantize_word(struct instr_data* instr, BF16 pos_slope, BF16 neg_slope, u_int64_t bias, u_int64_t factor)
{
    printf("set_quantize_word pos_slope: %lx, neg_slope: %lx, bias: %lx, factor: %lx\n", pos_slope, neg_slope, bias, factor);
    if(bias >= POWER_OF_TWO(16))
            panic("bias too large");
    else if(factor >= POWER_OF_TWO(16))
            panic("factor too large");
    
    instr->has_quanti = 1;
    u_int64_t word = instr->quanti;
    
    word |= (u_int64_t)pos_slope;
    word |= (((u_int64_t)neg_slope) << 16);
    word |= (bias << 32);
    word |= (factor << 48);

    instr->quanti = word;
}

void set_acti_word(struct instr_data* instr, BF16 pos_slope, BF16 neg_slope, BF16 pos_thd, BF16 neg_thd)
{
    instr->has_acti = 1;

    u_int64_t word = 0;
    word |= (u_int64_t)pos_slope;
    word |= (((u_int64_t)neg_slope) << 16);
    word |= (((u_int64_t)pos_thd) << 32);
    word |= (((u_int64_t)neg_thd) << 48);
    
    instr->acti = word;
}

void set_last_hard(struct instr_data* instr)
{
    u_int64_t word = instr->config;
    word |= ((u_int64_t)1) << 63;
    instr->config = word;
}
