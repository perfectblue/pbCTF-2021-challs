#!/bin/bash
iptables -I OUTPUT -j DROP -m owner --uid-owner nobody
socat -d -d TCP-LISTEN:12345,fork,reuseaddr EXEC:./server.py,stderr

