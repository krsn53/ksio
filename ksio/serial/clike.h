#pragma once

#include "../io.h"

// text format utils
bool        ks_io_print_indent                  (ks_io* io,  char indent, bool serialize);
bool        ks_io_print_endl                    (ks_io* io, bool serialize);
bool        ks_io_print_space                   (ks_io* io, bool serialize);
u32         ks_io_value_text                    (ks_io* io, ks_value_ptr v, ks_value_type type, u32 offset, bool serialize);
u32         ks_io_prop_text                     (ks_io* io, const char* str, const char* delims, bool serialize);
u32         ks_io_text_len                      (ks_io* io, u32 length, const char* str, bool serialize);
u32         ks_io_text                          (ks_io* io, const char* str, bool serialize);
u32         ks_io_fixed_text                    (ks_io* io, const char* str, bool serialize);

// common clike
bool        ks_io_key_clike                     (ks_io* io, const ks_io_funcs* funcs, const char* name, bool fixed, bool serialize);
bool        ks_io_string_clike                  (ks_io* io, const ks_io_funcs* funcs, ks_array_data array, ks_string* str, bool serialize);
bool        ks_io_value_clike                   (ks_io* io, const ks_io_funcs* funcs, ks_value value, u32 offset,  bool serialize);
bool        ks_io_array_begin_clike             (ks_io* io, const ks_io_funcs* funcs,  ks_array_data arr,  bool serialize);
bool        ks_io_array_elem_clike              (ks_io* io,  const ks_io_funcs* funcs, ks_array_data arr, u32 index, bool serialize);
bool        ks_io_array_end_clike               (ks_io* io, const ks_io_funcs* funcs,  ks_array_data arr,  bool serialize);
bool        ks_io_object_clike                  (ks_io* io, const ks_io_funcs* funcs,  ks_object_data obj, u32 offset, bool serialize);


ks_io_funcs_decl_ext_rw(clike)
