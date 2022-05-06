struct obj_node* transit_obj(struct two_child_node* obj);
struct array_node* transit_arr(struct two_child_node* arr);
void transit_pair(struct obj_node* obj, struct key_child_node* node);
struct val_node* transit_val(struct basic_node* node);

void travel_obj(struct obj_node* cur);
void travel_arr(struct array_node* cur);
void travel_val(struct val_node* val);
