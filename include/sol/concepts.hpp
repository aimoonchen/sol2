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

#ifndef SOL_CONCEPTS_HPP
#define SOL_CONCEPTS_HPP

#include <sol/performance_config.hpp>

#if SOL_IS_ON(SOL_USE_CONCEPTS)
#include <concepts>
#include <type_traits>
#include <string>
#include <string_view>

// Forward declarations
extern "C" {
    typedef struct lua_State lua_State;
    typedef double lua_Number;
    typedef long long lua_Integer;
}

namespace sol {
    namespace stack {
        template<typename T>
        int push(lua_State* L, T&& t);
        
        template<typename T>
        T get(lua_State* L, int index);
        
        template<typename T>
        bool check(lua_State* L, int index);
    }
    
    namespace meta {
        // Core type concepts - optimized for fastest compile-time evaluation
        template<typename T>
        concept LuaBasicType = requires {
            requires std::is_same_v<T, bool> ||
                    std::is_same_v<T, lua_Integer> ||
                    std::is_same_v<T, lua_Number> ||
                    std::is_same_v<T, std::string> ||
                    std::is_same_v<T, const char*> ||
                    std::is_same_v<T, void*>;
        };

        template<typename T>
        concept LuaIntegral = std::integral<T> && !std::same_as<T, bool>;

        template<typename T>
        concept LuaFloatingPoint = std::floating_point<T>;

        template<typename T>
        concept LuaArithmetic = LuaIntegral<T> || LuaFloatingPoint<T>;

        template<typename T>
        concept LuaString = requires {
            requires std::same_as<std::decay_t<T>, std::string> ||
                    std::same_as<std::decay_t<T>, const char*> ||
                    std::same_as<std::decay_t<T>, char*> ||
                    std::convertible_to<T, std::string_view>;
        };

        // Function concepts
        template<typename F>
        concept LuaCallable = std::is_function_v<F> || 
                             std::is_member_function_pointer_v<F> ||
                             requires(F f) { f(); };

        template<typename T>
        concept LuaUserType = !LuaBasicType<T> && 
                             !std::is_function_v<T> && 
                             !std::is_member_function_pointer_v<T> &&
                             std::is_class_v<std::decay_t<T>>;

        // Stack operation concepts
        template<typename T>
        concept StackPushable = requires(lua_State* L, T&& t) {
            { stack::push(L, std::forward<T>(t)) } -> std::convertible_to<int>;
        };

        template<typename T>
        concept StackGettable = requires(lua_State* L, int index) {
            { stack::get<T>(L, index) } -> std::convertible_to<T>;
        };

        template<typename T>
        concept StackCheckable = requires(lua_State* L, int index) {
            { stack::check<T>(L, index) } -> std::convertible_to<bool>;
        };

        // Container concepts
        template<typename T>
        concept LuaContainer = requires(T t) {
            typename T::value_type;
            { t.begin() } -> std::input_iterator;
            { t.end() } -> std::input_iterator;
        };

        template<typename T>
        concept LuaSequence = LuaContainer<T> && requires(T t) {
            { t.size() } -> std::convertible_to<std::size_t>;
        };

        template<typename T>
        concept LuaMapping = requires(T t) {
            typename T::key_type;
            typename T::mapped_type;
            { t.begin() } -> std::input_iterator;
            { t.end() } -> std::input_iterator;
        };

        // Optional-like concepts
        template<typename T>
        concept LuaOptional = requires(T t) {
            { static_cast<bool>(t) } -> std::convertible_to<bool>;
            { *t };
        };

        // Reference wrapper concepts
        template<typename T>
        concept LuaReference = requires {
            typename T::type;
        } && std::is_reference_v<typename T::type>;

        // Lightweight user type (for stack optimization)
        template<typename T>
        concept LightweightUserType = std::is_trivially_copyable_v<T> && 
                                     sizeof(T) <= sizeof(void*);

        // Polymorphic type concept
        template<typename T>
        concept PolymorphicType = std::is_polymorphic_v<T>;

        // Thread-safe type concept  
        template<typename T>
        concept ThreadSafeType = std::is_trivially_copyable_v<T> ||
                                requires {
                                    typename T::mutex_type;
                                };

        // Compile-time checkable types
        template<typename T>
        concept CompileTimeCheckable = std::is_arithmetic_v<T> ||
                                      std::is_pointer_v<T> ||
                                      std::is_enum_v<T>;

        // Runtime type identification needed
        template<typename T>
        concept NeedsRuntimeTypeCheck = LuaUserType<T> && 
                                       !CompileTimeCheckable<T>;

        // Variant-like concepts
        template<typename T>
        concept VariantLike = requires(T t) {
            { std::visit([](auto&&){}, t) };
        };

        // Any-like concepts
        template<typename T>
        concept AnyLike = requires(T t) {
            { t.type() } -> std::convertible_to<std::type_info>;
            { std::any_cast<int>(t) };
        };
    }
}

#endif // SOL_USE_CONCEPTS

#endif // SOL_CONCEPTS_HPP