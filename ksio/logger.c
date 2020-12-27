#include "logger.h"

KS_INLINE int ks_log(int type, const char* message, ...){
    const unsigned pre_size = sizeof ("Warning : ");
    const char* pre[NUM_KS_LOG_TYPES]= {
        "Info    : ",
        "Warning : ",
        "Error   : "
    };
    u32 size = pre_size + strlen(message) + 3;
    char* str = alloca(size);
    snprintf(str, size, "%s%s\n",pre[type], message);
    va_list va;
    va_start(va, message);
    int ret =vprintf(str, va);
    va_end(va);

    return ret;
}
