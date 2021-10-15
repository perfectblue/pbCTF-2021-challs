# Switching it up

**Category**: Rev

**Author**: UnblvR

**Description**: 

I upgraded our flag checker to Python 3.10.0b1. Can you figure it out?

**Hints**:

 * None.

**Public files**: 

 * dist/challenge.cpython-310.pyc

## Solution

The challenge is pretty straight-forward, but meant to mess with people who use uncompyle6 or even the dis module, as all the variables are Hangul Filler (U+3164). There should only be a single solution that produces "Correct".