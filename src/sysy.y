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
%token INT VOID CONST RETURN
%token <str_val> IDENT
%token <str_val> EqOp RelOp AddOp NotOp MulOp AndOp OrOp
%token <int_val> INT_CONST


// 非终结符的类型定义
%type <ast_val> Program CompUnit 
%type <ast_val> FuncDef
%type <ast_val> Decl ConstDecl ConstDef ConstInitVal
%type <ast_val> Block BlockItem Stmt 
%type <ast_val> ConstExp LVal Exp PrimaryExp UnaryExp AddExp MulExp LOrExp LAndExp EqExp RelExp


%type <vec_val> ExtendCompUnit ExtendBlockItem ExtendConstDef

%type <int_val> Number

%%

Program
  : CompUnit ExtendCompUnit {
    auto program = make_unique<ProgramAST>();
    auto comp_unit = $1;
    vector<unique_ptr<BaseAST>> *comp_unit_vec = $2;
    program->comp_units.emplace_back(move(comp_unit));
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
    comp_unit_vec->emplace_back(move($2));
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
  : '{' BlockItem ExtendBlockItem '}' {
    // 带语句的块
    auto ast = new BlockAST();
    auto block_item = $2;
    vector<unique_ptr<BaseAST>> *block_item_vec = $3;
    ast->block_items.emplace_back(move(block_item));
    for (auto& block_item : *block_item_vec) {
      ast->block_items.emplace_back(move(block_item));
    }
    $$ = ast;
  }
  | '{' '}'{
    // 空块
    auto ast = new BlockAST();
    ast->block_items = vector<unique_ptr<BaseAST>>();
    $$ = ast;
  }
  ;

ExtendBlockItem
  : {
    vector<unique_ptr<BaseAST>> *block_item_vec = new vector<unique_ptr<BaseAST>>;
    $$ = block_item_vec;
  }
  | ExtendBlockItem BlockItem {
    // 从 ExtendBlockItem 规约到 BlockItem, 则把 BlockItem 的解析返回值移动到 ExtendBlockItem 的 vector 中
    vector<unique_ptr<BaseAST>> *block_item_vec = $1;
    block_item_vec->emplace_back(move($2));
    $$ = block_item_vec;
  }
  ;

BlockItem
  : Decl {
    $$ = $1;
  }
  | Stmt {
    // 语句
    $$ = $1;
  }
  ;

Decl
  : ConstDecl {
    // 常量声明
    $$ = $1;
  }
  ;

ConstDecl
  : CONST INT ConstDef ExtendConstDef ';' {
    // 常量声明，要处理一行有多个常量定义的情况，如 int a = 1, b = 2;
    auto ast = new ConstDeclAST();
    auto const_def = $3;
    vector<unique_ptr<BaseAST>> *vec = $4;
    ast->const_defs.push_back(unique_ptr<BaseAST>(const_def));
    for (auto& ptr : *vec) {
      ast->const_defs.push_back(move(ptr));
    }
    $$ = ast;
  }
  ;

ExtendConstDef
  : {
    vector<unique_ptr<BaseAST>> *vec = new vector<unique_ptr<BaseAST>>;
    $$ = vec;
  }
  | ExtendConstDef ',' ConstDef {
    vector<unique_ptr<BaseAST>> *vec = $1;
    vec->push_back(unique_ptr<BaseAST>($3));
    $$ = vec;
  }
  ;

ConstDef
  : IDENT '=' ConstInitVal {
    // 常量定义
    auto ast = new ConstDefAST();
    ast->ident = *unique_ptr<string>($1);
    ast->value = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  ;

ConstInitVal
  : ConstExp {
    auto ast = new ConstInitValAST();
    ast->const_exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  ;

ConstExp
  : Exp {
    auto ast = new ConstExpAST();
    ast->exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  ;



Stmt
  : 
  Block {
    // 块语句，如 { int a = 1; }
    $$ = $1;
  }
  | RETURN Exp ';' {
    // 返回值, return exp;
    auto ast = new StmtReturnAST();
    ast->exp = unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  | RETURN ';' {
    // 空返回, return;
    auto ast = new StmtReturnAST();
    $$ = ast;
  }
  ;

LVal
  : IDENT  {
    // 左值
    auto ast = new LValAST();
    ast->ident = *unique_ptr<string>($1);
    $$ = ast;
  }
  ;

Exp
  : LOrExp {
    auto ast = new ExpAST();
    ast->l_or_exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  ;

LOrExp
  : LAndExp {
    auto ast = new LOrExpAST();
    ast->l_and_exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | LOrExp OrOp LAndExp {
    auto ast = new LExpWithOpAST();
    ast->logical_op = LExpWithOpAST::LogicalOp::LOGICAL_OR;
    ast->left = unique_ptr<BaseAST>($1);
    ast->right = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  ;

LAndExp 
  : EqExp {
    auto ast = new LAndExpAST();
    ast->eq_exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | LAndExp AndOp EqExp {
    auto ast = new LExpWithOpAST();
    ast->logical_op = LExpWithOpAST::LogicalOp::LOGICAL_AND;
    ast->left = unique_ptr<BaseAST>($1);
    ast->right = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  ;

EqExp
  : RelExp {
    auto ast = new EqExpAST();
    ast->rel_exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | EqExp EqOp RelExp {
    auto ast = new EqExpWithOpAST();
    auto eq_op = *unique_ptr<string>($2);
    ast->eq_op = ast->convert(eq_op);
    ast->left = unique_ptr<BaseAST>($1);
    ast->right = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  ;

RelExp
  : AddExp {
    auto ast = new RelExpAST();
    ast->add_exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | RelExp RelOp AddExp {
    auto ast = new RelExpWithOpAST();
    auto rel_op = *unique_ptr<string>($2);
    ast->rel_op = ast->convert(rel_op);
    ast->left = unique_ptr<BaseAST>($1);
    ast->right = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  ;

AddExp
  : MulExp {
    // 乘法表达式
    auto ast = new AddExpAST();
    ast->mul_exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | AddExp AddOp MulExp {
    auto ast = new AddExpWithOpAST();
    auto add_op = *unique_ptr<string>($2);
    ast->add_op = ast->convert(add_op);
    ast->left = unique_ptr<BaseAST>($1);
    ast->right = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  ;

MulExp
  : UnaryExp {
    auto ast = new MulExpAST();
    ast->unary_exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | MulExp MulOp UnaryExp {
    auto ast = new MulExpWithOpAST();
    auto mul_op = *unique_ptr<string>($2);
    ast->mul_op = ast->convert(mul_op);
    ast->left = unique_ptr<BaseAST>($1);
    ast->right = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  ;

UnaryExp
  : PrimaryExp {
    // 括号运算符表达式，如 (a)
    auto ast = new UnaryExpAST();
    ast->primary_exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | AddOp UnaryExp {
    auto ast = new UnaryExpWithOpAST();
    auto add_op = *unique_ptr<string>($1);
    ast->unary_op = ast->convert(add_op);
    ast->unary_exp = unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  | NotOp UnaryExp {
    auto ast = new UnaryExpWithOpAST();
    auto not_op = *unique_ptr<string>($1);
    ast->unary_op = ast->convert(not_op);
    ast->unary_exp = unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  ;

PrimaryExp
  : '(' Exp ')' {
    auto ast = new PrimaryExpAST();
    ast->exp = unique_ptr<BaseAST>($2);
    $$ = ast;
  } 
  | Number {
    auto ast = new PrimaryExpWithNumberAST();
    ast->number = $1;
    $$ = ast;
  }
  | LVal {
    // 变量表达式，如 a
    auto ast = new PrimaryExpWithLValAST();
    ast->l_val = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  ;
  ;

Number
  : INT_CONST { 
    $$ = $1;
  }
  ;

%%

// 定义错误处理函数, 其中第二个参数是错误信息
// parser 如果发生错误 (例如输入的程序出现了语法错误), 就会调用这个函数
void yyerror(unique_ptr<BaseAST> &ast, const char *s) {
  cerr << "error: " << s << endl;
}
