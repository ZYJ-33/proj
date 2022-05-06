#define STR  0
#define NUM  1
#define BOOL 2
#define ARR  3
#define OBJ  4


struct val_node
{
    int type;
    int num;
    int boolean;
    char* str;
    struct array_node* arr;
    struct obj_node* obj;
};
struct val_node* init(int type, void* data);

struct array_node
{
    struct val_node* val;
    struct array_node* next;
};
struct val_node* arr_get(struct array_node* arr, int i);
int arr_push(struct array_node* arr, struct val_node* val);

struct obj_node
{
    struct key_val_node* kv;
    struct obj_node* next;
};
struct val_node* obj_get(struct obj_node* obj, char* key);
int obj_set(struct obj_node* obj, char* key, struct val_node* val);

struct key_val_node
{
    char* key;
    struct val_node* val;
};
