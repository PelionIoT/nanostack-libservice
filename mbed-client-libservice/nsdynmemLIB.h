/*
 * Copyright (c) 2014 ARM. All rights reserved.
 */


/**
 * \file nsdynmemLIB.h
 * \brief Dynamical Memory API for library model
 *
 */
#ifndef NSDYNMEMLIB_H_
#define NSDYNMEMLIB_H_
#ifdef __cplusplus
extern "C" {
#endif
/*!
 * \enum heap_fail_t
 * \brief Dynamically heap system failure call back event types.
 */
typedef enum {
    NS_DYN_MEM_NULL_FREE,               /**< ns_dyn_mem_free(), NULL pointer free [obsolete - no longer faulted]  */
    NS_DYN_MEM_DOUBLE_FREE,                     /**< ns_dyn_mem_free(), Possible double pointer free */
    NS_DYN_MEM_ALLOCATE_SIZE_NOT_VALID, /**< Allocate size is 0 or smaller or size is bigger than max heap size  */
    NS_DYN_MEM_POINTER_NOT_VALID,       /**< ns_dyn_mem_free(), try to free pointer which not at heap sector */
    NS_DYN_MEM_HEAP_SECTOR_CORRUPTED,   /**< Heap system detect sector corruption */
    NS_DYN_MEM_HEAP_SECTOR_UNITIALIZED /**< ns_dyn_mem_free(), ns_dyn_mem_temporary_alloc() or ns_dyn_mem_alloc() called before ns_dyn_mem_init() */
} heap_fail_t;

/**
 * /struct mem_stat_t
 * /brief Struct for Memory stats Buffer structure
 */
typedef struct mem_stat_t {
    /*Heap stats*/
    int16_t heap_sector_size;                   /**< Heap total Sector len. */
    int16_t heap_sector_alloc_cnt;              /**< Reserved Heap sector cnt. */
    int16_t heap_sector_allocated_bytes;        /**< Reserved Heap data in bytes. */
    int16_t heap_sector_allocated_bytes_max;    /**< Reserved Heap data in bytes max value. */
    uint32_t heap_alloc_total_bytes;            /**< Total Heap allocated bytes. */
    uint32_t heap_alloc_fail_cnt;               /**< Counter for Heap allocation fail. */
} mem_stat_t;

/**
  * \brief Init and set Dynamical heap pointer and length.
  *
  * \param heap_ptr Pointer to dynamically heap buffer
  * \param heap_size size of the heap buffer
  * \return None
  */
extern void ns_dyn_mem_init(uint8_t *heap, uint16_t h_size, void (*passed_fptr)(heap_fail_t), mem_stat_t *info_ptr);


/**
  * \brief Free allocated memory.
  *
  * \param heap_ptr Pointer to allocated memory
  *
  * \return 0, Free OK
  * \return <0, Free Fail
  */
extern void ns_dyn_mem_free(void *heap_ptr);
/**
  * \brief Allocate temporary data.
  *
  * Space allocate started from beginning of the heap sector
  *
  * \param alloc_size Allocated data size
  *
  * \return 0, Allocate Fail
  * \return >0, Pointer to allocated data sector.
  */
extern void *ns_dyn_mem_temporary_alloc(int16_t alloc_size);
/**
  * \brief Allocate long period data.
  *
  * Space allocate started from end of the heap sector
  *
  * \param alloc_size Allocated data size
  *
  * \return 0, Allocate Fail
  * \return >0, Pointer to allocated data sector.
  */
extern void *ns_dyn_mem_alloc(int16_t alloc_size);
#ifdef __cplusplus
}
#endif
#endif /* NSDYNMEMLIB_H_ */

