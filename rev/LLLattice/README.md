# LLLattice

**Category**: Reverse, Hardware

**Author**: voidmercy

**Description**: It seems like there is a UART echoserver design running on a Lattice FPGA. The UART bus runs 8N1 at a rate of 100 clocks per symbol. Can you reverse it and find out what secret it holds?

**Public Files**: dist/chal.bit

# Solution

A perfect decompilation can be obtained from the bitstream using my tool (https://github.com/VoidMercy/Lattice-ECP5-Bitstream-Decompiler). However, there are a few minor bugs in the tool. Most of the bugs can be found by attempting to simulate using iverilog and are simple to fix. However, one important bug is the absence of an FPGA's GSR (Global Set Reset) functionality which is not emulated during simulation.

I might consider pushing these fixes to the tool after 24 hours if people are having trouble with the challenge. Finally, we can obtain perfect decompilation and simulate the design. The rest of the challenge is easy - we can use verilator or other tools to log the state of each flip flop during simulation, send UART input, observe patterns and brute force the state machine. After sending the right sequence of bytes to RX, then UART TX will send us back the flag.
