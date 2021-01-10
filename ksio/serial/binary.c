#include "./binary.h"

static KS_INLINE bool little_endian(){
    const union {
        char c[sizeof(int)];
        int i;
    } v = { .c={0x1, 0x0, 0x0, 0x0} };

    return v.i == 1;
}

union endian_checker{
    char c[sizeof(int)];
    int i;
};

KS_INLINE u32 ks_io_fixed_bin_len(ks_io* io, u32 length, const char* str, ks_io_serial_type serial_type){
    if(serial_type == KS_IO_DESERIALIZER){
        if(memcmp(io->str->data + io->seek, str, length) == 0){
            io->seek += length;
            return length;
        }
        return 0;
    }
    else {
        ks_string_add_n(io->str, length, str);
    }
    return 1;
}

KS_INLINE u32 ks_io_fixed_bin(ks_io* io, const char* str, ks_io_serial_type serial_type){
    if(serial_type == KS_IO_DESERIALIZER){
        return ks_io_fixed_bin_len(io, ks_string_first_c_of(io->str, io->seek, 0) + 1, str, serial_type);
    }
    else {
        return ks_io_fixed_bin_len(io, strlen(str) + 1, str, serial_type);
    }

}

KS_INLINE bool ks_io_value_bin(ks_io* io, u32 length, char* c, bool swap_endian,  ks_io_serial_type serial_type){
    if(serial_type == KS_IO_DESERIALIZER){
        if(io->seek + length > io->str->length) {
            ks_error("Unexcepted file end");
            return false;
        }
        if(!swap_endian){
            memcpy(c, io->str->data + io->seek, length);
        }
        else {
            for(i32 i= 0; i < (i32)length; i++){
                c[i] = io->str->data[io->seek + length - 1 - i];
            }
        }
        io->seek += length;
    }
    else {
        if(!swap_endian){
            ks_string_add_n(io->str, length, c);
        }
        else {
            ks_string_reserve(io->str, io->str->length + length);
            for(i32 i=length-1; i>= 0; i--){
                ks_string_add_c(io->str, c[i]);
            }
        }
    }
    return true;
}

KS_INLINE bool ks_io_key_binary(ks_io* io, const ks_io_methods* methods, const char* name, bool fixed, bool swap_endian, ks_io_serial_type serial_type){
    if(fixed) {
        return true;
    }
    u32 step = ks_io_fixed_bin(io, name, serial_type);
    return step != 0;
}

KS_INLINE bool ks_io_string_binary(ks_io* io, const ks_io_methods* methods, ks_array_data array, ks_string* str, bool swap_endian, ks_io_serial_type serial_type){
    if(serial_type == KS_IO_DESERIALIZER){
        u32 len, seek;
        if(array.length == KS_STRING_UNKNOWN_LENGTH){
            len = ks_string_first_c_of(io->str, io->seek, 0);
            seek = len+1;
        }
        else if(array.fixed_length){
            len = MIN(ks_string_first_c_of(io->str, io->seek, 0), array.length);
            seek = MIN(array.length, len+1);
        }
        else {
            len = seek = array.length;
        }
        if(io->seek + len > io->str->length) return false;
        ks_string_set_n(str, len, io->str->data + io->seek);
        io->seek += seek;
    }
    else {
        if(array.fixed_length){
            array.length = MIN(strlen(str->data)+1, array.length);
        }
        ks_string_add_n(io->str, array.length, str->data);
    }
    return true;

}

KS_INLINE bool ks_io_value_binary(ks_io* io, const ks_io_methods* methods, ks_value value,  u32 offset, bool swap_endian, ks_io_serial_type serial_type){
    switch (value.type) {
    case KS_VALUE_MAGIC_NUMBER:{
        if(serial_type == KS_IO_DESERIALIZER){
            u32 len = strlen(value.ptr.str);
            if(memcmp(value.ptr.str, io->str->data + io->seek, len) != 0){
                char c[5] = { 0 }; // normaly bytes of magic number is 4 ???
                memcpy(c, io->str->data + io->seek, strlen(value.ptr.str));
                ks_error("Excepted magic number \"%s\", detected \"%s\"", value.ptr.str, c);
               return false;
            }
            io->seek += len;
        } else {
            ks_string_add_n(io->str, strlen(value.ptr.str), value.ptr.str);
        }

        break;
    }
    case KS_VALUE_U64:{
        union {
            u64* u;
            char(* c)[sizeof(u64)];
        } v = {.u = value.ptr.u64v + offset};
        return ks_io_value_bin(io, sizeof(*v.c), *v.c, swap_endian, serial_type);
    }
    case KS_VALUE_U32:{
        union {
            u32* u;
            char(* c)[sizeof(u32)];
        } v = {.u = value.ptr.u32v + offset};
        return ks_io_value_bin(io, sizeof(*v.c), *v.c, swap_endian, serial_type);
    }
    case KS_VALUE_U16:{
        union {
            u16* u;
            char(* c)[sizeof(u16)];
        } v = {.u = value.ptr.u16v + offset};
        return ks_io_value_bin(io, sizeof(*v.c), *v.c, swap_endian, serial_type);
    }
    case KS_VALUE_U8:{
        union {
            u8* u;
            char(* c)[sizeof(u8)];
        } v = {.u = value.ptr.u8v + offset};
        return ks_io_value_bin(io, sizeof(*v.c), *v.c, swap_endian, serial_type);
    }
    case KS_VALUE_I64:{
        union {
            i64* u;
            char(* c)[sizeof(i64)];
        } v = {.u = value.ptr.i64v + offset};
        return ks_io_value_bin(io, sizeof(*v.c), *v.c, swap_endian, serial_type);
    }
    case KS_VALUE_I32:{
        union {
            i32* u;
            char(* c)[sizeof(i32)];
        } v = {.u = value.ptr.i32v + offset};
        return ks_io_value_bin(io, sizeof(*v.c), *v.c, swap_endian, serial_type);
    }
    case KS_VALUE_I16:{
        union {
            i16* u;
            char(* c)[sizeof(i16)];
        } v = {.u = value.ptr.i16v + offset};
        return ks_io_value_bin(io, sizeof(*v.c), *v.c, swap_endian, serial_type);
    }
    case KS_VALUE_I8:{
        union {
            i8* u;
            char(* c)[sizeof(i8)];
        } v = {.u = value.ptr.i8v + offset};
        return ks_io_value_bin(io, sizeof(*v.c), *v.c, swap_endian, serial_type);
    }
    default:
        return false;
    }
    return true;
}

KS_INLINE bool ks_io_array_begin_binary(ks_io* io, const ks_io_methods* methods,  ks_array_data arr, bool swap_endian, ks_io_serial_type serial_type){
    return true;
}

KS_INLINE bool ks_io_array_elem_binary(ks_io* io,  const ks_io_methods* methods, ks_array_data arr, u32 index, bool swap_endian, ks_io_serial_type serial_type){
    return ks_io_value(io, methods, arr.value, index, serial_type);
}
KS_INLINE bool ks_io_array_end_binary(ks_io* io, const ks_io_methods* methods,  ks_array_data arr, bool swap_endian, ks_io_serial_type serial_type){
    return true;
}

KS_INLINE bool ks_io_object_binary(ks_io* io, const ks_io_methods* methods,  ks_object_data obj, u32 offset, bool swap_endian, ks_io_serial_type serial_type){
    return (serial_type == KS_IO_DESERIALIZER ? obj.deserializer : obj.serializer)(io, methods, obj.data, offset);
}

KS_INLINE bool ks_io_key_binary_little_endian(ks_io* io, const ks_io_methods* methods, const char* name, bool fixed, ks_io_serial_type serial_type){
    return ks_io_key_binary(io, methods, name, fixed, !little_endian(), serial_type);
}
KS_INLINE bool ks_io_value_binary_little_endian(ks_io* io, const ks_io_methods* methods, ks_value value, u32 offset,  ks_io_serial_type serial_type){
    return ks_io_value_binary(io, methods, value, offset, !little_endian(), serial_type);
}
KS_INLINE bool ks_io_string_binary_little_endian(ks_io* io, const ks_io_methods* methods, ks_array_data array, ks_string* str, ks_io_serial_type serial_type){
    return ks_io_string_binary(io, methods, array, str, !little_endian(), serial_type);
}
KS_INLINE bool ks_io_array_begin_binary_little_endian(ks_io* io, const ks_io_methods* methods,  ks_array_data arr, ks_io_serial_type serial_type){
    return ks_io_array_begin_binary(io, methods, arr, !little_endian(), serial_type);
}
KS_INLINE bool ks_io_array_elem_binary_little_endian(ks_io* io,  const ks_io_methods* methods, ks_array_data arr, u32 index, ks_io_serial_type serial_type){
    return ks_io_array_elem_binary(io, methods, arr, index, !little_endian(), serial_type);
}
KS_INLINE bool ks_io_array_end_binary_little_endian(ks_io* io, const ks_io_methods* methods,  ks_array_data arr, ks_io_serial_type serial_type){
    return ks_io_array_end_binary(io, methods, arr, !little_endian(), serial_type);
}
KS_INLINE bool ks_io_object_binary_little_endian(ks_io* io, const ks_io_methods* methods,  ks_object_data obj, u32 offset,  ks_io_serial_type serial_type){
    return ks_io_object_binary(io, methods, obj, offset, !little_endian(), serial_type);
}

KS_INLINE bool ks_io_key_binary_big_endian(ks_io* io, const ks_io_methods* methods, const char* name, bool fixed, ks_io_serial_type serial_type){
    return ks_io_key_binary(io, methods, name, fixed, little_endian(), serial_type);
}
KS_INLINE bool ks_io_value_binary_big_endian(ks_io* io, const ks_io_methods* methods, ks_value value, u32 offset,  ks_io_serial_type serial_type){
    return ks_io_value_binary(io, methods, value, offset, little_endian(), serial_type);
}
KS_INLINE bool ks_io_string_binary_big_endian(ks_io* io, const ks_io_methods* methods, ks_array_data array, ks_string*  str, ks_io_serial_type serial_type){
    return ks_io_string_binary(io, methods, array, str, little_endian(), serial_type);
}
KS_INLINE bool ks_io_array_begin_binary_big_endian(ks_io* io, const ks_io_methods* methods,  ks_array_data arr, ks_io_serial_type serial_type){
    return ks_io_array_begin_binary(io, methods, arr, little_endian(), serial_type);
}
KS_INLINE bool ks_io_array_elem_binary_big_endian(ks_io* io,  const ks_io_methods* methods, ks_array_data arr, u32 index, ks_io_serial_type serial_type){
    return ks_io_array_elem_binary(io, methods, arr, index, little_endian(), serial_type);
}
KS_INLINE bool ks_io_array_end_binary_big_endian(ks_io* io, const ks_io_methods* methods,  ks_array_data arr, ks_io_serial_type serial_type){
    return ks_io_array_end_binary(io, methods, arr, little_endian(), serial_type);
}
KS_INLINE bool ks_io_object_binary_big_endian(ks_io* io, const ks_io_methods* methods,  ks_object_data obj, u32 offset, ks_io_serial_type serial_type){
    return ks_io_object_binary(io, methods, obj, offset, little_endian(), serial_type);
}


ks_io_methods_impl_rw(binary_little_endian)
ks_io_methods_impl_rw(binary_big_endian)
