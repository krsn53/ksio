#include "io.h"
#include <stdarg.h>
#include <stdlib.h>

ks_io* ks_io_new(){
    ks_io* ret = malloc(sizeof(ks_io));
    ks_vector_init(&ret->userdatas);
    ret->str = ks_string_new();
    ret->seek = 0;
    ret->indent = 0;

    return ret;
}

void ks_io_free(ks_io* io){
    ks_vector_free(&io->userdatas);
    ks_string_free(io->str);
    free(io);
}

void ks_io_reset(ks_io* io){
    ks_vector_clear(&io->userdatas);
    ks_string_clear(io->str);
    io->seek =0;
}



bool ks_io_begin(ks_io* io, const ks_io_funcs* funcs, bool serialize, ks_property prop){
    // Redundancy for KS_INLINE
    if(serialize){
        ks_string_clear(io->str);
        return ks_io_value(io, funcs, prop.value, 0, true);
    }
    else {
        io->seek = 0;
        return ks_io_value(io, funcs, prop.value, 0, false);
    }
}

bool ks_io_read_file(ks_io* io, const char* file){
    FILE* fp = fopen(file, "rb");
    if(!fp) return false;

    fseek(fp, 0, SEEK_END);
    u32 filesize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    ks_string_resize(io->str, filesize);
    u32 readsize =fread(io->str->data, 1, filesize, fp);

    fclose(fp);

    return readsize == filesize;
}
void ks_io_read_string_len(ks_io* io, u32 length, const char* data){
    ks_string_set_n(io->str, length, data);
}

void ks_io_read_string(ks_io* io, const char* data){
    ks_string_set_n(io->str, strlen(data), data);
}

KS_INLINE ks_value ks_val_ptr(void* ptr, ks_value_type type) {
    return (ks_value){ type, {.data=ptr} };
}

KS_INLINE ks_property ks_prop_v(void* name, ks_value value) {
    return (ks_property){name, value};
}



KS_INLINE ks_io_userdata* ks_io_top_userdata_from (ks_io* io, u32 index){
    if(io->userdatas.length <= index) return NULL;
    return &io->userdatas.data[io->userdatas.length - 1 - index];
}

KS_INLINE bool ks_io_push_userdata (ks_io* io, ks_io_userdata userdata){
    ks_vector_push(&io->userdatas, userdata);
    return true;
}

KS_INLINE bool ks_io_pop_userdata(ks_io* io){
    ks_vector_pop(&io->userdatas);
    return true;
}

KS_INLINE bool ks_io_property(ks_io* io, const ks_io_funcs* funcs,  ks_property prop, bool serialize){
    u32 prop_length = funcs->key(io, funcs, prop.name,  true);
    if(prop_length == 0){
        return false;
    }
    if(!ks_io_value(io, funcs, prop.value, 0, serialize)) return false;
    return true;
}

KS_INLINE bool ks_io_magic_number(ks_io* io, const ks_io_funcs* funcs, const char* data){
    ks_value  val={.ptr={ .str=data }, .type = KS_VALUE_MAGIC_NUMBER};
    return funcs->value(io, funcs, val, 0);
}

KS_INLINE bool ks_io_array_begin(ks_io* io, const ks_io_funcs* funcs, ks_array_data* array, u32 offset, bool serialize){

    if(!serialize && !array->fixed_length){
        void* ptr = array->value.ptr.data;
        if(array->value.type == KS_VALUE_OBJECT){
            ks_object_data *obj = ptr;
            ks_object_data* object = malloc(sizeof(ks_object_data));
            ptr = obj->data;
            // TODO: add offset
            *object = *obj;
            object->data = *(void**)ptr = array->length == 0 ? NULL : calloc(array->length, array->elem_size);
            array->value.ptr.obj = object;

        } else if(array->value.type == KS_VALUE_ARRAY){
            // TODO
        } else {
            // TODO: add offset
            array->value.ptr.data = *(void**)(ptr) = array->length == 0 ? NULL : calloc(array->length, array->elem_size);
        }
    }

    if(serialize && !array->fixed_length){
        void* ptr = array->value.ptr.data;
        if(array->value.type == KS_VALUE_OBJECT){
            ks_object_data *obj = ptr;
            ks_object_data* object = malloc(sizeof(ks_object_data));
            *object = *obj;
            // TODO : add offset
            object->data = *(void**)obj->data;
            array->value.ptr.obj = object;
        } else if(array->value.type == KS_VALUE_ARRAY){
            // TODO
        } else {
            array->value.ptr.data = *(array->value.ptr.vpp +  offset);
        }
    }


    if(! funcs->array_begin(io, funcs, *array)) return false;

    return true;
}

KS_INLINE bool ks_io_string(ks_io* io, const ks_io_funcs* funcs, ks_array_data array, u32 offset, bool serialize){
    ks_string * str = ks_string_new();
    if(serialize){
        if(array.length == KS_STRING_UNKNOWN_LENGTH){
            void * ptr =!array.fixed_length  ? *array.value.ptr.vpp: array.value.ptr.data;
            if(ptr != NULL ){
                array.length = strlen(ptr) + 1;
            }
        }
        ks_string_set_n(str, array.length, !array.fixed_length  ? *array.value.ptr.vpp: array.value.ptr.data);
    }
    if(!funcs->string(io, funcs, array,  str)) return false;

    if(!serialize){
        if(!array.fixed_length){
            array.value.ptr.data = *(array.value.ptr.vpp + offset) = str->length == 0 ? NULL : calloc(str->length, array.elem_size);
        }
        else {
            memset(array.value.ptr.data, 0, array.length*array.elem_size);
        }

        memcpy(array.value.ptr.data, str->data, str->length);
    }
    ks_string_free(str);

    return true;
}

KS_INLINE bool ks_io_array(ks_io* io, const ks_io_funcs* funcs, ks_array_data array, u32 offset, bool serialize){

    if(!ks_io_array_begin(io, funcs, &array, offset, serialize)) return false;

    for(u32 i=0, e=array.length; i< e; i++){
        if(! funcs->array_elem(io, funcs, array, i)) return false;
    }

    return ks_io_array_end(io, funcs, &array, offset, serialize);
}

bool ks_io_array_end(ks_io* io, const ks_io_funcs* funcs, ks_array_data* array, u32 offset, bool serialize){
    if(((!serialize && !array->fixed_length) ||
        (serialize && !array->fixed_length)) &&
            array->value.type == KS_VALUE_OBJECT){
        free(array->value.ptr.obj);
    }
    if(! funcs->array_end(io, funcs, *array)) return false;

    return true;
}

KS_INLINE bool ks_io_object(ks_io* io, const ks_io_funcs* funcs, ks_object_data obj, u32 offset, bool serialize){
    return funcs->object(io, funcs, obj, offset);
}

KS_INLINE bool ks_io_value(ks_io* io, const ks_io_funcs* funcs, ks_value value, u32 index, bool serialize){
    bool ret = false;

    switch (value.type) {
    case KS_VALUE_U8:
    case KS_VALUE_U16:
    case KS_VALUE_U32:
    case KS_VALUE_U64:
    case KS_VALUE_I8:
    case KS_VALUE_I16:
    case KS_VALUE_I32:
    case KS_VALUE_I64:
        ret = funcs->value(io, funcs, value, index);
        break;
    case KS_VALUE_ARRAY:{
        ks_array_data* array = value.ptr.arr;
        if(array->value.type == KS_VALUE_STRING_ELEM) {
            ret = ks_io_string(io, funcs, *array, index, serialize);
        }
        else {
            ret = ks_io_array(io, funcs, *array, index, serialize);
        }
        break;
    }
    case KS_VALUE_OBJECT:
        ret = ks_io_object(io, funcs, *(ks_object_data*)value.ptr.obj, index, serialize);
        break;
    default:
        break;
    }
    return ret;
}
