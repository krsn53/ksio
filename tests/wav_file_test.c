#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <ksio/serial/binary.h>
#include <ksio/formats/wave.h>

#define SAMPLING_RATE 48000
#define OUTPUT_LENGTH SAMPLING_RATE;


int main( void )
{

  i32 buf_len = OUTPUT_LENGTH;
  i32 buf_size = sizeof(i16) * buf_len;
  i16 *writebuf = malloc( buf_size );


  {

    ks_wave_file dat;

    dat.chunk_size = sizeof(dat) + 4 - sizeof(u8*) + buf_size;
    dat.fmt_chunk_size = 16;
    dat.audio_format = 1;
    dat.num_channels = 1;
    dat.sampling_freq = SAMPLING_RATE;
    dat.bytes_per_sec = SAMPLING_RATE*2;
    dat.block_size = 2;
    dat.bits_per_sample = 16;
    dat.subchunk_size = sizeof(dat) - sizeof(u8*)  + buf_size - 114;
    dat.data = (u8*)writebuf;

    for(int i=0; i<SAMPLING_RATE; i++){
        // square
        writebuf[i] = ((i / (SAMPLING_RATE / 880)) & 1) ? INT16_MAX / 2 : INT16_MIN / 2;
    }

    ks_io_serialize_to_file(binary_little_endian, "out.wav", dat, ks_wave_file);

  }

  free(writebuf);
}
