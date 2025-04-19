#pragma once

#include <memory>
#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>
using namespace std;



/**
 * @brief 结果类，表示前端生成的指令的结果
 * @note `type`: 结果类型，常量 IMM / 寄存器 REG
 * @note `value`: 结果值
 * @note  IMM: 常量值
 * @note  REG: 寄存器编号
 */
class Result {
public:
  enum class Type {
    IMM,
    REG
  };
  Type type;
  int value;
  // 友元函数, 打印结果
  friend ostream& operator<<(ostream& os, const Result& result) {
    os << (result.type == Type::REG ? "%" : "") << result.value;
    return os;
  }
  // 构造函数
  Result() : type(Type::IMM), value(0) {}
  Result(Type type, int value) : type(type), value(value) {}
};

/**
 * @brief 快速创建 Result 对象
 */
 #define IMM_(value) Result(Result::Type::IMM, value)
 #define REG_(value) Result(Result::Type::REG, value)

/**
* @brief 前端全局信息管理器，管理横跨 block 的全局信息
* @note - `short_circuit_count`：短路求值的计数，用于生成短路求值的标签
* @note - `temp_count`：临时变量分配计数，用于生成临时变量,SSA 静态单赋值使用
*/
class EnvironmentManager {
private:

  // 短路求值的计数，用于生成短路求值的标签
  int short_circuit_count = 0;
  // 临时变量分配计数，用于生成临时变量
  int temp_count = 0;

public:
  // 是否已经存在过分配某变量的指令，避免重复 alloc
  unordered_map<string, bool> is_symbol_allocated;

  // 短路相关
  string get_short_true_label();
  string get_short_false_label();
  string get_short_end_label();
  string get_short_result_reg();
  void add_short_circuit_count();

  // 获取临时变量
  int get_and_inc_temp_count();
  int get_cur_temp_count();
};

extern EnvironmentManager environment_manager;

/**
 * @brief 一些宏定义，用于快速创建 SSA 寄存器
 */
#define NEW_REG_ REG_(environment_manager.get_and_inc_temp_count())
#define CUR_REG_ REG_(environment_manager.get_cur_temp_count() - 1)