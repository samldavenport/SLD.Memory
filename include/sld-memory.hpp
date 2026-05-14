#ifndef SLD_MEMORY_HPP
#define SLD_MEMORY_HPP

#include "sld.hpp"

#ifdef SLD_MEMORY_STATIC
#   define SLD_MEMORY_API
#elif defined(SLD_MEMORY_DLL_EXPORT)
#   define SLD_MEMORY_API __declspec(dllexport)
#else
#   define SLD_MEMORY_API __declspec(dllimport)
#endif

namespace sld {

    //--------------------------------------------------------------------
    // MEMORY STRUCTURES
    //--------------------------------------------------------------------

    typedef void* block;

    struct stack_alctr;
    struct block_alctr;
    struct arena_alctr;
    struct arena;

    //--------------------------------------------------------------------
    // STACK MEMORY
    //--------------------------------------------------------------------

    SLD_MEMORY_API u32          stack_memory_size      (const u32 size, const u32 granularity);
    SLD_MEMORY_API stack_alctr* stack_memory_init      (const u32 size, const u32 granularity, void* memory);
    SLD_MEMORY_API u32          stack_alctr_size_total (const stack_alctr* alctr);
    SLD_MEMORY_API u32          stack_alctr_size_free  (const stack_alctr* alctr);
    SLD_MEMORY_API u32          stack_alctr_size_used  (const stack_alctr* alctr);
    SLD_MEMORY_API u32          stack_alctr_count      (const stack_alctr* alctr);
    SLD_MEMORY_API u32          stack_alloc_size       (const void* memory);
    SLD_MEMORY_API u32          stack_alloc_id         (const void* memory);
    SLD_MEMORY_API u64          stack_alloc_timestamp  (const void* memory);
    SLD_MEMORY_API void*        stack_alloc            (stack_alctr* alctr, const u32 size = 0, const u32 alignment = 0);
    SLD_MEMORY_API void         stack_free             (void* memory);

    //--------------------------------------------------------------------
    // BLOCK MEMORY
    //--------------------------------------------------------------------

    SLD_MEMORY_API u32          block_alctr_size            (const u32 block_count, const u32 block_size);
    SLD_MEMORY_API block_alctr* block_alctr_create          (const u32 block_count, const u32 block_size);
    SLD_MEMORY_API block_alctr* block_alctr_init            (const u32 block_count, const u32 block_size, const u32 mem_size, void* mem_ptr);
    SLD_MEMORY_API u32          block_alctr_assert_valid    (const block_alctr* alctr);
    SLD_MEMORY_API u32          block_size                  (const block_alctr* alctr);
    SLD_MEMORY_API u32          block_count_total           (const block_alctr* alctr);
    SLD_MEMORY_API u32          block_count_free            (const block_alctr* alctr);
    SLD_MEMORY_API u32          block_count_used            (const block_alctr* alctr);
    SLD_MEMORY_API bool         block_is_valid              (const block b);
    SLD_MEMORY_API u32          block_size                  (const block b);
    SLD_MEMORY_API u32          block_id                    (const block b);
    SLD_MEMORY_API u64          block_timestamp             (const block b);
    SLD_MEMORY_API block        block_alloc                 (block_alctr* alctr);
    SLD_MEMORY_API void         block_free                  (void* block);

    //--------------------------------------------------------------------
    // ARENA MEMORY
    //--------------------------------------------------------------------

    SLD_MEMORY_API u32          arena_alctr_size         (const u32 size, const u32 granularity);
    SLD_MEMORY_API arena_alctr* arena_alctr_init         (const u32 size, const u32 granularity, void* memory);
    SLD_MEMORY_API u32          arena_alctr_size_total   (const arena_alctr* alctr);
    SLD_MEMORY_API u32          arena_alctr_size_free    (const arena_alctr* alctr);
    SLD_MEMORY_API u32          arena_alctr_size_used    (const arena_alctr* alctr);
    SLD_MEMORY_API bool         arena_alctr_assert_valid (const arena_alctr* alctr); 
    SLD_MEMORY_API void         arena_assert_valid       (const arena* a);
    SLD_MEMORY_API u32          arena_size_total         (const arena* a);
    SLD_MEMORY_API u32          arena_size_free          (const arena* a);
    SLD_MEMORY_API u32          arena_size_used          (const arena* a);
    SLD_MEMORY_API u32          arena_id                 (const arena* a);
    SLD_MEMORY_API u64          arena_timestamp          (const arena* a);
    SLD_MEMORY_API arena*       arena_alloc              (arena_alctr* alctr);
    SLD_MEMORY_API void         arena_free               (arena* a);
    SLD_MEMORY_API void*        arena_push               (arena* a, const u32 size, const u32 alignment = 0);
    SLD_MEMORY_API u32          arena_state_save         (arena* a);
    SLD_MEMORY_API void         arena_state_revert       (arena* a, const u32 state);
    SLD_MEMORY_API void         arena_reset              (arena* a);
};

#endif //SLD_MEMORY_HPP