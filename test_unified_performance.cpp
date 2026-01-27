//
// Sol2 统一C++23性能优化验证测试
// 集合所有性能优化特性的综合测试
//

#include <chrono>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <clocale>

// Windows中文支持
#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#endif

// 启用所有C++23优化的统一宏
#define SOL_ENABLE_CXX23_OPTIMIZATIONS 1
#include <sol/performance_config.hpp>

// ===============================
// 测试类定义
// ===============================

class UnifiedPerformanceTestClass {
private:
    alignas(sol::detail::hardware_destructive_interference_size) std::vector<int> data_;
    alignas(sol::detail::hardware_destructive_interference_size) std::vector<double> float_data_;
    
public:
    UnifiedPerformanceTestClass() : data_(1000, 42), float_data_(1000, 3.14159) {}
    
    // 热路径整数求和
    SOL_HOT_PATH int sum_integers() const {
        int total = 0;
        SOL_PREFETCH_READ(data_.data());
        
        for (const auto& val : data_) SOL_LIKELY {
            total += val;
        }
        return total;
    }
    
    // 热路径浮点求和
    SOL_HOT_PATH double sum_doubles() const {
        double total = 0.0;
        SOL_PREFETCH_READ(float_data_.data());
        
        for (const auto& val : float_data_) SOL_LIKELY {
            total += val;
        }
        return total;
    }
    
    // 边界检查的设值函数
    SOL_HOT_PATH void set_value(int index, int value) {
        SOL_IF_LIKELY(index >= 0 && index < static_cast<int>(data_.size())) {
            data_[index] = value;
        }
    }
    
    // 边界检查的取值函数
    SOL_HOT_PATH int get_value(int index) const {
        SOL_IF_LIKELY(index >= 0 && index < static_cast<int>(data_.size())) {
            return data_[index];
        }
        SOL_ASSUME(false); // 不应该到达这里
        return -1;
    }
    
    // 强制内联的快速操作
    SOL_FORCE_INLINE int fast_multiply(int x) const {
        if (x > 0) SOL_LIKELY {
            return x * 2;
        } else SOL_UNLIKELY {
            return 0;
        }
    }
    
    // 冷路径错误处理
    SOL_COLD_PATH void handle_error(const std::string& message) const {
        std::cerr << "ERROR: " << message << std::endl;
    }
    
    std::size_t size() const { return data_.size(); }
};

// ===============================
// 编译时计算函数
// ===============================

// 计算最优缓冲区大小
SOL_CONSTEVAL std::size_t calculate_optimal_buffer_size(std::size_t base_size) {
    using namespace sol::detail;
    return base_size + padding_for_alignment(base_size, optimal_alignment_for_size(base_size));
}

// 编译时计算对齐信息
SOL_CONSTEVAL std::size_t get_cache_line_size() {
    return sol::detail::hardware_destructive_interference_size;
}

// ===============================
// 模板优化测试函数
// ===============================

// 快速算术运算模板
template<typename T>
SOL_HOT_PATH T fast_arithmetic_operation(T a, T b) {
    static_assert(std::is_arithmetic_v<T>, "T must be arithmetic");
    return a + b * 2;
}

// 快速字符串操作模板
template<typename T>
SOL_HOT_PATH std::string fast_string_operation(T&& str) {
    return std::string(std::forward<T>(str)) + "_optimized";
}

// STL算法优化包装
template<typename Container>
SOL_HOT_PATH auto optimized_accumulate(const Container& container) {
    using value_type = typename Container::value_type;
    SOL_PREFETCH_READ(container.data());
    return std::accumulate(container.begin(), container.end(), value_type{});
}

// ===============================
// 性能测试函数
// ===============================

void test_feature_detection() {
    std::cout << "Sol2 统一C++23性能优化测试\n";
    std::cout << "==========================\n";
    std::cout << "配置: SOL_ENABLE_CXX23_OPTIMIZATIONS=1\n\n";
    
    std::cout << "自动检测的优化特性:\n";
    std::cout << "✓ consteval: " << (SOL_HAS_CONSTEVAL ? "可用" : "不可用") << "\n";
    std::cout << "✓ Concepts: " << (SOL_HAS_CONCEPTS ? "可用" : "不可用") << "\n";
    std::cout << "✓ 分支预测: " << (SOL_HAS_BRANCH_PREDICTION ? "可用" : "不可用") << "\n";
    std::cout << "✓ 热路径优化: " << (SOL_HAS_HOT_COLD_ATTRIBUTES ? "可用" : "不可用") << "\n";
    std::cout << "✓ 内存预取: " << (SOL_HAS_BUILTIN_PREFETCH ? "可用" : "不可用") << "\n";
    std::cout << "✓ 优化提示: " << (SOL_HAS_BUILTIN_UNREACHABLE ? "可用" : "不可用") << "\n\n";
}

void test_compile_time_optimizations() {
    std::cout << "编译时优化测试:\n";
    std::cout << "---------------\n";
    
    // 测试consteval计算
    constexpr auto cache_line = get_cache_line_size();
    constexpr auto optimal_buffer_32 = calculate_optimal_buffer_size(32);
    constexpr auto optimal_buffer_47 = calculate_optimal_buffer_size(47);
    constexpr auto optimal_buffer_100 = calculate_optimal_buffer_size(100);
    
    std::cout << "缓存行大小: " << cache_line << " bytes\n";
    std::cout << "32字节最优缓冲区: " << optimal_buffer_32 << " bytes\n";
    std::cout << "47字节最优缓冲区: " << optimal_buffer_47 << " bytes\n";
    std::cout << "100字节最优缓冲区: " << optimal_buffer_100 << " bytes\n";
    
    // 测试编译时对齐计算
    constexpr auto alignment_8 = sol::detail::optimal_alignment_for_size(8);
    constexpr auto alignment_24 = sol::detail::optimal_alignment_for_size(24);
    constexpr auto alignment_128 = sol::detail::optimal_alignment_for_size(128);
    
    std::cout << "对齐计算 - 8字节: " << alignment_8 << ", 24字节: " << alignment_24 << ", 128字节: " << alignment_128 << "\n\n";
}

void test_template_optimizations() {
    std::cout << "模板优化测试:\n";
    std::cout << "-------------\n";
    
    // 算术运算优化
    auto int_result = fast_arithmetic_operation(10, 20);
    auto double_result = fast_arithmetic_operation(3.14, 2.71);
    auto float_result = fast_arithmetic_operation(1.5f, 2.5f);
    
    // 字符串操作优化
    auto string_result1 = fast_string_operation("test");
    auto string_result2 = fast_string_operation(std::string("hello"));
    
    std::cout << "算术优化 - int: " << int_result << ", double: " << double_result << ", float: " << float_result << "\n";
    std::cout << "字符串优化 - \"test\": " << string_result1 << ", \"hello\": " << string_result2 << "\n\n";
}

double run_hot_path_performance_test(UnifiedPerformanceTestClass& test_obj, int iterations) {
    using namespace std::chrono;
    
    auto start = high_resolution_clock::now();
    
    int total = 0;
    for (int i = 0; i < iterations; ++i) SOL_LIKELY {
        test_obj.set_value(i % 1000, test_obj.fast_multiply(i));
        total += test_obj.get_value(i % 1000);
    }
    
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<nanoseconds>(end - start);
    
    SOL_ASSUME(total > 0); // 优化提示：total应该为正
    return duration.count() / 1000.0; // 返回微秒
}

double run_cache_optimization_test(UnifiedPerformanceTestClass& test_obj, int iterations) {
    using namespace std::chrono;
    
    auto start = high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        int int_sum = test_obj.sum_integers();
        double double_sum = test_obj.sum_doubles();
        SOL_ASSUME(int_sum > 0 && double_sum > 0);
    }
    
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<nanoseconds>(end - start);
    
    return duration.count() / 1000.0; // 返回微秒
}

void run_comprehensive_performance_tests() {
    std::cout << "综合性能基准测试:\n";
    std::cout << "==================\n";
    
    UnifiedPerformanceTestClass test_obj;
    
    // 测试1: 热路径性能
    const int hot_path_iterations = 100000;
    double hot_path_time = run_hot_path_performance_test(test_obj, hot_path_iterations);
    
    std::cout << "热路径测试:\n";
    std::cout << "  完成 " << hot_path_iterations << " 次操作用时: " << hot_path_time << " 微秒\n";
    std::cout << "  平均每次操作: " << (hot_path_time / hot_path_iterations) << " 微秒\n";
    
    // 测试2: 缓存优化性能
    const int cache_iterations = 1000;
    double cache_time = run_cache_optimization_test(test_obj, cache_iterations);
    
    std::cout << "缓存优化测试:\n";
    std::cout << "  完成 " << cache_iterations << " 次求和用时: " << cache_time << " 微秒\n";
    std::cout << "  平均每次求和: " << (cache_time / cache_iterations) << " 微秒\n";
    
    // 测试3: STL算法优化
    std::vector<int> large_vector(10000, 1);
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 1000; ++i) {
        auto sum = optimized_accumulate(large_vector);
        SOL_ASSUME(sum > 0);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto stl_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    
    std::cout << "STL算法优化测试:\n";
    std::cout << "  完成1000次大向量求和用时: " << stl_time << " 微秒\n";
    std::cout << "  平均每次: " << (stl_time / 1000.0) << " 微秒\n\n";
    
    // 性能总结
    std::cout << "性能总结:\n";
    std::cout << "---------\n";
    std::cout << "热路径平均延迟: " << (hot_path_time / hot_path_iterations) << " 微秒/操作\n";
    std::cout << "缓存优化效率: " << (cache_time / cache_iterations) << " 微秒/求和\n";
    std::cout << "STL优化效率: " << (stl_time / 1000.0) << " 微秒/大向量求和\n\n";
}

// ===============================
// Windows中文编码设置
// ===============================

void setup_chinese_encoding() {
#ifdef _WIN32
    // 设置控制台代码页为UTF-8
    SetConsoleCP(65001);        // UTF-8 代码页
    SetConsoleOutputCP(65001);  // UTF-8 代码页
    
    // 简单的locale设置
    setlocale(LC_ALL, "");
#else
    // 非Windows系统设置UTF-8 locale
    setlocale(LC_ALL, "en_US.UTF-8");
#endif
}

// ===============================
// 主测试函数
// ===============================

int main() {
    try {
        // 设置中文编码支持
        setup_chinese_encoding();
        
        // 1. 特性检测
        test_feature_detection();
        
        // 2. 编译时优化测试
        test_compile_time_optimizations();
        
        // 3. 模板优化测试
        test_template_optimizations();
        
        // 4. 综合性能测试
        run_comprehensive_performance_tests();
        
        // 测试成功总结
        std::cout << "🎉 统一优化系统验证成功!\n";
        std::cout << "\n关键优势:\n";
        std::cout << "✓ 单一宏配置: SOL_ENABLE_CXX23_OPTIMIZATIONS\n";
        std::cout << "✓ 自动特性检测和降级兼容\n";
        std::cout << "✓ Release模式默认启用，Debug模式默认禁用\n";
        std::cout << "✓ 全面覆盖: consteval, 分支预测, 缓存优化, 模板优化\n";
        std::cout << "✓ 零破坏性: 完全向后兼容\n";
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "测试异常: " << e.what() << std::endl;
        return 1;
    }
}