struct basic_node
{
    int type;
    int meta_type;
};

struct data_node
{
    int type;
    int meta_type;
    char* data;
    
};
struct basic_node* new_data_node(int type, char* data);

struct key_data_node
{
    int type;
    int meta_type;
    char* key;
    char* data;
};
struct basic_node* new_key_data_node(int type, char* key, char* data);

struct key_child_node
{
    int type;
    int meta_type;
    char* key;
    struct basic_node* child;
};
struct basic_node* new_key_child_node(int type, char* key, struct basic_node* child);

struct there_child_node
{
    int type;
    int meta_type;
    struct basic_node* fst;
    struct basic_node* snd;
    struct basic_node* thr;
};
struct basic_node* new_there_child_node(int type, struct basic_node* fst, struct basic_node* snd, struct basic_node* thr);

struct two_child_node
{
    int type;
    int meta_type;
    struct basic_node* fst;
    struct basic_node* snd;
};
struct basic_node* new_two_child_node(int type, struct basic_node* fst, struct basic_node* snd);

struct one_child_node
{
    int type;
    int meta_type;
    struct basic_node* fst;
};
struct basic_node* new_one_child_node(int type, struct basic_node* child);

void travel(struct basic_node* node, void(*func)(struct basic_node*));
