#include <cassert>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include "include/ast.hpp"


using namespace std;

// 声明 lexer 的输入, 以及 parser 函数
// 注意, 这两个函数是由 flex/bison 生成的, 不要手动修改
extern FILE *yyin;
extern int yyparse(unique_ptr<BaseAST> &ast);

string mode = "-debug";

ofstream koopa_ofs;
ofstream riscv_ofs;

int main(int argc, const char *argv[]) {
  // 解析命令行参数. 测试脚本/评测平台要求你的编译器能接收如下参数:
  // compiler 模式 输入文件 -o 输出文件
  assert(argc == 5);
  mode = argv[1];
  auto input = argv[2];
  auto output = argv[4];

  // 打开输入文件, 并且指定 lexer 在解析的时候读取这个文件
  yyin = fopen(input, "r");
  assert(yyin);

  // 调用 parser 函数, parser 函数会进一步调用 lexer 解析输入文件的
  unique_ptr<BaseAST> ast;
  auto ret = yyparse(ast);
  assert(!ret);

  if (mode == string("-koopa")) { // 输出koopa IR
    // 打开输出文件, 并且指定 AST 在输出的时候将内容打印到这个文件中
    koopa_ofs.open(output);
    ast->print();
    koopa_ofs.close();
  } 
  // else if (mode == string("-riscv")) {
  //   koopa_ofs.open("ir.koopa");
	// 	ast->print();
	// 	koopa_ofs.close();

  //   char koopa_ir[1 << 20];
	// 	ifstream koopa_ifs("ir.koopa");
	// 	koopa_ifs.read(koopa_ir, sizeof(koopa_ir));
	// 	riscv_ofs.open(output);
	// 	parse_riscv(koopa_ir);
	// 	riscv_ofs.close();
  // }

  return 0;
}

