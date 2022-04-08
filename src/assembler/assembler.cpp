// vim: ts=4 sw=4 noet
#include "assembler.hpp"
#include <algorithm>
#include <array>
#include <iomanip>
#include <iterator>
#include <map>
#include <regex>
#include <string>
#include <tuple>
#include "core/instruction.h"
#include "util/log.h"

namespace Assembler {

static const std::map<std::string, std::tuple<mips_instr_format_t, mips_opcode_t, mips_funct_t>>
    opc_lookup = {
        {"addu", {MIPS_INSTR_FORMAT_R, MIPS_OPC_ADDU, MIPS_FUNCT_ADDU}},
        {"addiu", {MIPS_INSTR_FORMAT_I, MIPS_OPC_ADDIU, MIPS_FUNCT_NOP}},

        {"and", {MIPS_INSTR_FORMAT_R, MIPS_OPC_AND, MIPS_FUNCT_AND}},
        {"or", {MIPS_INSTR_FORMAT_R, MIPS_OPC_OR, MIPS_FUNCT_OR}},
        {"andi", {MIPS_INSTR_FORMAT_I, MIPS_OPC_ANDI, MIPS_FUNCT_NOP}},
        {"ori", {MIPS_INSTR_FORMAT_I, MIPS_OPC_ORI, MIPS_FUNCT_NOP}},

        {"lw", {MIPS_INSTR_FORMAT_I, MIPS_OPC_LW, MIPS_FUNCT_NOP}},

        {"j", {MIPS_INSTR_FORMAT_J, MIPS_OPC_J, MIPS_FUNCT_NOP}},
};

// From https://stackoverflow.com/a/2275160
static std::vector<std::string> split_on_whitespace(const std::string& input) {
	std::istringstream buffer(input);
	return {(std::istream_iterator<std::string>(buffer)), std::istream_iterator<std::string>()};
}

static int32_t get_int(std::string int_str, const char* line) {
	std::regex int_reg("^-?[0-9]{1,10}$");
	if (!std::regex_match(int_str, int_reg)) {
		log_err_exit("%s Unable to decode \"%s\" as an integer\n", line, int_str.c_str());
	}

	return static_cast<int32_t>(std::stoi(int_str));
}

static uint8_t get_reg(std::string reg_str, const char* line) {
	if (!reg_str.starts_with('$')) {
		log_err_exit(
		    "%s Expected \"%s\" to be a register starting with $\n", line, reg_str.c_str());
	}

	// remove $
	reg_str.erase(0, 1);
	if (reg_str.ends_with(',')) { reg_str.pop_back(); }

	const auto reg_it = std::find(std::begin(mips_reg_lookup), std::end(mips_reg_lookup), reg_str);

	if (reg_it == std::end(mips_reg_lookup)) {
		log_err_exit("%s Unrecognised register name \"$%s\"\n", line, reg_str.c_str());
	}

	return static_cast<uint8_t>(std::distance(std::begin(mips_reg_lookup), reg_it));
}

static uint32_t to_binary(const mips_instr_t& instr) {
	uint32_t val = static_cast<uint32_t>(instr.opcode) << 26;
	switch (instr.format) {
		case MIPS_INSTR_FORMAT_R: {
			val |= INSERT_BITS(25, 21, instr.r_data.rs);
			val |= INSERT_BITS(20, 16, instr.r_data.rt);
			val |= INSERT_BITS(15, 11, instr.r_data.rd);
			val |= INSERT_BITS(10, 6, instr.r_data.shamt);
			val |= INSERT_BITS(5, 0, instr.r_data.funct);
		} break;
		case MIPS_INSTR_FORMAT_I: {
			val |= INSERT_BITS(25, 21, instr.i_data.rs);
			val |= INSERT_BITS(20, 16, instr.i_data.rt);
			val |= INSERT_BITS(15, 0, instr.i_data.immediate);
		} break;
		case MIPS_INSTR_FORMAT_J: {
			val |= INSERT_BITS(25, 0, instr.j_data.address);
		} break;
		default: log_assert_fail("Unrecognised instruction format %d\n", instr.format); break;
	}
	return val;
}

std::vector<uint8_t> assemble(std::istream& assembly) {
	// The string is the label that instruction jumps to in the case of a j instr
	std::vector<std::pair<mips_instr_t, std::string>> instructions;
	uint32_t                                          address = 0x00000000;
	std::map<std::string, uint32_t>                   address_map;

	size_t line_num = 1;
	for (std::string line; std::getline(assembly, line); line_num++) {
		// Automatic line number string for error messages
		const std::string line_string = "line " + std::to_string(line_num) + ":";
		const char*       line_str    = line_string.c_str();

		const auto raw_items = split_on_whitespace(line);

		// Remove comments
		std::vector<std::string> items;
		for (const auto& item : raw_items) {
			if (item.starts_with('#')) { break; }
			items.push_back(item);
		}

		// Empty line
		if (items.size() == 0) { continue; }

		// Linker label
		if (items[0].ends_with(':')) {
			if (items[0].size() == 1) {
				log_err_exit("%s A name is required for labels\n", line_str);
			}

			// Remove :
			items[0].pop_back();

			if (items.size() != 1) {
				log_err_exit(
				    "%s Unexpected tokens after label \"%s\"\n", line_str, items[0].c_str());
			}

			if (address_map.count(items[0])) {
				log_err_exit("%s Label \"%s\" already defined\n", line_str, items[0].c_str());
			}

			address_map.emplace(items[0], address);
			continue;
		}

		// Decode item[0], instruction name
		const auto opc = opc_lookup.find(items[0]);
		if (opc == opc_lookup.end()) {
			log_err_exit("%s Unrecognised instruction \"%s\"\n", line_str, items[0].c_str());
		}

		mips_instr_t instr = {};
		std::string  label = "";
		instr.format       = std::get<0>(opc->second);
		instr.opcode       = std::get<1>(opc->second);

		if (instr.format == MIPS_INSTR_FORMAT_R) {
			if (items.size() != 4) {
				log_err_exit(
				    "%s Expected 3 arguments for instruction \"%s\", got %d\n", line_str,
				    items[0].c_str(), items.size() - 1);
			}

			instr.r_data.rs    = get_reg(items[2], line_str);
			instr.r_data.rt    = get_reg(items[3], line_str);
			instr.r_data.rd    = get_reg(items[1], line_str);
			instr.r_data.shamt = 0;
			instr.r_data.funct = std::get<2>(opc->second);
		}

		else if (instr.format == MIPS_INSTR_FORMAT_I) {
			if (items.size() != 4) {
				log_err_exit(
				    "%s Expected 3 arguments for instruction \"%s\", got %d\n", line_str,
				    items[0].c_str(), items.size() - 1);
			}

			instr.i_data.rs = get_reg(items[2], line_str);
			instr.i_data.rt = get_reg(items[1], line_str);
			// TODO: Check this is in a valid range
			instr.i_data.immediate = static_cast<uint16_t>(get_int(items[3], line_str));
		}

		else if (instr.format == MIPS_INSTR_FORMAT_J) {
			if (items.size() != 2) {
				log_err_exit(
				    "%s Expected 1 argument for instruction \"%s\", got %d\n", line_str,
				    items[0].c_str(), items.size() - 1);
			}

			label = items[1];
		}

		instructions.emplace_back(std::make_pair(instr, label));
		address += 4;
	}

	std::vector<uint8_t> binary;
	binary.reserve(4 * instructions.size());
	for (auto& instruction : instructions) {
		if (instruction.first.format == MIPS_INSTR_FORMAT_J) {
			if (address_map.count(instruction.second) == 0) {
				log_err_exit("Label \"%s\" not defined\n", instruction.second.c_str());
			}
			instruction.first.j_data.address = address_map[instruction.second] >> 2;
		}

		const auto binary_instruction = to_binary(instruction.first);
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
