#pragma once

#include "../io.h"

bool        ks_io_key_deleter                     (ks_io* io, const ks_io_funcs* funcs, const char* name, bool fixed, ks_io_serial_type serial_type);
bool        ks_io_string_deleter                  (ks_io* io, const ks_io_funcs* funcs, ks_array_data array, ks_string* str, ks_io_serial_type serial_type);
bool        ks_io_value_deleter                   (ks_io* io, const ks_io_funcs* funcs, ks_value value, u32 offset,  ks_io_serial_type serial_type);
bool        ks_io_array_begin_deleter             (ks_io* io, const ks_io_funcs* funcs,  ks_array_data arr,  ks_io_serial_type serial_type);
bool        ks_io_array_elem_deleter              (ks_io* io,  const ks_io_funcs* funcs, ks_array_data arr, u32 index, ks_io_serial_type serial_type);
bool        ks_io_array_end_deleter               (ks_io* io, const ks_io_funcs* funcs,  ks_array_data arr,  ks_io_serial_type serial_type);
bool        ks_io_object_deleter                  (ks_io* io, const ks_io_funcs* funcs,  ks_object_data obj, u32 offset, ks_io_serial_type serial_type);


ks_io_funcs_decl_ext_other(deleter);
