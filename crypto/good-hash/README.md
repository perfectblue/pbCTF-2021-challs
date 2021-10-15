# GoodHash

**Category**: Crypto

**Author**: rbtree

**Description**:
I think I made a good hash function based on AES. Could you test this?

**Hints**: None

**Public Files**:
 * dist

**Solution**:
Just recover the flag value as [NIST SP800-38D](http://csrc.nist.gov/publications/nistpubs/800-38D/SP-800-38D.pdf).
[Pycryptodome source code](https://github.com/Legrandin/pycryptodome/blob/master/lib/Crypto/Cipher/_mode_gcm.py) is also helpful.