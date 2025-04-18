#include "include/ast.hpp"

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
    for (auto &stmt : stmts) {
        stmt->print();
    }
    return Result();
}

/**
 * @brief 打印语句
 * */
Result StmtAST::print() const {
    koopa_ofs << "\tret ";
    koopa_ofs << number->print();
    koopa_ofs << endl;
    return Result();
}

/**
 * @brief 打印语句
 * */
Result NumberAST::print() const {
    return Result(number);
}