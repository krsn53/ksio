#include "midi.h"
#include "math.h"
#include <string.h>

#include "../serial/clike.h"
#include "../serial/binary.h"
#include "../serial/deleter.h"

bool ks_io_variable_length_number(ks_io* io, const ks_io_methods*methods, ks_property prop, ks_io_serial_type serial_type){
    if(methods->type != KS_SERIAL_BINARY) {
        return ks_io_property(io, methods, prop, serial_type);
    }

    if(serial_type == KS_IO_SERIALIZER){
        u8 out[4] = { 0 };
        u32 in = *prop.value.ptr.u32v;

        ks_property p = prop;
        p.value.type = KS_VALUE_U8;

        out[0] = in & 0x7f;
        in >>= 7;

        i32 i = 0;

        while(in != 0){
            i++;
            out[i] = in & 0x7f;
            out[i] |= 0x80;
            in >>= 7;
        }
        for(; i>=0; i--){
            p.value.ptr.u8v =  &out[i];
            if(!ks_serializer_call(ks_io_value, io, methods, p.value, 0)) return false;
        }

        return true;
    } else {
        u8 in=0;

        ks_property p = prop;
        p.value.type = KS_VALUE_U8;
        p.value.ptr.u8v = & in;
        u32 out=0;

        do{
            if(serial_type == KS_IO_DESERIALIZER){
                if(!ks_deserializer_call(ks_io_value, io, methods, p.value, 0)) return false;
            } else {
                if(!ks_othertype_call(ks_io_value, io, methods, p.value, 0)) return false;
            }
            out <<= 7;
            out |= in & 0x7f;
        }while(in >= 0x80);

        *prop.value.ptr.u32v = out;

        return true;
    }
}

ks_io_begin_custom_func(ks_midi_event)
    ks_func_prop(ks_io_variable_length_number, ks_prop_u32(delta));

    if(__SERIAL_TYPE == KS_IO_DESERIALIZER && __INDEX == 0){
        ks_io_push_userdata(io, (ks_io_userdata){ .v.val = -1 });
    }

    ks_u8(status);
    switch (ks_access(status)) {
        //SysEx
        case 0xf0:
        case 0xf7:
            ks_func_prop(ks_io_variable_length_number, ks_prop_u32(message.sys_ex.length));
            ks_arr_u8_len(message.sys_ex.data, ks_access(message.sys_ex.length));
            break;
       //meta
        case 0xff:
            ks_u8(message.meta.type);
            ks_func_prop(ks_io_variable_length_number, ks_prop_u32(message.meta.length));
            if(ks_access(message.meta.type) > 0x00 && ks_access(message.meta.type) < 0x08){
                ks_str_len(message.meta.data, ks_access(message.meta.length));
            } else {
                 ks_arr_u8_len(message.meta.data, ks_access(message.meta.length));
            }
            if(__SERIAL_TYPE == KS_IO_DESERIALIZER){
                if(ks_access(message.meta.type) == 0x2f){
                    ks_io_pop_userdata(io);
                }
            }

            break;
       //midi event
        case 0xf1:
        case 0xf3:
            ks_u8(message.data[0]);
            break;
        case 0xf6:
        case 0xf8:
        case 0xfa:
        case 0xfb:
        case 0xfc:
        case 0xfe:
            ks_arr_u8(message.data);
            break;
        default:
            if(__SERIAL_TYPE == KS_IO_DESERIALIZER){
                ks_io_userdata* ud = ks_io_top_userdata_from(io, 0);
                if(ud->v.val == -1 && (ks_access(status) >> 4) == 0x9){
                    ud->v.val = ks_access(status) & 0x0f;
                }
            }

            switch (ks_access(status) >> 4) {
            case 0xc:
            case 0xd:
                ks_u8(message.data[0]);
                break;
            case 0x8:
            case 0x9:
            case 0xa:
            case 0xb:
            case 0xe:
                 ks_arr_u8(message.data);
                break;
            default:
            {
                ks_io_userdata* ud = ks_io_top_userdata_from(io, 0);
                if(__SERIAL_TYPE == KS_IO_DESERIALIZER && ud->v.val != -1){
                    ks_access(message.data[0]) = ks_access(status);
                    ks_access(status) = 0x90 + ud->v.val;
                    ks_u8(message.data[1]);
                }
                else {
                    ks_error("Detected invalid status bit : %d", ks_access(status));
                    return false;
                }
            }
            }
    }
ks_io_end_custom_func(ks_midi_event)

ks_io_begin_custom_func(ks_midi_track)
    ks_magic_number("MTrk");
    ks_u32(length);

    if(__SERIAL_TYPE == KS_IO_SERIALIZER || __SERIAL_TYPE == KS_IO_OTHER_TYPE){
        ks_arr_obj_len(events, ks_midi_event, ks_access(num_events));
    } else {
        ks_access(num_events) = 0;

        ks_array_data arr = ks_prop_arr_data_len(ks_access(length) / 3, events, ks_val_obj(events, ks_midi_event), false);

        if(!ks_io_array_begin(__IO, __METHODS, &arr, 0, __SERIAL_TYPE)) return false;

        for(;;){
            __METHODS->array_elem(__IO, __METHODS, arr, ks_access(num_events));
            if(ks_access(events)[ks_access(num_events)].status == 0xff &&
                ks_access(events)[ks_access(num_events)].message.meta.type == 0x2f){
                ks_access(num_events) ++;
                break;
            }
            ks_access(num_events) ++;
        }


        ks_access(events) = realloc(ks_access(events), sizeof(ks_midi_event)*ks_access(num_events));

        if(!ks_io_array_end(__IO, __METHODS, &arr, 0)) return false;
    }
ks_io_end_custom_func(ks_midi_track)

ks_io_begin_custom_func(ks_midi_file)
    ks_magic_number("MThd");
    ks_u32(length);
    ks_u16(format);
    ks_u16(num_tracks);
    ks_u16(resolution);
    ks_arr_obj_len(tracks, ks_midi_track, ks_access(num_tracks));
ks_io_end_custom_func(ks_midi_file)

static void copy_midi_event(ks_midi_event* dest, const ks_midi_event* src){
    *dest = *src;

    switch (src->status) {
    case 0xf0:
    case 0xf7:
        if(src->message.sys_ex.length == 0) break;
        dest->message.sys_ex.data = malloc(src->message.sys_ex.length*sizeof(char));
        memcpy(dest->message.sys_ex.data, src->message.sys_ex.data, src->message.sys_ex.length);
        break;
    case 0xff:
        if(src->message.meta.length == 0) break;
        dest->message.meta.data = malloc(src->message.meta.length * sizeof(char));
        memcpy(dest->message.meta.data, src->message.meta.data, src->message.meta.length);
        break;
    default:
        break;
    }
}

ks_midi_file* ks_midi_file_new(){
    ks_midi_file* midi = calloc(1, sizeof(ks_midi_file));
    return midi;
}

void ks_midi_file_free(ks_midi_file* file){
    ks_midi_tracks_free(file->num_tracks, file->tracks);
    free(file);
}

static int compare_midi_event_time (const void *a, const void *b){
    const ks_midi_event* a1 = a, *b1 =b;
    i64 ret = (i64)a1->time - (i64)b1->time;
    if(ret != 0) return ret;
    ret = a1->status - b1->status;
    if(ret != 0) return ret;
    return a1->message.data[0] - b1->message.data[1];
}

static u32 calc_delta_bits(u32 val){
    u32 ret = 0;
    do{
        ret++;
        val>>= 7;
    }while(val != 0);
    return ret;
}

void ks_midi_file_calc_time(ks_midi_file* file){
    for(u32 t =0; t< file->num_tracks; t++){
        file->tracks[t].events[0].time = file->tracks[t].events[0].delta;
        for(u32 e=1; e<file->tracks[t].num_events; e++){
            file->tracks[t].events[e].time = file->tracks[t].events[e-1].time + file->tracks[t].events[e].delta;
        }
    }
}

ks_midi_file* ks_midi_file_conbine_tracks(ks_midi_file* file){
    ks_midi_file_calc_time(file);

    ks_midi_file *ret = malloc(sizeof(ks_midi_file));

    ret->format = 0;
    ret->length = file->length;
    ret->num_tracks = 1;
    ret->resolution = file->resolution;
    ret->tracks = ks_midi_tracks_new(1);

    u32 num_events = 0;
    u32 length =0;
    for(u32 i=0; i<file->num_tracks; i++){
        length += file->tracks[i].length;
        num_events += file->tracks[i].num_events;
    }
    // end of track * (num_tracks-1)
    num_events -= file->num_tracks -1;

    ret->tracks[0].length = length;
    ret->tracks[0].num_events = num_events;
    ret->tracks[0].events = ks_midi_events_new(num_events);

    u32 e=0;
    u32 end_time=0;

    for(u32 i=0; i<file->num_tracks; i++){
        for(u32 j=0; j<file->tracks[i].num_events; j++){
            //end of track
            if(file->tracks[i].events[j].status == 0xff &&
            file->tracks[i].events[j].message.meta.type == 0x2f){
                end_time = MAX(end_time, file->tracks[i].events[j].time);
                u32 delta = file->tracks[i].events[j].delta;
                i32 delta_bits=calc_delta_bits(delta);
                ret->tracks[0].length -= 3 + delta_bits;
                continue;
            }
            copy_midi_event(&ret->tracks[0].events[e],  &file->tracks[i].events[j]);
            e++;
        }
    }
    ret->tracks[0].events[e] = (ks_midi_event){
        .status = 0xff,
        .message.meta.type = 0x2f,
        .message.meta.length = 0,
        .time = end_time,
        .delta = end_time - ret->tracks[0].events[e-1].time,
    };
    ret->tracks[0].length += calc_delta_bits(ret->tracks[0].events[e].delta) + 3; // 1 delta bit + 3 event bits

    qsort(ret->tracks->events, num_events, sizeof(ks_midi_event), compare_midi_event_time);

    {
        u32 old = ret->tracks[0].events[0].delta;
        i32 old_bits = calc_delta_bits(old);
        u32 new = ret->tracks[0].events[0].time;
        i32 new_bits = calc_delta_bits(new);

        ret->tracks[0].length -= old_bits - new_bits;
    }

    for(u32 i=1; i<ret->tracks[0].num_events; i++){
        u32 old = ret->tracks[0].events[i].delta;
        u32 new;
        i32 old_bits = calc_delta_bits(old);
        ret->tracks[0].events[i].delta = new = ret->tracks[0].events[i].time - ret->tracks[0].events[i-1].time;
        i32 new_bits = calc_delta_bits(new);

        ret->tracks[0].length -= old_bits - new_bits;
    }

    return ret;
}



ks_midi_track* ks_midi_tracks_new(u32 num_tracks){
    return calloc(num_tracks, sizeof(ks_midi_track));
}

ks_midi_event* ks_midi_events_new(u32 num_events){
    return calloc(num_events, sizeof(ks_midi_event));
}


void ks_midi_tracks_free(u32 num_tracks, ks_midi_track* tracks){
    for(u32 t=0; t < num_tracks; t++){
        ks_midi_events_free(tracks[t].num_events, tracks[t].events);
    }
    free(tracks);
}

void ks_midi_events_free(u32 num_events, ks_midi_event* events){
    for(u32 e=0; e<num_events; e++){
        switch (events[e].status) {
        case 0xf0:
        case 0xf7:
            if(events[e].message.sys_ex.data != NULL && events[e].message.sys_ex.length > 0){
                free(events[e].message.sys_ex.data);
            }
            break;
        case 0xff:
            if(events[e].message.meta.data != NULL && events[e].message.meta.length > 0){
                free(events[e].message.meta.data);
            }
            break;
        default:
            break;
        }
    }
    free(events);
}


