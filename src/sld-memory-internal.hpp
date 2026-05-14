#ifndef SLD_MEMORY_INTERNAL_HPP
#define SLD_MEMORY_INTERNAL_HPP

#include "sld-memory.hpp"

#define SLD_MEMORY_API_IMPL
#define SLD_MEMORY_INTERNAL static

namespace sld {

    //--------------------------------------------------------------------
    // STACK MEMORY
    //--------------------------------------------------------------------

    struct stack_allocator {
        struct {
            stack_memory_node* head;
            stack_memory_node* tail;
        } node;
        u32 size_kb;
        u32 granularity;
    };

    struct stack_memory_node {
        stack_allocator*   alctr;
        stack_memory_node* next;
        stack_memory_node* prev;
        u64                timestamp;
        u64                size;
        void*              memory;
        u32                index;
    };

    //--------------------------------------------------------------------
    // BLOCK MEMORY
    //--------------------------------------------------------------------

    struct block_alctr {
        struct {
            block_memory* free;
            block_memory* used;
        } node_list;
        struct {
            u32 size;
            u32 count_free;
            u32 count_used;
            u32 count_total;
        } block_info;
    };

    struct block_memory {
        block_alctr*  alctr;
        block_memory* next;
        block_memory* prev;
        block         memory;
        u64           timestamp;
        u32           id;
    };

    static constexpr u32 STRUCT_SIZE_BLOCK_ALCTR = sizeof(block_alctr);
    static constexpr u32 STRUCT_SIZE_BLOCK_NODE  = sizeof(block_memory_node);  

    block_memory_node* block_node_from_memory  (const block b);
    void               block_node_assert_valid (const block_memory_node* node);

    //--------------------------------------------------------------------
    // ARENA MEMORY
    //--------------------------------------------------------------------

    struct arena_alctr {
        struct {
            arena* free;
            arena* used;
        } arena_list;

        struct {
            u32 size;
            u32 count_free;
            u32 count_used;
            u32 count_total;
        } arena_info;
    };

    struct arena {
        arena_alctr* alctr;
        arena*       next;
        arena*       prev;
        u64          timestamp;
        u32          id;
        u32          position;
        u32          save;
    };

    constexpr u32 STRUCT_SIZE_ARENA_ALCTR = sizeof(arena_alctr);
    constexpr u32 STRUCT_SIZE_ARENA_NODE  = sizeof(arena); 

    u32 calculate_size_pow_2_arena                (const u32 granularity_min); 
    u32 calculate_size_pow_2_memory_arena_aligned (const u32 size_min, const u32 size_pow_2_arena);
    u32 calculate_count_arenas                    (const u32 size_pow_2_memory_arena_aligned, const u32 size_pow_2_arena);
};

#endif //SLD_MEMORY_INTERNAL_HPP