#include "aseprite.h"

#include "../serial/binary.h"

ks_io_begin_custom_func(ks_aseprite_string)
    ks_u16(length);
    ks_str_len(characters, ks_access(length));
ks_io_end_custom_func(ks_aseprite_string)

ks_io_begin_custom_func(ks_aseprite_color)
    ks_u8(red);
    ks_u8(green);
    ks_u8(blue);
ks_io_end_custom_func(ks_aseprite_color)

ks_io_begin_custom_func(ks_aseprite_rgba_color)
    ks_u8(red);
    ks_u8(green);
    ks_u8(blue);
    ks_u8(alpha);
ks_io_end_custom_func(ks_aseprite_rgba_color)

ks_io_begin_custom_func(ks_aseprite_grayscale_color)
    ks_u8(value);
    ks_u8(alpha);
ks_io_end_custom_func(ks_aseprite_grayscale_color)

ks_io_begin_custom_func(ks_aseprite_indexed_color)
    ks_u8(index);
ks_io_end_custom_func(ks_aseprite_indexed_color)


ks_io_begin_custom_func(ks_aseprite_packet)
    ks_u8(num_palette_entries);
    ks_u8(num_colors);
    ks_arr_obj_len(colors, ks_aseprite_color, ks_access(num_colors));
ks_io_end_custom_func(ks_aseprite_packet)

ks_io_begin_custom_func(ks_aseprite_old_palette)
    ks_u16(num_packets);
    ks_arr_obj_len(packets, ks_aseprite_packet, ks_access(num_packets));
ks_io_end_custom_func(ks_aseprite_old_palette)

ks_io_begin_custom_func(ks_aseprite_layer)
    ks_u16(flags);
    ks_u16(type);
    ks_u16(child_level);
    ks_u16(default_width);
    ks_u16(default_height);
    ks_u16(blend_mode);
    ks_u8(opacity);
    ks_arr_u8(for_future);
    ks_obj(layer_name, ks_aseprite_string);
ks_io_end_custom_func(ks_aseprite_layer)


ks_io_begin_custom_func(ks_aseprite_raw_cel)
    ks_u16(width);
    ks_u16(height);
    switch (ks_io_top_userdata_from(io, 1)->v.val) {
        case 8:
            ks_arr_obj_len(pixels.indexed, ks_aseprite_indexed_color, ks_access(width)* ks_access(height));
            break;
        case 16:
            ks_arr_obj_len(pixels.grayscale, ks_aseprite_indexed_color, ks_access(width)* ks_access(height));
            break;
        case 32:
            ks_arr_obj_len(pixels.rgba, ks_aseprite_rgba_color, ks_access(width)* ks_access(height));
            break;
        default:
            return false;
    }
ks_io_end_custom_func(ks_aseprite_raw_cel)


ks_io_begin_custom_func(ks_aseprite_compressed_cel)
    ks_u16(width);
    ks_u16(height);
    ks_arr_u8_len(data, ks_io_top_userdata_from(io, 0)->v.val -
                  (sizeof(u32) + sizeof (u16) + //chunk
                    sizeof(u16) + sizeof(i16)*2 + sizeof(u8) + sizeof(u16) + sizeof(u8)*7 +  // cel chunk
                   sizeof(u16)*2) ); // width height
ks_io_end_custom_func(ks_aseprite_compressed_cel)

ks_io_begin_custom_func(ks_aseprite_cel)
    ks_u16(layer_index);
    ks_i16(x);
    ks_i16(y);
    ks_u8(opacity);
    ks_u16(type);
    ks_arr_u8(for_future);
    switch (ks_access(type)) {
        case KS_ASEPRITE_RAW_CEL:
            ks_obj(data.raw_cel, ks_aseprite_raw_cel);
            break;
        case KS_ASEPRITE_LINKED_CEL:
            ks_u16(data.linked_frame_position);
            break;
        case KS_ASEPRITE_COMPRESSED_IMAGE:{
            ks_obj(data.compressed_cel, ks_aseprite_compressed_cel);
            break;
        }
    }
ks_io_end_custom_func(ks_aseprite_cel)

ks_io_begin_custom_func(ks_aseprite_cel_extra)
    ks_u32(flags);
    ks_u32(x);
    ks_u32(y);
    ks_u32(width);
    ks_u32(height);
    ks_arr_u8(for_future);
ks_io_end_custom_func(ks_aseprite_cel_extra)

ks_io_begin_custom_func(ks_aseprite_profile)
    ks_u16(type);
    ks_u16(frags);
    ks_u32(fixed_gamma);
    ks_arr_u8(reserved);
ks_io_end_custom_func(ks_aseprite_profile)

// depricated
ks_io_begin_custom_func(ks_aseprite_mask)
    ks_i16(x);
    ks_i16(y);
    ks_u16(width);
    ks_u16(height);
    ks_arr_u8(for_future);
    ks_obj(name, ks_aseprite_string);
    ks_arr_u8_len(bitmap_data, ks_access(height)*(ks_access(width)+7)/8); // size = height*((width+7)/8)
ks_io_end_custom_func(ks_aseprite_mask)

// never ks_used.
ks_io_begin_custom_func(ks_aseprite_path)

ks_io_end_custom_func(ks_aseprite_path)

ks_io_begin_custom_func(ks_aseprite_tag)
    ks_u16(from_frame);
    ks_u16(to_frame);
    ks_u8(loop_animation_direction);
    ks_arr_u8(for_future);
    ks_arr_u8(rgb_value);
    ks_u8(extra);
    ks_obj(name, ks_aseprite_string);
ks_io_end_custom_func(ks_aseprite_tag)

ks_io_begin_custom_func(ks_aseprite_tags)
    ks_u16(num_tags);
    ks_arr_u8(for_future);
    ks_arr_obj_len(tags, ks_aseprite_tag, ks_access(num_tags));
ks_io_end_custom_func(ks_aseprite_tags)

ks_io_begin_custom_func(ks_aseprite_palette_entry)
    ks_u16(flags);
    ks_obj(color, ks_aseprite_rgba_color);
    if((ks_access(flags) & KS_ASEPRITE_PALETTE_ENTRY_HAS_NAME)){
        ks_obj(name, ks_aseprite_string);
    }
ks_io_end_custom_func(ks_aseprite_palette_entry)

ks_io_begin_custom_func(ks_aseprite_palette)
    ks_u32(new_size);
    ks_u32(first_color);
    ks_u32(last_color);
    ks_arr_u8(for_future);
    ks_arr_obj_len(palette_entries, ks_aseprite_palette_entry, ks_access(last_color) - ks_access(first_color) + 1);
ks_io_end_custom_func(ks_aseprite_palette)

ks_io_begin_custom_func(ks_aseprite_userdata)
    ks_u32(flags);
    if((ks_access(flags) & KS_ASEPRITE_USERDATA_HAS_TEXT) != 0){
        ks_obj(text, ks_aseprite_string);
    }
    if((ks_access(flags) & KS_ASEPRITE_USERDATA_HAS_COLOR) != 0){
        ks_obj(color, ks_aseprite_string);
    }
ks_io_end_custom_func(ks_aseprite_userdata)

ks_io_begin_custom_func(ks_aseprite_slice_key)
    ks_u32(frame_number);
    ks_i32(slice_x);
    ks_i32(slice_y);
    ks_u32(slice_width);
    ks_u32(slice_height);

    if((ks_io_top_userdata_from(io, 0)->v.val & KS_ASEPRITE_9_PATCHES_SLICE) != 0){
        ks_i32(center_x);
        ks_i32(center_y);
        ks_u32(center_width);
        ks_u32(center_height);
    }
    if((ks_io_top_userdata_from(io, 0)->v.val & KS_ASEPRITE_HAS_PIVOT_INFO) != 0){
        ks_i32(pivot_x);
        ks_i32(pivot_y);
    }
ks_io_end_custom_func(ks_aseprite_slice_key)

ks_io_begin_custom_func(ks_aseprite_slice)
    ks_u32(num_keys);
    ks_u32(flags); // 1: 9 pathes slice, 2: has pivot ks_information
    ks_u32(reserved);
    ks_obj(name, ks_aseprite_string);
    ks_io_push_userdata(io, (ks_io_userdata){.v.val = ks_access(flags)});
    ks_arr_obj_len(keys, ks_aseprite_slice_key, ks_access(num_keys));
    ks_io_pop_userdata(io);
ks_io_end_custom_func(ks_aseprite_slice)

ks_io_begin_custom_func(ks_aseprite_chunk)
    ks_u32(size);
    ks_io_push_userdata(io, (ks_io_userdata){.v.val = ks_access(size)});
    ks_u16(type);
    switch (ks_access(type)) {
        case KS_ASEPRITE_OLD_PALETTE_CHUNK_1:
        case KS_ASEPRITE_OLD_PALETTE_CHUNK_2:
            ks_obj(data.old_palette, ks_aseprite_old_palette);
            break;
        case KS_ASEPRITE_LAYER_CHUNK:
            ks_obj(data.layer, ks_aseprite_layer);
            break;
        case KS_ASEPRITE_CEL_CHUNK:
            ks_obj(data.cel, ks_aseprite_cel);
            break;
        case KS_ASEPRITE_CEL_EXTRA_CHUNK:
            ks_obj(data.cel_extra, ks_aseprite_cel_extra);
            break;
        case KS_ASEPRITE_PROFILE_CHUNK:
            ks_obj(data.profile, ks_aseprite_profile);
            break;
        case KS_ASEPRITE_PATH_CHUNK:
            ks_obj(data.path, ks_aseprite_path);
            break;
        case KS_ASEPRITE_TAGS_CHUNK:
            ks_obj(data.tags, ks_aseprite_tags);
            break;
        case KS_ASEPRITE_PALETTE_CHUNK:
            ks_obj(data.palette, ks_aseprite_palette);
            break;
        case KS_ASEPRITE_USERDATA_CHUNK:
            ks_obj(data.userdata, ks_aseprite_userdata);
            break;
        case KS_ASEPRITE_SLICE_CHUNK:
            ks_obj(data.slice, ks_aseprite_slice);
            break;
    }
    ks_io_pop_userdata(io);
ks_io_end_custom_func(ks_aseprite_chunk)

ks_io_begin_custom_func(ks_aseprite_frame)
    ks_u32(bytes_frame);
    ks_magic_number(((const char[3]){0xfa, 0xf1, 0x00}));
    ks_u16(old_num_chunks);
    ks_u16(frame_duration);
    ks_arr_u8(for_future_f);
    ks_u32(num_chunks);
    ks_arr_obj_len(chunks, ks_aseprite_chunk, ks_access(old_num_chunks) == 0xffff ? ks_access(num_chunks): ks_access(old_num_chunks));
ks_io_end_custom_func(ks_aseprite_frame)

ks_io_begin_custom_func(ks_aseprite_file)
    ks_u32(file_size);
    ks_magic_number(((const char[3]){0xe0, 0xa5, 0x00}));
    ks_u16(num_frames);
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
    ks_io_push_userdata(io, (ks_io_userdata){.v.val = ks_access(color_depth)});
    ks_arr_obj_len(frames, ks_aseprite_frame, ks_access(num_frames));
    ks_io_pop_userdata(io);
ks_io_end_custom_func(ks_aseprite_file)
