#pragma once

#include <memory>
#include <string>
#include <iostream>
#include <vector>
#include <optional>
#include <cassert>
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


/**
 * @brief 基本块 AST 类
 */
class BlockAST : public BaseAST {
 public:
  // 基本块中内容
  vector<unique_ptr<BaseAST>> block_items;

  Result print() const override;
};

/**
 * @brief 语句 AST 类
 */
class StmtReturnAST : public BaseAST {
public:
  // 返回值，可为空
  optional<unique_ptr<BaseAST>> exp;
  Result print() const override;
};

/**
 * @brief 表达式 AST 类
 */
class ExpAST : public BaseAST {
public:
    // 逻辑或表达式
    unique_ptr<BaseAST> l_or_exp;
    Result print() const override;
};

/**
* @brief 逻辑或表达式 AST 类
*/
class LOrExpAST : public BaseAST {
public:
  // 逻辑与表达式
  unique_ptr<BaseAST> l_and_exp;
  Result print() const override;
};

/**
  * @brief 逻辑与表达式 AST 类
  */
class LAndExpAST : public BaseAST {
public:
  // 等值表达式
  unique_ptr<BaseAST> eq_exp;
  Result print() const override;
};

/**
  * @brief 逻辑与表达式 AST 类
  */
class LExpWithOpAST : public BaseAST {
public:
  // 逻辑运算符
  enum class LogicalOp {
      LOGICAL_OR,
      LOGICAL_AND
  };
  LogicalOp logical_op;
  // 左操作数
  unique_ptr<BaseAST> left;
  // 右操作数
  unique_ptr<BaseAST> right;
  Result print() const override;
};

/**
  * @brief 等值表达式 AST 类
  */
class EqExpAST : public BaseAST {
public:
  // 关系表达式
  unique_ptr<BaseAST> rel_exp;
  Result print() const override;
};

/**
  * @brief 等值表达式 AST 类
  */
class EqExpWithOpAST : public BaseAST {
public:
  // 等值运算符
  enum class EqOp {
      EQ,
      NEQ
  };
  EqOp eq_op;
  // 左操作数
  unique_ptr<BaseAST> left;
  // 右操作数
  unique_ptr<BaseAST> right;
  // 将字符串形式的运算符转换为等值运算符
  EqOp convert(const string& op) const;
  Result print() const override;
};
  
/**
  * @brief 关系表达式 AST 类
  */
class RelExpAST : public BaseAST {
public:
  // 加法表达式
  unique_ptr<BaseAST> add_exp;
  Result print() const override;
};
  
/**
  * @brief 关系表达式 AST 类
*/
class RelExpWithOpAST : public BaseAST {
public:
  // 关系运算符
  enum class RelOp {
      LE,
      GE,
      LT,
      GT
  };
  RelOp rel_op;
  // 左操作数
  unique_ptr<BaseAST> left;
  // 右操作数
  unique_ptr<BaseAST> right;
  // 将字符串形式的运算符转换为关系运算符
  RelOp convert(const string& op) const;
  Result print() const override;
};


/**
 * @brief 加法表达式 AST 类
 */
class AddExpAST : public BaseAST {
public:
  // 乘法表达式
  unique_ptr<BaseAST> mul_exp;
  Result print() const override;
};

/**
 * @brief 加法表达式(带符号) AST 类
 */
class AddExpWithOpAST : public BaseAST {
public:
  // 加法运算符
  enum class AddOp {
      ADD,
      SUB
  };
  AddOp add_op;
  // 左操作数
  unique_ptr<BaseAST> left;
  // 右操作数
  unique_ptr<BaseAST> right;
  // 将字符串形式的运算符转换为加法运算符
  AddOp convert(const string& op) const;
  Result print() const override;
};

/**
 * @brief 乘法表达式 AST 类
 */
class MulExpAST : public BaseAST {
public:
  // 一元表达式
  unique_ptr<BaseAST> unary_exp;
  Result print() const override;
};

/**
 * @brief 乘法表达式(带符号) AST 类
 */
class MulExpWithOpAST : public BaseAST {
public:
  // 乘法运算符
  enum class MulOp {
    MUL,
    DIV,
    MOD
  };
  MulOp mul_op;
  // 左操作数
  unique_ptr<BaseAST> left;
  // 右操作数
  unique_ptr<BaseAST> right;
  // 将字符串形式的运算符转换为乘法运算符
  MulOp convert(const string& op) const;
  Result print() const override;
};

/**
 * @brief 一元表达式 AST 类
 */
class UnaryExpAST : public BaseAST {
public:
    // 优先表达式
    unique_ptr<BaseAST> primary_exp;
    Result print() const override;
};
  

/**
 * @brief 带运算符的一元表达式 AST 类
 */
class UnaryExpWithOpAST : public BaseAST {
public:
  // 一元操作符  
  enum class UnaryOp {
    POSITIVE,
    NEGATIVE,
    NOT
  };
  UnaryOp unary_op;
  // 一元表达式
  unique_ptr<BaseAST> unary_exp;
  // 将字符串形式的运算符转换为一元运算符
  UnaryOp convert(const string& op) const;
  Result print() const override;
};

/**
 * @brief 优先表达式 AST 类
 */
class PrimaryExpAST : public BaseAST {
public:
  // 表达式
  unique_ptr<BaseAST> exp;
  Result print() const override;
};

/**
 * @brief 数字字面量优先表达式 AST 类
 */
class PrimaryExpWithNumberAST : public BaseAST {
public:
  // 字面量
  int number;
  Result print() const override;
};