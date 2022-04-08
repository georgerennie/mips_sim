#ifndef ASSEMBLER_HPP
#define ASSEMBLER_HPP

#include <cinttypes>
#include <string>
#include <vector>

namespace Assembler {

// Produce little endian binary from assembly
std::vector<uint8_t> assemble(std::istream& assembly);

}; // namespace Assembler

#endif
