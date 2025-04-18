#include "include/asm.hpp"

// Riscv 辅助类，用于生成 riscv 汇编代码
Riscv riscv;

/**
 * @brief 翻译 Koopa IR 程序
 * @param[in] program 程序
 */
void visit(const koopa_raw_program_t& program) {
    // 翻译所有全局变量
	visit(program.values);
	// 翻译所有函数
	visit(program.funcs);
}

/**
 * @brief 翻译 Koopa IR 切片
 * @param[in] slice 切片
 */
void visit(const koopa_raw_slice_t& slice) {
    // 切片 slice 是存储一系列元素的数组
	// 遍历数组，对每个元素进行翻译
	for (size_t i = 0; i < slice.len; ++i) {
        auto ptr = slice.buffer[i];
        switch (slice.kind) {
        	case KOOPA_RSIK_FUNCTION: {
                // 访问函数
                visit(reinterpret_cast<koopa_raw_function_t>(ptr));
                break;
            }
            case KOOPA_RSIK_BASIC_BLOCK: {
                // 访问基本块
                visit(reinterpret_cast<koopa_raw_basic_block_t>(ptr));
                break;
            }
            case KOOPA_RSIK_VALUE: {
                // 访问指令
                visit(reinterpret_cast<koopa_raw_value_t>(ptr));
                break;
            }
            default: {
                cout << slice.kind << endl;
                assert(false);
            }
        }
    }
}

/**
 * @brief 翻译 Koopa IR 函数
 * @param[in] func 函数
 */
void visit(const koopa_raw_function_t& func) {

    if (func->bbs.len == 0) {
        // 空函数，不做任何处理
        return;
    }

    riscv_ofs << endl;
    
    riscv._text();
    riscv._globl(func->name + 1);
    riscv._label(func->name + 1);

    // 访问所有基本块
    visit(func->bbs);
}

/**
 * @brief 翻译 Koopa IR 基本块
 * @param[in] bb 基本块
 */
void visit(const koopa_raw_basic_block_t& bb) {
    
    // 输出基本块标号
    // riscv_ofs << bb->name + 1 << ":" << endl;
    // 访问所有指令
    visit(bb->insts);
}

/**
 * @brief 翻译 Koopa IR 指令
 * @param[in] value 指令
 */
void visit(const koopa_raw_value_t& value) {
    // 根据指令类型判断后续需要如何访问
    const auto &kind = value->kind;
    switch (kind.tag) {
        case KOOPA_RVT_RETURN:
            // 访问 return 指令
            visit(kind.data.ret);
            break;
        default:
            // 其他类型暂时遇不到
            cout << kind.tag << endl;
            assert(false);
    }
}


/**
 * @brief 处理return指令
 * @param[in] ret 指令
 */
void visit(const koopa_raw_return_t& ret) {
    if (ret.value != nullptr) {
    	switch (ret.value->kind.tag) {
            case KOOPA_RVT_INTEGER: 
                riscv._li("a0", ret.value->kind.data.integer.value);
			    break;
            default:
            // 其他类型暂时遇不到
            cout << ret.value->kind.tag << endl;
            assert(false);
        }
    }

    riscv._ret();
}