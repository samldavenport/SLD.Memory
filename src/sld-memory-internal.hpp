#ifndef SLD_MEMORY_INTERNAL_HPP
#define SLD_MEMORY_INTERNAL_HPP

#include "sld.hpp"

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

    struct block_allocator {
        struct {
            block_memory_node* free;
            block_memory_node* used;
        } node_list;
        struct {
            u32 size;
            u32 count;
        } block_info;
    };

    struct block_memory_node {
        block_allocator*   alctr;
        block_memory_node* next;
        block_memory_node* prev;
        u64                timestamp;
        void*              memory;
        u32                id;
    };

    //--------------------------------------------------------------------
    // ARENA MEMORY
    //--------------------------------------------------------------------

    struct arena_allocator {
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
        arena_allocator* alctr;
        arena*           next;
        arena*           prev;
        u64              timestamp;
        u32              id;
        u32              position;
        u32              save;
    };
};

#endif //SLD_MEMORY_INTERNAL_HPP