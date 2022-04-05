#! /usr/bin/env python3

import os, argparse, pathlib, re
from jinja2 import Template

def create_output(path, cycles, ref_cycles, instr_bytes, test_name="trace"):
    template_path = os.path.join(os.path.dirname(__file__), "template.c.jinja2")
    with open(template_path, "r") as f:
        template = Template(f.read())

    with open(path, "w") as f:
        f.write(template.render(
            cycles = cycles,
            ref_cycles = ref_cycles,
            instr_bytes = instr_bytes,
            test_name = test_name
        ))

    print("Created new test case at", str(path))

def create_undef_output(path, cycles, ref_cycles, num_instrs, test_name="trace"):
    instr_bytes = list("nondet_u8()" for _ in range(4 * num_instrs))
    create_output(path, cycles, ref_cycles, instr_bytes, test_name)

def read_graphml_vals(graphml_path):
    with open(graphml_path, "r") as f:
        matches = re.findall(r"instr_mem\[(\d+)\]\s*=\s*([^;]+)", f.read())
        matches = list(map(lambda v: (int(v[0]), int(v[1])), matches))

    assert len(matches) > 0, "No matches were found in the graphml file"
    matches.sort(key=lambda v: v[0])

    assert matches[0][0] == 0, "graphml assignments must start at idx 0"
    assert matches[-1][0] == len(matches) - 1, "graphml assignments must be to consecutive bytes"

    keys = set(map(lambda v: v[0], matches))
    assert len(matches) == len(keys), "graphml assignments must be to consecutive bytes"

    return list(map(lambda v: v[1], matches))

def main():
    parser = argparse.ArgumentParser(description="Create ESBMC/Directed equivalence tests")
    parser.add_argument("output", type=pathlib.Path, help="Output test path")
    parser.add_argument("--add-id", action="store_true", help="Add id to generated path")
    parser.add_argument("--cycles", "-c", metavar="N", type=int, required=True,
                        help="How many cycles to run the pipelined core for")
    parser.add_argument("--ref-cycles", "-r", metavar="N", type=int, required=True,
                        help="How many cycles to run the reference core for")

    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument("--num-instrs", "-n", metavar="N", type=int,
                       help="How many input instructions to generate for ESBMC")
    group.add_argument("--graphml_path", "-g", metavar="PATH", type=pathlib.Path,
                       help="ESBMC counter example to turn into directed test")

    args = parser.parse_args()

    if args.num_instrs:
        create_undef_output(args.output, args.cycles, args.ref_cycles, args.num_instrs)
    else:
        instr_bytes = read_graphml_vals(args.graphml_path)
        if args.add_id:
            id = "".join(map(lambda i: "{0:02x}".format(i), instr_bytes))
            args.output = args.output.with_suffix("." + id + ".c")
            create_output(args.output, args.cycles, args.ref_cycles, instr_bytes, "trace" + id)
        else:
            create_output(args.output, args.cycles, args.ref_cycles, instr_bytes)

if __name__ == "__main__":
    main()
