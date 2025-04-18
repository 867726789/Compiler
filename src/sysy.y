/* 控制 Bison 的某些行为 */
%code requires {
  /* 编译时包含的头文件 */
  #include <memory>
  #include <string>
  #include "include/ast.hpp"
}

%{
/* 运行时包含的头文件 */
#include <iostream>
#include <memory>
#include <string>
#include "include/ast.hpp"

using namespace std;

// 声明 lexer 函数和错误处理函数
int yylex();
void yyerror(unique_ptr<BaseAST> &ast, const char *s);

%}

// 定义 parser 函数和错误处理函数的附加参数
// 我们需要返回一个字符串作为 AST, 所以我们把附加参数定义成字符串的智能指针
// 解析完成后, 我们要手动修改这个参数, 把它设置成解析得到的字符串
%parse-param { unique_ptr<BaseAST> &ast }

// yylval 的定义, 我们把它定义成了一个联合体 (union)
%union {
  string *str_val;
  int int_val;
  BaseAST *ast_val;
  vector<unique_ptr<BaseAST>> *vec_val;
}

// lexer 返回的所有 token 种类的声明
// 注意 IDENT 和 INT_CONST 会返回 token 的值, 分别对应 str_val 和 int_val
%token INT VOID RETURN
%token <str_val> IDENT
%token <int_val> INT_CONST

// 非终结符的类型定义
%type <ast_val> Program CompUnit 
%type <ast_val> FuncDef
%type <ast_val> Block Stmt Number


%type <vec_val> ExtendCompUnit ExtendStmt

%%

Program
  : CompUnit ExtendCompUnit {
    auto program = make_unique<ProgramAST>();
    auto comp_unit = $1;
    vector<unique_ptr<BaseAST>> *comp_unit_vec = $2;
    program->comp_units.emplace_back(comp_unit);
    for (auto& comp_unit : *comp_unit_vec) {
      program->comp_units.emplace_back(move(comp_unit));
    }
    ast = move(program);
  }
  ;

ExtendCompUnit
  : {
    vector<unique_ptr<BaseAST>>* comp_unit_vec = new vector<unique_ptr<BaseAST>>;
    // $$ 是 Bison 提供的宏, 它代表当前规则的返回值
    $$ = comp_unit_vec;
  }
  | ExtendCompUnit CompUnit {
    vector<unique_ptr<BaseAST>>* comp_unit_vec = $1;
    comp_unit_vec->emplace_back($2);
    $$ = comp_unit_vec;
  }
  ;

CompUnit
  : FuncDef {
    $$ = $1;
  }
  ;

FuncDef
  : INT IDENT '(' ')' Block {
    auto ast = new FuncDefAST();
    ast->func_type = FuncDefAST::FuncType::INT;
    ast->ident = *unique_ptr<string>($2);
    ast->block = unique_ptr<BaseAST>($5);
    $$ = ast;
  }
  | VOID IDENT '(' ')' Block {
    auto ast = new FuncDefAST();
    ast->func_type = FuncDefAST::FuncType::VOID;
    ast->ident = *unique_ptr<string>($2);
    ast->block = unique_ptr<BaseAST>($5);
    $$ = ast;
  }
  ;

Block
  : '{' Stmt ExtendStmt'}' {
    auto ast = new BlockAST();
    auto stmt = $2;
    vector<unique_ptr<BaseAST>>* stmts_vec = $3;
    ast->stmts.emplace_back(stmt);
    for (auto& stmt : *stmts_vec) {
      ast->stmts.emplace_back(move(stmt));
    }
    $$ = ast;
  }
  | '{' '}' {
    // 空代码块
    auto ast = new BlockAST();
    ast->stmts = vector<unique_ptr<BaseAST>>();
    $$ = ast;
  }
  ;

ExtendStmt
  : {
    vector<unique_ptr<BaseAST>> *stmts_vec = new vector<unique_ptr<BaseAST>>;
    $$ = stmts_vec;
  }
  | ExtendStmt Stmt {
    vector<unique_ptr<BaseAST>> *stmts_vec = $1;
    stmts_vec->emplace_back($2);
    $$ = stmts_vec;
  }

Stmt
  : RETURN Number ';' {
    auto ast = new StmtAST();
    ast->number = unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  ;

Number
  : INT_CONST { 
    auto ast = new NumberAST();
    ast-> number = $1;
    $$ = ast;
  }
  ;

%%

// 定义错误处理函数, 其中第二个参数是错误信息
// parser 如果发生错误 (例如输入的程序出现了语法错误), 就会调用这个函数
void yyerror(unique_ptr<BaseAST> &ast, const char *s) {
  cerr << "error: " << s << endl;
}
