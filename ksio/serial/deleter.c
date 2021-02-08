#include "deleter.h"
#include "./clike.h"

#if(KS_PRINT_DELETE_LOG)
static KS_INLINE const char* print_pushed_property_name(ks_io* io, u32 len, char arr[]){
    if(io->states.length == 0){
        return  "root";
    }
    else {
        u32 s=0;
        for(i32 i=io->states.length-1; i>=0; i--){

            if(ks_io_top_state_from(io, i)->type == KS_USER_DATA_STRING){
                if(i != (i32)io->states.length-1){
                    s += snprintf(arr+s, len-s, ".");
                }
                s += snprintf(arr+s, len-s, "%s", ks_io_top_state_from(io, i)->v.str );
            }
            else{
                s += snprintf(arr+s, len-s, "[%ld]", ks_io_top_state_from(io, i)->v.val );
            }
        }
    }
    return arr;
}
#endif

KS_INLINE bool ks_io_key_deleter(ks_io* io, const ks_io_methods* methods, const char* name, bool fixed, ks_io_serial_type serial_type){
    #if(KS_PRINT_DELETE_LOG)
        ks_io_push_state(io, (ks_io_userdata){.type =KS_USER_DATA_STRING ,.v.str = name});
    #endif
    return true;
}
KS_INLINE bool ks_io_string_deleter (ks_io* io, const ks_io_methods* methods, ks_array_data array, ks_string* str, ks_io_serial_type serial_type){
    if(!array.fixed_length && array.length != 0 &&  *array.value.ptr.vpp != NULL) {
        free(*array.value.ptr.vpp);
    }
    #if(KS_PRINT_DELETE_LOG)
        if(!array.fixed_length && *array.value.ptr.vpp != NULL) {
            char arr[256];
            ks_info("Pointer to a string \"%s\" deleted", print_pushed_property_name(io, 256, arr));
        }
        ks_io_pop_state(io);
    #endif
    return true;
}
KS_INLINE bool ks_io_value_deleter  (ks_io* io, const ks_io_methods* methods, ks_value value, u32 offset,  ks_io_serial_type serial_type){
    #if(KS_PRINT_DELETE_LOG)
        if(value.type != KS_VALUE_MAGIC_NUMBER)ks_io_pop_state(io);
    #endif
    return true;
}
KS_INLINE bool ks_io_array_begin_deleter  (ks_io* io, const ks_io_methods* methods,  ks_array_data arr,  ks_io_serial_type serial_type){
    return true;
}
KS_INLINE bool ks_io_array_elem_deleter(ks_io* io,  const ks_io_methods* methods, ks_array_data arr, u32 index, ks_io_serial_type serial_type){
    #if(KS_PRINT_DELETE_LOG)
        ks_io_push_state(io, (ks_io_userdata){.type = KS_USERDATA_VALUE, .v.val = index});
    #endif
    return ks_io_value(io, methods, arr.value, index, serial_type);
}
KS_INLINE bool ks_io_array_end_deleter (ks_io* io, const ks_io_methods* methods,  ks_array_data arr,  ks_io_serial_type serial_type){
    if(!arr.fixed_length && arr.length != 0) {
        if(arr.value.type == KS_VALUE_OBJECT){
            free(arr.value.ptr.obj->data);
        } else {
            free(arr.value.ptr.data);
        }

        int i =0;
    }
    #if(KS_PRINT_DELETE_LOG)
        if(!arr.fixed_length && arr.value.ptr.data != NULL) {
            char arr[256];
            ks_info("Pointer to an array \"%s\" deleted", print_pushed_property_name(io, 256, arr));
        }

        ks_io_pop_state(io);
    #endif
    return true;
}
KS_INLINE bool ks_io_object_deleter (ks_io* io, const ks_io_methods* methods,  ks_object_data obj, u32 offset, ks_io_serial_type serial_type){
    bool ret = obj.func(io, methods, obj.data, offset);
    if(io->states.length != 0)ks_io_pop_state(io);
    return ret;
}

ks_io_methods_impl_other(deleter, KS_SERIAL_OTHERS);
