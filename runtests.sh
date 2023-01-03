#!/usr/bin/env bash

CXX="g++"
CXXFLAGS="-Wall -Werror -Wpedantic -std=c++17 -Weffc++"

[[ $1 == "clang" ]] && CXX="clang++"

echo "test 1: tests/must_compile.cpp"
$CXX $CXXFLAGS -o tool tests/must_compile.cpp  > /dev/null 2>&1;
if [ $? -eq 0 ]
then
    echo "[OK] test passed!"
else
    echo "[ERROR] test failed!"
fi

echo "test 2: tests/must_fail_to_compile.cpp"
$CXX $CXXFLAGS tests/must_fail_to_compile.cpp -o a.out > /dev/null 2>&1;
if [ $? -ne 0 ]
then
    echo "[OK] test passed!"
else
    echo "[ERROR] test failed!"
fi
