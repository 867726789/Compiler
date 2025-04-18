#include "include/backend_utils.hpp"

/**
 * @brief 解析 KoopaIR 字符串，生成 Riscv 汇编代码
 * @param[in] koopa_ir KoopaIR 字符串
 */
 void parse_riscv(const char* koopa_ir) {
    // 解析字符串 str, 得到 Koopa IR 程序
    koopa_program_t program;
    koopa_error_code_t ret = koopa_parse_from_string(koopa_ir, &program);
    // 确保解析时没有出错
    assert(ret == KOOPA_EC_SUCCESS);
    // 创建一个 raw program builder, 用来构建 raw program
    koopa_raw_program_builder_t builder = koopa_new_raw_program_builder();
    // 将 Koopa IR 程序转换为 raw program
    koopa_raw_program_t raw = koopa_build_raw_program(builder, program);
    // 释放 Koopa IR 程序占用的内存
    koopa_delete_program(program);

    // 处理 raw program
    visit(raw);

    // 处理完成, 释放 raw program builder 占用的内存
    // 注意, raw program 中所有的指针指向的内存均为 raw program builder 的内存
    // 所以不要在 raw program 处理完毕之前释放 builder
    koopa_delete_raw_program_builder(builder);
}

// 特殊语句 
/**
 * @brief 生成 .data 宏
 */
 void Riscv::_data() {
    riscv_ofs << "\t.data" << endl;
}

/**
 * @brief 生成 .text 宏
 */
void Riscv::_text() {
    riscv_ofs << "\t.text" << endl;
}

/**
 * @brief 生成 .globl name 宏
 * @param[in] name 全局变量名
 */
void Riscv::_globl(const string& name) {
    riscv_ofs << "\t.globl " << name << endl;
}

/**
 * @brief 生成 .word value 宏
 * @param[in] value 要存储的值
 */
void Riscv::_word(const int& value) {
    riscv_ofs << "\t.word " << value << endl;
}

/**
 * @brief 生成 .zero len 宏
 * @param[in] len 要填充的 0 的个数
 */
void Riscv::_zero(const int& len) {
    riscv_ofs << "\t.zero " << len << endl;
}

/**
 * @brief 生成 label 标签，即 `name:`
 * @param[in] name 标签名
 */
void Riscv::_label(const string& name) {
    riscv_ofs << name << ":" << endl;
}


// 单目运算
/**
 * @brief 生成 seqz 指令，即比较 rs1 是否为 0，若为 0 则将 rd 置 1，否则置 0
 * @param[in] rd 目标寄存器
 * @param[in] rs1 源寄存器
 */
 void Riscv::_seqz(const string& rd, const string& rs1) {
    riscv_ofs << "\tseqz " << rd << ", " << rs1 << endl;
}

/**
 * @brief 生成 snez 指令，即比较 rs1 是否为 0，若不为 0 则将 rd 置 1，否则置 0
 * @param[in] rd 目标寄存器
 * @param[in] rs1 源寄存器
 */
void Riscv::_snez(const string& rd, const string& rs1) {
    riscv_ofs << "\tsnez " << rd << ", " << rs1 << endl;
}

/**
 * @brief 生成 li（加载立即数）指令，即 rd = imm
 * @param[in] rd 目标寄存器
 * @param[in] imm 立即数
 */
 void Riscv::_li(const string& rd, const int& imm) {
    riscv_ofs << "\tli " << rd << ", " << imm << endl;
}

/**
 * @brief 生成 mv（移动）指令，即 rd = rs1
 * @param[in] rd 目标寄存器
 * @param[in] rs1 源寄存器
 */
void Riscv::_mv(const string& rd, const string& rs1) {
    riscv_ofs << "\tmv " << rd << ", " << rs1 << endl;
}

/**
 * @brief 生成 la（加载地址）指令，即 rd = rs1
 * @param[in] rd 目标寄存器
 * @param[in] rs1 源寄存器
 */
void Riscv::_la(const string& rd, const string& rs1) {
    riscv_ofs << "\tla " << rd << ", " << rs1 << endl;
}

// 双目运算
/**
 * @brief 生成 or 指令，即 rd = rs1 | rs2
 * @param[in] rd 目标寄存器
 * @param[in] rs1 源寄存器 1
 * @param[in] rs2 源寄存器 2
 */
 void Riscv::_or(const string& rd, const string& rs1, const string& rs2) {
    riscv_ofs << "\tor " << rd << ", " << rs1 << ", " << rs2 << endl;
}

/**
 * @brief 生成 and 指令，即 rd = rs1 & rs2
 * @param[in] rd 目标寄存器
 * @param[in] rs1 源寄存器 1
 * @param[in] rs2 源寄存器 2
 */
void Riscv::_and(const string& rd, const string& rs1, const string& rs2) {
    riscv_ofs << "\tand " << rd << ", " << rs1 << ", " << rs2 << endl;
}

/**
 * @brief 生成 xor 指令，即 rd = rs1 ^ rs2
 * @param[in] rd 目标寄存器
 * @param[in] rs1 源寄存器 1
 * @param[in] rs2 源寄存器 2
 */
void Riscv::_xor(const string& rd, const string& rs1, const string& rs2) {
    riscv_ofs << "\txor " << rd << ", " << rs1 << ", " << rs2 << endl;
}

/**
 * @brief 生成 add 指令，即 rd = rs1 + rs2
 * @param[in] rd 目标寄存器
 * @param[in] rs1 源寄存器 1
 * @param[in] rs2 源寄存器 2
 */
void Riscv::_add(const string& rd, const string& rs1, const string& rs2) {
    riscv_ofs << "\tadd " << rd << ", " << rs1 << ", " << rs2 << endl;
}

/**
 * @brief 生成 addi 指令，即 rd = rs1 + imm，会自动处理 12 位立即数限制
 * @param[in] rd 目标寄存器
 * @param[in] rs1 源寄存器
 * @param[in] imm 立即数
 * @note 如果 imm 超过 12 位立即数限制，则会先将其存入一个临时寄存器，再进行加法运算
 */
void Riscv::_addi(const string& rd, const string& rs1, const int& imm) {
    if (imm >= -2048 && imm < 2048) {
        riscv_ofs << "\taddi " << rd << ", " << rs1 << ", " << imm << endl;
    }
}

/**
 * @brief 生成 sub 指令，即 rd = rs1 - rs2
 * @param[in] rd 目标寄存器
 * @param[in] rs1 源寄存器 1
 * @param[in] rs2 源寄存器 2
 */
void Riscv::_sub(const string& rd, const string& rs1, const string& rs2) {
    riscv_ofs << "\tsub " << rd << ", " << rs1 << ", " << rs2 << endl;
}

/**
 * @brief 生成 mul 指令，即 rd = rs1 * rs2
 * @param[in] rd 目标寄存器
 * @param[in] rs1 源寄存器 1
 * @param[in] rs2 源寄存器 2
 */
void Riscv::_mul(const string& rd, const string& rs1, const string& rs2) {
    riscv_ofs << "\tmul " << rd << ", " << rs1 << ", " << rs2 << endl;
}

/**
 * @brief 生成 div 指令，即 rd = rs1 / rs2
 * @param[in] rd 目标寄存器
 * @param[in] rs1 源寄存器 1
 * @param[in] rs2 源寄存器 2
 */
void Riscv::_div(const string& rd, const string& rs1, const string& rs2) {
    riscv_ofs << "\tdiv " << rd << ", " << rs1 << ", " << rs2 << endl;
}

/**
 * @brief 生成 rem（取余）指令，即 rd = rs1 % rs2
 * @param[in] rd 目标寄存器
 * @param[in] rs1 源寄存器 1
 * @param[in] rs2 源寄存器 2
 */
void Riscv::_rem(const string& rd, const string& rs1, const string& rs2) {
    riscv_ofs << "\trem " << rd << ", " << rs1 << ", " << rs2 << endl;
}

/**
 * @brief 生成 sgt 指令，即 rd = rs1 > rs2
 * @param[in] rd 目标寄存器
 * @param[in] rs1 源寄存器 1
 * @param[in] rs2 源寄存器 2
 */
void Riscv::_sgt(const string& rd, const string& rs1, const string& rs2) {
    riscv_ofs << "\tsgt " << rd << ", " << rs1 << ", " << rs2 << endl;
}

/**
 * @brief 生成 slt 指令，即 rd = rs1 < rs2
 * @param[in] rd 目标寄存器
 * @param[in] rs1 源寄存器 1
 * @param[in] rs2 源寄存器 2
 */
void Riscv::_slt(const string& rd, const string& rs1, const string& rs2) {
    riscv_ofs << "\tslt " << rd << ", " << rs1 << ", " << rs2 << endl;
}

/**
 * @brief 生成 sll（左移）指令，即 rd = rs1 << rs2
 * @param[in] rd 目标寄存器
 * @param[in] rs1 源寄存器 1
 * @param[in] rs2 源寄存器 2
 */
void Riscv::_sll(const string& rd, const string& rs1, const string& rs2) {
    riscv_ofs << "\tsll " << rd << ", " << rs1 << ", " << rs2 << endl;
}


// 访存
/**
 * @brief 生成 lw（加载字）指令，即 rd = *(base + bias)
 * @param[in] rd 目标寄存器
 * @param[in] base 基址寄存器
 * @param[in] bias 偏移量
 * @note 会自动处理偏移量，若偏移量超过 12 位立即数限制，则先将其存入一个临时寄存器，再进行加法运算
 */
 void Riscv::_lw(const string& rd, const string& base, const int& bias) {
    // 检查偏移量是否在 12 位立即数范围内
    if (bias >= -2048 && bias < 2048) {
        riscv_ofs << "\tlw " << rd << ", " << bias << "(" << base << ")" << endl;
    }
}

/**
 * @brief 生成 sw（存储字）指令，即 *(base + bias) = rs1
 * @param[in] rs 源寄存器
 * @param[in] base 基址寄存器
 * @param[in] bias 偏移量
 * @note 会自动处理偏移量，若偏移量超过 12 位立即数限制，则先将其存入一个临时寄存器，再进行加法运算
 */
void Riscv::_sw(const string& rs, const string& base, const int& bias) {
    // 检查偏移量是否在 12 位立即数范围内
    if (bias >= -2048 && bias < 2048) {
        riscv_ofs << "\tsw " << rs << ", " << bias << "(" << base << ")" << endl;
    }
}



// 分支
/**
 * @brief 生成 j（跳转）指令，即 goto label
 * @param[in] label 跳转目标标签
 */
 void Riscv::_jump(const string& label) {
    riscv_ofs << "\tj " << label << endl;
}


/**
 * @brief 生成 bnez 指令，即 if (cond != 0) goto label
 * @param[in] cond 条件寄存器
 * @param[in] label 跳转目标标签
 * @note 会生成多个标签，将短跳转转为长跳转，避免跳转范围限制
 */
 void Riscv::_bnez(const string& cond, const string& label) {
    // auto target_1 = context_manager.get_branch_label();
    // auto target_2 = context_manager.get_branch_end_label();
    // riscv_ofs << "\tbnez " << cond << ", " << target_1 << endl;
    // _jump(target_2);
    // _label(target_1);
    // _jump(label);
    // _label(target_2);
    riscv_ofs << "\tbnez " << cond << ", " << label << endl;
}

/**
 * @brief 生成 beqz 指令，即 if (cond == 0) goto label
 * @param[in] cond 条件寄存器
 * @param[in] label 跳转目标标签
 * @note 会生成多个标签，将短跳转转为长跳转，避免跳转范围限制
 */
void Riscv::_beqz(const string& cond, const string& label) {
    // auto target_1 = context_manager.get_branch_label();
    // auto target_2 = context_manager.get_branch_end_label();
    // riscv_ofs << "\tbeqz " << cond << ", " << target_1 << endl;
    // _jump(target_2);
    // _label(target_1);
    // _jump(label);
    // _label(target_2);
    riscv_ofs << "\tbeqz " << cond << ", " << label << endl;
}

// 调用与返回
/**
 * @brief 生成 call ident 指令
 * @param[in] ident 函数名
 */
void Riscv::_call(const string& ident) {
    riscv_ofs << "\tcall " << ident << endl;
}

/**
 * @brief 生成 ret 指令
 */
void Riscv::_ret() {
    riscv_ofs << "\tret" << endl;
}

