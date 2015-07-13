
#include "ip6string.h"
#include "ip6tos_stub.h"

ip6tos_def ip6tos_stub;

void ip6tos(const void *ip6addr, char *p)
{
    if (ip6tos_stub.h) {
        p[0] = ip6tos_stub.c;
        p[1] = '\0';
    } else {
        p[0] = '\0';
    }
}
