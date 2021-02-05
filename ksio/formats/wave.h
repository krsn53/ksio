#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../io.h"




typedef struct ks_wave_file{
    // magic number "RIFF"
    u32     chunk_size;
    // magic number "WAVE"
    // magic number "fmt "
    u32     fmt_chunk_size;
    u16     audio_format;
    u16     num_channels;
    u32     sampling_freq;
    u32     bytes_per_sec;
    u16     block_size;
    u16     bits_per_sample;
    // omit extention params

    // magic number "data"
    u32     subchunk_size;
    u8      *data;
}ks_wave_file;


ks_io_decl_custom_func(ks_wave_file);


#ifdef __cplusplus
}
#endif
