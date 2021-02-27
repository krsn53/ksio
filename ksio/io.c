#include "io.h"
#include <stdarg.h>
#include <stdlib.h>


ks_io* ks_io_new(){
    ks_io* ret = malloc(sizeof(ks_io));
    ks_vector_init(&ret->userdatas);
    ks_vector_init(&ret->states);
    ret->str = ks_string_new();
    ret->seek = 0;
    ret->indent = 0;

    return ret;
}

void ks_io_free(ks_io* io){
    ks_vector_free(&io->userdatas);
    ks_vector_free(&io->states);
    ks_string_free(io->str);
    free(io);
}

void ks_io_reset(ks_io* io){
    ks_vector_clear(&io->userdatas);
    ks_vector_clear(&io->states);
    ks_string_clear(io->str);
    io->seek =0;
    io->indent = 0;
}

KS_NOINLINE bool ks_io_begin_base (ks_io*io, const ks_io_methods* methods, ks_object_data obj, ks_io_serial_type type){
    ks_property prop ={.value= (ks_value){.type = KS_VALUE_OBJECT, .ptr.obj = &obj}};

    switch (type) {
    case KS_IO_DESERIALIZER:
        ks_vector_clear(&io->userdatas);
        ks_vector_clear(&io->states);
        io->seek =0;
        io->indent = 0;
        return ks_io_value(io, methods, prop.value, 0, KS_IO_DESERIALIZER);
    case KS_IO_SERIALIZER:
        ks_io_reset(io);
        return ks_io_value(io, methods, prop.value, 0, KS_IO_SERIALIZER);
    case KS_IO_OTHER_TYPE:
        ks_io_reset(io);
        return ks_io_value(io, methods, prop.value, 0, KS_IO_OTHER_TYPE);
    }
    return false;
}

bool ks_io_serialize_base(const ks_io_methods* methods, const char* file, ks_object_data obj){
    ks_property prop ={.value= (ks_value){.type = KS_VALUE_OBJECT, .ptr.obj = &obj}};

    ks_io* io = ks_io_new();
    bool ret = ks_io_value(io, methods, prop.value, 0, KS_IO_SERIALIZER) && ks_io_write_file(io, file);
    ks_io_free(io);
    return ret;
}

bool ks_io_deserialize_base(const ks_io_methods* methods, const char* file, ks_object_data obj){
    ks_property prop ={.value= (ks_value){.type = KS_VALUE_OBJECT, .ptr.obj = &obj}};

    ks_io*io = ks_io_new();
    ks_io_read_file(io, file);
    bool ret =  ks_io_value(io, methods, prop.value, 0, KS_IO_DESERIALIZER);
    ks_io_free(io);
    return  ret;
}

bool ks_io_deserialize_data_base(const ks_io_methods* methods, u32 length, const char* data, ks_object_data obj){
    ks_property prop ={.value= (ks_value){.type = KS_VALUE_OBJECT, .ptr.obj = &obj}};

    ks_io*io = ks_io_new();
    ks_io_read_string_len(io, length, data);
    bool ret =  ks_io_value(io, methods, prop.value, 0, KS_IO_DESERIALIZER);
    ks_io_free(io);
    return  ret;
}

bool ks_io_other_base(ks_io* io, const ks_io_methods* methods, ks_object_data obj){
    ks_property prop ={.value= (ks_value){.type = KS_VALUE_OBJECT, .ptr.obj = &obj}};

    ks_io_reset(io);
    return ks_io_value(io, methods, prop.value, 0, KS_IO_OTHER_TYPE);
}

bool ks_io_write_file(ks_io* io, const char* file){
    FILE* fp = fopen(file, "wb");
    if(!fp) return false;
    long w = fwrite(io->str->data, sizeof(char), io->str->length, fp);
    fclose(fp);

    return w == io->str->length;
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


ks_io_userdata* ks_io_top_userdata_from (ks_io* io, u32 index){
    if(io->userdatas.length <= index) return NULL;
    return &io->userdatas.data[io->userdatas.length - 1 - index];
}

bool ks_io_push_userdata (ks_io* io, ks_io_userdata userdata){
    ks_vector_push(&io->userdatas, userdata);
    return true;
}

bool ks_io_pop_userdata(ks_io* io){
    ks_vector_pop(&io->userdatas);
    return true;
}

ks_io_userdata* ks_io_top_state_from (ks_io* io, u32 index){
    if(io->states.length <= index) return NULL;
    return &io->states.data[io->states.length - 1 - index];
}

bool ks_io_push_state(ks_io* io, ks_io_userdata userdata){
    ks_vector_push(&io->states, userdata);
    return true;
}

bool ks_io_pop_state(ks_io* io){
    ks_vector_pop(&io->states);
    return true;
}

bool ks_io_magic_number(ks_io* io, const ks_io_methods* methods, const char* data){
    ks_value  val={.ptr={ .str=data }, .type = KS_VALUE_MAGIC_NUMBER};
    return methods->value(io, methods, val, 0);
}

KS_FORCEINLINE static bool ks_impl_func(ks_io_array_begin)(ks_io* io, const ks_io_methods* methods, ks_array_data* array, u32 offset, ks_io_serial_type serial_type){

    if(serial_type == KS_IO_DESERIALIZER && !array->fixed_length){
        void* ptr = array->value.ptr.data;
        if(array->value.type == KS_VALUE_OBJECT){
            ks_object_data *obj = ptr;
            ptr = obj->data;
            // TODO: add offset
            obj->data = *(void**)ptr = array->length == 0 ? NULL : calloc(array->length, array->elem_size);

        } else if(array->value.type == KS_VALUE_ARRAY){
            // TODO
        } else {
            // TODO: add offset
            array->value.ptr.data = *(void**)(ptr) = array->length == 0 ? NULL : calloc(array->length, array->elem_size);
        }
    }

    if(serial_type != KS_IO_DESERIALIZER && !array->fixed_length){
        void* ptr = array->value.ptr.data;
        if(array->value.type == KS_VALUE_OBJECT){
            ks_object_data *obj = ptr;
            // TODO : add offset
            obj->data = *(void**)obj->data;
            array->value.ptr.obj = obj;
        } else if(array->value.type == KS_VALUE_ARRAY){
            // TODO
        } else {
            array->value.ptr.data = *(array->value.ptr.vpp +  offset);
        }
    }


    if(! methods->array_begin(io, methods, *array)) return false;

    return true;
}

ks_impl_branch(bool, ks_io_array_begin, (ks_io* io, const ks_io_methods* methods, ks_array_data* array, u32 offset), io, methods, array, offset)


KS_FORCEINLINE static bool ks_impl_func(ks_io_string)(ks_io* io, const ks_io_methods* methods, ks_array_data array, u32 offset, ks_io_serial_type serial_type){
    ks_string * str = ks_string_new();
    if(serial_type != KS_IO_DESERIALIZER){
        if(array.length == KS_STRING_UNKNOWN_LENGTH){
            void * ptr =!array.fixed_length  ? *array.value.ptr.vpp: array.value.ptr.data;
            if(ptr != NULL ){
                array.length = strlen(ptr) + 1;
            }
        }
        ks_string_set_n(str, array.length, !array.fixed_length  ? *array.value.ptr.vpp: array.value.ptr.data);
    }
    if(!methods->string(io, methods, array,  str)) return false;

    if(serial_type == KS_IO_DESERIALIZER){
        if(!array.fixed_length){
            // + 1 for end of string
            array.value.ptr.data = *(array.value.ptr.vpp + offset) = str->length == 0 ? NULL : calloc(str->length+1, array.elem_size);
        }
        else {
            memset(array.value.ptr.data, 0, array.length*array.elem_size);
        }

        memcpy(array.value.ptr.data, str->data, str->length);
    }
    ks_string_free(str);

    return true;
}

KS_FORCEINLINE static bool ks_impl_func(ks_io_array)(ks_io* io, const ks_io_methods* methods, ks_array_data array, u32 offset, ks_io_serial_type serial_type){

    if(!ks_io_array_begin(io, methods, &array, offset, serial_type)) return false;

    for(u32 i=0, e=array.length; i< e; i++){
        if(! methods->array_elem(io, methods, array, i)) return false;
    }

    return ks_io_array_end(io, methods, &array, offset);
}

KS_INLINE bool ks_io_array_end(ks_io* io, const ks_io_methods* methods, ks_array_data* array, u32 offset){
    if(! methods->array_end(io, methods, *array)) return false;

    return true;
}

bool ks_io_object(ks_io* io, const ks_io_methods* methods, ks_object_data obj, u32 offset){
    return methods->object(io, methods, obj, offset);
}

ks_impl_branch(bool, ks_io_string, (ks_io* io, const ks_io_methods* methods, ks_array_data array, u32 index), io, methods, array, index)
ks_impl_branch(bool, ks_io_array, (ks_io* io, const ks_io_methods* methods, ks_array_data array, u32 index), io, methods, array, index)


KS_FORCEINLINE static bool ks_impl_func(ks_io_value)(ks_io* io, const ks_io_methods* methods, ks_value value, u32 index, ks_io_serial_type serial_type){
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
        ret = methods->value(io, methods, value, index);
        break;
    case KS_VALUE_ARRAY:{
        ks_array_data* array = value.ptr.arr;
        if(array->value.type == KS_VALUE_STRING_ELEM) {
            ret = ks_io_string(io, methods, *array, index, serial_type);
        }
        else {
            ret = ks_io_array(io, methods, *array, index, serial_type);
        }
        break;
    }
    case KS_VALUE_OBJECT:
        ret = ks_io_object(io, methods, *(ks_object_data*)value.ptr.obj, index);
        break;
    default:
        break;
    }
    return ret;
}

ks_impl_branch(bool, ks_io_value, (ks_io* io, const ks_io_methods* methods, ks_value value, u32 index), io, methods, value, index)



KS_FORCEINLINE static bool ks_impl_func(ks_io_property)(ks_io* io, const ks_io_methods* methods,  ks_property prop, ks_io_serial_type serial_type){
    u32 prop_length = methods->key(io, methods, prop.name,  true);
    if(prop_length == 0){
        return false;
    }
    return ks_io_value(io, methods, prop.value, 0, serial_type);
}

ks_impl_branch(bool,  ks_io_property, (ks_io* io, const ks_io_methods* methods, ks_property prop), io, methods, prop)




ks_object_data* ks_set_object_data( ks_object_data* data , ks_object_data value ){
    *data = value;
    return data;
}

ks_array_data* ks_set_array_data( ks_array_data* data , ks_array_data value ){
    *data = value;
    return data;
}
