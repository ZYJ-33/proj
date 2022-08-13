#include "code_gen.h"
#include "config_offset.h"
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include "../weight.h"
#include "../convert.h"
#include "../write_bin/write_bin.h"

#define SET_BIT(WORD, WHICH) (WORD | (1 << WHICH))
#define CLEAN_BIT(WORD, WHICH) (WORD & ~(1 << WHICH))
#define POWER_OF_TWO(i) ((u_int64_t)1<<i)

#define TRUE 1
#define FALSE 0
#define WORD_LEN 32
#define SOFT_INSTR_WORD_COUNT 6

extern char* weights;
extern u_int32_t weight_cur_index;
extern u_int32_t weight_len;

void set_hard_instr_default(struct instr_data* instr)
{
    set_accuC_mode(instr, 1);
    instr->config = CLEAN_BIT_BETWEEN(instr->config, MAGIC_BEGIN, MAGIC_END);
    instr->config = instr->config | ((u_int64_t)INSTR_MAGIC<<MAGIC_BEGIN);
}

void set_soft_instr_default(struct instr_data* instr)
{
    instr->soft_first_word = 0;
    instr->soft_second_word = 0;
    instr->input_addr = 0;
    instr->output_addr = 0;
    instr->para_offset = 0;
    instr->para_byte_len = 0;
}

void set_weights(struct instr_data* instr, u_int32_t channum, u_int32_t ker_size)
{
    set_has_weight(instr);
    instr->has_weight = 1;
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
    instr->weight_count = ((total + 8 - 1)/8)*8; 
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
        res = CLEAN_HIGH_BIT(res, (8-total)*8);
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
    printf("\nConfig: %lx\n", data->config);
    output_binary(data->config, 64);
    write_bin_64(data->config);
    if(data->has_conv_size)
    {
            printf("Conv size: %lx\n", data->conv_size);
            output_binary(data->conv_size, 64);
            write_bin_64(data->conv_size);
    }
    if(data->has_accu_b_size)
    {
            printf("Accu B Size: %lx\n", data->accu_b_size);
            output_binary(data->accu_b_size, 64);
            write_bin_64(data->accu_b_size);
    }
    if(data->has_accu_c_size)
    {
            printf("Accu C Size: %lx\n", data->accu_c_size);
            output_binary(data->accu_c_size, 64);
            write_bin_64(data->accu_c_size);
    }
    if(data->has_wb_size)
    {
            printf("Wb Size: %lx\n", data->wb_size);
            output_binary(data->wb_size, 64);
            write_bin_64(data->wb_size);
    }
    if(data->has_base_addr0)
    {
            printf("base_addr0: %lx\n", data->base_addr0);
            output_binary(data->base_addr0, 64);
            write_bin_64(data->base_addr0);
    }
    if(data->has_base_addr1)
    {
            printf("base_addr1: %lx\n", data->base_addr1);
            output_binary(data->base_addr1, 64);
            write_bin_64(data->base_addr1);
    }
    if(data->has_quanti)
    {
            printf("quanti: %lx\n", data->quanti);
            output_binary(data->quanti, 64);
            write_bin_64(data->quanti);
    }
    if(data->has_acti)
    {
            printf("acti: %lx\n", data->acti);
            output_binary(data->acti, 64);
            write_bin_64(data->acti);
    }
}

void travel_weight(struct weight_list* l)
{
    struct weight_node* cur = l->header;
    printf("\n========== weight ==========\n");
    while(cur)
    {
        printf("%x\n", cur->weight);
        write_bin_64(cur->weight);
        cur = cur->next;
    }
    printf("\n===================== weight_end =====================\n");
}

u_int64_t instr_hdr(u_int8_t id, u_int8_t type, u_int16_t len)
{
    u_int64_t hdr = 0;
    hdr |= len;
    hdr |= ((u_int64_t)type << 16);
    hdr |= ((u_int64_t)id << 24);
    hdr |= ((u_int64_t)HEADER_MAGIC << 32);
    return hdr;
}

u_int8_t total_instr_count = 0;
void write_hardinstrs(struct instrs* first, struct instrs* last, u_int32_t len)
{
    if(total_instr_count == 255)
            panic("total instr has over 255");
    u_int64_t hdr = instr_hdr(total_instr_count++, HARD, len+2);
    write_bin_64(hdr);
    set_last_hard(last->data);
    while(first != last)
    {
        travel_data(first->data);
        if(first->data->has_weight)
            travel_weight(first->data->weights);
        first = first->next;
    }
    travel_data(first->data);
    if(first->data->has_weight)
            travel_weight(first->data->weights);
}

void write_softinstr(struct instrs* instr)
{
    if(total_instr_count == 255)
            panic("total instr has over 255");
    u_int64_t hdr = instr_hdr(total_instr_count++, SOFT, SOFT_INSTR_WORD_COUNT);
    write_bin_64(hdr);
    write_bin_32(instr->data->soft_first_word);
    write_bin_32(instr->data->soft_second_word);
    write_bin_32(instr->data->input_addr);
    write_bin_32(instr->data->output_addr);
    write_bin_32(instr->data->para_offset);
    write_bin_32(instr->data->para_byte_len);
}

void init_write_buf(struct instrs* go)
{
    u_int32_t total = 0;
    u_int32_t first_hard = 0;

    while(go->next)
    {
        if(get_type(go->hdr) == HARD)
        {
            if(first_hard == 0)
            {
                total += 8;
                first_hard = 1;
            }
            total += ((get_len(go->hdr) - 2)*WORD_LEN)/8;
        }
        else if (get_type(go->hdr) == SOFT)
        {
            first_hard = 0;
            total += 8;
            total += (SOFT_INSTR_WORD_COUNT*WORD_LEN)/8;
        }
        go = go->next;
    }
    char* buf = (char*) malloc(total);
    write_bin_init(buf, total);
}

void code_gen(struct instrs* instrs)
{
    struct instrs* instr = instrs;
    struct instrs* first_hard = 0;
    struct instrs* last_hard = 0;
    u_int8_t last_type = 2;
    u_int32_t hard_len = 0;
    init_write_buf(instrs); 
    while(instr->next != 0)
    {
       if(get_type(instr->hdr) == HARD)
       {
            if(first_hard == 0)
                    first_hard = instr;
            last_hard = instr;
            hard_len += (get_len(instr->hdr)-2);
            last_type = HARD;
       }
       else if(get_type(instr->hdr) == SOFT)
       {
            if(first_hard != 0)
                write_hardinstrs(first_hard, last_hard, hard_len);
            first_hard = 0;
            hard_len = 0;
            write_softinstr(instr);
            last_type = SOFT;
       }
       else
       {
            panic("unknow instr type");
       }
       instr = instr->next;
    }
    if(last_type == HARD)
        write_hardinstrs(first_hard, last_hard, hard_len);
}

struct instrs* new_instrs()
{
    struct instrs* instrs = (struct instrs*) malloc(sizeof(*instrs));
    memset((void*)instrs, 0, sizeof(*instrs));
    return instrs;
}

struct instr_header* new_header(u_int8_t id, u_int8_t type, u_int16_t len)
{
    struct instr_header* hdr = malloc(sizeof(*hdr));
    memset(hdr, 0, sizeof(*hdr));
    hdr->magic = HEADER_MAGIC;
    hdr->word |= len;
    hdr->word |= ((u_int32_t)type << 16);
    hdr->word |= ((u_int32_t)id << 24);
}

u_int8_t get_type(struct instr_header *header)
{
    return (header->word & 0x00FF0000) >> 16;
}

u_int16_t get_len(struct instr_header* header)
{
    return header->word & 0x0000FFFF;
}

void set_len(struct instr_header* header, u_int16_t len)
{
    header->word &= 0xFFFF0000;
    header->word |= (u_int32_t)len;
}

static u_int32_t glb_count = 0;

struct instr_header* calculate_header(struct instr_data* data)
{
    u_int32_t id = glb_count++;
    u_int32_t len = 64/WORD_LEN;

    if(data->type == HARD)
    {
    len += 64/WORD_LEN;
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
    if(data->has_weight)
            len += (data->weight_count * 8)/WORD_LEN;
    return new_header(id, HARD, len);
    }
    else if(data->type == SOFT)
    {
        return new_header(id, SOFT, len+6);
    }
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


void set_has_weight(struct instr_data* instr)
{
    if(instr->type != HARD)
            panic("no a hard_instr in set_has_weight");
    
    u_int64_t word = instr->config;
    word = SET_BIT(word, HAS_WEIGHT);
    instr->config = word;
}

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

void set_quantize_word(struct instr_data* instr, BF16 pos_slope, BF16 neg_slope, BF16 bias, BF16 factor)
{
    printf("set_quantize_word pos_slope: %lx, neg_slope: %lx, bias: %lx, factor: %lx\n", pos_slope, neg_slope, bias, factor);
    
    instr->has_quanti = 1;
    u_int64_t word = 0;
    
    word |= (u_int64_t)pos_slope;
    word |= (((u_int64_t)neg_slope) << 16);
    word |= ((u_int64_t)bias << 32);
    word |= ((u_int64_t)factor << 48);

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

void set_soft_operator_addr(struct instr_data* instr, u_int16_t th, u_int16_t tw, u_int32_t input_addr, u_int32_t output_addr)
{
    instr->soft_second_word = (u_int32_t)(((u_int32_t)th<<16) | ((u_int32_t)tw));
    instr->input_addr = input_addr;
    instr->output_addr = output_addr;
}

void set_last_hard(struct instr_data* instr)
{
    u_int64_t word = instr->config;
    word |= ((u_int64_t)1) << 63;
    instr->config = word;
}
