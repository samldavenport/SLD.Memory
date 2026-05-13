#pragma once

#include "sld-memory-internal.hpp"

namespace sld {

    SLD_MEMORY_API_IMPL u32
    block_memory_size(
        const u32 block_count,
        const u32 block_size) {
        
        assert(
            block_count != 0 &&
            block_size  != 0
        );

        const u32 block_size_adjusted = size_is_pow_2(block_size)
            ? block_size
            : size_round_up_pow2(block_size); 


        const u32 size_memory = block_count * block_size_adjusted;
        const u32 size_nodes  = block_count * STRUCT_SIZE_BLOCK_NODE;
        const u32 size_total  = STRUCT_SIZE_BLOCK_ALCTR + size_nodes + size_memory;

        return(size_total);
    }

    SLD_MEMORY_API_IMPL block_allocator*
    block_memory_init(
        const u32 block_count,
        const u32 block_size,
        const u32 memory_size,
        void*     memory) {

        // check args
        assert(
            block_count != 0          &&
            block_size  != 0          &&
            memory_size != 0          &&
            block_size  < memory_size &&
            memory      != NULL             
        );

        // assert size is what we expect
        const u32 memory_size_needed = block_memory_size(block_count, block_size);
        assert(memory_size == memory_size_needed);

        // clear memory and initialize the allocator
        memset(memory, 0, memory_size);
        auto alctr = (block_allocator*)memory;
        alctr->block_info.size        = block_size;
        alctr->block_info.count_total = block_count;
        alctr->block_info.count_free  = block_count;
        alctr->block_info.count_used  = 0;
        alctr->node_list.free         = NULL;
        alctr->node_list.used         = NULL;

        // initialize the blocks and return
        const void* block_mem_start = memory     + STRUCT_SIZE_BLOCK_ALCTR; 
        const u32   block_stride    = block_size + STRUCT_SIZE_BLOCK_NODE;
        for (
            u32 block_index = 0;
                block_index < block_count;
              ++block_index) {


            // calculate pointers
            const u32 block_offset = (block_stride    * block_index);
            void*     block_start  = (block_mem_start + block_offset);
            auto      block_prev   = alctr->node_list.free;

            // initialize block
            auto block = (block_memory_node*)block_start;   
            block->alctr     = alctr;
            block->next      = NULL;
            block->prev      = block_prev;
            block->memory    = block_start + STRUCT_SIZE_BLOCK_NODE;
            block->timestamp = 0;
            block->id        = block_index;  

            // add block to the free list
            if (block_prev) {
                block_prev->next = block;
            }
            alctr->node_list.free = block; 
        }

        block_memory_assert_valid(alctr);
        return(alctr);
    }

    SLD_MEMORY_API_IMPL bool
    block_memory_assert_valid(
        const block_allocator* alctr) {

        const bool is_valid = (
            alctr != NULL &&
            alctr->block_info.size        != 0 &&
            alctr->block_info.count_total != 0 &&
            (
                alctr->node_list.free != NULL ||
                alctr->node_list.used != NULL ||
            ) &&
            alctr->block_info.count_total == (alctr->block_info.count_free  + alctr->block_info.count_used); 
        );
        assert(is_valid);
    }


    SLD_MEMORY_API_IMPL u32
    block_size(
        const block_allocator* alctr) {

        block_memory_assert_valid(alctr);
        return(alctr->block_info.size);
    }

    SLD_MEMORY_API_IMPL u32
    block_count_total(
        const block_allocator* alctr) {

        block_memory_assert_valid(alctr);
        return(alctr->block_info.count_total);
    }

    SLD_MEMORY_API_IMPL u32
    block_allocator_count_free(
        const block_allocator* alctr) {

        block_memory_assert_valid(alctr);
        return(alctr->block_info.count_free);
    }

    SLD_MEMORY_API_IMPL u32
    block_allocator_count_used(
        const block_allocator* alctr) {

        block_memory_assert_valid(alctr);
        return(alctr->block_info.count_used);
    }

    SLD_MEMORY_API_IMPL u32
    block_allocation_size(
        const block b) {

        const block_memory_node* node = block_node_from_memory(block);
        assert(node != NULL);

        const auto alctr = node->alctr;
        return(alctr->block_info.size);
    }

    SLD_MEMORY_API_IMPL u32
    block_allocation_id(
        const block b) {

        const block_memory_node* node = block_node_from_memory(block);
        assert(node != NULL);

        return(node->id);
    }

    SLD_MEMORY_API_IMPL u64
    block_allocation_timestamp(
        const block b) {

        const block_memory_node* node = block_node_from_memory(block);
        assert(node != NULL);

        return(node->timestamp);
    }

    SLD_MEMORY_API_IMPL block
    block_alloc(block_allocator* alctr) {
        
        block_memory_assert_valid(alctr);

        block_memory_node* b = alctr->node_list.free;
        if (!b) {
            return(NULL);
        }

        block_memory_node* next_free = b->next;
        if (next_free) {
            next_free->prev = NULL;
        }

        block_memory_node* next_used = alctr->node_list.used;
        if (next_used) {
            next_used->prev = b;
        }

        alctr->node_list.free = next_free;
        alctr->node_list.used = b;

        b->next = next_used;
        b->prev = NULL; 

        return(b->memory);
    }
    
    SLD_MEMORY_API_IMPL void
    block_free(block b) {

        block_memory_node* node = block_node_from_memory(b);
        assert(node != NULL);

        block_allocator* alctr = node->alctr;

        block_memory_node* next_used = node->next;
        block_memory_node* prev_used = node->prev;

        if (prev_used) {
            prev_used->next = next_used;
        }
        else {
            assert(node == alctr->node_list.used);
            alctr->node_list.used = next_used;
        }

        if (next_used) {
            next_used->prev = prev;
        }

        block_memory_node* next_free = alctr->node_list.free;
        if (next_free) {
            next_free->prev = node;
        }
        alctr->node_list.free = node;
        
        node->prev = NULL;
        node->next = next_free;
    }

    SLD_MEMORY_INTERNAL void
    block_node_assert_valid(
        const block_memory_node* node) {

        const block_allocator*   alctr     = node->alctr;
        const void*              mem_start = ((void*)(alctr) + STRUCT_SIZE_BLOCK_ALCTR);
        const void*              mem_end   = mem_start + (STRUCT_SIZE_BLOCK_NODE * alctr->block_info.count_total); 
        const void               memory    = (void*)(node + STRUCT_SIZE_BLOCK_NODE);
        const block_memory_node* node_next = node->next; 
        const block_memory_node* node_prev = node->prev; 

        const bool is_valid = (
            node->memory == memory       &&
            node->timestamp != 0         &&
            memory          >= mem_start &&
            memory          <= mem_end   &&
            node->alctr != NULL          &&
            (node_next != NULL ? node_next->alctr == alctr : true) &&
            (node_prev != NULL ? node_prev->alctr == alctr : true) &&
            node->id < (alctr->block_info.count_total - 1)
        );
        assert(is_valid);
    }


    SLD_MEMORY_INTERNAL block_memory_node*
    block_node_from_memory(
        const block mem) {

        assert(mem != NULL);
        block_memory_node* node = (block_memory_node*)(mem - STRUCT_SIZE_BLOCK_NODE);
        assert(node->memory == mem); 

        block_node_assert_valid(node);

        return(node);
    }


};