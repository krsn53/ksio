#pragma once

#include "../io.h"

// binary utils
u32         ks_io_fixed_bin_len                 (ks_io* io, u32 length, const char* str, bool serialize);
u32         ks_io_fixed_bin                     (ks_io* io, const char* str, bool serialize);
bool        ks_io_value_bin                     (ks_io* io, u32 length, char* c, bool swap_endian, bool serialize);

// common binary
bool        ks_io_key_binary                    (ks_io* io, const ks_io_funcs* funcs, const char* name, bool fixed, bool swap_endian, bool serialize);
bool        ks_io_value_binary                  (ks_io* io, const ks_io_funcs* funcs, ks_value value, u32 offset,  bool swap_endian, bool serialize);
bool        ks_io_string_binary                 (ks_io* io, const ks_io_funcs* funcs, ks_array_data array, ks_string* str, bool swap_endian, bool serialize);
bool        ks_io_array_begin_binary            (ks_io* io, const ks_io_funcs* funcs,  ks_array_data arr, bool swap_endian,  bool serialize);
bool        ks_io_array_elem_binary             (ks_io* io,  const ks_io_funcs* funcs, ks_array_data arr, u32 index, bool swap_endian, bool serialize);
bool        ks_io_array_end_binary              (ks_io* io, const ks_io_funcs* funcs,  ks_array_data arr, bool swap_endian, bool serialize);
bool        ks_io_object_binary                 (ks_io* io, const ks_io_funcs* funcs,  ks_object_data obj, u32 offset, bool swap_endian, bool serialize);

// binary little endian
bool        ks_io_key_binary_little_endian          (ks_io* io, const ks_io_funcs* funcs, const char* name, bool fixed, bool serialize);
bool        ks_io_value_binary_little_endian        (ks_io* io, const ks_io_funcs* funcs, ks_value value, u32 offset,  bool serialize);
bool        ks_io_string_binary_little_endian       (ks_io* io, const ks_io_funcs* funcs, ks_array_data array, ks_string* str, bool serialize);
bool        ks_io_array_begin_binary_little_endian  (ks_io* io, const ks_io_funcs* funcs,  ks_array_data arr, bool serialize);
bool        ks_io_array_elem_binary_little_endian   (ks_io* io,  const ks_io_funcs* funcs, ks_array_data arr, u32 index, bool serialize);
bool        ks_io_array_end_binary_little_endian    (ks_io* io, const ks_io_funcs* funcs,  ks_array_data arr, bool serialize);
bool        ks_io_object_binary_little_endian       (ks_io* io, const ks_io_funcs* funcs,  ks_object_data obj, u32 offset, bool serialize);

// binary big endian
bool        ks_io_key_binary_big_endian             (ks_io* io, const ks_io_funcs* funcs, const char* name, bool fixed, bool serialize);
bool        ks_io_value_binary_big_endian           (ks_io* io, const ks_io_funcs* funcs, ks_value value, u32 offset,  bool serialize);
bool        ks_io_string_binary_big_endian          (ks_io* io, const ks_io_funcs* funcs, ks_array_data array, ks_string* str, bool serialize);
bool        ks_io_array_begin_binary_big_endian     (ks_io* io, const ks_io_funcs* funcs,  ks_array_data arr,  bool serialize);
bool        ks_io_array_elem_binary_big_endian      (ks_io* io,  const ks_io_funcs* funcs, ks_array_data arr, u32 index, bool serialize);
bool        ks_io_array_end_binary_big_endian       (ks_io* io, const ks_io_funcs* funcs,  ks_array_data arr,bool serialize);
bool        ks_io_object_binary_big_endian          (ks_io* io, const ks_io_funcs* funcs,  ks_object_data obj, u32 offset, bool serialize);

ks_io_funcs_decl_ext_rw(binary_little_endian)
ks_io_funcs_decl_ext_rw(binary_big_endian)
