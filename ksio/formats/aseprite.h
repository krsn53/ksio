/*
 * https://github.com/aseprite/aseprite/blob/master/docs/ase-file-specs.md
*/

#pragma once

#include "../io.h"

enum ks_asperite_header_flags{
    KS_ASPERITE_LAYER_OPECITY_HAS_VALID = 1,
};

enum ks_asperite_layer_flags{
    KS_ASPERITE_VISIBLE             = 1,
    KS_ASPERITE_EDITABLE            = 2,
    KS_ASPERITE_LOCK_MOVEMENT       = 4,
    KS_ASPERITE_BACKGROUND          = 8,
    KS_ASPERITE_PREFER_LINKED_CELS  = 16,
    KS_ASPERITE_SHOUD_BE_COLLASPED  = 32,
    KS_ASPERITE_REFERENCE_LAYER     =64,
};

enum ks_asperite_layer_type{
    KS_ASPERITE_NORMAL_LAYER = 0,
    KS_ASPERITE_GROUP_LAYER = 1,
};

enum ks_asperite_layer_blend_mode {
    KS_ASPERITE_BLEND_NORMAL        = 0,
    KS_ASPERITE_BLEND_MULTIPLY      = 1,
    KS_ASPERITE_BLEND_SCREEN        = 2,
    KS_ASPERITE_BLEND_OVERLAY       = 3,
    KS_ASPERITE_BLEND_DARKEN        = 4,
    KS_ASPERITE_BLEND_LIGHTEN       = 5,
    KS_ASPERITE_BLEND_COLOR_DODGE   = 6,
    KS_ASPERITE_BLEND_COLOR_BURN    = 7,
    KS_ASPERITE_BLEND_HARD_LIGHT    = 8,
    KS_ASPERITE_BLEND_SOFT_LIGHT    = 9,
    KS_ASPERITE_BLEND_DIFFERENCE    = 10,
    KS_ASPERITE_BLEND_EXCLUSION     = 11,
    KS_ASPERITE_BLEND_HUE           = 12,
    KS_ASPERITE_BLEND_SATURATION    = 13,
    KS_ASPERITE_BLEND_COLOR         = 14,
    KS_ASPERITE_BLEND_LUMINOSITY    = 15,
    KS_ASPERITE_BLEND_ADDITION      = 16,
    KS_ASPERITE_BLEND_SUBSTRACT     = 17,
    KS_ASPERITE_BLEND_DIVIDE        = 18,
};

enum ks_asperite_cel_type{
    KS_ASPERITE_RAW_CEL             = 0,
    KS_ASPERITE_LINKED_CEL          = 1,
    KS_ASPERITE_COMPRESSED_IMAGE    = 2,
};

enum ks_asperite_cel_extra_flags{
    KS_ASPERITE_USE_SPECIAL_FIXED_GAMMA = 1,
};

enum ks_asperite_loop_anime_dir{
    KS_ASPERITE_LOOP_FORWARD    = 0,
    KS_ASPERITE_LOOP_REVERSE    = 1,
    KS_ASPERITE_LOOP_PING_PONG  = 2,
};

enum ks_asperite_palette_entry_flags{
    KS_ASPERITE_PALETTE_ENTRY_HAS_NAME = 1,
};

enum ks_asperite_userdata_flags{
    KS_ASPERITE_USERDATA_HAS_TEXT   = 1,
    KS_ASPERITE_USERDATA_HAS_COLOR  = 2,
};

enum ks_asperite_slice_flags{
    KS_ASPERITE_9_PATCHES_SLICE     = 1,
    KS_ASPERITE_HAS_PIVOT_INFO      = 2,
};

typedef struct ks_asperite_string{
    u16                 length;
    u8                  *characters;
}ks_asperite_string;

typedef struct ks_asperite_color{
    u8                  red;
    u8                  green;
    u8                  blue;
}ks_asperite_color;

typedef struct ks_asperite_rgba_color{
    u8                  red;
    u8                  green;
    u8                  blue;
    u8                  alpha;
}ks_asperite_rgba_color;

typedef struct ks_asperite_grayscale_color{
    u8                  value;
    u8                  alpha;
}ks_asperite_grayscale_color;

typedef struct ks_asperite_indexed_color{
    u8                  index;
}ks_asperite_indexed_color;

typedef union ks_asperite_pixels{
    ks_asperite_rgba_color      *rgba;
    ks_asperite_grayscale_color *grayscale;
    ks_asperite_indexed_color   *indexed;
}ks_asperite_pixels;

typedef struct ks_asperite_packet{
    u8                  num_palette_entries; // to skip from the last packet
    u8                  num_colors; // 0 means 256
    ks_asperite_color*  colors;
} ks_asperite_packet;

typedef struct ks_asperite_old_palette{
    u16                 num_packets;
    ks_asperite_packet  *packets;
} ks_asperite_old_palette;

typedef struct ks_asperite_layar{
    u16                 flags;
    u16                 type; // normal, group
    u16                 child_level;
    u16                 default_width;
    u16                 default_height;
    u16                 blend_mode; // normal, multiply, screen, overlay, darken ...
    u8                  opacity;
    u8                  for_future[3];
    ks_asperite_string  layer_name;
} ks_asperite_layar;


typedef struct ks_asperite_raw_cel{
    u16                 width;
    u16                 height;
    ks_asperite_pixels  pixels;
}ks_asperite_raw_cel;

typedef struct ks_asperite_compressed_cel{
    u16                 width;
    u16                 height;
    u8                  *data; // compressed
}ks_asperite_compressed_cel;

typedef struct ks_asperite_cel{
    u16                 layer_index;
    i16                 x;
    i16                 y;
    u8                  opacity;
    u16                 type;
    u8                  for_future[7];
    union{
        ks_asperite_raw_cel         raw_cel;
        u16                         linked_frame_position;
        ks_asperite_compressed_cel  compressed_cel;
    }data;
} ks_asperite_cel;

typedef struct ks_asperite_cel_extra{
    u32                 flags; // 1: precise bounds are set
    u32                 x; // precise position  fixed point 16
    u32                 y; // precise position  fixed point 16
    u32                 width; // fixed point 16
    u32                 height; // fixed point 16
    u8                  for_future[16];
}ks_asperite_cel_extra;

typedef struct ks_asperite_profile{
    u16                 type; // no color, srgb, embedded icc
    u16                 frags; // 1: use special fixed gamma
    u32                 fixed_gamma; // fixed point 16
    u8                  reserved[8]; // if type icc: u32 length, byte[] profile_data
}ks_asperite_profile;

// depricated
typedef struct ks_asperite_mask{
    i16                 x;
    i16                 y;
    u16                 width;
    u16                 height;
    u8                  for_future[8];
    ks_asperite_string  *name;
    u8                  *bitmap_data; // size = height*((width+7)/8)
}ks_asperite_mask;

// never used.
typedef struct ks_asperite_path{

} ks_asperite_path;

typedef struct ks_asperite_tag{
    u16                 from_frame;
    u32                 to_frame;
    u8                  loop_animation_direction; // forward, reverse, ping-pong
    u8                  for_future[8];
    u8                  rgb_tag[3];
    u8                  extra; // zero
    ks_asperite_string  name;
}ks_asperite_tag;

typedef struct ks_asperite_tags{
    u16                 num_tags;
    u8                  for_future[8];
    ks_asperite_tag     *tags;
}ks_asperite_tags;

typedef struct ks_asperite_palette_entry{
    u16                     flags; // 1: has name
    ks_asperite_rgba_color  color;
    ks_asperite_string      name; // if flag == 1
}ks_asperite_palette_entry;

typedef struct ks_asperite_palette{
    u32                 new_size;
    u32                 first_color; // index to change
    u32                 last_color;  // index to change
    u8                  for_future[8];
}ks_asperite_palette;

typedef struct ks_asperite_userdata{
    u32                     flags; // 1: has text, 2: has color
    ks_asperite_string      text; // if flags have 1
    ks_asperite_rgba_color  color;  // if flags color
}ks_asperite_userdata;

typedef struct ks_asperite_slice_key{
    u32                     frame_number;
    i32                     slice_x;
    i32                     slice_y;
    u32                     slice_width; // can be 0 if this slice hidden in the animation from the given frame
    u32                     slice_height;
    // if flags have 1
    i32                     center_x;
    i32                     center_y;
    u32                     center_width;
    u32                     center_height;
    // if flags have 2
    i32                     pivot_x;
    i32                     pivot_y;
}ks_asperite_slice_key;

typedef struct ks_asperite_slice{
    u32                     num_keys;
    u32                     flags; // 1: 9 pathes slice, 2: has pivot information
    u32                     reserved;
    ks_asperite_string      name;
    ks_asperite_slice_key   *keys;
}ks_asperite_slice;

typedef struct ks_asperite_chunk{
    u32                 size;
    u16                 type;
    union {
        // 0x0004 or 0x0011
        ks_asperite_old_palette     old_palette;
        // 0x2004
        ks_asperite_layar           layer;
        // 0x2005
        ks_asperite_cel             cel;
        // 0x2006
        ks_asperite_cel_extra       cel_extra;
        // 0x2007
        ks_asperite_profile         profile;
        // 0x2017
         ks_asperite_path           path;
         // 0x2018
         ks_asperite_tags           tags;
         // 0x2019
         ks_asperite_palette        palette;
         // 0x2020
         ks_asperite_userdata       userdata;
         // 0x2022
         ks_asperite_slice          slice;
    } data;
}ks_asperite_chunk;

typedef struct ks_asperite_frame{
    u32                 bytes_frame;
    // u16              magic number    0xF1FA
    u16                 frame_duration; // in milliseconds
    u8                  for_future_f[2];
    u32                 num_chunks; // new field which specifies the number of "chunks" in this frame (if 0, use old)
    ks_asperite_chunk*  chunks;
}ks_asperite_frame;

typedef struct ks_aseprite_file{
    u32                 file_size;
    // u16              magic number 0xA5E0
    u16                 frames;
    u16                 width;
    u16                 height;
    u16                 color_depth; // 32bpp = RGBA, 16bpp = Grayscale, 8bpp = Indexed
    u32                 flags;
    u16                 speed; // deprecated
    u32                 zero_1;
    u32                 zero_2;
    u8                  palette_entry;
    u8                  ignore_1[3];
    u16                 num_colors; // 0 means 256 for old sprites
    u8                  pixel_width; // pixed ratio is pixel_width/pixel_height
    u8                  pixel_height;
    i16                 x;
    i16                 y;
    u16                 grid_width; // zero if there is no grid, grid size is 16x16 on default
    u16                 grid_height;
    u8                  for_future[84]; // set to zero
    ks_asperite_frame   frame;
}ks_aseprite_file;

ks_io_decl_custom_func(ks_asperite_string);
ks_io_decl_custom_func(ks_asperite_rgba_color);
ks_io_decl_custom_func(ks_asperite_grayscale_color);
ks_io_decl_custom_func(ks_asperite_indexed_color);
ks_io_decl_custom_func(ks_asperite_packet);
ks_io_decl_custom_func(ks_asperite_old_palette);
ks_io_decl_custom_func(ks_asperite_layer);
ks_io_decl_custom_func(ks_asperite_raw_cel);
ks_io_decl_custom_func(ks_asperite_compressed_cel);
ks_io_decl_custom_func(ks_asperite_cel);
ks_io_decl_custom_func(ks_asperite_cel_extra);
ks_io_decl_custom_func(ks_asperite_profile);
ks_io_decl_custom_func(ks_asperite_mask);
ks_io_decl_custom_func(ks_asperite_path);
ks_io_decl_custom_func(ks_asperite_tag);
ks_io_decl_custom_func(ks_asperite_tags);
ks_io_decl_custom_func(ks_asperite_palette_entry);
ks_io_decl_custom_func(ks_asperite_palette);
ks_io_decl_custom_func(ks_asperite_userdata);
ks_io_decl_custom_func(ks_asperite_slice_key);
ks_io_decl_custom_func(ks_asperite_slice);
ks_io_decl_custom_func(ks_asperite_chunk);
ks_io_decl_custom_func(ks_asperite_frame);
ks_io_decl_custom_func(ks_asperite_file);
