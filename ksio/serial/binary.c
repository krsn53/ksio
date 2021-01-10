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

KS_INLINE u32 ks_io_fixed_bin_len(ks_io* io, u32 length, const char* str, bool serialize){
    if(!serialize){
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

KS_INLINE u32 ks_io_fixed_bin(ks_io* io, const char* str, bool serialize){
    if(!serialize){
        return ks_io_fixed_bin_len(io, ks_string_first_c_of(io->str, io->seek, 0) + 1, str, serialize);
    }
    else {
        return ks_io_fixed_bin_len(io, strlen(str) + 1, str, serialize);
    }

}

KS_INLINE bool ks_io_value_bin(ks_io* io, u32 length, char* c, bool swap_endian,  bool serialize){
    if(!serialize){
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

KS_INLINE bool ks_io_key_binary(ks_io* io, const ks_io_funcs* funcs, const char* name, bool fixed, bool swap_endian, bool serialize){
    if(fixed) {
        return true;
    }
    u32 step = ks_io_fixed_bin(io, name, serialize);
    return step != 0;
}

KS_INLINE bool ks_io_string_binary(ks_io* io, const ks_io_funcs* funcs, ks_array_data array, ks_string* str, bool swap_endian, bool serialize){
    if(!serialize){
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

KS_INLINE bool ks_io_value_binary(ks_io* io, const ks_io_funcs* funcs, ks_value value,  u32 offset, bool swap_endian, bool serialize){
    switch (value.type) {
    case KS_VALUE_MAGIC_NUMBER:{
        if(!serialize){
            if(memcmp(value.ptr.str, io->str->data + io->seek, strlen(value.ptr.str)) != 0){
                char c[5] = { 0 };
                memcpy(c, io->str->data + io->seek, 4);
                ks_error("Excepted magic number \"%s\", detected \"%s\"", value.ptr.str, c);
               return false;
            }
            io->seek += 4;
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
        return ks_io_value_bin(io, sizeof(*v.c), *v.c, swap_endian, serialize);
    }
    case KS_VALUE_U32:{
        union {
            u32* u;
            char(* c)[sizeof(u32)];
        } v = {.u = value.ptr.u32v + offset};
        return ks_io_value_bin(io, sizeof(*v.c), *v.c, swap_endian, serialize);
    }
    case KS_VALUE_U16:{
        union {
            u16* u;
            char(* c)[sizeof(u16)];
        } v = {.u = value.ptr.u16v + offset};
        return ks_io_value_bin(io, sizeof(*v.c), *v.c, swap_endian, serialize);
    }
    case KS_VALUE_U8:{
        union {
            u8* u;
            char(* c)[sizeof(u8)];
        } v = {.u = value.ptr.u8v + offset};
        return ks_io_value_bin(io, sizeof(*v.c), *v.c, swap_endian, serialize);
    }
    case KS_VALUE_I64:{
        union {
            i64* u;
            char(* c)[sizeof(i64)];
        } v = {.u = value.ptr.i64v + offset};
        return ks_io_value_bin(io, sizeof(*v.c), *v.c, swap_endian, serialize);
    }
    case KS_VALUE_I32:{
        union {
            i32* u;
            char(* c)[sizeof(i32)];
        } v = {.u = value.ptr.i32v + offset};
        return ks_io_value_bin(io, sizeof(*v.c), *v.c, swap_endian, serialize);
    }
    case KS_VALUE_I16:{
        union {
            i16* u;
            char(* c)[sizeof(i16)];
        } v = {.u = value.ptr.i16v + offset};
        return ks_io_value_bin(io, sizeof(*v.c), *v.c, swap_endian, serialize);
    }
    case KS_VALUE_I8:{
        union {
            i8* u;
            char(* c)[sizeof(i8)];
        } v = {.u = value.ptr.i8v + offset};
        return ks_io_value_bin(io, sizeof(*v.c), *v.c, swap_endian, serialize);
    }
    default:
        return false;
    }
    return true;
}

KS_INLINE bool ks_io_array_begin_binary(ks_io* io, const ks_io_funcs* funcs,  ks_array_data arr, bool swap_endian, bool serialize){
    return true;
}

KS_INLINE bool ks_io_array_elem_binary(ks_io* io,  const ks_io_funcs* funcs, ks_array_data arr, u32 index, bool swap_endian, bool serialize){
    return ks_io_value(io, funcs, arr.value, index, serialize);
}
KS_INLINE bool ks_io_array_end_binary(ks_io* io, const ks_io_funcs* funcs,  ks_array_data arr, bool swap_endian, bool serialize){
    return true;
}

KS_INLINE bool ks_io_object_binary(ks_io* io, const ks_io_funcs* funcs,  ks_object_data obj, u32 offset, bool swap_endian, bool serialize){
    return (serialize ? obj.serializer : obj.deserializer)(io, funcs, obj.data, offset);
}

KS_INLINE bool ks_io_key_binary_little_endian(ks_io* io, const ks_io_funcs* funcs, const char* name, bool fixed, bool serialize){
    return ks_io_key_binary(io, funcs, name, fixed, !little_endian(), serialize);
}
KS_INLINE bool ks_io_value_binary_little_endian(ks_io* io, const ks_io_funcs* funcs, ks_value value, u32 offset,  bool serialize){
    return ks_io_value_binary(io, funcs, value, offset, !little_endian(), serialize);
}
KS_INLINE bool ks_io_string_binary_little_endian(ks_io* io, const ks_io_funcs* funcs, ks_array_data array, ks_string* str, bool serialize){
    return ks_io_string_binary(io, funcs, array, str, !little_endian(), serialize);
}
KS_INLINE bool ks_io_array_begin_binary_little_endian(ks_io* io, const ks_io_funcs* funcs,  ks_array_data arr, bool serialize){
    return ks_io_array_begin_binary(io, funcs, arr, !little_endian(), serialize);
}
KS_INLINE bool ks_io_array_elem_binary_little_endian(ks_io* io,  const ks_io_funcs* funcs, ks_array_data arr, u32 index, bool serialize){
    return ks_io_array_elem_binary(io, funcs, arr, index, !little_endian(), serialize);
}
KS_INLINE bool ks_io_array_end_binary_little_endian(ks_io* io, const ks_io_funcs* funcs,  ks_array_data arr, bool serialize){
    return ks_io_array_end_binary(io, funcs, arr, !little_endian(), serialize);
}
KS_INLINE bool ks_io_object_binary_little_endian(ks_io* io, const ks_io_funcs* funcs,  ks_object_data obj, u32 offset,  bool serialize){
    return ks_io_object_binary(io, funcs, obj, offset, !little_endian(), serialize);
}

KS_INLINE bool ks_io_key_binary_big_endian(ks_io* io, const ks_io_funcs* funcs, const char* name, bool fixed, bool serialize){
    return ks_io_key_binary(io, funcs, name, fixed, little_endian(), serialize);
}
KS_INLINE bool ks_io_value_binary_big_endian(ks_io* io, const ks_io_funcs* funcs, ks_value value, u32 offset,  bool serialize){
    return ks_io_value_binary(io, funcs, value, offset, little_endian(), serialize);
}
KS_INLINE bool ks_io_string_binary_big_endian(ks_io* io, const ks_io_funcs* funcs, ks_array_data array, ks_string*  str, bool serialize){
    return ks_io_string_binary(io, funcs, array, str, little_endian(), serialize);
}
KS_INLINE bool ks_io_array_begin_binary_big_endian(ks_io* io, const ks_io_funcs* funcs,  ks_array_data arr, bool serialize){
    return ks_io_array_begin_binary(io, funcs, arr, little_endian(), serialize);
}
KS_INLINE bool ks_io_array_elem_binary_big_endian(ks_io* io,  const ks_io_funcs* funcs, ks_array_data arr, u32 index, bool serialize){
    return ks_io_array_elem_binary(io, funcs, arr, index, little_endian(), serialize);
}
KS_INLINE bool ks_io_array_end_binary_big_endian(ks_io* io, const ks_io_funcs* funcs,  ks_array_data arr, bool serialize){
    return ks_io_array_end_binary(io, funcs, arr, little_endian(), serialize);
}
KS_INLINE bool ks_io_object_binary_big_endian(ks_io* io, const ks_io_funcs* funcs,  ks_object_data obj, u32 offset, bool serialize){
    return ks_io_object_binary(io, funcs, obj, offset, little_endian(), serialize);
}


ks_io_funcs_impl(binary_little_endian)
ks_io_funcs_impl(binary_big_endian)
