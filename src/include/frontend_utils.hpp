#pragma once

#include <memory>
#include <string>
#include <iostream>
#include <fstream>
using namespace std;



/**
 * @brief 结果类，表示前端生成的指令的结果
 * @note `value`: 结果值
 */
class Result {
public:
  int value;  // 结果值

  // 友元函数, 打印结果
  friend ostream& operator<<(ostream& os, const Result& result) {
    os << result.value;
    return os;
  }

  // 构造函数
  Result() : value(0) {}
  Result(int value) : value(value) {}
};

