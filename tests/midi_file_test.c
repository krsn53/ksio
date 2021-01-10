#include "ksio/formats/midi.h"
#include "ksio/serial/clike.h"
#include "ksio/serial/binary.h"
#include "ksio/serial/deleter.h"
#include <stdio.h>

int main(int argc, char** argv) {
    ks_io *io = ks_io_new();
    ks_midi_file midi;

    if(!ks_io_read_file(io, "resources/test.mid")) return -1;

    ks_io_deserialize_begin(io, binary_big_endian, midi, ks_midi_file);
    ks_io_serialize_begin(io, clike, midi, ks_midi_file);

    printf("%s", io->str->data);

    ks_midi_file* midi2 = ks_midi_file_conbine_tracks(&midi);

    ks_io_serialize_begin(io, binary_big_endian, *midi2, ks_midi_file);

    FILE* fp = fopen("test0.mid", "wb");
    fwrite(io->str->data, 1, io->str->length, fp);
    fclose(fp);

    ks_io_delete(io, midi, ks_midi_file);

#if(KS_PRINT_DELETE_LOG)
    printf("%s\n", io->str->data);
#endif

    ks_io_delete(io, *midi2, ks_midi_file);
    free(midi2);



    ks_io_free(io);
    return 0;
}
