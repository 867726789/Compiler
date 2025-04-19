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
    return add_exp->print();
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