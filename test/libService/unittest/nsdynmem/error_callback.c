#include "nsdynmemLIB.h"
#include <stdint.h>

heap_fail_t current_heap_error;
static bool failed;

void heap_fail_callback(heap_fail_t err) {
	current_heap_error = err;
	failed = true;
}

void reset_heap_error() {
	failed = false;
}

bool heap_have_failed() {
	return failed;
}