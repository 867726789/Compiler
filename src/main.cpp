#include <cassert>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include "AST_Def.h"
#include "koopa.h"


using namespace std;

// 声明 lexer 的输入, 以及 parser 函数
// 注意, 这两个函数是由 flex/bison 生成的, 不要手动修改
extern FILE *yyin;
extern int yyparse(unique_ptr<BaseAST> &ast);

// 递归实现IR到汇编的转换
// 访问 raw program函数
string Visit(const koopa_raw_program_t &program);
// 访问 raw slice
string Visit(const koopa_raw_slice_t &slice);
// 访问函数
string Visit(const koopa_raw_function_t &func);
// 访问基本块
string Visit(const koopa_raw_basic_block_t &block);
// 访问指令
string Visit(const koopa_raw_value_t &value);
// 访问操作数
string Visit(const koopa_raw_slice_t &operands);


int main(int argc, const char *argv[]) {
  // 解析命令行参数. 测试脚本/评测平台要求你的编译器能接收如下参数:
  // compiler 模式 输入文件 -o 输出文件
  assert(argc == 5);
  auto mode = argv[1];
  auto input = argv[2];
  auto output = argv[4];

  // 打开输入文件, 并且指定 lexer 在解析的时候读取这个文件
  yyin = fopen(input, "r");
  assert(yyin);

  // 调用 parser 函数, parser 函数会进一步调用 lexer 解析输入文件的
  unique_ptr<BaseAST> ast;
  auto ret = yyparse(ast);
  assert(!ret);

  // 输出解析得到的 AST, 其实就是个字符串
  // 修改main函数中的输出部分
  ast->Dump();
  cout << endl;
  // 新增IR输出
  unique_ptr<string> ir = make_unique<string>(ast->GenerateIR());
  const char* str = ir->c_str();
  cout << str << endl;

  // 将 文本IR 转换为 内存 IR
  koopa_program_t program;
  koopa_error_code_t ret = koopa_parse_from_string(str, &program);
  assert(ret == KOOPA_EC_SUCCESS);  // 确保解析时没有出错
  // 创建一个 raw program builder, 用来构建 raw program
  koopa_raw_program_builder_t builder = koopa_new_raw_program_builder();
  // 将 Koopa IR 程序转换为 raw program
  koopa_raw_program_t raw = koopa_build_raw_program(builder, program);
  // 释放 Koopa IR 程序占用的内存
  koopa_delete_program(program);

  Visit(raw);

  // 处理完成, 释放 raw program builder 占用的内存
  // 注意, raw program 中所有的指针指向的内存均为 raw program builder 的内存
  // 所以不要在 raw program 处理完毕之前释放 builder
  koopa_delete_raw_program_builder(builder);
  return 0;
}


// 访问 raw program
string Visit(const koopa_raw_program_t &program) {
  // 执行一些其他的必要操作
  // ...
  // 访问所有全局变量
  Visit(program.values);
  // 访问所有函数
  Visit(program.funcs);
}

// 访问 raw slice
string Visit(const koopa_raw_slice_t &slice) {
  for (size_t i = 0; i < slice.len; ++i) {
    auto ptr = slice.buffer[i];
    // 根据 slice 的 kind 决定将 ptr 视作何种元素
    switch (slice.kind) {
      case KOOPA_RSIK_FUNCTION:
        // 访问函数
        Visit(reinterpret_cast<koopa_raw_function_t>(ptr));
        break;
      case KOOPA_RSIK_BASIC_BLOCK:
        // 访问基本块
        Visit(reinterpret_cast<koopa_raw_basic_block_t>(ptr));
        break;
      case KOOPA_RSIK_VALUE:
        // 访问指令
        Visit(reinterpret_cast<koopa_raw_value_t>(ptr));
        break;
      default:
        // 我们暂时不会遇到其他内容, 于是不对其做任何处理
        assert(false);
    }
  }
}

// 访问函数
string Visit(const koopa_raw_function_t &func) {
  // 执行一些其他的必要操作
  // ...
  // 访问所有基本块
  Visit(func->bbs);
}

// 访问基本块
string Visit(const koopa_raw_basic_block_t &bb) {
  // 执行一些其他的必要操作
  // ...
  // 访问所有指令
  Visit(bb->insts);
}

// 访问指令
string Visit(const koopa_raw_value_t &value) {
  // 根据指令类型判断后续需要如何访问
  const auto &kind = value->kind;
  switch (kind.tag) {
    case KOOPA_RVT_RETURN:
      // 访问 return 指令
      Visit(kind.data.ret);
      break;
    case KOOPA_RVT_INTEGER:
      // 访问 integer 指令
      Visit(kind.data.integer);
      break;
    default:
      // 其他类型暂时遇不到
      assert(false);
  }
}

string Visit(const koopa_raw_return_t &ret) {
  // 执行一些其他的必要操作
  //...
  // 访问返回值
  Visit(ret->value);  
}

string Visit(const koopa_raw_integer_t &integer) {
  // 执行一些其他的必要操作
  //...
  // 访问操作数
  Visit(integer->operands);
}