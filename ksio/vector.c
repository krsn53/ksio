#include "vector.h"
#include "logger.h"
#include <stdlib.h>
#include <memory.h>

void ks_vector_init_base(void** data, u32 type_size, u32 *length, u32* capacity){
    *data = calloc(1, type_size);
    *length = 0;
    *capacity=1;
}

void ks_vector_reserve_base(void** data, u32 type_size,  u32 *capacity, u32 new_cap){
    if(new_cap > *capacity){
        *data = realloc(*data, new_cap*type_size);
        memset(*(char**)data + *capacity*type_size, 0, (new_cap-*capacity)*type_size);
        *capacity = new_cap;
    }
}

void  ks_vector_clear_base(void**data, u32 type_size, u32* length){
    memset(*data, 0, *length*type_size);
    *length = 0;
}

void ks_vector_push_range_base(void** data, u32 type_size, u32* length, u32 *capacity, u32 obj_length, const void*objs){
    u32 out_len = *length +obj_length;
    u32 cap = *capacity;
    while(out_len > cap){
       cap*=2;
    }
    ks_vector_reserve_base(data, type_size, capacity, cap);
    memcpy(*(char**)data + *length * type_size , objs, type_size * obj_length);
    *length += obj_length;
}

void ks_vector_push_base(void** data, u32 type_size, u32* length, u32 *capacity, const void*obj){
    if(*length +1 > *capacity){
        ks_vector_reserve_base(data, type_size, capacity, *capacity * 2);
    }
    memcpy(*(char**)data + *length*type_size , obj, type_size);
    ++ *length;
}

void ks_vector_pop_base(void**data, u32 type_size, u32* length){
    if(*length == 0) {
        ks_warning("Failed to pop from vector because length is zero");
        return;
    }
    -- *length;
    memset(*(char**)data + *length*type_size, 0, type_size);
}

void ks_vector_resize_base(void**data, u32 type_size, u32*length, u32* capacity, u32 new_size){
    ks_vector_reserve_base(data, type_size, capacity, new_size);
    *length = new_size;
}

void ks_vector_insert_base(void**data, u32 type_size, u32*length, u32* capacity, u32 index, const void* obj){
    if(index >= *length) {
        return ks_vector_push_base(data, type_size, length, capacity, obj);
    }
    if(*length +1 > *capacity){
        ks_vector_reserve_base(data, type_size, capacity, *capacity * 2);
    }
    ks_vector_resize_base(data, type_size, length, capacity, *length +1);
    for(unsigned i=*length-1; i>index; i--){
        memcpy(*(char**)data + i*type_size, *(char**)data + (i-1)*type_size, type_size);
    }
    memcpy(*(char**)data + index*type_size, obj, type_size);
}

void ks_vector_remove_base(void** data, u32 type_size, u32*length, u32 index){
    if(*length <= index) return;

    for(unsigned i=index ;i<*length; i++){
        memcpy(*(char**)data + i*type_size, *(char**)data + (i+1)*type_size, type_size);
    }

    ks_vector_pop_base(data, type_size, length);
}

void ks_vector_free_base(void** data){
    free(*data);
}
