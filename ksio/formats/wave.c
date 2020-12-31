#include "wave.h"

ks_io_begin_custom_func(ks_wave_file)
    ks_magic_number("RIFF");
    ks_u32(chunk_size);
    ks_magic_number("WAVE");
    ks_magic_number("fmt ");
    ks_u32(fmt_chunk_size);
    ks_u16(audio_format);
    ks_u16(num_channels);
    ks_u32(sampling_freq);
    ks_u32(bytes_per_sec);
    ks_u16(block_size);
    ks_u16(bits_per_sample);

    ks_magic_number("data");
    ks_u32(subchunk_size);
    switch (ks_access(bits_per_sample)) {
        case 8:
            ks_arr_u8_len(data, ((ks_access(chunk_size) - 8)));
            break;
        case 16:
            ks_arr_u16_len(data, ((ks_access(chunk_size) - 8) >> 1));
            break;
    }
ks_io_end_custom_func(ks_wave_file)
