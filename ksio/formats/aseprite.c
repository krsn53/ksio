#include "aseprite.h"


ks_io_begin_custom_func(ks_asperite_string)
    ks_u16(length);
    ks_str_p(characters);
ks_io_end_custom_func(ks_asperite_string)

ks_io_begin_custom_func(ks_asperite_color)
    ks_u8(red);
    ks_u8(green);
    ks_u8(blue);
ks_io_end_custom_func(ks_asperite_color)

ks_io_begin_custom_func(ks_asperite_rgba_color)
    ks_u8(red);
    ks_u8(green);
    ks_u8(blue);
    ks_u8(alpha);
ks_io_end_custom_func(ks_asperite_rgba_color)

ks_io_begin_custom_func(ks_asperite_grayscale_color)
    ks_u8(value);
    ks_u8(alpha);
ks_io_end_custom_func(ks_asperite_grayscale_color)

ks_io_begin_custom_func(ks_asperite_indexed_color)
    ks_u8(index);
ks_io_end_custom_func(ks_asperite_indexed_color)


ks_io_begin_custom_func(ks_asperite_packet)
    ks_u8(num_palette_entries);
    ks_u8(num_colors);
    ks_arr_obj_len(colors, ks_asperite_color, ks_access(num_colors));
ks_io_end_custom_func(ks_asperite_packet)

ks_io_begin_custom_func(ks_asperite_old_palette)
    ks_u16(num_packets);
    ks_arr_obj_len(packets, ks_asperite_packet, ks_access(num_packets));
ks_io_end_custom_func(ks_asperite_old_palette)

ks_io_begin_custom_func(ks_asperite_layar)
    ks_u16(flags);
    ks_u16(type);
    ks_u16(child_level);
    ks_u16(default_width);
    ks_u16(default_height);
    ks_u16(blend_mode);
    ks_u8(opacity);
    ks_arr_u8(for_future);
    ks_obj(layer_name, ks_asperite_string);
ks_io_end_custom_func(ks_asperite_layar)


ks_io_begin_custom_func(ks_asperite_raw_cel)
    ks_u16(width);
    ks_u16(height);
    switch (ks_io_top_userdata_from(io, 0)->val) {
        case 8:
            ks_obj(pixels.indexed, ks_asperite_indexed_color);
            break;
        case 16:
            ks_obj(pixels.grayscale, ks_asperite_indexed_color);
            break;
        case 32:
            ks_obj(pixels.rgba, ks_asperite_rgba_color);
            break;
        default:
            return false;
    }
ks_io_end_custom_func(ks_asperite_raw_cel)

// unsupport for zlib
ks_io_begin_custom_func(ks_asperite_compressed_cel)
    return false;
ks_io_end_custom_func(ks_asperite_compressed_cel)

ks_io_begin_custom_func(ks_asperite_cel)
    ks_u16(layer_index);
    ks_i16(x);
    ks_i16(y);
    ks_u8(opacity);
    ks_u16(type);
    ks_arr_u8(for_future);
    switch (ks_access(type)) {
        case KS_ASPERITE_RAW_CEL:
            ks_obj(data.raw_cel, ks_asperite_raw_cel);
            break;
        case KS_ASPERITE_LINKED_CEL:
            ks_u16(data.linked_frame_position);
            break;
        case KS_ASPERITE_COMPRESSED_IMAGE:
            ks_obj(data.compressed_cel, ks_asperite_compressed_cel);
            break;
    }
ks_io_end_custom_func(ks_asperite_cel)

ks_io_begin_custom_func(ks_asperite_cel_extra)
    ks_u32(flags);
    ks_u32(x);
    ks_u32(y);
    ks_u32(width);
    ks_u32(height);
    ks_arr_u8(for_future);
ks_io_end_custom_func(ks_asperite_cel_extra)

ks_io_begin_custom_func(ks_asperite_profile)
    ks_u16(type);
    ks_u16(frags);
    ks_u32(fixed_gamma);
    ks_arr_u8(reserved);
ks_io_end_custom_func(ks_asperite_profile)

// depricated
ks_io_begin_custom_func(ks_asperite_mask)
    ks_i16(x);
    ks_i16(y);
    ks_u16(width);
    ks_u16(height);
    ks_arr_u8(for_future);
    ks_obj(name, ks_asperite_string);
    ks_arr_u8_len(bitmap_data, ks_access(height)*(ks_access(width)+7)/8); // size = height*((width+7)/8)
ks_io_end_custom_func(ks_asperite_mask)

// never ks_used.
ks_io_begin_custom_func(ks_asperite_path)

ks_io_end_custom_func(ks_asperite_path)

ks_io_begin_custom_func(ks_asperite_tag)
    ks_u16(from_frame);
    ks_u32(to_frame);
    ks_u8(loop_animation_direction);
    ks_arr_u8(for_future);
    ks_arr_u8(rgb_tag);
    ks_u8(extra);
    ks_obj(name, ks_asperite_string);
ks_io_end_custom_func(ks_asperite_tag)

ks_io_begin_custom_func(ks_asperite_tags)
    ks_u16(num_tags);
    ks_arr_u8(for_future);
    ks_arr_obj_len(tags, ks_asperite_tag, ks_access(num_tags));
ks_io_end_custom_func(ks_asperite_tags)

ks_io_begin_custom_func(ks_asperite_palette_entry)
    ks_u16(flags);
    ks_obj(color, ks_asperite_rgba_color);
    ks_obj(name, ks_asperite_string);
ks_io_end_custom_func(ks_asperite_palette_entry)

ks_io_begin_custom_func(ks_asperite_palette)
    ks_u32(new_size);
    ks_u32(first_color);
    ks_u32(last_color);
    ks_arr_u8(for_future);
ks_io_end_custom_func(ks_asperite_palette)

ks_io_begin_custom_func(ks_asperite_userdata)
    ks_u32(flags);
    if((ks_access(flags) & KS_ASPERITE_USERDATA_HAS_TEXT) != 0){
        ks_obj(text, ks_asperite_string);
    }
    if((ks_access(flags) & KS_ASPERITE_USERDATA_HAS_COLOR) != 0){
        ks_obj(color, ks_asperite_string);
    }
ks_io_end_custom_func(ks_asperite_userdata)

ks_io_begin_custom_func(ks_asperite_slice_key)
    ks_u32(frame_number);
    ks_i32(slice_x);
    ks_i32(slice_y);
    ks_u32(slice_width);
    ks_u32(slice_height);

    if((ks_io_top_userdata_from(io, 0)->val & KS_ASPERITE_9_PATCHES_SLICE) != 0){
        ks_i32(center_x);
        ks_i32(center_y);
        ks_u32(center_width);
        ks_u32(center_height);
    }
    if((ks_io_top_userdata_from(io, 0)->val & KS_ASPERITE_HAS_PIVOT_INFO) != 0){
        ks_i32(pivot_x);
        ks_i32(pivot_y);
    }
ks_io_end_custom_func(ks_asperite_slice_key)

ks_io_begin_custom_func(ks_asperite_slice)
    ks_u32(num_keys);
    ks_u32(flags); // 1: 9 pathes slice, 2: has pivot ks_information
    ks_u32(reserved);
    ks_obj(name, ks_asperite_string);
    ks_io_push_userdata(io, (ks_io_userdata){.val = ks_access(flags)});
    ks_arr_obj_len(keys, ks_asperite_slice_key, ks_access(num_keys));
    ks_io_pop_userdata(io);
ks_io_end_custom_func(ks_asperite_slice)

ks_io_begin_custom_func(ks_asperite_chunk)
    ks_u32(size);
    ks_u16(type);
    switch (ks_access(type)) {
        case 0x0004:
        case 0x0011:
            ks_obj(data.old_palette, ks_asperite_old_palette);
            break;
        case 0x2004:
            ks_obj(data.layer, ks_asperite_layer);
            break;
        case 0x2005:
            ks_obj(data.cel, ks_asperite_cel);
            break;
        case 0x2006:
            ks_obj(data.cel_extra, ks_asperite_cel_extra);
            break;
        case 0x2007:
            ks_obj(data.profile, ks_asperite_profile);
            break;
        case 0x2017:
            ks_obj(data.path, ks_asperite_path);
            break;
        case 0x2018:
            ks_obj(data.tags, ks_asperite_tags);
            break;
        case 0x2019:
            ks_obj(data.palette, ks_asperite_palette);
            break;
        case 0x2020:
            ks_obj(data.userdata, ks_asperite_userdata);
            break;
        case 0x2022:
            ks_obj(data.slice, ks_asperite_slice);
            break;
    }
ks_io_end_custom_func(ks_asperite_chunk)

ks_io_begin_custom_func(ks_asperite_frame)
    ks_u32(bytes_frame);
    ks_magic_number(((const char[]){0xfa, 0xf1}));
    ks_u16(frame_duration);
    ks_arr_u8(for_future_f);
    ks_u32(num_chunks);
    ks_arr_obj_len(chunks, ks_asperite_chunk, ks_access(num_chunks));
ks_io_end_custom_func(ks_asperite_frame)

ks_io_begin_custom_func(ks_aseprite_file)
    ks_u32(file_size);
    ks_magic_number(((const char[]){0xe0, 0xa5}));
    ks_u16(frames);
    ks_u16(width);
    ks_u16(height);
    ks_u16(color_depth);
    ks_u32(flags);
    ks_u16(speed);
    ks_u32(zero_1);
    ks_u32(zero_2);
    ks_u8(palette_entry);
    ks_arr_u8(ignore_1);
    ks_u16(num_colors);
    ks_u8(pixel_width);
    ks_u8(pixel_height);
    ks_i16(x);
    ks_i16(y);
    ks_u16(grid_width);
    ks_u16(grid_height);
    ks_arr_u8(for_future); // set to zero
    ks_io_push_userdata(io, (ks_io_userdata){.val = ks_access(color_depth)});
    ks_obj(frame, ks_asperite_frame);
    ks_io_pop_userdata(io);
ks_io_end_custom_func(ks_aseprite_file)
