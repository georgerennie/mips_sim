#! /usr/bin/env python3

import os, argparse, pathlib, re
from jinja2 import Template

def read_graphml_data(graphml_path):
    with open(graphml_path, "r") as f:
        contents = f.read()

    re_matches = re.findall(r"instr_mem_32\s+\+\s+(\d+)\)\s+=\s+(-?\d+)", contents)

    dict_matches = {int(m[0]) : int(m[1]) for m in re_matches}
    max_idx = max(dict_matches.keys())

    instrs = []
    for i in range(max_idx + 1):
        if i in dict_matches:
            instrs.append(dict_matches[i])
        else:
            print("WARNING: could not find assignment to instr_mem_32[", i,
                  "], writing 0 instead", sep="")
            instrs.append(0)

    delay_slots_match = re.search(r"delay_slots\s+=\s+(-?\d+)", contents)
    if delay_slots_match:
        delay_slots = int(delay_slots_match.group(1)) != 0
    else:
        print("WARNING: could not find delay slots assignment, assuming false")
        delay_slots = False

    return instrs, delay_slots

def main():
    parser = argparse.ArgumentParser(description="Parse the output of ESBMC tests")
    parser.add_argument("input", type=pathlib.Path,
                       help="ESBMC graphml counter example to turn into directed test")
    parser.add_argument("output", type=pathlib.Path, help="Output test path")

    args = parser.parse_args()

    instrs = read_graphml_data(args.input)
    instr_bytes = b"".join(map(lambda i: i.to_bytes(4, byteorder='little'), instrs[0]))

    with open(args.output, "wb") as f:
        f.write(instr_bytes)

    print("Delay slots enabled:", instrs[1])

if __name__ == "__main__":
    main()
