# Cosmo

**Category**: Reverse

**Author**: unblvr

**Description**: To make it fair for everyone, this binary is fully portable. Run it anywhere! This definitely makes it easier, right?

**Public Files**:

- dist/hello.com



**Solution:**

Easy flag checker, using incremental adler32 to verify the flag, 2 characters at the time. Can be brute-forced in milliseconds.

To make it even easier, the binary also early exits upon error, and has an exit code that reveals your progress, which should open up a lot of ways to solve it.
