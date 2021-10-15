# BTLE

**Category**: Misc

**Author**: UnblvR

**Description**: 

I stored my flag inside a remote database, but when I tried to read it back it had been redacted! Can you recover what I wrote?

**Hints**:

 * None.

**Public files**: 

 * dist/btle.pcap (Note: Grep for flag format and make sure only the redacted one shows up, as redaction is a manual operation when generating the file).

## Solution

BTLE is Bluetooth Low Energy, which is readily supported by Wireshark. The flag is written to a GATT characteristic value handle, using Reliable Writes (aka. "Queued Writes"). These can specify offsets to where you write, and the data will only overwrite a substring of the value.

To solve it, the intention is to read up enough about BTLE to know what this feature is, extract all the writes, and keep the latest byte written to all data byte offsets. The traffic is deliberately writing garbage to every offset in the beginning, and writes 1-3 actual flag bytes per write cycle.

An alternative solution is to replay the traffic against a host that has a similar GATT database (which is why the entire discovery procedure is included in the beginning).