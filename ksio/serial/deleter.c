#include "deleter.h"
#include "./clike.h"

#if(KS_PRINT_DELETE_LOG)
static KS_INLINE void print_pushed_property_name(ks_io* io){
    if(io->states.length == 0){
        ks_io_text(io, "\"root\"", KS_IO_SERIALIZER);
    }
    else {
        ks_io_text(io, "\"", KS_IO_SERIALIZER);
        ks_io_text(io, ks_io_top_state_from(io, 0)->str, KS_IO_SERIALIZER);
        ks_io_text(io, "\"", KS_IO_SERIALIZER);
    }
}

static KS_INLINE void print_type_value(ks_io* io, ks_value value){
    switch (value.type) {
    case KS_VALUE_U64:
        ks_io_text(io,  "u64", KS_IO_SERIALIZER);
        break;
    case KS_VALUE_U32:
        ks_io_text(io,  "u32", KS_IO_SERIALIZER);
        break;
    case KS_VALUE_U16:
        ks_io_text(io,  "u16", KS_IO_SERIALIZER);
        break;
    case KS_VALUE_U8:
        ks_io_text(io,  "u8", KS_IO_SERIALIZER);
        break;
    case KS_VALUE_I64:
        ks_io_text(io,  "i64", KS_IO_SERIALIZER);
        break;
    case KS_VALUE_I32:
        ks_io_text(io,  "i32", KS_IO_SERIALIZER);
        break;
    case KS_VALUE_I16:
        ks_io_text(io,  "i16", KS_IO_SERIALIZER);
        break;
    case KS_VALUE_I8:
        ks_io_text(io,  "i8", KS_IO_SERIALIZER);
        break;
    case KS_VALUE_OBJECT:
        ks_io_text(io,  value.ptr.obj->type, KS_IO_SERIALIZER);
        break;
    case KS_VALUE_ARRAY:
    default:
        // TODO
        break;
    }
}
#endif

KS_INLINE bool ks_io_key_deleter(ks_io* io, const ks_io_funcs* funcs, const char* name, bool fixed, ks_io_serial_type serial_type){
    #if(KS_PRINT_DELETE_LOG)
        ks_io_push_state(io, (ks_io_userdata){.str = name});
    #endif
    return true;
}
KS_INLINE bool ks_io_string_deleter (ks_io* io, const ks_io_funcs* funcs, ks_array_data array, ks_string* str, ks_io_serial_type serial_type){
    if(!array.fixed_length && array.length != 0 &&  *array.value.ptr.vpp != NULL) {
        free(*array.value.ptr.vpp);
    }
    #if(KS_PRINT_DELETE_LOG)
        if(!array.fixed_length && *array.value.ptr.vpp != NULL) {
            ks_io_print_indent(io, '\t', KS_IO_SERIALIZER);
            ks_io_text(io, "Pointer to a string ", KS_IO_SERIALIZER);
            print_pushed_property_name(io);
            ks_io_text(io, " deleted", KS_IO_SERIALIZER);
            ks_io_print_endl(io, KS_IO_SERIALIZER);
        }
        ks_io_pop_state(io);
    #endif
    return true;
}
KS_INLINE bool ks_io_value_deleter  (ks_io* io, const ks_io_funcs* funcs, ks_value value, u32 offset,  ks_io_serial_type serial_type){
    #if(KS_PRINT_DELETE_LOG)
        if(value.type != KS_VALUE_MAGIC_NUMBER)ks_io_pop_state(io);
    #endif
    return true;
}
KS_INLINE bool ks_io_array_begin_deleter  (ks_io* io, const ks_io_funcs* funcs,  ks_array_data arr,  ks_io_serial_type serial_type){
    #if(KS_PRINT_DELETE_LOG)
        ks_io_print_indent(io, '\t', KS_IO_SERIALIZER);
        ks_io_text(io, "Enter to array(", KS_IO_SERIALIZER);
        print_type_value(io, arr.value);
        ks_io_text(io,  ") ", KS_IO_SERIALIZER);
        print_pushed_property_name(io);
        ks_io_text(io, ":", KS_IO_SERIALIZER);
        ks_io_print_endl(io, KS_IO_SERIALIZER);

        io->indent ++;
    #endif
    return true;
}
KS_INLINE bool ks_io_array_elem_deleter(ks_io* io,  const ks_io_funcs* funcs, ks_array_data arr, u32 index, ks_io_serial_type serial_type){
    #if(KS_PRINT_DELETE_LOG)
        char array_indexed [16];
        snprintf(array_indexed, 10, "[%d]", index);
        ks_io_push_state(io, (ks_io_userdata){.str=array_indexed});
    #endif
    ks_object_data obj;
    if(!arr.fixed_length && arr.length != 0) {
        if(arr.value.type == KS_VALUE_OBJECT){
            obj = *arr.value.ptr.obj;
            obj.data = *(void**)arr.value.ptr.obj->data;
            arr.value.ptr.obj = &obj;
        } else {
             arr.value.ptr.data = *arr.value.ptr.vpp;
        }

    }
    return ks_io_value(io, funcs, arr.value, index, serial_type);
}
KS_INLINE bool ks_io_array_end_deleter (ks_io* io, const ks_io_funcs* funcs,  ks_array_data arr,  ks_io_serial_type serial_type){
    if(!arr.fixed_length && arr.length != 0) {
        if(arr.value.type == KS_VALUE_OBJECT){
            free(*(void**)arr.value.ptr.obj->data);
        } else {
            free(*arr.value.ptr.vpp);
        }

    }
    #if(KS_PRINT_DELETE_LOG)
        io->indent --;


        ks_io_print_indent(io, '\t', KS_IO_SERIALIZER);
        ks_io_text(io, "Exit to array", KS_IO_SERIALIZER);

        if(!arr.fixed_length && arr.value.ptr.data != NULL) {
            ks_io_text(io, " and Pointer to an array deleted", KS_IO_SERIALIZER);
        }

        ks_io_print_endl(io, KS_IO_SERIALIZER);

        ks_io_pop_state(io);
    #endif
    return true;
}
KS_INLINE bool ks_io_object_deleter (ks_io* io, const ks_io_funcs* funcs,  ks_object_data obj, u32 offset, ks_io_serial_type serial_type){
    #if(KS_PRINT_DELETE_LOG)
        ks_io_print_indent(io, '\t', KS_IO_SERIALIZER);
        ks_io_text(io, "Enter to object(", KS_IO_SERIALIZER);
        ks_io_text(io, obj.type, KS_IO_SERIALIZER);
        ks_io_text(io, ") ", KS_IO_SERIALIZER);
        print_pushed_property_name(io);
        ks_io_text(io, " :", KS_IO_SERIALIZER);
        ks_io_print_endl(io, KS_IO_SERIALIZER);
        io->indent ++;
    #endif
    bool ret = obj.other(io, funcs, obj.data, offset);
    #if(KS_PRINT_DELETE_LOG)
        io->indent --;
        ks_io_print_indent(io, '\t', KS_IO_SERIALIZER);
        ks_io_text(io, "Exit to object", KS_IO_SERIALIZER);
        ks_io_print_endl(io, KS_IO_SERIALIZER);
    #endif
    return ret;
}

ks_io_funcs_impl_other(deleter);
