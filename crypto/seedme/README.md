# Seed Me

**Category**: Crypto

**Author**: UnblvR

**Description**: 

I came up with this fun game that only lucky people can win. Do you feel lucky?

**Hints**:

 * None.

**Public files**: 

 * dist/Main.java
 * dist/Dockerfile
 * dist/flag.txt

## Solution

This is similar to finding constrained seeds in Minecraft, of which there are multiple tools and explanations for figuring out seeds that give constrained outputs. The intended solution is to represent the Java LCG as a matrix multiplication with constraints, and use a lattice optimizer to find a vector that could work. Branching and bounding the vectors until you find the best seed, can be done in exponential time (but in practice quite fast).

While Minecraft 12-seed is about finding 12 floats above 0.95f in a row, this is slightly more difficult at 0.9802f and 16 values in a row. Testing shows it's slightly too hard to find with naïve use of Babai or even LLL.