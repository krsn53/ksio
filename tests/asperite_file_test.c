#include "ksio/formats/aseprite.h"
#include "ksio/serial/clike.h"
#include "ksio/serial/binary.h"
#include "ksio/serial/deleter.h"


int main(int argc, char** argv) {
    ks_io *io = ks_io_new();
    ks_aseprite_file ase;

    if(!ks_io_read_file(io, "resources/test.ase")) return -1;

    ks_io_deserialize_begin(io, binary_little_endian, ase, ks_aseprite_file);
    ks_io_serialize_begin(io, clike, ase, ks_aseprite_file);

    printf("%s\n", io->str->data);

    ks_io_delete(io, ase, ks_aseprite_file);

#if(KS_PRINT_DELETE_LOG &&0)
    printf("%s\n", io->str->data);
#endif

    ks_io_free(io);
    return 0;
}
