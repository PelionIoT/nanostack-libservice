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
#ifdef STANDARD_MALLOC
#include <stdlib.h>
#endif

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
#endif

static void heap_failure(heap_fail_t reason)
{
    if (heap_failure_callback) {
        heap_failure_callback(reason);
    }
}

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

//Fix return value allways good fo aligment
static int heap_alloc_internal_check(int16_t alloc_size)
{
    int temp_int, temp_sector = 0, temp_size;
    temp_size = sizeof(int);
    if (heap_main == 0) {
        heap_failure(NS_DYN_MEM_HEAP_SECTOR_UNITIALIZED);
        alloc_size = 0;
    } else if (alloc_size < 1) {
        heap_failure(NS_DYN_MEM_ALLOCATE_SIZE_NOT_VALID);
        alloc_size = 0;
    }


    if (alloc_size) {
        //
        temp_sector = alloc_size;
        temp_int = (temp_sector % temp_size);
        if (temp_int) {
            temp_sector += (temp_size - temp_int);
        }


        temp_sector /= temp_size;

        temp_int = (heap_size / temp_size);
        temp_int -= 2;
        if (temp_sector > temp_int) {
            heap_failure(NS_DYN_MEM_ALLOCATE_SIZE_NOT_VALID);
            temp_sector = 0;
        }
    }
    return temp_sector;
}
#endif


static int8_t ns_sector_validate(int *sector_start, int direction)
{
    int8_t ret_val = -1;
    int size_start, size_end;
    if (direction) {
        //Up
        size_start = *sector_start++;
        if (size_start < 0) {
            sector_start += -(size_start);
        } else {
            sector_start += size_start;
        }
        size_end = *sector_start;
    } else {
        size_end = *sector_start--;
        if (size_end < 0) {
            sector_start -= -(size_end);
        } else {
            sector_start -= size_end;
        }
        size_start = *sector_start;
    }
    if (size_start == size_end) {
        ret_val = 0;
    }
    return ret_val;
}

void *ns_dyn_mem_alloc(int16_t alloc_size)
{
#ifndef STANDARD_MALLOC

    int *ptr = heap_main_end;
    void *retval = 0;
    int h_size, alloc_sector, s_size;
    int moved = 0;

    alloc_sector = heap_alloc_internal_check(alloc_size);
    if (alloc_sector) {
        h_size = (heap_size / sizeof(int));
        platform_enter_critical();

        while (moved < h_size) {
            if (ns_sector_validate(ptr, 0) == 0) {
                s_size = *ptr;
                if (s_size == 0) {
                    heap_failure(NS_DYN_MEM_HEAP_SECTOR_CORRUPTED);
                    retval = 0;
                    break;
                } else if (s_size < 0) {
                    //Free
                    if (((-s_size) >= alloc_sector)) {
                        /*found block*/
                        //Convert Current size
                        s_size = -s_size;

                        if (s_size > (alloc_sector + 4)) {
                            //debug("Bigger\r\n");
                            //Set sector start len & Move pointer to Allocated data sector
                            *ptr = alloc_sector;
                            //Move Pointer  to next free place
                            ptr -= alloc_sector;
                            //Set return Pointer
                            retval = (void *) ptr;
                            ptr--;
                            //Set End length
                            *ptr-- = alloc_sector;
                            //Now set new slice start & End len fields
                            //Move pointer to New Free Sector
                            //Calculate new sector size
                            alloc_sector += 2; //

                            s_size = (s_size - alloc_sector);

                            *ptr-- = -(s_size);
                            ptr -= s_size;
                            *ptr = -(s_size);
                            if (mem_stat_info_ptr) {
                                alloc_sector -= 2;
                            }
                        } else {
                            //debug("USE Same\r\n");
                            //Set sector start len & Move pointer to Allocated data sector
                            *ptr = s_size;
                            ptr -= s_size;
                            retval = (void *) ptr;
                            ptr--;
                            //Set End length
                            *ptr-- = s_size;
                        }
                        break;
                    } else {
                        //Set Number from negative to Positive
                        s_size = -s_size;
                    }
                }
                //Move sector to next
                //Allocated sector
                moved += (s_size + 2);
                ptr -= 2; //Skip Both of length fileds
                ptr -= s_size;
                if (heap_main == ptr) {
                    break;
                }
            } else {
                heap_failure(NS_DYN_MEM_HEAP_SECTOR_CORRUPTED);
                retval = 0;
                break;
            }
        }


        if (mem_stat_info_ptr) {
            alloc_size = (alloc_sector * sizeof(int));
            if (retval) {
                //Update Allocate OK
                dev_stat_update(DEV_HEAP_ALLOC_OK, alloc_size);

            } else {
                //Update Allocate Fail
                dev_stat_update(DEV_HEAP_ALLOC_FAIL, alloc_size);
            }
        }
        platform_exit_critical();
    }
    return retval;
#else
    uint32_t value;

    void *retval = 0;
    if (alloc_size) {
        platform_enter_critical();
        retval = malloc(alloc_size);
        platform_exit_critical();
    } else {
        heap_free_corrupt();
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
    int *ptr = heap_main;
    void *retval = 0;
    int h_size, alloc_sector, s_size;
    int moved = 0; //int16_t -->int

    alloc_sector = heap_alloc_internal_check(alloc_size);
    if (alloc_sector) {
        h_size = (heap_size / sizeof(int));
        platform_enter_critical();
        while (moved < h_size) {
            if (ns_sector_validate(ptr, 1) == 0) {
                s_size = *ptr;
                if (s_size == 0) {
                    heap_failure(NS_DYN_MEM_HEAP_SECTOR_CORRUPTED);
                    retval = 0;
                    break;
                } else if (s_size < 0) {
                    //Free
                    if (((-s_size) >= alloc_sector)) {
                        /*found block*/
                        //Convert Current size
                        s_size = -s_size;
                        if (s_size > (alloc_sector + 4)) {
                            //debug("Bigger\r\n");
                            //Set sectror start len & Move pointer to Allocated data sector
                            *ptr++ = alloc_sector;
                            //Set return Pointer
                            retval = (void *) ptr;
                            ptr += alloc_sector;
                            *ptr++ = alloc_sector;
                            //Now set new slice start & End len fields
                            //Move pointer to New Free Sector
                            //Calculate new sector size
                            alloc_sector += 2; //
                            s_size = (s_size - alloc_sector);

                            *ptr++ = -(s_size);
                            ptr += s_size;
                            *ptr = -(s_size);
                            if (mem_stat_info_ptr) {
                                alloc_sector -= 2;
                            }
                        } else {
                            //debug("USE Same\r\n");
                            //Move pointer to Allocated data sector
                            *ptr++ = s_size;
                            //Set return Pointer
                            retval = (void *) ptr;
                            ptr += s_size;//Move Pointer  to next free place
                            *ptr = s_size;
                        }
                        break;
                    } else {
                        //Set Number from negative to Positive
                        s_size = -s_size;
                    }

                }
                //Allocated sector
                //Move
                //Set Move field
                moved += (s_size + 2);
                ptr += 2; //Skip Both of length fileds
                ptr += s_size; //Move Pointer  to next free place
                if (heap_main_end == ptr) {
                    break;
                }
            } else {
                heap_failure(NS_DYN_MEM_HEAP_SECTOR_CORRUPTED);
                retval = 0;
                break;
            }
        }
        if (mem_stat_info_ptr) {
            alloc_size = (alloc_sector * sizeof(int));
            if (retval) {

                //Update Allocate OK
                dev_stat_update(DEV_HEAP_ALLOC_OK, alloc_size);

            } else {
                //Update Allocate Fail
                dev_stat_update(DEV_HEAP_ALLOC_FAIL, alloc_size);
            }
        }
        platform_exit_critical();
    }
    return retval;

#else
    uint32_t value;

    void *retval = 0;
    if (alloc_size) {
        platform_enter_critical();
        retval = malloc(alloc_size);
        platform_exit_critical();
        if (retval) {
            value = (uint32_t) retval;
            if (value < 0x20000000 || value > 0x2001FFFF) {
                tracef(TRACE_LEVEL_ERROR, "mem", "Malloc fail");
                heap_free_corrupt();
                return 0;
            }
        }
    } else {
        heap_free_corrupt();
    }
    return retval;
#endif
}

void heap_df(void)
{

}

static void ns_free_and_merge_next_sector(int *cur_sector, int size)
{
    int *end_sector;
    int next_len;
    end_sector = cur_sector;
    end_sector += size + 1;
    if (end_sector != heap_main_end) {
        end_sector++;
        next_len = *end_sector;

        if (next_len > 0) {
            //Convert End of len
            end_sector--;
        } else {
            //Merge First to second
            //Set First End
            //Skip Data And sector start of len
            //debug("Free Next\r\n");
            next_len = -(next_len);
            end_sector += (next_len + 1);
            size += (next_len + 2);
        }

    }
    //Set new sector start & End length
    *cur_sector = -(size);
    *end_sector = -(size);
}

static void ns_merge_prev_sector(int *cur_sector)
{
    int *prev_sector = 0;
    int prev_len, cur_len;

    if (cur_sector != heap_main) {
        prev_sector = cur_sector;
        prev_sector--;
        prev_len = *prev_sector;

        if (prev_len < 0) {
            //Merge First to second
            //Set First End
            //Skip Data And sector start of len
            //debug("Merge Prev\r\n");
            prev_len = -(prev_len);
            prev_sector -= (prev_len + 1); // SET PREV Start

            cur_len = *cur_sector;
            cur_len = -cur_len; //Invert
            cur_sector += (cur_len + 1); //New End pointer
            prev_len += (cur_len + 2); // SET NEW LEN
        } else {
            prev_sector = 0;
        }

    }
    //Set new sector start & End length
    if (prev_sector) {
        *prev_sector = -(prev_len);
        *cur_sector = -(prev_len);
    }
}

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
    } else if ((size < 0) || ((ptr + size) >= heap_main_end)) {
        heap_failure(NS_DYN_MEM_POINTER_NOT_VALID);
    } else {
        // Validate Sector
        if (ns_sector_validate(ptr, 1) != 0) {
            heap_failure(NS_DYN_MEM_HEAP_SECTOR_CORRUPTED);
        } else {
            //Set Pointer to sector end len
            //Link always prev sector
            //Sub sequency Free
            //Check  prev sector
            //Try Link Next between cur
            ns_free_and_merge_next_sector(ptr, size);
            //Try Merge Prev with cur
            ns_merge_prev_sector(ptr);
            if (mem_stat_info_ptr) {
                //Update Free Counter
                size = (size * sizeof(int));
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
#ifdef USE_IAR
#pragma optimize=none
#endif
#endif

uint8_t heap_cor_scan(void)
{
#ifndef STANDARD_MALLOC
#ifdef HEAP_CORRUPT_CHECK
    if (heap_main) {
        int *ptr;
        int size, tail_size, h_size;
        int jump_size;
        int scanned = 0;
        h_size = heap_size / 4;
        ptr = heap_main;

        platform_enter_critical();
        while (scanned < h_size) {
            size = *ptr++;
            if (size < 0) {
                jump_size = -size;

            } else {
                jump_size = size;
            }
            if (jump_size == 0) {
                heap_free_corrupt();
                platform_exit_critical();
                return 0;
            }
            ptr += jump_size;
            tail_size = *ptr++;
            if (size != tail_size) {
                heap_free_corrupt();
                platform_exit_critical();
                return 0;
            }
            scanned += (jump_size + 2);
        }
        platform_exit_critical();
    }
#endif
#endif
    return 0;
}

void heap_scan(void)
{
//#if 0
//#ifndef STANDARD_MALLOC
//
//  if(heap_main)
//  {
//#ifdef HAVE_DEBUG
//  int16_t *ptr = (int16_t *)heap_main;
//  int16_t size;
//  int16_t scanned = 0;
//  uint8_t *ptr_8;
//  uint16_t out;
//  ptr_8 = &(heap_main[0]);
//  out = (uint16_t) ptr_8;
//  debug_hex((out >> 8));
//  debug_hex((out & 0xFF));
//  debug(" st\r\n");
//  ptr_8 = (heap_main + heap_size);
//  out = (uint16_t) ptr_8;
//  debug_hex((out >> 8));
//  debug_hex((out & 0xFF));
//  debug(" end\r\n");
//  platform_enter_critical();
//  while(scanned < heap_size)
//  {
//      size = *ptr;
//      debug_put('@');
//
//      out = (uint16_t) ptr;
//      debug_hex((out >> 8));
//      debug_hex((out & 0xFF));
//      debug_put('=');
//      if (size > 0)
//      {
//          //buffer_t * buf;
//          debug_put('a');
//          /*ptr++;
//          buf = (buffer_t *) ptr;
//
//          debug_hex(buf->originator);
//          debug(" ori, info ");
//          debug_hex(buf->info);
//
//          debug_hex(buf->options.event);
//          debug(" event\r\n");
//          ptr--;   */
//
//      }
//      if (size < 0)
//      {   debug_put('f');
//          size = -size;
//      }
//      if (size == 0)
//      {
//          heap_failure(NS_DYN_MEM_HEAP_SECTOR_CORRUPTED);
//          platform_exit_critical();
//          return;
//      }
//      debug_int(size);
//      //debug_put(':');
//
//      ptr_8 = (uint8_t*) ptr;
//      ptr_8 += (size + 4);
//      ptr = (int16_t *) ptr_8;
//      scanned += (size + 4);
//      debug("\r\n");
//  }
//  platform_exit_critical();
//  if (scanned != HEAP_SIZE)
//  {
//      heap_failure(NS_DYN_MEM_HEAP_SECTOR_CORRUPTED);
//  }
//  debug("\r\n");
//#endif
//  }
//#endif
//#endif
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
