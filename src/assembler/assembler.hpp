#ifndef _ASSEMBLER_HPP_
#define _ASSEMBLER_HPP_

#include <cstdint>
#include <iostream>
#include <vector>

namespace Assembler {

std::vector<uint32_t> assemble(std::istream& assembly);

};

#endif
