#pragma once

#include "koopa.h"
#include <iostream>
#include <cassert>
#include <fstream>
#include <cstring>
#include "include/backend_utils.hpp"

using namespace std;

extern ofstream koopa_ofs;
extern ofstream riscv_ofs;

void visit(const koopa_raw_program_t& program);
void visit(const koopa_raw_slice_t& slice);
void visit(const koopa_raw_function_t& func);
void visit(const koopa_raw_basic_block_t& bb);
void visit(const koopa_raw_value_t& value);
void visit(const koopa_raw_return_t& ret);