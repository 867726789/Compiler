#pragma once

#include "koopa.h"
#include "include/other_utils.hpp"
#include "include/asm.hpp"

using namespace std;

void parse_riscv(const char* koopa_ir);

/**
 * @brief Riscv 类，用于生成 Riscv 汇编代码
 * @note - 包含 Riscv 汇编代码的生成方法
 * @note - 会自动处理偏置量，使之不超过 12 位限制
 */
class Riscv {
public:
    // 特殊语句

    void _data();
    void _text();
    void _globl(const string& name);
    void _word(const int& value);
    void _zero(const int& len);
    void _label(const string& name);

    // 单目运算

    void _seqz(const string& rd, const string& rs1);
    void _snez(const string& rd, const string& rs1);
    void _li(const string& rd, const int& imm);
    void _mv(const string& rd, const string& rs1);
    void _la(const string& rd, const string& rs1);

    // 双目运算

    void _or(const string& rd, const string& rs1, const string& rs2);
    void _and(const string& rd, const string& rs1, const string& rs2);
    void _xor(const string& rd, const string& rs1, const string& rs2);
    void _add(const string& rd, const string& rs1, const string& rs2);
    void _addi(const string& rd, const string& rs1, const int& imm);
    void _sub(const string& rd, const string& rs1, const string& rs2);
    void _mul(const string& rd, const string& rs1, const string& rs2);
    void _div(const string& rd, const string& rs1, const string& rs2);
    void _rem(const string& rd, const string& rs1, const string& rs2);
    void _sgt(const string& rd, const string& rs1, const string& rs2);
    void _slt(const string& rd, const string& rs1, const string& rs2);
    void _sll(const string& rd, const string& rs1, const string& rs2);

    // 访存

    void _lw(const string& rd, const string& base, const int& bias);
    void _sw(const string& rs, const string& base, const int& bias);

    // 分支

    void _jump(const string& label);
    void _bnez(const string& cond, const string& label);
    void _beqz(const string& cond, const string& label);
    

    // 调用与返回

    void _call(const string& ident);
    void _ret();
};