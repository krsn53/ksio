#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h> // NULL
#include "./string.h"
#include "./vector.h"
#include "./logger.h"

typedef struct ks_io_methods ks_io_methods;

typedef union ks_io_userdata{
    i64         val;
    void*       ptr;
    const char* str;
} ks_io_userdata;

typedef struct ks_io_userdata_list{
     ks_io_userdata     *data;
     u32                length;
     u32                capacity;
}ks_io_userdata_list;


typedef struct ks_io{
    ks_string               *str;
    ks_io_userdata_list     userdatas;
    ks_io_userdata_list     states;
    u32                     seek;
    u32                     indent;
}ks_io;

typedef struct ks_object_data ks_object_data;
typedef struct ks_array_data ks_array_data;
typedef struct ks_value ks_value;
typedef union ks_value_ptr ks_value_ptr;

typedef enum ks_io_sereal_type{
    KS_IO_DESERIALIZER,
    KS_IO_SERIALIZER,
    KS_IO_OTHER_TYPE,
}ks_io_serial_type;



typedef enum ks_value_type{
    KS_VALUE_MAGIC_NUMBER,
    KS_VALUE_U64,
    KS_VALUE_U32,
    KS_VALUE_U16,
    KS_VALUE_U8,
    KS_VALUE_I64,
    KS_VALUE_I32,
    KS_VALUE_I16,
    KS_VALUE_I8,
    KS_VALUE_STRING_ELEM,
    KS_VALUE_OBJECT,
    KS_VALUE_ARRAY,
}ks_value_type ;

typedef struct ks_io_methods {
    bool        (* key)            (ks_io*, const ks_io_methods*, const char*, bool);
    bool        (* value)          (ks_io*, const ks_io_methods*, ks_value , u32);
    bool        (* string)         (ks_io*, const ks_io_methods*, ks_array_data , ks_string* );
    bool        (* array_begin)    (ks_io*, const ks_io_methods*, ks_array_data);
    bool        (* array_elem)     (ks_io*,const ks_io_methods*,  ks_array_data, u32 );
    bool        (* array_end)      (ks_io*, const ks_io_methods*, ks_array_data);
    bool        (* object)         (ks_io*, const ks_io_methods*, ks_object_data, u32);
}ks_io_methods;

typedef u32     (* ks_object_func)  (ks_io*, const ks_io_methods*, void*,  u32);


typedef union ks_value_ptr{
    u8                      *u8v;
    u16                     *u16v;
    u32                     *u32v;
    u64                     *u64v;
    i8                      *i8v;
    i16                     *i16v;
    i32                     *i32v;
    i64                     *i64v;
    const char              *str;
    char                    *ch;
    ks_array_data           *arr;
    ks_object_data          *obj;
    void                    *data;
    void*                   *vpp;
}ks_value_ptr;

typedef struct ks_value{
    ks_value_type   type;
    ks_value_ptr    ptr;
}ks_value;


typedef struct ks_property{
    const char      *name;
    ks_value        value;
}ks_property;


typedef struct ks_array_data{
    u32             length;
    u32             elem_size;
    ks_value        value;
    bool            fixed_length;
}ks_array_data;

typedef struct ks_object_data{
    const char      *type;
    ks_object_func  serializer,
                    deserializer,
                    other;
    void            *data;
}ks_object_data;


ks_io*      ks_io_new                           ();
void        ks_io_free                          (ks_io* io);
void        ks_io_reset                         (ks_io *io);

bool        ks_io_begin_base                    (ks_io*io, const ks_io_methods* methods, ks_property prop, ks_io_serial_type type);
bool        ks_io_serialize_base                (const ks_io_methods* methods, const char* file, ks_property prop);
bool        ks_io_deserialize_base              (const ks_io_methods* methods, const char* file, ks_property prop);
bool        ks_io_other_base                    (ks_io*io, const ks_io_methods* methods, ks_property prop);

#define     ks_io_serialize_begin(io, methods, prop, type)                      ks_io_begin_base(io, & methods ## _serializer, ks_prop_root(prop, type), KS_IO_SERIALIZER)
#define     ks_io_deserialize_begin(io, methods, prop, type)                    ks_io_begin_base(io, & methods ## _deserializer, ks_prop_root(prop, type), KS_IO_DESERIALIZER)
#define     ks_io_other_begin(io, methods, prop, type)                          ks_io_begin_base(io, & methods ## _impl, ks_prop_root(prop, type), KS_IO_OTHER_TYPE)
#define     ks_io_serialize_to_file(methods, file, prop, type)                  ks_io_serialize_base(& methods ## _serializer, file, ks_prop_root(prop, type))
#define     ks_io_deserialize_from_file(methods, file, prop, type)              ks_io_deserialize_base( & methods ## _deserializer, file, ks_prop_root(prop, type))
#define     ks_io_other(io, methods, prop, type)                                ks_io_other_base(io, & methods ## _impl, ks_prop_root(prop, type))

bool        ks_io_write_file                    (ks_io* io, const char* file);
bool        ks_io_read_file                     (ks_io* io, const char* file);
void        ks_io_read_string_len               (ks_io* io, u32 length, const char* data);
void        ks_io_read_string                   (ks_io* io, const char* data);

// io common
ks_io_userdata* ks_io_top_userdata_from         (ks_io* io, u32 index);
bool            ks_io_push_userdata             (ks_io* io, ks_io_userdata userdata);
bool            ks_io_pop_userdata              (ks_io* io);
ks_io_userdata* ks_io_top_state_from            (ks_io* io, u32 index);
bool            ks_io_push_state                (ks_io* io, ks_io_userdata userdata);
bool            ks_io_pop_state                 (ks_io* io);
bool            ks_io_property                  (ks_io* io, const ks_io_methods* methods,  ks_property prop, ks_io_serial_type serial_type);
bool            ks_io_magic_number              (ks_io* io, const ks_io_methods* methods, const char* data);
bool            ks_io_string                    (ks_io* io, const ks_io_methods* methods, ks_array_data array, u32 offset, ks_io_serial_type serial_type);
bool            ks_io_array_begin               (ks_io* io, const ks_io_methods* methods, ks_array_data* array,  u32 offset, ks_io_serial_type serial_type);
bool            ks_io_array_end                 (ks_io* io, const ks_io_methods* methods, ks_array_data* array, u32 offset, ks_io_serial_type serial_type);
bool            ks_io_array                     (ks_io* io, const ks_io_methods* methods, ks_array_data array, u32 offset, ks_io_serial_type serial_type);
bool            ks_io_object                    (ks_io* io, const ks_io_methods* methods, ks_object_data obj, u32 offset, ks_io_serial_type serial_type);
bool            ks_io_value                     (ks_io* io, const ks_io_methods* methods, ks_value value, u32 index, ks_io_serial_type serial_type);

ks_value    ks_val_ptr                              (void* ptr, ks_value_type type);

#define     ks_io_begin_with(io, methods, with, serial_type, prop)         ks_io_begin(io, & methods ## with, serial_type, prop)
#define     ks_io_begin_other(io, methods, prop)                           ks_io_begin_with(io, methods, _impl, KS_IO_OTHER_TYPE, prop)
#define     ks_io_begin_serialize(io, methods, prop)                       ks_io_begin_with(io, methods, _serializer, KS_IO_SERIALIZER, prop)
#define     ks_io_begin_deserialize(io, methods, prop)                     ks_io_begin_with(io, methods, _deserializer, KS_IO_DESERIALIZER, prop)

#define ks_io_methods_func(pre, name) ks_io_ ## pre ## _ ## name

#define ks_io_methods_decl_ext_base(name) extern const ks_io_methods name;
#define ks_io_methods_decl_ext_other(name) ks_io_methods_decl_ext_base(name ## _impl)
#define ks_io_methods_decl_ext_rw(name) \
    ks_io_methods_decl_ext_base(name ## _serializer) \
    ks_io_methods_decl_ext_base(name ## _deserializer)

#define ks_io_methods_decl(name)\
    const ks_io_methods name = { \
        ks_io_methods_func(key , name), \
        ks_io_methods_func(value , name), \
        ks_io_methods_func(string , name), \
        ks_io_methods_func(array_begin , name), \
        ks_io_methods_func(array_elem , name), \
        ks_io_methods_func(array_end , name), \
        ks_io_methods_func(object , name), \
    };
#define ks_io_methods_decl_with(name, with)  ks_io_methods_decl(name ## with)

#define ks_io_methods_decl_serializer( name ) ks_io_methods_decl_with( name ,  _serializer)
#define ks_io_methods_decl_deserializer( name ) ks_io_methods_decl_with( name , _deserializer)

#define ks_io_methods_impl_fn_add(func_name, name, add) \
    bool ks_io_methods_func(key , func_name) (ks_io* io, const ks_io_methods* methods, const char* name, bool fixed) { \
        return ks_io_methods_func(key, name ) (io, methods, name, fixed, add); \
    } \
    bool ks_io_methods_func(value , func_name) (ks_io* io, const ks_io_methods* methods, ks_value value, u32 offset) { \
        return ks_io_methods_func(value, name ) (io, methods, value, offset, add);\
    } \
    bool ks_io_methods_func(string , func_name) (ks_io* io, const ks_io_methods* methods, ks_array_data array, ks_string* str) { \
        return ks_io_methods_func(string, name) (io, methods, array, str, add );\
    } \
    bool ks_io_methods_func(array_begin , func_name) (ks_io* io, const ks_io_methods* methods,  ks_array_data arr) { \
        return ks_io_methods_func(array_begin, name ) (io, methods, arr, add); \
    } \
    bool ks_io_methods_func(array_elem , func_name) (ks_io* io,  const ks_io_methods* methods, ks_array_data arr, u32 index) { \
        return ks_io_methods_func(array_elem, name ) (io,  methods, arr,index, add); \
    } \
    bool ks_io_methods_func(array_end , func_name) (ks_io* io, const ks_io_methods* methods,  ks_array_data arr) { \
        return ks_io_methods_func(array_end, name ) (io, methods, arr, add); \
    } \
    bool ks_io_methods_func(object , func_name) (ks_io* io,  const ks_io_methods* methods, ks_object_data obj, u32 offset) { \
        return ks_io_methods_func(object, name ) (io, methods, obj, offset, add); \
    }

#define ks_io_methods_impl_with_add( name, with, add)     ks_io_methods_impl_fn_add(name ## with, name, add)

#define ks_io_methods_impl_serializer( name ) ks_io_methods_impl_with_add( name, _serializer, KS_IO_SERIALIZER )
#define ks_io_methods_impl_deserializer( name ) ks_io_methods_impl_with_add( name, _deserializer, KS_IO_DESERIALIZER )

#define ks_io_methods_impl_other(name) \
    ks_io_methods_impl_with_add( name, _impl, KS_IO_OTHER_TYPE ) \
    ks_io_methods_decl_with(name, _impl)

#define ks_io_methods_impl_rw(name) \
    ks_io_methods_impl_serializer(name) \
    ks_io_methods_impl_deserializer(name) \
    ks_io_methods_decl_serializer(name) \
    ks_io_methods_decl_deserializer(name)

#define ks_begin_props(io, methods, serialize, offset, type, obj) \
    u32 __RETURN = 0; \
    { \
    ks_io * __IO = io; \
    const ks_io_methods * __FUNCS = methods; \
    ks_io_serial_type __SERIAL_TYPE = serial_type; \
    u32 __INDEX = offset; \
    type * __OBJECT = (type*)((char*)obj + offset*sizeof(type)); \
    (void)__INDEX;

#define ks_end_props }

#define ks_io_custom_func(type)                     ks_io_ ## type
#define ks_io_custom_func_serializer(type)          ks_io_custom_func(type ## _serializer)
#define ks_io_custom_func_deserializer(type)        ks_io_custom_func(type ## _deserializer)
#define ks_io_custom_func_other(type)               ks_io_custom_func(type ## _other)

#define ks_io_custom_func_args                      ks_io* io, const ks_io_methods* methods,  void *v, u32 offset

#define ks_io_decl_custom_func(type) \
    u32 ks_io_custom_func_other(type) (ks_io_custom_func_args); \
     u32 ks_io_custom_func_serializer(type) (ks_io_custom_func_args); \
     u32 ks_io_custom_func_deserializer(type) (ks_io_custom_func_args)

#define ks_io_begin_custom_func(type) \
    static inline u32 ks_io_custom_func(type)( ks_io_custom_func_args, ks_io_serial_type serial_type ){ \
    ks_begin_props(io, methods, serialize, offset, type, v);

#define ks_io_end_custom_func(type) ks_end_props return __RETURN; } \
    u32 ks_io_custom_func_other(type)(ks_io_custom_func_args){ return ks_io_custom_func(type)(io, methods, v, offset, KS_IO_OTHER_TYPE);  } \
    u32 ks_io_custom_func_serializer(type)(ks_io_custom_func_args){ return ks_io_custom_func(type)(io, methods, v, offset, KS_IO_SERIALIZER); } \
    u32 ks_io_custom_func_deserializer(type)(ks_io_custom_func_args){ return ks_io_custom_func(type)(io, methods, v, offset, KS_IO_DESERIALIZER); }

ks_property ks_prop_v(void *name, ks_value value);

#ifdef __cplusplus
    #define ks_type(type) type
#else
    #define ks_type(type) (type)
#endif

#define ks_access(elem)                     __OBJECT ->  elem
#define ks_access_before(elem, num)         __OBJECT[-num] . elem
#define ks_val(elem, type)                  ks_val_ptr(& ks_access(elem), type)
#define ks_prop_f(name, var, type)          ks_prop_v(name, ks_val(var, type))

#define ks_arr_type(type, ...)              (ks_type(type []) {  __VA_ARGS__ } )\

#define ks_prop_obj_ptr_data(ptr, type) \
    (ks_type(ks_object_data) {\
        #type , \
        ks_io_custom_func_serializer(type) , \
        ks_io_custom_func_deserializer(type) , \
        ks_io_custom_func_other(type) , \
        & ptr,\
    })

#define ks_prop_obj_data(var, type)         ks_prop_obj_ptr_data(ks_access(var), type)
#define ks_val_obj(var, type)               ks_val_ptr(ks_arr_type(ks_object_data, ks_prop_obj_data(var, type)), KS_VALUE_OBJECT)

#define ks_val_str_elem(elem)               ks_val(elem, KS_VALUE_STRING_ELEM)

#define ks_val_u64(elem)                    ks_val(elem, KS_VALUE_U64)
#define ks_val_u32(elem)                    ks_val(elem, KS_VALUE_U32)
#define ks_val_u16(elem)                    ks_val(elem, KS_VALUE_U16)
#define ks_val_u8(elem)                     ks_val(elem, KS_VALUE_U8)

#define ks_val_i64(elem)                    ks_val(elem, KS_VALUE_I64)
#define ks_val_i32(elem)                    ks_val(elem, KS_VALUE_I32)
#define ks_val_i16(elem)                    ks_val(elem, KS_VALUE_I16)
#define ks_val_i8(elem)                     ks_val(elem, KS_VALUE_I8)

#define ks_prop_u64_as(name, var)           ks_prop_f(name, var, KS_VALUE_U64)
#define ks_prop_u32_as(name, var)           ks_prop_f(name, var, KS_VALUE_U32)
#define ks_prop_u16_as(name, var)           ks_prop_f(name, var, KS_VALUE_U16)
#define ks_prop_u8_as(name, var)            ks_prop_f(name, var, KS_VALUE_U8)

#define ks_prop_i64_as(name, var)           ks_prop_f(name, var, KS_VALUE_I64)
#define ks_prop_i32_as(name, var)           ks_prop_f(name, var, KS_VALUE_I32)
#define ks_prop_i16_as(name, var)           ks_prop_f(name, var, KS_VALUE_I16)
#define ks_prop_i8_as(name, var)            ks_prop_f(name, var, KS_VALUE_I8)

#define ks_prop_obj_as(name, type, var)     ks_prop_v(name, ks_val_obj( var, type ))

#define ks_prop_u64(name)                   ks_prop_u64_as(#name, name)
#define ks_prop_u32(name)                   ks_prop_u32_as(#name, name)
#define ks_prop_u16(name)                   ks_prop_u16_as(#name, name)
#define ks_prop_u8(name)                    ks_prop_u8_as(#name, name)

#define ks_prop_i64(name)                   ks_prop_i64_as(#name, name)
#define ks_prop_i32(name)                   ks_prop_i32_as(#name, name)
#define ks_prop_i16(name)                   ks_prop_i16_as(#name, name)
#define ks_prop_i8(name)                    ks_prop_i8_as(#name, name)

#define ks_prop_obj(name, type)             ks_prop_obj_as(#name, type, name)

#define ks_func_prop(func, prop)        if(! func ( __IO, __FUNCS, prop,  __SERIAL_TYPE )) return false

#define ks_prop_root(obj, type)         ks_prop_v("", ks_val_ptr(ks_arr_type(ks_object_data, ks_prop_obj_ptr_data(obj, type)), KS_VALUE_OBJECT))


#define ks_prop_arr_data_size_len(len,  size, value, fixed) \
    (ks_type(ks_array_data) { \
        len, \
        size, \
        value, \
        fixed, \
    })

#define KS_STRING_UNKNOWN_LENGTH UINT32_MAX

#define ks_elem_size(var)                                           (sizeof(*ks_access(var)))

#define ks_prop_arr_data_len(len, var, value, fixed)                ks_prop_arr_data_size_len(len, ks_elem_size(var), value, fixed)
#define ks_arr_size(var)                                            (sizeof(ks_access(var))/ sizeof(*ks_access(var)))

#define ks_val_arr_len_fixed(len, var, value, fixed)                ks_val_ptr(ks_arr_type(ks_array_data, ks_prop_arr_data_len(len,  var, value, fixed)), KS_VALUE_ARRAY)
#define ks_prop_arr_len_fixed_as(name, len, var, value, fixed)      ks_prop_v(name, ks_val_arr_len_sparse_fixed( len, var, value, fixed) )

#define ks_prop_arr_as(name, var, value)    ks_prop_v(name, ks_val_arr_len_fixed( ks_arr_size(var), var, value, true) )
#define ks_prop_arr(name, value)            ks_prop_arr_as(#name, name, value)
#define ks_prop_arr_obj(name, type)         ks_prop_arr(name, ks_val_obj(name, type))
#define ks_prop_str(name)                   ks_prop_arr(name, ks_val_str_elem(name))

#define ks_prop_arr_u64(name)               ks_prop_arr(name, ks_val_u64(name))
#define ks_prop_arr_u32(name)               ks_prop_arr(name, ks_val_u32(name))
#define ks_prop_arr_u16(name)               ks_prop_arr(name, ks_val_u16(name))
#define ks_prop_arr_u8(name)                ks_prop_arr(name, ks_val_u8(name))

#define ks_prop_arr_i64(name)               ks_prop_arr(name, ks_val_i64(name))
#define ks_prop_arr_i32(name)               ks_prop_arr(name, ks_val_i32(name))
#define ks_prop_arr_i16(name)               ks_prop_arr(name, ks_val_i16(name))
#define ks_prop_arr_i8(name)                ks_prop_arr(name, ks_val_i8(name))


#define ks_prop_arr_len(name, len, value)       ks_prop_v(#name, ks_val_arr_len_fixed( len, name, value, false) )
#define ks_prop_arr_obj_len(name, type, len)    ks_prop_arr_len(name, len, ks_val_obj(name, type))
#define ks_prop_str_len(name, len)              ks_prop_arr_len(name, len, ks_val_str_elem(name))

#define ks_prop_arr_u64_len(name, len)          ks_prop_arr_len(name, len, ks_val_u64(name))
#define ks_prop_arr_u32_len(name, len)          ks_prop_arr_len(name, len, ks_val_u32(name))
#define ks_prop_arr_u16_len(name, len)          ks_prop_arr_len(name, len, ks_val_u16(name))
#define ks_prop_arr_u8_len(name, len)           ks_prop_arr_len(name, len, ks_val_u8(name))

#define ks_prop_arr_i64_len(name, len)          ks_prop_arr_len(name, len, ks_val_i64(name))
#define ks_prop_arr_i32_len(name, len)          ks_prop_arr_len(name, len, ks_val_i32(name))
#define ks_prop_arr_i16_len(name, len)          ks_prop_arr_len(name, len, ks_val_i16(name))
#define ks_prop_arr_i8_len(name, len)           ks_prop_arr_len(name, len, ks_val_i8(name))


#define ks_p(prop)                          __RETURN += ks_io_property(__IO, __FUNCS, prop, __SERIAL_TYPE) ? 1 : 0

#define ks_u64(name)                        ks_p(ks_prop_u64(name))
#define ks_u32(name)                        ks_p(ks_prop_u32(name))
#define ks_u16(name)                        ks_p(ks_prop_u16(name))
#define ks_u8(name)                         ks_p(ks_prop_u8(name))

#define ks_i64(name)                        ks_p(ks_prop_i64(name))
#define ks_i32(name)                        ks_p(ks_prop_i32(name))
#define ks_i16(name)                        ks_p(ks_prop_i16(name))
#define ks_i8(name)                         ks_p(ks_prop_i8(name))

#define ks_obj(name, type)                  ks_p(ks_prop_obj(name, type))

#define ks_arr_u64(name)                    ks_p(ks_prop_arr_u64(name))
#define ks_arr_u32(name)                    ks_p(ks_prop_arr_u32(name))
#define ks_arr_u16(name)                    ks_p(ks_prop_arr_u16(name))
#define ks_arr_u8(name)                     ks_p(ks_prop_arr_u8(name))

#define ks_arr_i64(name)                    ks_p(ks_prop_arr_i64(name))
#define ks_arr_i32(name)                    ks_p(ks_prop_arr_i32(name))
#define ks_arr_i16(name)                    ks_p(ks_prop_arr_i16(name))
#define ks_arr_i8(name)                     ks_p(ks_prop_arr_i8(name))

#define ks_arr_obj(name, type)              ks_p(ks_prop_arr_obj(name, type))
#define ks_str(name)                        ks_p(ks_prop_str(name))

#define ks_arr_u64_len(name, len)           ks_p(ks_prop_arr_u64_len(name, len))
#define ks_arr_u32_len(name, len)           ks_p(ks_prop_arr_u32_len(name, len))
#define ks_arr_u16_len(name, len)           ks_p(ks_prop_arr_u16_len(name, len))
#define ks_arr_u8_len(name, len)            ks_p(ks_prop_arr_u8_len(name, len))

#define ks_arr_i64_len(name, len)           ks_p(ks_prop_arr_i64_len(name, len))
#define ks_arr_i32_len(name, len)           ks_p(ks_prop_arr_i32_len(name, len))
#define ks_arr_i16_len(name, len)           ks_p(ks_prop_arr_i16_len(name, len))
#define ks_arr_i8_len(name, len)            ks_p(ks_prop_arr_i8_len(name, len))

#define ks_arr_obj_len(name, type, len)     ks_p(ks_prop_arr_obj_len(name, type, len))
#define ks_str_p(name)                      ks_p(ks_prop_str_len(name, KS_STRING_UNKNOWN_LENGTH))
#define ks_str_len(name, len)               ks_p(ks_prop_str_len(name, len))

#define ks_magic_number(num)                if(!ks_io_magic_number(__IO, __FUNCS, num)) return 0;




