#pragma once

#include "sld-memory-internal.hpp"

namespace sld {


    //--------------------------------------------------------------------
    // API METHODS
    //--------------------------------------------------------------------

    SLD_MEMORY_API_IMPL u32
    arena_alctr_size(
        const u32 size,
        const u32 granularity) {

        const u32 size_pow_2_arena                = calculate_size_pow_2_arena                (granularity);
        const u32 size_pow_2_memory_arena_aligned = calculate_size_pow_2_memory_arena_aligned (size, size_pow_2_arena);
        const u32 count_arenas                    = calculate_count_arenas                    (size_pow_2_memory_arena_aligned, size_pow_2_arena);
        const u32 size_total                      = calculate_total_size                      (size_pow_2_memory_arena_aligned, size_pow_2_arena, count_arenas);

        return(size_pow_2_total);
    }

    SLD_MEMORY_API_IMPL arena_alctr*
    arena_alctr_init(
        const u32 size,
        const u32 granularity,
        const u32 mem_size,
        void*     mem_ptr) {

        assert(
            size        != 0
            granularity != 0
            mem_size    != 0
            mem_ptr     != NULL          
        );

        const u32 size_pow_2_arena                = calculate_size_pow_2_arena                (granularity);
        const u32 size_pow_2_memory_arena_aligned = calculate_size_pow_2_memory_arena_aligned (size, size_pow_2_arena);
        const u32 count_arenas                    = calculate_count_arenas                    (size_pow_2_memory_arena_aligned, size_pow_2_arena);
        const u32 size_total                      = calculate_total_size                      (size_pow_2_memory_arena_aligned, size_pow_2_arena, count_arenas);

        assert(mem_size == size_total);

        // cast pointers
        auto alctr                     = (arena_alctr*)mem_ptr;
        auto arena_mem                 = (void*)(alctr + STRUCT_SIZE_ARENA_ALCTR);
        auto arena_prev                = (arena*)(arena_mem);

        // arena info
        alctr->arena_info.count_free  = count_arenas;
        alctr->arena_info.count_total = count_arenas;
        alctr->arena_info.count_used  = 0;
        alctr->arena_info.size        = size_total;
        
        // arena list
        alctr->arena_list.used        = NULL;
        alctr->arena_list.free        = arena_prev;

        // first arena
        arena_prev->alctr             = alctr;
        arena_prev->id                = 0; 
        arena_prev->next              = NULL;
        arena_prev->position          = 0;
        arena_prev->save              = 0;
        arena_prev->timestamp         = 0;

        for (
            u32 index = 1;
                index < count_arenas;
              ++index) {

            u32  offset     = index * (size_pow_2_arena + STRUCT_SIZE_ARENA_NODE);
            auto arena_next = (arena*)(arena_mem + offset);

            arena_next->alctr     = alctr;
            arena_next->id        = index;
            arena_next->next      = NULL;
            arena_next->position  = 0;
            arena_next->save      = 0;
            arena_next->timestamp = 0;

            arena_prev->next = arena_next;
            arena_next->prev = arena_prev;
            arena_prev       = arena_next;
        }

        return(alctr);
    }

    SLD_MEMORY_API_IMPL u32
    arena_alctr_size_total(
        const arena_alctr* alctr) {

    }

    SLD_MEMORY_API_IMPL u32
    arena_alctr_size_free(
        const arena_alctr* alctr) {

    }

    SLD_MEMORY_API_IMPL u32
    arena_alctr_size_used(
        const arena_alctr* alctr) {

    }

    SLD_MEMORY_API_IMPL void
    arena_alctr_assert_valid(
        const arena_alctr* alctr) {

        const bool is_valid = (
            alctr                         != NULL &&
            alctr->arena_info.count_total != 0    &&
            alctr->arena_info.size        != 0    &&
            (
                alctr->arena_list.free != NULL ||
                alctr->arena_info.used != NULL                
            ) &&
            alctr->arena_info.count_total == (alctr->arena_info.count_free + alctr->arena_info.count_used)
        );
    
        assert(is_valid);
    }

    SLD_MEMORY_API_IMPL bool
    arena_assert_valid(
        const arena* a) {

        const auto alctr = a ? a->alctr : NULL;
        arena_alctr_assert_valid(alctr);

        const bool is_valid = (

            a            != NULL                                     &&
            a->alctr     != NULL                                     &&
            a->timestamp != 0                                        &&
            a->id        <  alctr->arena_info.count_total             &&
            a->position  <= alctr->arena_info.size                    &&
            a->save      <= a->position                              &&
            (void*)a     >= ((void*)alctr + STRUCT_SIZE_ARENA_ALCTR) &&
            (void*)a     <= ((void*)alctr + (STRUCT_SIZE_ARENA_ALCTR * alctr->arena_info.count_total))
        );

        assert(is_valid);
    }

    SLD_MEMORY_API_IMPL u32
    arena_size_total(
        const arena* a) {

        arena_assert_valid(a);

        const auto alctr = a->alctr;
        return(alctr->arena_info.size);
    }

    SLD_MEMORY_API_IMPL u32
    arena_size_free(
        const arena* a) {

        arena_assert_valid(a);

        const auto alctr      = a->alctr;
        const u32  size_used  = a->position;
        const u32  size_total = alctr->arena_info.size;
        const u32  size_free  = (size_total - size_used); 
    
        return(size_free);
    }

    SLD_MEMORY_API_IMPL u32
    arena_size_used(
        const arena* a) {

        arena_assert_valid(a);

        return(a->position);    
    }

    SLD_MEMORY_API_IMPL u32
    arena_id(
        const arena* a) {

        arena_assert_valid(a);

        return(a->id);
    }

    SLD_MEMORY_API_IMPL u64
    arena_timestamp(
        const arena* a) {

        arena_assert_valid(a);

        return(a->timestamp);
    }

    SLD_MEMORY_API_IMPL arena*
    arena_alloc(
        arena_alctr* alctr) {

        arena_alctr_assert_valid(alctr);

        auto new_arena  = alctr->arena_list.free;
        auto next_free  = new_arena->next; 
        auto first_used = alctr->arena_list.used;

        assert(
            new_arena->prev  == NULL &&
            first_used->prev == NULL
        );

        if (next_free) {
            next_free->prev  = NULL;
        }
        if (first_used) {
            first_used->prev = new_arena;
        }

        new_arena->next = first_used;

        alctr->arena_list.used = new_arena;
        alctr->arena_list.free = next_free;

        return(new_arena);
    }

    SLD_MEMORY_API_IMPL void
    arena_free(
        arena* a) {

        arena_assert_valid(a);

        auto alctr = a->alctr;

        auto next_used = a->next;
        auto prev_used = a->prev;
        auto next_free = alctr->arena_list.free;

        if (next_used) {
            next_used->prev = prev_used;
        }

        if (prev_used) {
            prev_used->next = next_used;
        }

        if (a == alctr->arena_list.used) {
            assert(a->prev == NULL);
            alctr->arena_list.used = next_used;
        }

        if (next_free) {
            assert(next_free->prev == NULL);
        }

        a->next = next_free;
        a->prev = NULL;

        alctr->arena_list.free = a;
    }

    SLD_MEMORY_API_IMPL void*
    arena_push(
        arena*    a,
        const u32 size,
        const u32 alignment) {

        arena_assert_valid(a);
        assert(size != 0);

        // align the size
        const u32 size_aligned = size_is_pow_2(alignment)
            ? size_align_pow_2 (alignment)
            : size_align       (alignment);

        // make sure we have space available
        const auto alctr      = a->alctr;
        const auto size_total = alctr->arena_info.size; 
        assert(
            alctr      != NULL &&
            size_total != 0    &&
            size_total >= a->position
        );
        const u32 size_free = (size_total - a->position);
        if (size_free < size_aligned) {
            return(NULL);
        }

        // get the memory and update the arena
        void* mem = get_arena_memory(a, a->position);
        assert(mem != NULL);
        a->position += size_aligned;

        // return memory
        arena_assert_valid(a);
        return(mem);
    }
    
    SLD_MEMORY_API_IMPL u32
    arena_state_save(
        arena* a) {

        arena_assert_valid(a);
        assert(a->save == 0);

        a->save = a->position;

        arena_assert_valid(a);
        return(a->save);
    }
    
    SLD_MEMORY_API_IMPL void
    arena_state_revert(
        arena* a, const u32 state) {

        arena_assert_valid(a);

        assert(a->save == state);

        a->position = state;
        a->save     = 0;

        arena_assert_valid(a);
    }
    
    SLD_MEMORY_API_IMPL void
    arena_reset(
        arena* a) {

        arena_assert_valid(a);
        
        a->position  = 0;
        a->save      = 0;
        a->timestamp = 0;
        
        arena_assert_valid(a);
    }
    
    //--------------------------------------------------------------------
    // INTERNAL METHODS
    //--------------------------------------------------------------------

    SLD_MEMORY_INTERNAL u32
    calculate_size_pow_2_arena(
        const u32 granularity_min) {

        assert(granularity_min != 0);

        const u32 arena_size = size_is_pow_2(granularity_min)
            ? granularity_min
            : size_round_up_pow2(granularity_min);

        return(arena_size);        
    }


    SLD_MEMORY_INTERNAL u32
    calculate_size_pow_2_memory_arena_aligned(
        const u32 size_min,
        const u32 size_pow_2_arena) {

        assert(size_is_pow_2(size_pow_2_arena));

        const u32 size_pow_2 = size_is_pow_2(size_min)
            ? size_min
            : size_round_up_pow2(size_min);

        const u32 size_arena_aligned = size_align_pow_2(size_pow_2, size_pow_2_arena);

        return(size_arena_aligned);
    }

    SLD_MEMORY_INTERNAL u32
    calculate_count_arenas(
        const u32 size_pow_2_memory_arena_aligned,
        const u32 size_pow_2_arena) {

        assert(
            size_pow_2_memory_arena_aligned != 0           &&
            size_pow_2_arena                != 0           &&
            size_is_pow_2(size_pow_2_memory_arena_aligned) &&
            size_is_pow_2(size_pow_2_arena)                &&
            (size_pow_2_memory_arena_aligned % size_pow_2_arena) == 0
        );

        const u32 count = (size_pow_2_memory_arena_aligned / size_pow_2_arena); 
        return(count);
    }

    SLD_MEMORY_INTERNAL u32
    calculate_total_size(
        const u32 size_pow_2_memory_arena_aligned,
        const u32 size_pow_2_arena,        
        const u32 count_arenas) {

        assert(
            size_pow_2_memory_arena_aligned != 0           &&
            size_pow_2_arena                != 0           &&
            size_is_pow_2(size_pow_2_memory_arena_aligned) &&
            size_is_pow_2(size_pow_2_arena)                &&
            size_pow_2_memory_arena_aligned == (size_pow_2_arena * count_arenas)            
        );
    
        const u32 size_arena_memory = (STRUCT_SIZE_ARENA_NODE  + size_pow_2_arena) * count_arenas;
        const u32 size_total        = (STRUCT_SIZE_ARENA_ALCTR + size_arena_memory); 

        return(size_total);
    }


    SLD_MEMORY_INTERNAL void*
    get_arena_memory(
        const arena* a,
        const u32    offset) {

    }
};