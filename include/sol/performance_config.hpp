// sol2

// The MIT License (MIT)

// Copyright (c) 2013-2022 Rapptz, ThePhD and contributors

// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef SOL_PERFORMANCE_CONFIG_HPP
#define SOL_PERFORMANCE_CONFIG_HPP

#include <sol/version.hpp>
#include <cstddef>

// =============================================================================
// 简化的性能优化配置系统
// 用户只需要定义一个主控宏即可启用所有C++23优化
// =============================================================================

// 主控开关：启用所有可用的C++23性能优化
#ifndef SOL_ENABLE_CXX23_OPTIMIZATIONS
    #ifdef NDEBUG
        #define SOL_ENABLE_CXX23_OPTIMIZATIONS 1  // Release模式默认启用
    #else
        #define SOL_ENABLE_CXX23_OPTIMIZATIONS 0  // Debug模式默认禁用
    #endif
#endif

// =============================================================================
// 编译器和C++标准检测
// =============================================================================

// C++23特性可用性检测
#if SOL_ENABLE_CXX23_OPTIMIZATIONS

    // consteval检测
    #if (__cplusplus >= 202002L) && \
        ((SOL_IS_ON(SOL_COMPILER_GCC) && __GNUC__ >= 11) || \
         (SOL_IS_ON(SOL_COMPILER_CLANG) && __clang_major__ >= 11) || \
         (SOL_IS_ON(SOL_COMPILER_VCXX) && _MSC_VER >= 1930))
        #define SOL_HAS_CONSTEVAL 1
    #else
        #define SOL_HAS_CONSTEVAL 0
    #endif

    // C++20 Concepts检测
    #if (__cplusplus >= 202002L) && \
        ((SOL_IS_ON(SOL_COMPILER_GCC) && __GNUC__ >= 10) || \
         (SOL_IS_ON(SOL_COMPILER_CLANG) && __clang_major__ >= 10) || \
         (SOL_IS_ON(SOL_COMPILER_VCXX) && _MSC_VER >= 1920))
        #define SOL_HAS_CONCEPTS 1
        #define SOL_USE_CONCEPTS 1
        #define SOL_USE_CONCEPTS_I_ SOL_ON
    #else
        #define SOL_HAS_CONCEPTS 0
        #define SOL_USE_CONCEPTS 0
        #define SOL_USE_CONCEPTS_I_ SOL_OFF
    #endif

    // 分支预测提示检测
    #if (__cplusplus >= 202002L) && \
        (SOL_IS_ON(SOL_COMPILER_GCC) || SOL_IS_ON(SOL_COMPILER_CLANG) || SOL_IS_ON(SOL_COMPILER_VCXX))
        #define SOL_HAS_BRANCH_PREDICTION 1
        #define SOL_USE_BRANCH_PREDICTION 1
        #define SOL_USE_BRANCH_PREDICTION_I_ SOL_ON
    #else
        #define SOL_HAS_BRANCH_PREDICTION 0
        #define SOL_USE_BRANCH_PREDICTION 0
        #define SOL_USE_BRANCH_PREDICTION_I_ SOL_OFF
    #endif

    // 编译器内置函数检测
    #if SOL_IS_ON(SOL_COMPILER_GCC) || SOL_IS_ON(SOL_COMPILER_CLANG)
        #define SOL_HAS_BUILTIN_EXPECT 1
        #define SOL_HAS_BUILTIN_PREFETCH 1
        #define SOL_HAS_BUILTIN_UNREACHABLE 1
        #define SOL_HAS_HOT_COLD_ATTRIBUTES 1
    #else
        #define SOL_HAS_BUILTIN_EXPECT 0
        #define SOL_HAS_BUILTIN_PREFETCH 0
        #define SOL_HAS_BUILTIN_UNREACHABLE 0
        #define SOL_HAS_HOT_COLD_ATTRIBUTES 0
    #endif

#else
    // 优化完全禁用
    #define SOL_HAS_CONSTEVAL 0
    #define SOL_HAS_CONCEPTS 0
    #define SOL_HAS_BRANCH_PREDICTION 0
    #define SOL_HAS_BUILTIN_EXPECT 0
    #define SOL_HAS_BUILTIN_PREFETCH 0
    #define SOL_HAS_BUILTIN_UNREACHABLE 0
    #define SOL_HAS_HOT_COLD_ATTRIBUTES 0
    
    #define SOL_USE_CONCEPTS 0
    #define SOL_USE_CONCEPTS_I_ SOL_OFF
    #define SOL_USE_BRANCH_PREDICTION 0
    #define SOL_USE_BRANCH_PREDICTION_I_ SOL_OFF
#endif

// =============================================================================
// 统一的性能优化宏定义
// =============================================================================

// consteval宏
#if SOL_HAS_CONSTEVAL
    #define SOL_CONSTEVAL consteval
#else
    #define SOL_CONSTEVAL constexpr
#endif

// 分支预测宏
#if SOL_HAS_BRANCH_PREDICTION
    #define SOL_LIKELY [[likely]]
    #define SOL_UNLIKELY [[unlikely]]
    #define SOL_IF_LIKELY(cond) if (cond) [[likely]]
    #define SOL_IF_UNLIKELY(cond) if (cond) [[unlikely]]
#else
    #define SOL_LIKELY
    #define SOL_UNLIKELY
    #define SOL_IF_LIKELY(cond) if (cond)
    #define SOL_IF_UNLIKELY(cond) if (cond)
#endif

// 热路径优化宏
#if SOL_HAS_HOT_COLD_ATTRIBUTES
    #define SOL_HOT_PATH __attribute__((hot))
    #define SOL_COLD_PATH __attribute__((cold))
    #define SOL_FORCE_INLINE __attribute__((always_inline)) inline
#elif SOL_IS_ON(SOL_COMPILER_VCXX)
    #define SOL_HOT_PATH
    #define SOL_COLD_PATH
    #define SOL_FORCE_INLINE __forceinline
#else
    #define SOL_HOT_PATH
    #define SOL_COLD_PATH
    #define SOL_FORCE_INLINE inline
#endif

// 优化提示宏
#if SOL_HAS_BUILTIN_UNREACHABLE && defined(__cpp_lib_unreachable)
    #include <utility>
    #define SOL_ASSUME(expr) if (!(expr)) std::unreachable()
#elif SOL_HAS_BUILTIN_UNREACHABLE
    #define SOL_ASSUME(expr) if (!(expr)) __builtin_unreachable()
#elif SOL_IS_ON(SOL_COMPILER_VCXX)
    #define SOL_ASSUME(expr) __assume(expr)
#else
    #define SOL_ASSUME(expr) ((void)0)
#endif

// 内存预取宏
#if SOL_HAS_BUILTIN_PREFETCH
    #define SOL_PREFETCH_READ(ptr) __builtin_prefetch(ptr, 0, 3)
    #define SOL_PREFETCH_WRITE(ptr) __builtin_prefetch(ptr, 1, 3)
#else
    #define SOL_PREFETCH_READ(ptr) ((void)0)
    #define SOL_PREFETCH_WRITE(ptr) ((void)0)
#endif

// =============================================================================
// 硬件感知的内存优化
// =============================================================================

#if SOL_ENABLE_CXX23_OPTIMIZATIONS && __cpp_lib_hardware_interference_size >= 201603
    #include <new>
    namespace sol { 
        namespace detail {
            constexpr std::size_t hardware_destructive_interference_size = std::hardware_destructive_interference_size;
            constexpr std::size_t hardware_constructive_interference_size = std::hardware_constructive_interference_size;
        }
    }
#else
    namespace sol { 
        namespace detail {
            constexpr std::size_t hardware_destructive_interference_size = 64;
            constexpr std::size_t hardware_constructive_interference_size = 64;
        }
    }
#endif

// 编译时内存对齐优化函数
namespace sol {
    namespace detail {
        SOL_CONSTEVAL std::size_t optimal_alignment_for_size(std::size_t obj_size) noexcept {
            if (obj_size <= 8) return 8;
            if (obj_size <= 16) return 16;
            if (obj_size <= 32) return 32;
            return hardware_destructive_interference_size;
        }
        
        SOL_CONSTEVAL std::size_t padding_for_alignment(std::size_t obj_size, std::size_t alignment) noexcept {
            return (alignment - (obj_size % alignment)) % alignment;
        }
    }
}

#endif // SOL_PERFORMANCE_CONFIG_HPP