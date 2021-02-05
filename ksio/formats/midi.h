#pragma once


#ifdef __cplusplus
extern "C" {
#endif

#include "../io.h"


typedef struct ks_midi_event{
    u64 time;
    u32 delta;
    u8 status;
    union{
        u8 data[2];

        struct{
            u32 length;
            u8* data;
        }sys_ex;

        struct{
            u8 type;
            u32 length;
            u8* data;
        }meta;

    }message;
} ks_midi_event;

typedef struct ks_midi_track{
    //chunk type MTrk
    u32             length;
    u32             num_events;
    ks_midi_event   * events;
} ks_midi_track;

typedef struct ks_midi_file{
    // chunk type MThd
    u32             length;
    u16             format;
    u16             num_tracks;
    u16             resolution;
    ks_midi_track   *tracks;
}ks_midi_file;

ks_io_decl_custom_func(ks_midi_event);
ks_io_decl_custom_func(ks_midi_track);
ks_io_decl_custom_func(ks_midi_file);

bool                    ks_io_variable_length_number                (ks_io* io, const ks_io_methods*methods, ks_property prop, ks_io_serial_type serialize);
ks_midi_file*           ks_midi_file_new                            ();
void                    ks_midi_file_free                           (ks_midi_file* file);
void                    ks_midi_file_calc_time                      (ks_midi_file* file);
ks_midi_file *          ks_midi_file_conbine_tracks                 (ks_midi_file *file);
ks_midi_track*          ks_midi_tracks_new                          (u32 num_tracks);
void                    ks_midi_tracks_free                         (u32 num_tracks, ks_midi_track* tracks);
ks_midi_event*          ks_midi_events_new                          (u32 num_events);
void                    ks_midi_events_free                         (u32 num_events, ks_midi_event* events);



#ifdef __cplusplus
}
#endif
