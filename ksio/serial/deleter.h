#pragma once


#ifdef __cplusplus
extern "C" {
#endif

#include "../io.h"

#ifdef NDEBUG
#define     KS_PRINT_DELETE_LOG     false
#else
#define     KS_PRINT_DELETE_LOG     true
#endif
bool        ks_io_key_deleter                     (ks_io* io, const ks_io_methods* methods, const char* name, bool fixed, ks_io_serial_type serial_type);
bool        ks_io_string_deleter                  (ks_io* io, const ks_io_methods* methods, ks_array_data array, ks_string* str, ks_io_serial_type serial_type);
bool        ks_io_value_deleter                   (ks_io* io, const ks_io_methods* methods, ks_value value, u32 offset,  ks_io_serial_type serial_type);
bool        ks_io_array_begin_deleter             (ks_io* io, const ks_io_methods* methods,  ks_array_data arr,  ks_io_serial_type serial_type);
bool        ks_io_array_elem_deleter              (ks_io* io,  const ks_io_methods* methods, ks_array_data arr, u32 index, ks_io_serial_type serial_type);
bool        ks_io_array_end_deleter               (ks_io* io, const ks_io_methods* methods,  ks_array_data arr,  ks_io_serial_type serial_type);
bool        ks_io_object_deleter                  (ks_io* io, const ks_io_methods* methods,  ks_object_data obj, u32 offset, ks_io_serial_type serial_type);


ks_io_methods_decl_ext_other(deleter);

bool        ks_io_delete_base(const ks_io_methods* methods, ks_object_data obj);

#define     ks_io_delete(prop, type)                ks_io_delete_base( &deleter_impl, ks_obj_data_other(prop, type))



#ifdef __cplusplus
}
#endif
