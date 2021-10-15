# J U S T

**Category**: Rev

**Author**: cts

**Description**:

Just fuck my shit up.

Remote: 1.2.3.4:1337

**Hints**:
 * It is solveable in under 48 hours.

**Public Files**:
 * dist

**Remote files**:
 - src/a.out
 - flag.txt
 - src/motd.ansi

**Solution**:

Differential fault analysis to separate rounds. Then you can undo the cipher. Then just send `cat flag` to it
