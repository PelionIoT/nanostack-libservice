/*
 * Copyright (c) 2014-2015 ARM Limited. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <stdint.h>
#include <string.h>
#include "nsdynmemLIB.h"
#include "platform/arm_hal_interrupt.h"
#include <stdlib.h>

void (*heap_failure_callback)(heap_fail_t);

#ifndef STANDARD_MALLOC
static int *heap_main = 0;
static int *heap_main_end = 0;
static uint16_t heap_size = 0;

typedef enum mem_stat_update_t {
    DEV_HEAP_ALLOC_OK,
    DEV_HEAP_ALLOC_FAIL,
    DEV_HEAP_FREE,
} mem_stat_update_t;


static mem_stat_t *mem_stat_info_ptr = 0;


static void heap_failure(heap_fail_t reason)
{
    if (heap_failure_callback) {
        heap_failure_callback(reason);
    }
}

#endif

void ns_dyn_mem_init(uint8_t *heap, uint16_t h_size, void (*passed_fptr)(heap_fail_t), mem_stat_t *info_ptr)
{
#ifndef STANDARD_MALLOC
    int *ptr;
    int temp_int, temp_size;
    temp_size = sizeof(int);
    /* Do memory aligment check */
    temp_int = ((uintptr_t)heap % temp_size);
    if (temp_int) {
        heap += (temp_size - temp_int);
        h_size -= (temp_size - temp_int);
    }

    /* Make correction for total length also */
    temp_int = (h_size % temp_size);
    if (temp_int) {
        h_size -= (temp_size - temp_int);
    }
    heap_main = (int *)heap; // SET Heap Pointer
    heap_size = h_size; //Set Heap Size
    temp_int = (h_size / temp_size);
    temp_int -= 2;
    ptr = heap_main;
    *ptr = -(temp_int);
    ptr += (temp_int + 1);
    *ptr = -(temp_int);
    heap_main_end = ptr;
    //RESET Memory by Hea Len
    if (info_ptr) {
        mem_stat_info_ptr = info_ptr;
        memset(mem_stat_info_ptr, 0, sizeof(mem_stat_t));
        mem_stat_info_ptr->heap_sector_size = heap_size;
    }
#endif
    heap_failure_callback = passed_fptr;
}

const mem_stat_t *ns_dyn_mem_get_mem_stat(void)
{
#ifndef STANDARD_MALLOC
    return mem_stat_info_ptr;
#else
    return NULL;
#endif
}

#ifndef STANDARD_MALLOC
void dev_stat_update(mem_stat_update_t type, int16_t size)
{
    if (mem_stat_info_ptr) {
        switch (type) {
            case DEV_HEAP_ALLOC_OK:
                mem_stat_info_ptr->heap_sector_alloc_cnt++;
                mem_stat_info_ptr->heap_sector_allocated_bytes += size;
                if (mem_stat_info_ptr->heap_sector_allocated_bytes_max < mem_stat_info_ptr->heap_sector_allocated_bytes) {
                    mem_stat_info_ptr->heap_sector_allocated_bytes_max = mem_stat_info_ptr->heap_sector_allocated_bytes;
                }
                mem_stat_info_ptr->heap_alloc_total_bytes += size;
                break;
            case DEV_HEAP_ALLOC_FAIL:
                mem_stat_info_ptr->heap_alloc_fail_cnt++;
                break;
            case DEV_HEAP_FREE:
                mem_stat_info_ptr->heap_sector_alloc_cnt--;
                mem_stat_info_ptr->heap_sector_allocated_bytes -= size;
                break;
        }
    }
}

//Fix return value to be always good for alignment
static int heap_alloc_internal_check(int16_t alloc_size)
{
    int temp_sector = 0;

    if (heap_main == 0) {
        heap_failure(NS_DYN_MEM_HEAP_SECTOR_UNITIALIZED);
    } else if (alloc_size < 1) {
        heap_failure(NS_DYN_MEM_ALLOCATE_SIZE_NOT_VALID);
    } else if (alloc_size > (heap_size - 2 * sizeof(int)) ) {
        heap_failure(NS_DYN_MEM_ALLOCATE_SIZE_NOT_VALID);
    } else if (alloc_size) {
        temp_sector = (alloc_size + sizeof(int) - 1) / sizeof(int);
    }
    return temp_sector;
}

// Checks that sector length indicators equals
// Sector has format: Length (word) | data area in words | Length (word)
// For direction, use 1 for direction up and -1 for down
static int8_t ns_sector_validate(int *sector_start, int direction)
{
    int8_t ret_val = -1;
    int size_start = *sector_start;
    if (direction > 0) {
        sector_start += (1 + abs(size_start));
    } else {
        sector_start -= (1 + abs(size_start));
    }

    if (size_start && size_start == *sector_start) {
        ret_val = 0;
    }
    return ret_val;
}
#endif

// For direction, use 1 for direction up and -1 for down
static void *ns_dyn_mem_internal_alloc(int16_t alloc_size, int direction)
{
    int *ptr = direction > 0?heap_main:heap_main_end;
    int *end = direction > 0?heap_main_end:heap_main;
    int *temp_ptr;
    void *retval = 0;
    int h_size, alloc_sector, s_size, free_size;
    int moved = 0;

    alloc_sector = heap_alloc_internal_check(alloc_size);
    if (alloc_sector) {
        h_size = (heap_size / sizeof(int));
        platform_enter_critical();

        while (moved < h_size) {
            if (ns_sector_validate(ptr, direction) == 0) {
                s_size = *ptr;

                if (s_size < 0) {
                    s_size = -s_size;
                    if (s_size >= alloc_sector) {
                        /*found block*/
                        if (s_size > (alloc_sector + 4)) {
                            //There is enough room for new hole so create it first
                            temp_ptr = (ptr + ((alloc_sector + 2) * direction));
                            free_size = s_size - alloc_sector - 2;
                            *temp_ptr = -(free_size);
                            temp_ptr += direction * (free_size + 1);
                            *temp_ptr = -(free_size);
                        }else{
                            alloc_sector = s_size;
                        }

                        //Set sector start len & Move pointer to Allocated data sector
                        *ptr = alloc_sector;
                        if (direction > 0) {
                            ptr++;
                            retval = (void *) ptr;
                            ptr += alloc_sector;
                        } else {
                            ptr -= alloc_sector;
                            retval = (void *) ptr;
                            ptr--;
                        }
                        *ptr = alloc_sector;
                        break;
                    }
                }
                //Move sector to next
                //Allocated sector
                moved += (s_size + 2);
                ptr += direction*(s_size + 2);
            } else {
                heap_failure(NS_DYN_MEM_HEAP_SECTOR_CORRUPTED);
                retval = 0;
                break;
            }
        }


        if (mem_stat_info_ptr) {
            // If alloc fail needs to collect size of failed alloc,
            // this code needs to be revisited
            alloc_size = ((alloc_sector + 2) * sizeof(int));

            if (retval) {
                //Update Allocate OK
                dev_stat_update(DEV_HEAP_ALLOC_OK, alloc_size);

            } else {
                //Update Allocate Fail
                dev_stat_update(DEV_HEAP_ALLOC_FAIL, 0);
            }
        }
        platform_exit_critical();
    }
    return retval;
}

void *ns_dyn_mem_alloc(int16_t alloc_size)
{
#ifndef STANDARD_MALLOC
    return ns_dyn_mem_internal_alloc(alloc_size, -1);
#else
    void *retval = 0;
    if (alloc_size) {
        platform_enter_critical();
        retval = malloc(alloc_size);
        platform_exit_critical();
    }
    return retval;
#endif
}


#ifdef STANDARD_MALLOC
#ifdef USE_IAR
#pragma optimize=none
#endif
#endif
void *ns_dyn_mem_temporary_alloc(int16_t alloc_size)
{
#ifndef STANDARD_MALLOC
    return ns_dyn_mem_internal_alloc(alloc_size, 1);
#else

    void *retval = 0;
    if (alloc_size) {
        platform_enter_critical();
        retval = malloc(alloc_size);
        platform_exit_critical();
    }
    return retval;
#endif
}

#ifndef STANDARD_MALLOC
static void ns_free_and_merge_with_adjacent_sectors(int *cur_sector, int size)
{
    // Theory of operation: Sector is always in form | Len | Data | Len |
    // So we need to check length of previous (if current not heap start)
    // and next (if current not heap end) sectors. Negative length means
    // free memory so we can merge freed sector with those.

    int *start = cur_sector;
    int *end = cur_sector + size + 1;
    int total_size = size;

    if (cur_sector != heap_main) {
        cur_sector--;
        if (*cur_sector < 0) {
            total_size += (2 - *cur_sector);
            start -= (2 - *cur_sector);
        }
        cur_sector++;
    }

    if (end != heap_main_end) {
        end++;
        if (*end < 0) {
            total_size += (2 - *end);
            end += (1 - *end);
        }else{
            end--;
        }
    }
    *cur_sector = -total_size;
    *start = -total_size;
    *end = -total_size;
}
#endif

#ifdef STANDARD_MALLOC
#ifdef USE_IAR
#pragma optimize=none
#endif
#endif
void ns_dyn_mem_free(void *block)
{
#ifndef STANDARD_MALLOC
    int *ptr = block;
    int size;

    if (!block) {
        return;
    }

    if (!heap_main) {
        heap_failure(NS_DYN_MEM_HEAP_SECTOR_UNITIALIZED);
        return;
    }

    platform_enter_critical();
    ptr --;
    //Read Current Size
    size = *ptr;
    if (size < 0) {
        heap_failure(NS_DYN_MEM_DOUBLE_FREE);
    } else if (ptr < heap_main || ptr >= heap_main_end) {
        heap_failure(NS_DYN_MEM_POINTER_NOT_VALID);
    } else if ((ptr + size) >= heap_main_end) {
        heap_failure(NS_DYN_MEM_POINTER_NOT_VALID);
    } else {
        // Validate Sector
        if (ns_sector_validate(ptr, 1) != 0) {
            heap_failure(NS_DYN_MEM_HEAP_SECTOR_CORRUPTED);
        } else {
            ns_free_and_merge_with_adjacent_sectors(ptr, size);
            if (mem_stat_info_ptr) {
                //Update Free Counter
                size = ((size+2) * sizeof(int));
                dev_stat_update(DEV_HEAP_FREE, size);
            }
        }
    }
    platform_exit_critical();
#else
    platform_enter_critical();
    free(block);
    platform_exit_critical();
#endif
}

#ifndef STANDARD_MALLOC
#ifdef DEV_STAT

int16_t ns_dyn_mem_longest_free_sector(void)
{

    int *ptr;
    int size, h_size;
    int scanned = 0;
    int16_t longest_sector = 0;
    if (heap_main) {
        h_size = heap_size / 4;
        ptr = heap_main;
        platform_enter_critical();
        while (scanned < h_size) {
            size = *ptr;
            if (size < 0) {
                size = -size;
                if (size > longest_sector) {
                    longest_sector = size;
                }
            }
            if (size == 0) {
                heap_failure(NS_DYN_MEM_HEAP_SECTOR_CORRUPTED);
                platform_exit_critical();
                return 0;
            }
            ptr += size + 2;
            scanned += (size + 2);
        }
        platform_exit_critical();
    }
    return longest_sector;
}
#endif
#endif
