// vim: ts=4 sw=4 noet
#include "assembler.hpp"
#include <algorithm>
#include <array>
#include <iomanip>
#include <iterator>
#include <map>
#include <string>
#include <tuple>
#include "core/instruction.h"
#include "util/log.h"

namespace Assembler {

static const std::map<std::string, std::tuple<mips_instr_format_t, mips_opcode_t, mips_funct_t>>
    opc_lookup = {
        {"add", {MIPS_INSTR_FORMAT_R, MIPS_OPC_ADD, MIPS_FUNCT_ADD}},
        {"addu", {MIPS_INSTR_FORMAT_R, MIPS_OPC_ADDU, MIPS_FUNCT_ADDU}},
        {"addi", {MIPS_INSTR_FORMAT_I, MIPS_OPC_ADDI, MIPS_FUNCT_NONE}},
        {"addiu", {MIPS_INSTR_FORMAT_I, MIPS_OPC_ADDIU, MIPS_FUNCT_NONE}},

        {"sub", {MIPS_INSTR_FORMAT_R, MIPS_OPC_SUB, MIPS_FUNCT_SUB}},
        {"subu", {MIPS_INSTR_FORMAT_R, MIPS_OPC_SUBU, MIPS_FUNCT_SUBU}},

        {"and", {MIPS_INSTR_FORMAT_R, MIPS_OPC_AND, MIPS_FUNCT_AND}},
        {"or", {MIPS_INSTR_FORMAT_R, MIPS_OPC_OR, MIPS_FUNCT_OR}},
        {"xor", {MIPS_INSTR_FORMAT_R, MIPS_OPC_XOR, MIPS_FUNCT_XOR}},
        {"nor", {MIPS_INSTR_FORMAT_R, MIPS_OPC_NOR, MIPS_FUNCT_NOR}},
        {"andi", {MIPS_INSTR_FORMAT_I, MIPS_OPC_ANDI, MIPS_FUNCT_NONE}},
        {"ori", {MIPS_INSTR_FORMAT_I, MIPS_OPC_ORI, MIPS_FUNCT_NONE}},

        {"lui", {MIPS_INSTR_FORMAT_I, MIPS_OPC_LUI, MIPS_FUNCT_NONE}},

        {"lb", {MIPS_INSTR_FORMAT_I, MIPS_OPC_LB, MIPS_FUNCT_NONE}},
        {"lbu", {MIPS_INSTR_FORMAT_I, MIPS_OPC_LBU, MIPS_FUNCT_NONE}},
        {"lhu", {MIPS_INSTR_FORMAT_I, MIPS_OPC_LHU, MIPS_FUNCT_NONE}},
        {"lw", {MIPS_INSTR_FORMAT_I, MIPS_OPC_LW, MIPS_FUNCT_NONE}},

        {"j", {MIPS_INSTR_FORMAT_J, MIPS_OPC_J, MIPS_FUNCT_NONE}},
};

// From https://stackoverflow.com/a/2275160
static std::vector<std::string> split_on_whitespace(const std::string& input) {
	std::istringstream buffer(input);
	return {(std::istream_iterator<std::string>(buffer)), std::istream_iterator<std::string>()};
}

static uint8_t get_reg(std::string reg_str) {
	log_assert(reg_str.front() == '$');
	reg_str.erase(reg_str.begin());
	if (reg_str.back() == ',') { reg_str.erase(reg_str.end() - 1); }

	const auto reg_it = std::find(std::begin(mips_reg_lookup), std::end(mips_reg_lookup), reg_str);
	log_assert(reg_it != std::end(mips_reg_lookup)); // TODO: Make this an error print

	return static_cast<uint8_t>(std::distance(std::begin(mips_reg_lookup), reg_it));
}

static uint32_t to_binary(const mips_instr_t& instr) {
	// TODO: Add assertions in here
	uint32_t val = static_cast<uint32_t>(instr.opcode) << 26;
	switch (instr.format) {
		case MIPS_INSTR_FORMAT_R: {
			val |= static_cast<uint32_t>(instr.r_data.rs) << 21;
			val |= static_cast<uint32_t>(instr.r_data.rt) << 16;
			val |= static_cast<uint32_t>(instr.r_data.rd) << 11;
			val |= static_cast<uint32_t>(instr.r_data.shamt) << 6;
			val |= static_cast<uint32_t>(instr.r_data.funct);
		} break;
		case MIPS_INSTR_FORMAT_I: {
			val |= static_cast<uint32_t>(instr.i_data.rs) << 21;
			val |= static_cast<uint32_t>(instr.i_data.rt) << 16;
			val |= static_cast<uint32_t>(instr.i_data.immediate);
		} break;
		case MIPS_INSTR_FORMAT_J: {
			val |= static_cast<uint32_t>(instr.j_data.address);
		} break;
		default: log_assert_fail("Unrecognised instruction format %d\n", instr.format); break;
	}
	return val;
}

std::vector<uint8_t> assemble(std::istream& assembly) {
	std::vector<mips_instr_t> instructions;
	for (std::string line; std::getline(assembly, line);) {
		// TODO: Check how many items in items
		// TODO: Support J instructions and store instructions
		// TODO: convert assertions to error reporting
		const auto items = split_on_whitespace(line);

		mips_instr_t instr = {};
		// Decode item[0], instruction name
		const auto opc = opc_lookup.find(items[0]);
		log_assert(opc != opc_lookup.end());

		instr.format = std::get<0>(opc->second);
		instr.opcode = std::get<1>(opc->second);
		if (instr.format == MIPS_INSTR_FORMAT_R) { instr.r_data.funct = std::get<2>(opc->second); }

		// Decode item[1], R: rd, I: rt
		log_assert(items.size() > 1);
		const auto reg_1 = get_reg(items[1]);
		switch (instr.format) {
			case MIPS_INSTR_FORMAT_R: instr.r_data.rd = reg_1; break;
			case MIPS_INSTR_FORMAT_I: instr.r_data.rt = reg_1; break;
			default: log_assert_fail("Unrecognised instruction format %d\n", instr.format); break;
		}

		// Decode item[2], R: rs, I: rs
		log_assert(items.size() > 2);
		const auto reg_2 = get_reg(items[2]);
		switch (instr.format) {
			case MIPS_INSTR_FORMAT_R: instr.r_data.rs = reg_2; break;
			case MIPS_INSTR_FORMAT_I: instr.r_data.rs = reg_2; break;
			default: log_assert_fail("Unrecognised instruction format %d\n", instr.format); break;
		}

		// Decode item[2], R: rt, I: imm
		log_assert(items.size() > 3);
		switch (instr.format) {
			case MIPS_INSTR_FORMAT_R: instr.r_data.rs = get_reg(items[3]); break;
			// TODO: Error check this stoi
			case MIPS_INSTR_FORMAT_I:
				instr.i_data.immediate = static_cast<uint8_t>(std::stoi(items[3]));
				break;
			default: log_assert_fail("Unrecognised instruction format %d\n", instr.format); break;
		}

		instructions.emplace_back(instr);
	}

	std::vector<uint8_t> binary;
	binary.reserve(4 * instructions.size());
	for (const auto instruction : instructions) {
		const auto binary_instruction = to_binary(instruction);
		// Little endian
		binary.insert(
		    binary.end(), {
		                      static_cast<uint8_t>(binary_instruction),
		                      static_cast<uint8_t>(binary_instruction >> 8),
		                      static_cast<uint8_t>(binary_instruction >> 16),
		                      static_cast<uint8_t>(binary_instruction >> 24),
		                  });
	}

	return binary;
}

} // namespace Assembler
