#ifndef SLD_MEMORY_HPP
#define SLD_MEMORY_HPP

#include "sld.hpp"

#ifdef SLD_MEMORY_STATIC
#   define SLD_MEMORY_API
#elif defined(SLD_DLL_EXPORT)
#   define SLD_MEMORY_API __declspec(dllexport)
#else
#   define SLD_MEMORY_API __declspec(dllimport)
#endif

namespace sld {

    // allocators
    struct stack_allocator;
    struct block_allocator;
    struct arena_allocator;
    struct stack_memory_node;
    struct block_memory_node;
    struct arena;

    // stack memory
    SLD_MEMORY_API u32              stack_allocator_memory_size (const u32 size, const u32 granularity);
    SLD_MEMORY_API stack_allocator* stack_allocator_memory_init (const u32 size, const u32 granularity, void* memory);
    SLD_MEMORY_API void*            stack_alloc                 (stack_allocator* alctr, const u32 size = 0, const u32 alignment = 0);
    SLD_MEMORY_API void             stack_free                  (stack_allocator* alctr, void* memory);

    // block memory
    SLD_MEMORY_API u32              block_allocator_memory_size (const u32 block_count, const u32 block_size);
    SLD_MEMORY_API block_allocator* block_allocator_memory_init (const u32 block_count, const u32 block_size);
    SLD_MEMORY_API void*            block_alloc                 (block_allocator* alctr);
    SLD_MEMORY_API void*            block_free                  (block_allocator* alctr);

    SLD_MEMORY_API u32             are
};

#endif //SLD_MEMORY_HPP