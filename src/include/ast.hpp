#pragma once

#include <memory>
#include <string>
#include <iostream>
#include "include/utils.hpp"

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
 * @brief 编译开始单元 AST 类
 */
class CompUnitAST : public BaseAST {
 public:
  unique_ptr<BaseAST> func_def;

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
  // 函数体                   
  unique_ptr<BaseAST> block;    

  Result print() const override;
};


/**
 * @brief 基本块 AST 类
 */
class BlockAST : public BaseAST {
 public:
  unique_ptr<BaseAST> stmt;

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