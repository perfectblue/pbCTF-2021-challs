# Out of Order

**Category**: Pwn

**Author**: cts

**Description**:

Who the fuck pwns x86 in 2021? cmon the $$$money$$$ is all in mobile now, LOL

**Hints**:
 * The remote instance is running on a Raspberry Pi 4 with Ubuntu 20.04.3 LTS

**Public Files**:
 * dist

**Solution**:

The race condition does not exist on x86, only on ARM64 due to memory reordering.
Because of this, the write to `wq.head` may be seen by the consumer thread before the write to `wq.backing_buf`, leading to a double free.

See solve.py for solve script
