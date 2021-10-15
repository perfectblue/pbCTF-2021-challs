# Ghost writer

**Category**: Misc

**Author**: UnblvR

**Description**: 

While I was writing the beginning of my new bestselling novel, a ghost possessed my keyboard and began typing in a flag! I tried to finish the story, but the computer died shortly after. Luckily, my digital voice recorder caught it all. Can you recover it?

I'm a slow writer, and my custom keyboard has only 27 unique keys; "a-z" and " " (space). No other keys are used. Also, I seem to recall that there's exactly 275 key presses recorded.

Note: Follows flag format, but **add underscores**.



**Hints**:

 * The text starts with "the day".

**Public files**: 

 * dist/dist.zip



## Solution

Since the sounds are synthetically generated, this can be solved by segmenting the sounds and calculate similarities between the segments. The problem can then be solved as a monoalphabetic substitution cipher, optionally by including English bigram and trigram statistics to converge at a better solution faster.

There is also a tool called "Keytap2" by Georgi Gerganov, which is able to do half of this job for you. However, it is detecting the space bar presses as two separate keys, so some knowledge about the program is required to fix this up before running. The WAV is also recorded as stereo in 44.1KHz, while keytap2 expects headerless (RAW) WAV with 32-bit floats, and at a rate like 16KHz instead. By knowing the amount of keys pressed, it is possible to manually remove the double detections.