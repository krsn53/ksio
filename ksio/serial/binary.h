#pragma once

#include "../io.h"

// binary utils
u32         ks_io_fixed_bin_len                 (ks_io* io, u32 length, const char* str, ks_io_serial_type serial_type);
u32         ks_io_fixed_bin                     (ks_io* io, const char* str, ks_io_serial_type serial_type);
bool        ks_io_value_bin                     (ks_io* io, u32 length, char* c, bool swap_endian, ks_io_serial_type serial_type);

// common binary
bool        ks_io_key_binary                    (ks_io* io, const ks_io_methods* methods, const char* name, bool fixed, bool swap_endian, ks_io_serial_type serial_type);
bool        ks_io_value_binary                  (ks_io* io, const ks_io_methods* methods, ks_value value, u32 offset,  bool swap_endian, ks_io_serial_type serial_type);
bool        ks_io_string_binary                 (ks_io* io, const ks_io_methods* methods, ks_array_data array, ks_string* str, bool swap_endian, ks_io_serial_type serial_type);
bool        ks_io_array_begin_binary            (ks_io* io, const ks_io_methods* methods,  ks_array_data arr, bool swap_endian,  ks_io_serial_type serial_type);
bool        ks_io_array_elem_binary             (ks_io* io,  const ks_io_methods* methods, ks_array_data arr, u32 index, bool swap_endian, ks_io_serial_type serial_type);
bool        ks_io_array_end_binary              (ks_io* io, const ks_io_methods* methods,  ks_array_data arr, bool swap_endian, ks_io_serial_type serial_type);
bool        ks_io_object_binary                 (ks_io* io, const ks_io_methods* methods,  ks_object_data obj, u32 offset, bool swap_endian, ks_io_serial_type serial_type);

// binary little endian
bool        ks_io_key_binary_little_endian          (ks_io* io, const ks_io_methods* methods, const char* name, bool fixed, ks_io_serial_type serial_type);
bool        ks_io_value_binary_little_endian        (ks_io* io, const ks_io_methods* methods, ks_value value, u32 offset,  ks_io_serial_type serial_type);
bool        ks_io_string_binary_little_endian       (ks_io* io, const ks_io_methods* methods, ks_array_data array, ks_string* str, ks_io_serial_type serial_type);
bool        ks_io_array_begin_binary_little_endian  (ks_io* io, const ks_io_methods* methods,  ks_array_data arr, ks_io_serial_type serial_type);
bool        ks_io_array_elem_binary_little_endian   (ks_io* io,  const ks_io_methods* methods, ks_array_data arr, u32 index, ks_io_serial_type serial_type);
bool        ks_io_array_end_binary_little_endian    (ks_io* io, const ks_io_methods* methods,  ks_array_data arr, ks_io_serial_type serial_type);
bool        ks_io_object_binary_little_endian       (ks_io* io, const ks_io_methods* methods,  ks_object_data obj, u32 offset, ks_io_serial_type serial_type);

// binary big endian
bool        ks_io_key_binary_big_endian             (ks_io* io, const ks_io_methods* methods, const char* name, bool fixed, ks_io_serial_type serial_type);
bool        ks_io_value_binary_big_endian           (ks_io* io, const ks_io_methods* methods, ks_value value, u32 offset,  ks_io_serial_type serial_type);
bool        ks_io_string_binary_big_endian          (ks_io* io, const ks_io_methods* methods, ks_array_data array, ks_string* str, ks_io_serial_type serial_type);
bool        ks_io_array_begin_binary_big_endian     (ks_io* io, const ks_io_methods* methods,  ks_array_data arr,  ks_io_serial_type serial_type);
bool        ks_io_array_elem_binary_big_endian      (ks_io* io,  const ks_io_methods* methods, ks_array_data arr, u32 index, ks_io_serial_type serial_type);
bool        ks_io_array_end_binary_big_endian       (ks_io* io, const ks_io_methods* methods,  ks_array_data arr,ks_io_serial_type serial_type);
bool        ks_io_object_binary_big_endian          (ks_io* io, const ks_io_methods* methods,  ks_object_data obj, u32 offset, ks_io_serial_type serial_type);




ks_io_methods_decl_ext_rw(binary_little_endian)
ks_io_methods_decl_ext_rw(binary_big_endian)
