#ifndef _ASSEMBLER_HPP_
#define _ASSEMBLER_HPP_

#include <cstdint>
#include <iostream>
#include <vector>

namespace Assembler {

// Produce little endian binary from assembly
std::vector<uint8_t> assemble(std::istream& assembly);

}; // namespace Assembler

#endif
