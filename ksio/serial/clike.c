#include "./clike.h"


KS_INLINE bool ks_io_print_indent(ks_io* io,  char indent, bool serialize){
    if(!serialize) return true;
    char* c = alloca(io->indent+1);
    ks_string_add(io->str, ks_char_array_fill(indent, io->indent+1, c));
    return true;
}

KS_INLINE bool ks_io_print_endl(ks_io* io, bool serialize){
    if(!serialize) return true;
    ks_string_add(io->str, "\n");
    return true;
}

KS_INLINE char* ks_char_array_fill(char v, u32 length, char c[]){
    for(u32 i=0; i<length-1; i++){
        c[i] = v;
    }
    c[length-1] = 0;
    return c;
}


KS_INLINE u32 ks_io_value_text(ks_io* io, ks_value_ptr v, ks_value_type type, u32 offset,  bool serialize){
    if(!serialize){
        u32 first = ks_string_first_not_of(io->str, io->seek, "\t\n ");

        u64 p;
        u32 read;

        int ret = sscanf(io->str->data + io->seek + first,"%ld", &p);
        read  = ks_string_first_not_of(io->str, io->seek + first, "0123456789");
        if((ret != 1 && io->seek + first + read < io->str->length) || io->seek + first + read >= io->str->length) {
            ks_error("Failed to read value");
            return 0;
        }

        switch (type) {
        case KS_VALUE_U64:
        {
            u64 *u = v.u64v;
            u+= offset;
            *u=p;
            break;
        }
        case KS_VALUE_U32:
        {
            u32 *u = v.u32v;
            u+= offset;
            *u=p;
            break;
        }
        case KS_VALUE_U16:
        {
            u16 *u = v.u16v;
            u+= offset;
            *u=p;
            break;
        }
        case KS_VALUE_U8:
        {
            u8* u = v.u8v;
            u+= offset;
            *u = p;
            break;
        }
        case KS_VALUE_I64:
        {
            i64 *u = v.i64v;
            u+= offset;
            *u=p;
            break;
        }
        case KS_VALUE_I32:
        {
            i32 *u = v.i32v;
            u+= offset;
            *u=p;
            break;
        }
        case KS_VALUE_I16:
        {
            i16 *u = v.i16v;
            u+= offset;
            *u=p;
            break;
        }
        case KS_VALUE_I8:
        {
            i8* u = v.i8v;
            u+= offset;
            *u = p;
            break;
        }
        default:
            break;
        }

        io->seek += first + read;
        return  first + read;
    } else {
        char s[10];

        int p;

        switch (type) {
        case KS_VALUE_U64:
        {
            u64 *u = v.u64v;
            u+= offset;
            p = *u;
            break;
        }
        case KS_VALUE_U32:
        {
            u32 *u = v.u32v;
            u+= offset;
            p = *u;
            break;
        }
        case KS_VALUE_U16:
        {
            u16 *u = v.u16v;
            u+= offset;
            p = *u;
            break;
        }
        case KS_VALUE_U8:
        {
            u8* u = v.u8v;
            u+= offset;
            p = *u;
            break;
        }
        default:
            p=0;
            break;
        }

        snprintf(s, 10, "%d", p);
        ks_string_add(io->str, s);

    }
    return 1;
}


KS_INLINE u32 ks_io_prop_text(ks_io* io, const char* str, const char* delims, bool serialize){
    if(!serialize){
        u32 first = ks_string_first_not_of(io->str, io->seek, "\t\n ");
        u32 prop_length = ks_string_first_of(io->str, io->seek + first, delims);

        if(prop_length == 0 && io->seek + first != io->str->length){
            ks_error("Failed to read property name");
            return 0;
        }

        io->seek += first + prop_length;
        return first + prop_length;
    }
    else {
        ks_string_add(io->str, str);

    }
    return 1;
}

KS_INLINE u32 ks_io_text_len(ks_io* io, u32 length, const char* str, bool serialize){
    if(!serialize){
        u32 first = ks_string_first_not_of(io->str, io->seek, "\t\n ");
        bool ret = strncmp(io->str->data + io->seek + first, str, length) == 0;
        if(!ret){
            return false;
        }

        io->seek += first + length;
        return first + length;
    }
    else {
        ks_string_add(io->str, str == NULL ? "" : str);

    }
    return 1;
}

KS_INLINE u32 ks_io_text(ks_io* io, const char* str, bool serialize){
    return ks_io_text_len(io, strlen(str), str, serialize);
}

KS_INLINE u32 ks_io_fixed_text(ks_io* io, const char* str, bool serialize){
    if(!ks_io_text(io, str, serialize)){
        ks_error("Unexcepted syntax, excepted \"%s\"", str);
        return false;
    }
    return true;
}

KS_INLINE bool ks_io_key_clike(ks_io* io, const ks_io_funcs* funcs, const char* name, bool fixed, bool serialize){
    ks_io_print_indent(io, '\t', serialize);

    if(!serialize){
        io->seek += ks_string_first_not_of(io->str, io->seek, "\t\n ");
        if( io->seek >= io->str->length || io->str->data[io->seek] == '}'){
            return false;
        }
    }
    u32 seek_tmp = io->seek;
    ks_io_fixed_text(io, ".", serialize );
    u32 ret =   ks_io_prop_text(io, name, "\t\n =", serialize) + ks_io_fixed_text(io, "=", serialize );

    if(!serialize && fixed && strncmp(name, io->str->data + io->seek - ret, strlen(name)) != 0){
       io->seek = seek_tmp;
        return false;
    }

    return ret != 0;
}

KS_INLINE bool ks_io_value_clike(ks_io* io, const ks_io_funcs* funcs, ks_value value, u32 offset,  bool serialize){
    switch(value.type){
    case KS_VALUE_MAGIC_NUMBER:
        ks_io_print_indent(io,'\t', serialize);
        return ks_io_text(io, "// Magic number : ", serialize) && ks_io_text(io, value.ptr.str, serialize) && ks_io_print_endl(io, serialize);
    default:
        break;
    }
    u32 ret = ks_io_value_text(io, value.ptr, value.type, offset, serialize);
    if(ret == 0) return false;
    ret += ks_io_fixed_text(io, ",", serialize);
    ks_io_print_endl(io, serialize);
    return ret != 0;
}

bool ks_io_string_clike(ks_io* io, const ks_io_funcs* funcs, ks_array_data array,ks_string*  str,  bool serialize){
    if(!ks_io_fixed_text(io, "\"", serialize)) return false;

    if(serialize){
        u32 length;
        length = MIN(array.length, strlen(str->data));
        // TODO: escape sequence"
        ks_io_text_len(io, length, str->data, serialize);
    }
    else {
        u32 l =ks_string_first_c_of(io->str, io->seek, '\"');
        ks_string_set_n( str, l, io->str->data + io->seek);
        io->seek += l;
    }
    return ks_io_fixed_text(io, "\"", serialize) &&  ks_io_fixed_text(io, ",", serialize) && ks_io_print_endl(io, serialize);
}

KS_INLINE bool ks_io_array_begin_clike(ks_io* io, const ks_io_funcs* funcs,  ks_array_data arr, bool serialize){
    if(!arr.fixed_length){
        ks_io_fixed_text(io, "(", serialize);
        switch (arr.value.type) {
        case KS_VALUE_U8:
            ks_io_fixed_text(io, "u8", serialize);
            break;
        case KS_VALUE_U16:
            ks_io_fixed_text(io, "u16", serialize);
            break;
        case KS_VALUE_U32:
            ks_io_fixed_text(io, "u32", serialize);
            break;
        case KS_VALUE_U64:
            ks_io_fixed_text(io, "u64", serialize);
            break;
        case KS_VALUE_I8:
            ks_io_fixed_text(io, "i8", serialize);
            break;
        case KS_VALUE_I16:
            ks_io_fixed_text(io, "i16", serialize);
            break;
        case KS_VALUE_I32:
            ks_io_fixed_text(io, "i32", serialize);
            break;
        case KS_VALUE_I64:
            ks_io_fixed_text(io, "i64", serialize);
            break;
        case KS_VALUE_OBJECT:{
            ks_object_data *data = arr.value.ptr.obj;
            ks_io_fixed_text(io, data->type , serialize);
            break;
        }
        default:
            // TODO ARRAY
            break;
        }
        ks_io_fixed_text(io, "[", serialize);
        ks_io_value_text(io, (ks_value_ptr){.u32v = &arr.length}, KS_VALUE_U32, 0, serialize);
        ks_io_fixed_text(io, "]", serialize);
        ks_io_fixed_text(io, ")", serialize);
    }
    bool ret = ks_io_fixed_text(io, "{", serialize) && ks_io_print_endl(io, serialize);
    io->indent ++;
    return ret;
}

KS_INLINE bool ks_io_array_elem_clike(ks_io* io,  const ks_io_funcs* funcs, ks_array_data arr, u32 index, bool serialize){
     ks_io_print_indent(io,'\t', serialize);
    if(!ks_io_value(io, funcs, arr.value, index, serialize)) return false;

    return true;
}

KS_INLINE bool ks_io_array_end_clike(ks_io* io, const ks_io_funcs* funcs,  ks_array_data arr, bool serialize){
    io->indent--;
    return  ks_io_print_indent(io,'\t', serialize)&&
            ks_io_fixed_text(io, "}", serialize)  &&
            ks_io_fixed_text(io, ",", serialize) &&
            ks_io_print_endl(io, serialize) ;
}

KS_INLINE bool ks_io_object_clike(ks_io* io, const ks_io_funcs* funcs,  ks_object_data obj, u32 offset, bool serialize){
    if(! (ks_io_fixed_text(io, "{", serialize)  &&
            ks_io_print_endl(io, serialize))) return false;

    io->indent++;

    ks_object_func objfunc = serialize ? obj.serializer : obj.deserializer;
    if(serialize){
        if(! objfunc(io, funcs, obj.data, offset)) return false;
        io->indent--;
        if(! (ks_io_print_indent(io,'\t', serialize)&&
              ks_io_fixed_text(io, "}", serialize)  &&
              (io->indent == 0 || ks_io_fixed_text(io, ",", serialize))  &&
              ks_io_print_endl(io, serialize))) return false;
    } else {
        for(;;){
            u32 readp = objfunc(io, funcs, obj.data, offset);
            if( ks_io_text(io, "}", serialize)) break;
            // if unknown property skip
            if(readp == 0){
                i32 br = 0;
                do{
                    io->seek ++;
                    if(io->str->data[io->seek] == '{') br++;
                    else if(io->str->data[io->seek] == '}') br--;
                }while(io->str->data[io->seek] != ',' || br != 0);
                io->seek++;
            }
        }
        io->indent--;
        if(!(io->indent == 0 || ks_io_fixed_text(io, ",", serialize))) return false;
    }

    return true;
}

ks_io_funcs_impl(clike)
