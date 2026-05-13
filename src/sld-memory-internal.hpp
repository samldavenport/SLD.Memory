#ifndef SLD_MEMORY_INTERNAL_HPP
#define SLD_MEMORY_INTERNAL_HPP

#include "sld.hpp"

namespace sld {

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

};

#endif //SLD_MEMORY_INTERNAL_HPP