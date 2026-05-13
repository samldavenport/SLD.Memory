#pragma once

#include "sld-memory-internal.hpp"

namespace sld {

    u32 calculate_pow_2_arena_size  (const u32 granularity_min);
    u32 calculate_pow_2_total_size  (const u32 size_min, const u32 arena_size_pow_2);
    u32 calculate_arena_size        (const u32 total_size_pow_2, const u32 granularity_min);


    SLD_MEMORY_API u32
    arena_memory_size(
        const u32 size,
        const u32 granularity) {

        const u32 size_pow_2_arena = calculate_pow_2_arena_size (granularity); 
        const u32 size_pow_2_total = calculate_pow_2_total_size (size, size_pow_2_granularity);


        assert(
            size_pow_2_arena != 0 &&
            size_pow_2_total != 0 &&
        );

        return(size_pow_2_total);
    }

    SLD_MEMORY_API arena_allocator*
    arena_memory_init(
        const u32 size,
        const u32 granularity,
        const u32 mem_size
        void*     memory) {

        const u32 size_pow_2_arena = calculate_pow_2_arena_size (granularity); 
        const u32 size_pow_2_total = calculate_pow_2_total_size (size, size_pow_2_granularity);
        assert(mem_size == size_pow_2_total);

        void* mem = malloc(mem_size);
        assert(mem != NULL);

        
    }
    
    SLD_MEMORY_API u32
    arena_memory_size_total(
        const arena_allocator* alctr) {

    }

    SLD_MEMORY_API u32
    arena_memory_size_free(
        const arena_allocator* alctr) {

    }

    SLD_MEMORY_API u32
    arena_memory_size_used(
        const arena_allocator* alctr) {

    }

    SLD_MEMORY_API u32
    arena_size_total(
        const arena* a) {

    }

    SLD_MEMORY_API u32
    arena_size_free(
        const arena* a) {

    }

    SLD_MEMORY_API u32
    arena_size_used(
        const arena* a) {

    }

    SLD_MEMORY_API u32
    arena_id(
        const arena* a) {

    }

    SLD_MEMORY_API u64
    arena_timestamp(
        const arena* a) {

    }

    SLD_MEMORY_API arena*           arena_alloc                 (arena_allocator* alctr);
    SLD_MEMORY_API void*            arena_free                  (arena_allocator* alctr);
    SLD_MEMORY_API void*            arena_push                  (arena* a, const u32 size, const u32 alignment = 0);
    SLD_MEMORY_API u32              arena_state_save            (arena* a);
    SLD_MEMORY_API void             arena_state_revert          (arena* a);
    SLD_MEMORY_API void             arena_reset                 (arena* a);

    u32
    calculate_total_size_pow_2(
        const u32 size_min) {

        assert(size_min != 0);
        const u32 size_pow_2 = size_is_pow_2(size_min)
            ? size_min
            : size_round_up_pow2(size_min);
        
        return(size_pow_2);
    }


    u32
    calculate_arena_size_pow_2(
        const u32 total_size_pow_2,
        const u32 granularity_min) {

        assert(
            total_size_pow_2 != 0                &&
            granularity_min  != 0                &&
            granularity_min  <= total_size_pow_2 &&
            size_is_pow_2(total_size_pow_2)
        );

        const u32 granularity_size_pow_2 = size_is_pow_2(granularity_min)
            ? size_align_pow_2   (granularity_min, total_size_pow_2)
            : size_round_up_pow2 (granularity_min);

        const u32 arena_size_pow_2 = size_align_pow_2(granularity_size_pow_2, total_size_pow_2);
        
        assert(arena_size_pow_2 < total_size_pow_2);
        return(arena_size_pow_2);

    }

};