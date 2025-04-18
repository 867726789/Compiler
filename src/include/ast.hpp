#pragma once

#include <memory>
#include <string>
#include <iostream>
#include <vector>
#include "include/frontend_utils.hpp"

using namespace std;


extern string mode;
extern ofstream koopa_ofs;
extern ofstream riscv_ofs;

/**
 * @brief 所有 AST 的基类
 */
class BaseAST {
 public:
  virtual ~BaseAST() = default;
  virtual Result print() const = 0;
};

/**
 * @brief 程序 AST 类
 */
class ProgramAST : public BaseAST {
 public:
  vector<unique_ptr<BaseAST>> comp_units;
  Result print() const override;
};


/**
 * @brief 函数定义 AST 类
 */
class FuncDefAST : public BaseAST {
public:
  // 函数类型
  enum class FuncType {
    INT,
    VOID
  };
  FuncType func_type;  
  // 函数名
  string ident;
  // 函数参数列表 (to do)
  // vector<unique_ptr<BaseAST>>* func_params;
  // 函数体                   
  unique_ptr<BaseAST> block;    

  Result print() const override;
};

// /**
// * @brief 函数参数 AST 类
// */
// class FuncParamAST : public BaseAST {
//  public:
//   // 函数参数名
//   string ident;
//   // 是否为数组
//   bool is_array;
//   // 数组
//   vector<unique_ptr<BaseAST>>* array_index;
//   Result print() const override;
// }


/**
 * @brief 基本块 AST 类
 */
class BlockAST : public BaseAST {
 public:
  // 基本块中内容
  vector<unique_ptr<BaseAST>> stmts;

  Result print() const override;
};

/**
 * @brief 语句 AST 类
 */
class StmtAST : public BaseAST {
 public:
  unique_ptr<BaseAST> number;

  Result print() const override;
};

/**
 * @brief 数字字面量表达式 AST 类
 */
class NumberAST : public BaseAST {
 public:
  // 数字字面量的值
  int number;

  Result print() const override;
};