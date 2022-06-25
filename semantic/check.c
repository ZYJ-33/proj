#include "../inner_node.h"
#include "../code_gen/code_gen.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <sys/types.h>
#include "../default.h"
#include "../convert.h"
#define TRUE  1
#define FALSE 0
#define BOOL_TYPE int
void panic(char*);

struct instr_data* cur_instr_data = 0;
struct instrs* res = 0;


float to_float(char* f)
{
    char* end;
    return strtof(f, &end);
}

void warning()
{
    printf("warning: ");
}

#define TOP   0
#define BOT   1
#define LEFT  2
#define RIGHT 3

void check_padding(struct array_node* arr)
{
    struct array_node* cur = arr;
    int index = 0;
    while(cur->next)
    {
        if(cur->val->type != NUM)
            panic("paddings can only be 0 or 1");
        if(!(cur->val->num == 0 || cur->val->num == 1))
            panic("pad can only have 0 or 1 as value");
        if(index == TOP)
        {
            if(cur->val->num == 1)
                    set_padtop(cur_instr_data);
        }
        else if(index == BOT)
        {
             if(cur->val->num == 1)
                    set_padbot(cur_instr_data);
        }
        else if(index == LEFT)
        {
             if(cur->val->num == 1)
                    set_padleft(cur_instr_data);
        }
        else if(index == RIGHT)
        {
              if(cur->val->num == 1)
                    set_padright(cur_instr_data);
        }
        else
            panic(" \"pad\" : [top, bot, left, right]");
        cur = cur->next;
        index += 1;
    }

    if (index != 4)
        panic(" \"pad\" : [top, bot, left, right]");
}

void check_conv3(struct obj_node* obj)
{
    struct obj_node* cur = obj;
    BOOL_TYPE type = FALSE;
    BOOL_TYPE channum = FALSE;
    BOOL_TYPE bias = FALSE;
    u_int64_t _channum = 0;
    warning();
    set_conv_en(cur_instr_data);
    set_conv_mod(cur_instr_data, 1);
        
    while(cur->next)
    {
        struct key_val_node* kv = cur->kv;
        if(strcmp(kv->key, "type") == 0)
        {
            if(kv->val->type != STR)
                    panic("type should be declare as string");
            type = TRUE;
            if(strcmp(kv->val->str, "int8") == 0)
                  continue;
            else if(strcmp(kv->val->str, "uint8") == 0)
                  set_conv_sign(cur_instr_data, 1);  
        }
        else if(strcmp(kv->key, "chan_num") == 0)
        {
            if(kv->val->type != NUM)
                    panic("chan_num should be declare as num");
            channum = TRUE;
            _channum = kv->val->num;
            set_conv_channum(cur_instr_data, (u_int64_t)kv->val->num);
        }
        else if(strcmp(kv->key, "bias") == 0)
        {
            bias = TRUE;
            if(kv->val->type != STR)
                panic("bias should be float string");
            float bias_float = to_float(kv->val->str);
            BF16 bias_bf = fp32_to_bf16(&bias_float);
            set_quantize_word(
                            cur_instr_data, 
                            (BF16)QUANTI_POS_SLOPE(cur_instr_data->quanti), 
                            (BF16)QUANTI_NEG_SLOPE(cur_instr_data->quanti),
                            bias_bf,
                            (BF16)QUANTI_ACCUB_FACTOR(cur_instr_data->quanti)
                            );
        }
        else if(strcmp(kv->key, "pad") == 0)
        {
            if(kv->val->type != ARR)
                panic("pad not a arr");
            check_padding(kv->val->arr);
        }
        else
        {
            printf("unknow keyword %s found in conv3\n", kv->key);
            panic("");
        }
        cur = cur->next;
    }
    if(!channum)
        panic("conv3 have to specify chan_num\n");
    set_weights(cur_instr_data, _channum, 9);
}


void check_conv1(struct obj_node* obj)
{
    struct obj_node* cur = obj;
    BOOL_TYPE type = FALSE;
    BOOL_TYPE channum = FALSE;
    BOOL_TYPE bias = FALSE;
    u_int32_t _channum = 0;
    printf("TODO: conv1 need to be finish");
    while(cur->next)
    {
        struct key_val_node* kv = cur->kv;
        if(strcmp(kv->key, "type") == 0)
        {
            type = TRUE;
        }
        else if(strcmp(kv->key, "chan_num") == 0)
        {
            channum = TRUE;
            _channum = kv->val->num;
        }
        else if(strcmp(kv->key, "bias") == 0)
        {
            bias = TRUE;
        }
        else
        {
            printf("unknow keyword %s found in conv1", kv->key);
            panic("");
        }

        cur = cur->next;
    }
    if(!channum)
        panic("conv1 have to specify chan_num\n");
    set_weights(cur_instr_data, _channum, 1);
}

void check_activate(struct obj_node* obj)
{
    struct obj_node* cur = obj;
    BOOL_TYPE pos_slope = FALSE;
    BOOL_TYPE neg_slope = FALSE;
    BOOL_TYPE pos_thd = FALSE;
    BOOL_TYPE neg_thd = FALSE;
    BF16 _pos_slope = 0;
    BF16 _neg_slope = 0;
    BF16 _pos_thd = ACTI_POSI_THD_DEFAULT;
    BF16 _neg_thd = ACTI_NEG_THD_DEFAULT;
    while(cur->next)
    {
       struct key_val_node* kv = cur->kv;
       if (kv->val->type != STR)
       {
            printf("in activate %s should have float string type\n", kv->key);
            panic("");
       }
       if(strcmp(kv->key, "pos_slope") == 0)
       {
            pos_slope = TRUE;
            float tmp = to_float(kv->val->str);
            _pos_slope = fp32_to_bf16(&tmp);
       }
       else if(strcmp(kv->key, "neg_slope") == 0)
       {
            neg_slope = TRUE;
            float tmp = to_float(kv->val->str);
            _neg_slope = fp32_to_bf16(&tmp);
       }
       else if(strcmp(kv->key, "pos_thd") == 0)
       {
            pos_thd = TRUE;
            float tmp = to_float(kv->val->str);
            _pos_thd = fp32_to_bf16(&tmp);
       }
       else if(strcmp(kv->key, "neg_thd") == 0)
       {
            neg_thd = TRUE;
            float tmp = to_float(kv->val->str);
            _neg_thd = fp32_to_bf16(&tmp);
       }
       cur = cur->next;
    }
    if(!(pos_slope && neg_slope))
    {
        if(pos_slope)
           panic("activate should have pos_slope");
        if(neg_slope)
           panic("activate should have neg_slope");
    }
    set_acti_word(cur_instr_data, _pos_slope, _neg_slope, _pos_thd, _neg_thd);
}

void check_quantize(struct obj_node* obj)
{
    struct obj_node* cur = obj;
    BOOL_TYPE pos_slope = FALSE;
    BOOL_TYPE neg_slope = FALSE;
    BF16 _pos_slope = 0;
    BF16 _neg_slope = 0;

    while(cur->next)
    {
       struct key_val_node* kv = cur->kv;
       if(strcmp(kv->key, "pos_slope") == 0)
       {
            if(kv->val->type != STR)
                    panic("in quantize pos_slope should have float string type");
            pos_slope = TRUE;
            float tmp = to_float(kv->val->str);
            _pos_slope = fp32_to_bf16(&tmp);
       }
       else if(strcmp(kv->key, "neg_slope") == 0)
       {
               if(kv->val->type != STR)
                       panic("in quantize neg_slope should have float string type");
            neg_slope = TRUE;
            float tmp = to_float(kv->val->str);
            _neg_slope = fp32_to_bf16(&tmp);
       }
       else
       {
            printf("unknow keyword %s in quantize\n", kv->key);
            panic("");
       }
       cur = cur->next;
    }
    if(!(pos_slope && neg_slope))
    {
        if(pos_slope == FALSE)
           panic("quantize should have pos_slope");
        if(neg_slope == FALSE)
           panic("quantize should have neg_slope");
    }

    set_quantize_word(cur_instr_data, _pos_slope, _neg_slope, QUANTI_ACCUC_VALUE(cur_instr_data->quanti),
                    QUANTI_ACCUB_FACTOR(cur_instr_data->quanti));
}

void check_at_most_one_conv(struct obj_node* obj)
{
    int conv_count = 0;
    struct obj_node* cur = obj;
    while(cur->next)
    {
        if(strcmp(cur->kv->key, "conv3") == 0 || strcmp(cur->kv->key, "conv1") == 0)
                conv_count += 1;
        cur = cur->next;
    }
    if(conv_count > 1)
        panic("more than one conv in instruction found\n");
}

#define MAX_CHAR_PTR 40
int get_all_operators(struct obj_node* obj, char* saves[MAX_CHAR_PTR])
{
    int i = 0;
    while(i < MAX_CHAR_PTR && obj->next != 0)
    {
        saves[i++] = obj->kv->key;
        obj = obj->next;
    }
    return i;
}

int get_acti(char** save, int count)
{
    int i = 0;
    while(i < count)
    {
        if(strcmp(save[i], "activate") == 0)
                return i;
        i+=1;
    }
    return -1;
}

int get_quanti(char** save, int count)
{
    int i = 0;
    while(i < count)
    {
        if(strcmp(save[i], "quantize") == 0)
                return i;
        i+=1;
    }
    return -1;
}

int get_maxpool(char** save, int count)
{
    int i = 0;
    while(i < count)
    {
        if(strcmp(save[i], "maxpool") == 0)
                return i;
        i+=1;
    }
    return -1;
}


void check_postmode(struct obj_node* obj)
{
   char* saves[MAX_CHAR_PTR];
   int count = get_all_operators(obj, saves);
   
   int i = get_acti(saves, count);
   int j = get_quanti(saves, count);
   int k = get_maxpool(saves, count);

   if(i >= 0) // has activate
   {
       if (j >= 0) // has activate has quantize
       {
            if(k >= 0) // has activate has quantize has maxpool
            {
                if (i < j && j < k) // activate -> quantize -> maxpool
                {
                   set_post_mode(cur_instr_data, 3);
                }
                else
                {
                    panic("not supported postmode");
                }
            }
            else // has activate has quantize no maxpool
            {
                if (i < j) // activate -> quantize
                {
                    set_post_mode(cur_instr_data, 5);
                }
                else if(j < i) // quantize -> activate
                {
                    set_post_mode(cur_instr_data, 1);
                }
            }
       }
       else // has activate no quantize
       {
            if(k >= 0) // has activate no quantize has maxpool
                panic("not supported postmode");
            else // activate only
            {
                set_post_mode(cur_instr_data, 0);
            }
       }
   }
   else // no activate
   {
        if(j >= 0) // no activate has quantize
        {
            if(k >= 0) // no activate has quantize has maxpool
            {
                if (j < k) //quantize -> maxpool
                {
                    set_post_mode(cur_instr_data, 1);
                }
                else
                   panic("no supported postmode");
            }
            else // quantize only
            {
                set_post_mode(cur_instr_data, 4);
            }
        }
        else // no activate no quantize
        {
            if(k < 0) // not activate not quantize not maxpool
            {
                set_post_mode(cur_instr_data, 6);
            }
            else
            {
                panic("no supported postmode");
            }
        }
   }

}

void check_operators(struct obj_node* obj)
{
    if(obj == 0)
        return;
    check_at_most_one_conv(obj);
    check_postmode(obj);
    struct obj_node* cur = obj;
    BOOL_TYPE at_least_one = FALSE;
    warning();
    printf("operators only have conv3 activate quantize impl yet\n");
    while(cur->next)
    { 
        struct key_val_node* kv = cur->kv;
        if(strcmp(kv->key, "conv3") == 0)
        {
            at_least_one = TRUE;
            check_conv3(kv->val->obj);    
        }
        else if(strcmp(kv->key, "conv1") == 0)
        {
            at_least_one = TRUE;
            check_conv1(kv->val->obj);
        }
        else if(strcmp(kv->key, "quantize") == 0)
        {
            at_least_one = TRUE;
            check_quantize(kv->val->obj);
        }
        else if(strcmp(kv->key, "maxpool") == 0)
        {
            if(kv->val != 0)
                   panic("maxpool don't have parameter");
            at_least_one = TRUE;
        }
        else if(strcmp(kv->key, "activate") == 0)
        {
            at_least_one = TRUE;
            check_activate(kv->val->obj);
        }
        else
        {
            printf("did not impl operator %s\n", kv->key);
            panic("");
        }
        cur = cur->next;
    } 
    if(!at_least_one)
            panic("operators should have one operator at least");
}

void check_inputaddr(struct obj_node* obj, BOOL_TYPE is_for_conv)
{
    struct obj_node* cur = obj;
    BOOL_TYPE th = FALSE;
    BOOL_TYPE tw = FALSE;
    BOOL_TYPE addr = FALSE;
    BOOL_TYPE bpl = FALSE;
    BOOL_TYPE bpc = FALSE;
    
    u_int64_t _th = 0;
    u_int64_t _tw = 0;
    u_int64_t _addr = 0;
    u_int64_t _bpl = 0;
    u_int64_t _bpc = 0;

    char* tmp;
    
    warning();

    if(is_for_conv)
    {
    set_conv_size(cur_instr_data);
    while(cur->next)
    {
        struct key_val_node* kv = cur->kv;
        if(strcmp(kv->key, "addr") == 0)
        {
            addr = TRUE;
            _addr = strtoul(kv->val->str,&tmp, 16);
        }
        else if(strcmp(kv->key, "tile_height") == 0)
        {
            th = TRUE;
            _th = kv->val->num;
        }
        else if(strcmp(kv->key, "tile_width") == 0)
        {
            tw = TRUE;
            _tw = kv->val->num;
        }
        else if(strcmp(kv->key, "byte_perline") == 0)
        {
            bpl = TRUE;
            _bpl = kv->val->num;
        }
        else if(strcmp(kv->key, "byte_perchan") == 0)
        {
            bpc = TRUE;
            _bpc = kv->val->num;
        }
        else 
        {
            printf("unknow keyword %s in input_addr", kv->key);
            panic("");
        }
        cur = cur->next;
    }
    }
    else
    {
        panic("input addr is no support for other operator except conv yet");
    }
    if(!(bpc && bpl && addr && th && tw))
        panic("missing keyword in input_addr");

    set_baseaddr0(cur_instr_data);
    set_conv_size_word(cur_instr_data, _tw, _th, _bpl, _bpc);
    set_baseaddr0_word(cur_instr_data, _addr, ACCUB_ADDR(cur_instr_data->base_addr0));
}


void check_writeback(struct obj_node* obj)
{
    struct obj_node* cur = obj;
    BOOL_TYPE addr = FALSE;
    BOOL_TYPE bpl = FALSE;
    BOOL_TYPE bpc = FALSE;
    u_int64_t _bpl = 0;
    u_int64_t _bpc = 0;
    u_int64_t _addr = 0;
    char* tmp;
    while(cur->next)
    {
        struct key_val_node* kv = cur->kv;
        char* str = kv->val->str;
        if(strcmp(kv->key, "addr") == 0)
        {
            addr = TRUE;
            _addr = strtoul(str,&tmp, 16);
        }
        else if(strcmp(kv->key, "byte_perline") == 0)
        {
            bpl = TRUE;
            _bpl = kv->val->num;
        }
        else if(strcmp(kv->key, "byte_perchan") == 0)
        {
            bpc = TRUE;
            _bpc = kv->val->num;
        }
        else 
        {
            printf("unknow keyword %s in write_back", kv->key);
            panic("");
        }
        cur = cur->next;
    }
    if(!(bpc && bpl && addr))
            panic("missing keyword in write_back");
    set_wb_size_word(cur_instr_data, _bpl, _bpc);
    set_baseaddr1_word(cur_instr_data, ACCUC_ADDR(cur_instr_data->base_addr1), _addr);
}

void check_otheraddr(struct obj_node* obj)
{
    struct obj_node* cur = obj;
    BOOL_TYPE th = FALSE;
    BOOL_TYPE tw = FALSE;
    BOOL_TYPE addr = FALSE;
    BOOL_TYPE bpl = FALSE;
    BOOL_TYPE bpc = FALSE;
    
    u_int64_t _th = 0;
    u_int64_t _tw = 0;
    u_int64_t _addr = 0;
    u_int64_t _bpl = 0;
    u_int64_t _bpc = 0;
    char* tmp;
    while(cur->next)
    {
        struct key_val_node* kv = cur->kv;
        if(strcmp(kv->key, "addr") == 0)
        {
            addr = TRUE;
            _addr = strtoul(kv->val->str, &tmp, 16);
        }
        else if(strcmp(kv->key, "tile_height") == 0)
        {
            th = TRUE;
            _th = kv->val->num;
        }
        else if(strcmp(kv->key, "tile_width") == 0)
        {
            tw = TRUE;
            _tw = kv->val->num;
        }
        else if(strcmp(kv->key, "byte_perline") == 0)
        {
            bpl = TRUE;
            _bpl = kv->val->num;
        }
        else if(strcmp(kv->key, "byte_perchan") == 0)
        {
            bpc = TRUE;
            _bpc = kv->val->num;
        }
        else 
        {
            printf("unknow keyword %s in other_addr", kv->key);
            panic("");
        }
        cur = cur->next;
    }
    if(!(bpc && bpl && addr && th && tw))
            panic("missing keyword in other_addr");
    set_accuC_mode(cur_instr_data, 0);
    set_accuc_size_word(cur_instr_data, _tw, _th, _bpl, _bpc);
    set_baseaddr1_word(cur_instr_data, _addr, WRITE_BACK_ADDR(cur_instr_data->base_addr1));
}

BOOL_TYPE is_soft(struct obj_node* instr)
{
    struct obj_node* cur = instr;
    while(cur->next)
    {
        if(strcmp(cur->kv->key, "soft") == 0)
                return TRUE;
        cur = cur->next;
    }
    return FALSE;
}

void check_soft_addr(struct obj_node* obj)
{
    BOOL_TYPE has_tw = FALSE;
    BOOL_TYPE has_th = FALSE;
    BOOL_TYPE has_input = FALSE;
    BOOL_TYPE has_output = FALSE;
    struct obj_node* cur = obj;
    while(cur -> next)
    {
        if(strcmp(cur->kv->key, "input_addr") == 0)
        {
            has_input = TRUE;
        }
        else if(strcmp(cur->kv->key, "output_addr") == 0)
        {
            has_output = TRUE;
        }
        else if(strcmp(cur->kv->key, "tile_width") == 0)
        {
            has_tw = TRUE;
        }
        else if(strcmp(cur->kv->key, "tile_height") == 0)
        {
            has_th = TRUE;
        }
        else
        {
            panic("unknown keyword in softaddr\n");
        }
        cur = cur->next;
    }
}

void check_soft_instr(struct obj_node* instr)
{
    struct obj_node* cur = instr;
    BOOL_TYPE has_name = FALSE;
    BOOL_TYPE has_para = FALSE;
    BOOL_TYPE has_addr = FALSE;
    while(cur -> next)
    {
        if(strcmp(cur->kv->key, "name") == 0)
        {
            has_name = TRUE;
            if (cur->kv->val->type != STR)
            {
                panic("soft operator's name have to be a string\n");
            } 
        }
        else if(strcmp(cur->kv->key, "paras") == 0)
        {
            has_para = TRUE;
            if(cur->kv->val->type != OBJ)
            {
                panic("soft operator's paras have to be a obj\n");
            }
        }
        else if (strcmp(cur->kv->key, "soft") == 0)
        {
        
        }
        else if(strcmp(cur->kv->key, "addr") == 0)
        {
            has_addr = TRUE;
            if(cur->kv->val->type != OBJ)
                    panic("addr type should be a obj\n");
            check_soft_addr(cur->kv->val);
        }
        else
        {
            printf("unknown keyword %s\n", cur->kv->key);
            panic("");
        }
        cur = cur->next;
    }
    if(has_name == FALSE)
    {
        panic("soft operator did not have name\n");
    }
    if(has_para == FALSE)
    {
        panic("soft operator did not have para\n");
    }
}

BOOL_TYPE is_for_conv(struct obj_node* instr)
{
    struct obj_node* cur = instr;
    while(cur->next)
    {
       struct key_val_node* kv = cur->kv;
       if(strcmp(kv->key, "operators") == 0)
       {
            struct obj_node* ops = kv->val->obj;
            while(ops->next)
            {
               if(strcmp(ops->kv->key, "conv3") == 0 || strcmp(ops->kv->key, "conv1") == 0)
                       return TRUE;
               ops = ops->next;
            }
            return FALSE;
       }
       cur = cur->next;
    }    
    return FALSE;
}

BOOL_TYPE check_has_bias(struct obj_node* conv)
{
    struct obj_node* cur = conv;
    while(cur->next)
    {
        if(strcmp(cur->kv->key, "bias") == 0)
                return TRUE;
        cur = cur->next;
    }
    return FALSE;
}

BOOL_TYPE check_conv_has_bias(struct obj_node* obj)
{
    struct obj_node* cur = obj;
    while(cur->next)
    {
        if(strcmp(cur->kv->key, "conv3") == 0 || strcmp(cur->kv->key, "conv1") == 0)
        {
                if (check_has_bias(cur->kv->val->obj) == TRUE)
                      return TRUE;
                return FALSE;
        }
        cur = cur->next;
    }
    return FALSE;
}

void check_accuc_conflict(struct obj_node* instr)
{
    struct obj_node* cur = instr;
    BOOL_TYPE has_other = FALSE;
    BOOL_TYPE has_bias = FALSE;
    while(cur->next)
    {
         struct key_val_node* kv = cur->kv;
         if(strcmp(kv->key, "other_addr") == 0)
         {
            has_other = TRUE;
         }
         else if(strcmp(kv->key, "operators") == 0)
         {
              if(check_conv_has_bias(kv->val->obj))
                      has_bias = TRUE;
         }
         cur = cur->next;
    }
    if (has_other == TRUE && has_bias == TRUE)
                    panic("can not have other_addr and bias in the same instr");
}



void check_hard_instr(struct obj_node* instr)
{
    struct obj_node* cur = instr;
    BOOL_TYPE operators = FALSE;
    BOOL_TYPE input_addr = FALSE;
    BOOL_TYPE write_back = FALSE;
    BOOL_TYPE other_addr = FALSE;
    check_accuc_conflict(instr);
    while(cur->next)
    {
        struct key_val_node* kv = cur->kv;
        if(kv->val->type != OBJ)
        {
            printf("%s should be a object\n", kv->key);
            panic("");
        }
        if(strcmp(kv->key, "operators") == 0)
        {
            operators = TRUE;
            check_operators(kv->val->obj);          
        }
        else if(strcmp(kv->key, "input_addr") == 0)
        {
            input_addr = TRUE;
            BOOL_TYPE is_conv_addr = is_for_conv(instr);
            check_inputaddr(kv->val->obj, is_conv_addr);
        }
        else if(strcmp(kv->key, "other_addr") == 0)
        {
             other_addr = TRUE;
             warning();
             check_otheraddr(kv->val->obj);
        }
        else if(strcmp(kv->key, "write_back") == 0)
        {
              write_back = TRUE;
              check_writeback(kv->val->obj);
        }
        else
        {
            printf("unknow keyword %s in instr\n", kv->key);
            panic("");
        }
        cur = cur->next;
    }
    if(!(operators && input_addr && write_back))
    {
        if(!operators)
            panic("missing operators");
        if(!input_addr)
            panic("missing input_addr");
        if(!write_back)
            panic("missing write_back");
    }       
}

void check_instrs(struct array_node* cur)
{
    struct instrs* header = new_instrs();
    struct instrs* cur_instr = header;
    struct instrs* last_hard_instr = 0;
    while(cur->next)
    {
        if(cur->val->type != OBJ)
            panic("one of the instr is not a object\n");
        if(is_soft(cur->val->obj))
        {
            cur_instr->data = new_instr(SOFT);
            cur_instr_data = cur_instr->data;
            set_soft_instr_default(cur_instr_data);
            check_soft_instr(cur->val->obj);
        }
        else
        {
            cur_instr->data = new_instr(HARD);
            cur_instr_data = cur_instr->data;
            set_hard_instr_default(cur_instr_data);
            check_hard_instr(cur->val->obj);
            last_hard_instr = cur_instr;
        }
        cur_instr->hdr = calculate_header(cur_instr->data);

        struct instrs* new_one = new_instrs();
        config_setup(cur_instr);
        cur_instr->next = new_one;
        cur_instr = new_one;

        cur = cur->next;        
    }
    set_last_hard(last_hard_instr->data);
    code_gen(header);
}

void check_progarm(struct obj_node* obj, char* weights, u_int32_t len)
{
    struct key_val_node* kv = obj->kv;
    if(strcmp(kv->key, "progarm") != 0)
        panic("first key -> val is not a progarm\n");
    if(obj->next->kv != 0)
        panic("should only exist a progarm in top level\n");
    printf("have to set weight_data before going on");
    check_instrs(obj->kv->val->arr);   
}
