#ifndef ERROR_CALLBACK_H
#define ERROR_CALLBACK_H
#ifdef __cplusplus
extern "C" {
#endif

#include "nsdynmemLIB.h"
#include <stdint.h>

extern heap_fail_t current_heap_error;

void heap_fail_callback(heap_fail_t err);
void reset_heap_error();
bool heap_have_failed();

#ifdef __cplusplus
}
#endif
#endif /*ERROR_CALLBACK_H*/