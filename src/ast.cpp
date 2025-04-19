#include "include/ast.hpp"



// 全局环境管理器
EnvironmentManager environment_manager;


/**
 * @brief 打印程序根节点 ProgramAST
 * */
Result ProgramAST::print() const {
    for (auto &comp_unit : comp_units) {
        comp_unit->print();
    }
    return Result();
}


/**
 * @brief 打印函数定义 FuncDefAST
 * */
Result FuncDefAST::print() const {
    koopa_ofs << endl;

    koopa_ofs << "fun @" << ident;
    koopa_ofs << "(";
    // todo 参数列表
    koopa_ofs << ")";

    if (func_type == FuncType::INT) {
        koopa_ofs << ": i32"; 
    } else {
        koopa_ofs << ": void";
    }
    koopa_ofs << " {" << endl;
    koopa_ofs << "%" << ident << "_entry:" << endl;

    // 打印函数体
    block->print();  

    koopa_ofs << "}" << endl;

    // // 打印函数返回语句
    // f (func_type == FuncType::INT) {
    //     koopa_ofs << "\tret 0" << endl;
    // }
    // else {
    //     koopa_ofs << "\tret" << endl;
    // }
    // koopa_ofs << "}" << endl;
    
    return Result();
}

/**
 * @brief 打印函数体
 * */
Result BlockAST::print() const {
    for (auto &block_item : block_items) {
        block_item->print();
    }
    return Result();
}

/**
 * @brief 打印返回语句
 * */
Result StmtReturnAST::print() const {
    if (exp) {
        Result exp_result = (*exp)->print();
        koopa_ofs << "\tret " << exp_result << endl;
    }
    else {
        koopa_ofs << "\tret" << endl;
    }
    return Result();
}

/**
 * @brief 打印表达式
 * */
Result ExpAST::print() const {
    return l_or_exp->print();
}

/**
 * @brief 打印逻辑或表达式
 * */
 Result LOrExpAST::print() const {
    return l_and_exp->print();
}

/**
 * @brief 打印逻辑与表达式
 * */
Result LAndExpAST::print() const {
    return eq_exp->print();
}

/**
 * @brief 打印逻辑表达式
 * @return 计算结果所在寄存器或立即数
 */
Result LExpWithOpAST::print() const {
    // 先打印计算左表达式结果的语句，并获取左表达式结果
    Result lhs = left->print();
    // 根据逻辑运算符的类型，打印逻辑运算语句
    // 逻辑或运算符
    if (logical_op == LogicalOp::LOGICAL_OR) {
        // 左侧为立即数
        if (lhs.type == Result::Type::IMM) {
            // 如果左侧为立即数，且值不为 0，则直接返回 1，进行短路求值
            if (lhs.value != 0) {
                return IMM_(1);
            }
            // 如果左侧为立即数，且值为 0，则计算右表达式结果
            else {
                Result rhs = right->print();
                // 如果右表达式结果为立即数，则直接返回右表达式结果
                if (rhs.type == Result::Type::IMM) {
                    return IMM_(rhs.value != 0);
                }
                else {
                    // 生成一条 ne 0 指令，相当于 rhs != 0，得到布尔值
                    koopa_ofs << "\t" << NEW_REG_ << " = ne " << rhs << ", 0" << endl;
                    return CUR_REG_;
                }
            }
        }
        // 左侧不为立即数
        auto true_label = environment_manager.get_short_true_label();
        auto false_label = environment_manager.get_short_false_label();
        auto end_label = environment_manager.get_short_end_label();
        auto result = environment_manager.get_short_result_reg();
        environment_manager.add_short_circuit_count();

        // 生成 alloc 指令
        koopa_ofs << "\t" << result << " = alloc i32" << endl;
        environment_manager.is_symbol_allocated[result] = true;

        // 生成 br 指令
        koopa_ofs << "\tbr " << lhs << ", " << true_label << ", " << false_label << endl;

        // 生成 true 分支
        koopa_ofs << true_label << ":" << endl;
        koopa_ofs << "\t" << "store 1, " << result << endl;
        koopa_ofs << "\tjump " << end_label << endl;

        // 生成 false 分支
        koopa_ofs << false_label << ":" << endl;
        Result rhs = right->print();
        Result temp = NEW_REG_;
        // 生成一条 ne 0 指令，相当于 rhs != 0，得到布尔值
        koopa_ofs << "\t" << temp << " = ne " << rhs << ", 0" << endl;
        koopa_ofs << "\t" << "store " << temp << ", " << result << endl;
        koopa_ofs << "\tjump " << end_label << endl;

        // 生成 end 标签
        koopa_ofs << end_label << ":" << endl;
        Result result_reg = NEW_REG_;
        koopa_ofs << "\t" << result_reg << " = load " << result << endl;

        return result_reg;
    }
    // 逻辑与运算符
    else if (logical_op == LogicalOp::LOGICAL_AND) {
        // 左侧为立即数
        if (lhs.type == Result::Type::IMM) {
            if (lhs.value == 0) {
                return IMM_(0);
            }
            else {
                Result rhs = right->print();
                if (rhs.type == Result::Type::IMM) {
                    return IMM_(rhs.value != 0);
                }
                else {
                    // 生成一条 ne 0 指令，相当于 rhs != 0，得到布尔值
                    koopa_ofs << "\t" << NEW_REG_ << " = ne " << rhs << ", 0" << endl;
                    return CUR_REG_;
                }
            }
        }
        // 左侧不为立即数
        auto true_label = environment_manager.get_short_true_label();
        auto false_label = environment_manager.get_short_false_label();
        auto end_label = environment_manager.get_short_end_label();
        auto result = environment_manager.get_short_result_reg();
        environment_manager.add_short_circuit_count();

        // 生成 alloc 指令
        koopa_ofs << "\t" << result << " = alloc i32" << endl;
        environment_manager.is_symbol_allocated[result] = true;

        // 生成 br 指令
        koopa_ofs << "\tbr " << lhs << ", " << true_label << ", " << false_label << endl;

        // 生成 false 分支
        koopa_ofs << false_label << ":" << endl;
        koopa_ofs << "\t" << "store 0, " << result << endl;
        koopa_ofs << "\tjump " << end_label << endl;

        // 生成 true 分支
        koopa_ofs << true_label << ":" << endl;
        Result rhs = right->print();
        Result temp = NEW_REG_;
        // 生成一条 ne 0 指令，相当于 rhs != 0，得到布尔值
        koopa_ofs << "\t" << temp << " = ne " << rhs << ", 0" << endl;
        koopa_ofs << "\t" << "store " << temp << ", " << result << endl;
        koopa_ofs << "\tjump " << end_label << endl;

        // 生成 end 标签
        koopa_ofs << end_label << ":" << endl;
        Result result_reg = NEW_REG_;
        koopa_ofs << "\t" << result_reg << " = load " << result << endl;

        return result_reg;
    }
    else {
        assert(false);
    }
}

/**
 * @brief 转换等式运算符，输出枚举类型
 * @param[in] op 等式运算符
 * @return 等式运算符枚举类型
 */
EqExpWithOpAST::EqOp EqExpWithOpAST::convert(const string& op) const {
    if (op == "==") {
        return EqOp::EQ;
    }
    else if (op == "!=") {
        return EqOp::NEQ;
    }
    throw runtime_error("Invalid operator: " + op);
}

/**
 * @brief 打印等式表达式
 * */
Result EqExpAST::print() const {
    return rel_exp->print();
}

/**
 * @brief 打印带符号的等式表达式
 * @return 计算结果所在寄存器或立即数
 */
Result EqExpWithOpAST::print() const {
    // 先计算左右表达式结果
    Result lhs = left->print();
    Result rhs = right->print();
    // 若左右表达式结果均为常量，则直接返回常量结果
    if (lhs.type == Result::Type::IMM && rhs.type == Result::Type::IMM) {
        switch (eq_op) {
        case EqOp::EQ:
            return IMM_(lhs.value == rhs.value);
        case EqOp::NEQ:
            return IMM_(lhs.value != rhs.value);
        default:
            assert(false);
        }
    }
    // 若左右表达式结果不均为常量，则使用临时变量计算结果并存储之
    else {
        Result result = NEW_REG_;
        switch (eq_op) {
        case EqOp::EQ:
            koopa_ofs << "\t" << result << " = eq " << lhs << ", " << rhs << endl;
            break;
        case EqOp::NEQ:
            koopa_ofs << "\t" << result << " = ne " << lhs << ", " << rhs << endl;
            break;
        default:
            assert(false);
        }
        return result;
    }
}

/**
 * @brief 打印关系表达式
 * */
Result RelExpAST::print() const {
    return add_exp->print();
}

/**
 * @brief 转换关系运算符，输出枚举类型
 * @param[in] op 关系运算符
 * @return 关系运算符枚举类型
 */
RelExpWithOpAST::RelOp RelExpWithOpAST::convert(const string& op) const {
    if (op == "<=") {
        return RelOp::LE;
    }
    else if (op == ">=") {
        return RelOp::GE;
    }
    else if (op == "<") {
        return RelOp::LT;
    }
    else if (op == ">") {
        return RelOp::GT;
    }
    throw runtime_error("Invalid operator: " + op);
}

/**
 * @brief 打印带符号的关系表达式
 * @return 计算结果所在寄存器或立即数
 */
Result RelExpWithOpAST::print() const {
    // 先计算左右表达式结果
    Result lhs = left->print();
    Result rhs = right->print();
    // 若左右表达式结果均为常量，则直接返回常量结果
    if (lhs.type == Result::Type::IMM && rhs.type == Result::Type::IMM) {
        switch (rel_op) {
        case RelOp::LE:
            return IMM_(lhs.value <= rhs.value);
        case RelOp::GE:
            return IMM_(lhs.value >= rhs.value);
        case RelOp::LT:
            return IMM_(lhs.value < rhs.value);
        case RelOp::GT:
            return IMM_(lhs.value > rhs.value);
        default:
            assert(false);
        }
    }
    // 若左右表达式结果不均为常量，则使用临时变量计算结果并存储之
    else {
        Result result = NEW_REG_;
        switch (rel_op) {
        case RelOp::LE:
            koopa_ofs << "\t" << result << " = le " << lhs << ", " << rhs << endl;
            break;
        case RelOp::GE:
            koopa_ofs << "\t" << result << " = ge " << lhs << ", " << rhs << endl;
            break;
        case RelOp::LT:
            koopa_ofs << "\t" << result << " = lt " << lhs << ", " << rhs << endl;
            break;
        case RelOp::GT:
            koopa_ofs << "\t" << result << " = gt " << lhs << ", " << rhs << endl;
            break;
        default:
            assert(false);
        }
        return result;
    }
}

/**
 * @brief 打印加法表达式
 * */
 Result AddExpAST::print() const {
    return mul_exp->print();
}

/**
 * @brief 转换加法运算符，输出枚举类型
 * @param[in] op 加法运算符
 * @return 加法运算符枚举类型
 */
AddExpWithOpAST::AddOp AddExpWithOpAST::convert(const string& op) const {
    if (op == "+") {
        return AddOp::ADD;
    }
    else if (op == "-") {
        return AddOp::SUB;
    }
    throw runtime_error("Invalid operator: " + op);
}

/**
 * @brief 打印带符号的加法表达式
 * @return 计算结果所在寄存器或立即数
 */
Result AddExpWithOpAST::print() const {
    // 先计算左右表达式结果
    Result lhs = left->print();
    Result rhs = right->print();
    // 若左右表达式结果均为常量，则直接返回常量结果
    if (lhs.type == Result::Type::IMM && rhs.type == Result::Type::IMM) {
        switch (add_op) {
        case AddOp::ADD:
            return IMM_(lhs.value + rhs.value);
        case AddOp::SUB:
            return IMM_(lhs.value - rhs.value);
        default:
            assert(false);
        }
    }
    // 若左右表达式结果不均为常量，则使用临时变量计算结果并存储之
    else {
        Result result = NEW_REG_;
        switch (add_op) {
        case AddOp::ADD:
            koopa_ofs << "\t" << result << " = add " << lhs << ", " << rhs << endl;
            break;
        case AddOp::SUB:
            koopa_ofs << "\t" << result << " = sub " << lhs << ", " << rhs << endl;
            break;
        default:
            assert(false);
        }
        return result;
    }
}

/**
 * @brief 打印乘法表达式
 * */
Result MulExpAST::print() const {
    return unary_exp->print();
}

/**
 * @brief 转换乘法运算符，输出枚举类型
 * @param[in] op 乘法运算符
 * @return 乘法运算符枚举类型
 */
MulExpWithOpAST::MulOp MulExpWithOpAST::convert(const string& op) const {
    if (op == "*") {
        return MulOp::MUL;
    }
    else if (op == "/") {
        return MulOp::DIV;
    }
    else if (op == "%") {
        return MulOp::MOD;
    }
    throw runtime_error("Invalid operator: " + op);
}

/**
 * @brief 打印带符号的乘法表达式
 * @return 计算结果所在寄存器或立即数
 */
Result MulExpWithOpAST::print() const {
    // 先计算左右表达式结果
    Result lhs = left->print();
    Result rhs = right->print();
    // 若左右表达式结果均为常量，则直接返回常量结果
    if (lhs.type == Result::Type::IMM && rhs.type == Result::Type::IMM) {
        switch (mul_op) {
        case MulOp::MUL:
            return IMM_(lhs.value * rhs.value);
        case MulOp::DIV:
            return IMM_(lhs.value / rhs.value);
        case MulOp::MOD:
            return IMM_(lhs.value % rhs.value);
        default:
            assert(false);
        }
    }
    // 若左右表达式结果不均为常量，则使用临时变量计算结果并存储之
    else {
        Result result = NEW_REG_;
        switch (mul_op) {
        case MulOp::MUL:
            koopa_ofs << "\t" << result << " = mul " << lhs << ", " << rhs << endl;
            break;
        case MulOp::DIV:
            koopa_ofs << "\t" << result << " = div " << lhs << ", " << rhs << endl;
            break;
        case MulOp::MOD:
            koopa_ofs << "\t" << result << " = mod " << lhs << ", " << rhs << endl;
            break;
        default:
            assert(false);
        }
        return result;
    }
}

/**
 * @brief 打印一元表达式
 * */
Result UnaryExpAST::print() const {
    return primary_exp->print();
}

/**
 * @brief 转换一元运算符，输出枚举类型
 * @param[in] op 一元运算符
 * @return 一元运算符枚举类型
 */
UnaryExpWithOpAST::UnaryOp UnaryExpWithOpAST::convert(const string& op) const {
    
    if (op == "+") {
        return UnaryOp::POSITIVE;
    }
    else if (op == "-") {
        return UnaryOp::NEGATIVE;
    }
    else if (op == "!") {
        return UnaryOp::NOT;
    }
    throw runtime_error("Invalid operator: " + op);
}

/**
 * @brief 打印带符号的一元表达式
 * @return 计算结果所在寄存器或立即数
 */
 Result UnaryExpWithOpAST::print() const {
    
    // 先计算表达式结果
    Result unary_exp_result = unary_exp->print();
    // 若表达式结果为常量，则直接返回常量结果
    if (unary_exp_result.type == Result::Type::IMM) {
        switch (unary_op) {
        case UnaryOp::POSITIVE:
            return IMM_(unary_exp_result.value);
        case UnaryOp::NEGATIVE:
            return IMM_(-unary_exp_result.value);
        case UnaryOp::NOT:
            return IMM_(!unary_exp_result.value);
        default:
            assert(false);
        }
    }
    // 若表达式结果为临时变量，则使用临时变量计算结果并存储之
    else {
        Result result = NEW_REG_;
        switch (unary_op) {
        case UnaryOp::POSITIVE:
            koopa_ofs << "\t" << result << " = add 0, " << unary_exp_result << endl;
            break;
        case UnaryOp::NEGATIVE:
            koopa_ofs << "\t" << result << " = sub 0, " << unary_exp_result << endl;
            break;
        case UnaryOp::NOT:
            koopa_ofs << "\t" << result << " = eq 0, " << unary_exp_result << endl;
            break;
        default:
            assert(false);
        }
        return result;
    }
}

/**
* @brief 打印括号优先表达式，即 (a)
* @return 计算结果所在寄存器或立即数
*/
Result PrimaryExpAST::print() const {
    return exp->print();
}

/**
 * @brief 打印数字优先表达式，即 1
 * @return 立即数
 */
Result PrimaryExpWithNumberAST::print() const {
    return IMM_(number);
}