from scapy.all import *

flag = bytearray(100)
cap = rdpcap("dist/btle.pcap")

for pdu in cap:
    if pdu.haslayer(ATT_Prepare_Write_Request):
        offset = pdu[ATT_Prepare_Write_Request].offset
        data = pdu[ATT_Prepare_Write_Request].data
        flag[offset:offset+len(data)] = data

    elif pdu.haslayer(ATT_Write_Request):
        data = pdu[ATT_Write_Request].data
        flag[:len(data)] = data

print(flag.rstrip(b"\x00"))