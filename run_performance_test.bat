@echo off
rem Sol2 C++23性能优化测试脚本
rem 自动设置UTF-8编码并运行性能测试

echo 正在设置UTF-8编码...
chcp 65001 > nul

echo.
echo ====================================
echo   Sol2 C++23性能优化测试
echo ====================================
echo.

if not exist test_unified_performance.exe (
    echo 正在编译测试程序...
    clang++ -std=c++23 -O3 -I include test_unified_performance.cpp -o test_unified_performance.exe
    if errorlevel 1 (
        echo 编译失败！
        pause
        exit /b 1
    )
    echo 编译成功！
    echo.
)

echo 运行性能测试...
echo.
test_unified_performance.exe

echo.
echo ====================================
echo   测试完成
echo ====================================
pause