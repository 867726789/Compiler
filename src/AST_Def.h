#pragma once

#include <memory>
#include <string>
#include <iostream>

// 所有 AST 的基类
class BaseAST {
 public:
  virtual ~BaseAST() = default;
  virtual void Dump() const = 0;
  virtual std::string GenerateIR() = 0;  // 新增IR生成接口
};

// CompUnit 对应根节点
class CompUnitAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> func_def;

  void Dump() const override {
    std::cout << "CompUnitAST { ";
    func_def->Dump();
    std::cout << " }";
  }

  // CompUnitAST
  std::string GenerateIR() override {
    return func_def->GenerateIR();
  }
};

// 函数定义节点
class FuncDefAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> func_type;  // 函数类型
  std::string ident;                   // 函数名
  std::unique_ptr<BaseAST> block;     // 函数体

  void Dump() const override {
    std::cout << "FuncDefAST { ";
    func_type->Dump();
    std::cout << ", " << ident << ", ";
    block->Dump();
    std::cout << " }";
  }

  // FuncDefAST 
  std::string GenerateIR() override {
    std::string ir;
    ir += "fun @" + ident + "(): " + func_type->GenerateIR() + "{\n";
    ir += "%entry:\n";
    ir += block->GenerateIR();
    ir += "}\n";
    return ir;
  }
};

// 函数类型节点
class FuncTypeAST : public BaseAST {
 public:
  std::string type;  // 目前只有 "int"

  void Dump() const override {
    std::cout << "FuncTypeAST { " << type << " }";
  }

  // FuncTypeAST
  std::string GenerateIR() override {
    if ("int" == type)
      return "i32";
    return "void";  // 函数类型信息已包含在FuncDef中
  }
};

// 语句块节点
class BlockAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> stmt;  // 块内的语句
  void Dump() const override {
    std::cout << "BlockAST { ";
    stmt->Dump();
    std::cout << " }";
  }

  // BlockAST
  std::string GenerateIR() override {
    return stmt->GenerateIR();
  }
};

// 语句节点
class StmtAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> number;  // return 后的数值
  void Dump() const override {
    std::cout << "StmtAST { ";
    number->Dump();
    std::cout << " }"; 
  }

  // StmtAST
  std::string GenerateIR() override {
    return "  " + number->GenerateIR() + "\n";
  }
};

// 数值节点
class NumberAST : public BaseAST {
 public:
  int val;  // 整数值
  void Dump() const override {
    std::cout << "NumberAST { " << val << " }";
  }

  // NumberAST
  std::string GenerateIR() override {
    return "ret " + std::to_string(val);
  }
};