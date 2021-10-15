import os
import sys
import time
import random
import socket
import sqlite3
import hashlib
import threading
import subprocess
from collections import deque

random.seed()
os.chdir('/root/vm')
assert os.getcwd() == '/root/vm'
assert os.system('iptables -t nat -P PREROUTING ACCEPT') == 0
assert os.system('iptables -t nat -F PREROUTING') == 0
assert os.system('sysctl net.ipv4.ip_forward=1') == 0
assert os.system('sysctl net.ipv4.conf.all.route_localnet=1') == 0

wait_queue = deque()
queue_lock = threading.Lock()

class Client(object):
    def __init__(self, conn, addr):
        self.conn = conn
        self.addr = addr
        self.lock = threading.Lock()
        self.team_id = -1
    def send(self, what):
        self.conn.send(what.encode('ascii'))
    def recv(self, n):
        return self.conn.recv(n).decode('ascii')
    def recvline(self):
        buf = ''
        while not buf.endswith('\n'):
            buf += self.recv(1)
        return buf
    def close(self):
        try:
            self.conn.close()
        except:
            print('conn close failed?')
            pass
    @property
    def closed(self):
        return self.conn._closed or self.conn.fileno() == -1
    def __enter__(self):
        self.lock.acquire()
        return self
    def __exit__(self, exc_type, exc_val, exc_tb):
        self.lock.release()

def validate_team_code(code):
    import hmac
    import hashlib
    HMAC_KEY = b'5116723a36f9e8cd6ab7341be44ca04abd2fe54866bc16cff20c79b787f16934'

    for team_id in range(10000):
        if hmac.new(HMAC_KEY, str(team_id).encode(), hashlib.sha256).hexdigest() == code:
            return team_id
    #example: 0440fc8b03e5f1578c8655c88de529a60af954dd240e0b4260b93d17068dd690

    return -1

def check_for_duplicate_team(team_id):
    with queue_lock:
        for client in wait_queue:
            if client.team_id == team_id:
                return True
    return False


def handle_client(client):
    client.send("Hello!\n")

    nonce = str(random.randint(111111111111111,999999999999999))
    client.send(f"PoW: Give me x where sha256( {nonce} + x ) starts with 0000000\n")
    client.send(f"(120 second timeout)\n")
    client.conn.settimeout(120)
    x = client.recvline().rstrip()
    client.conn.settimeout(30)

    m = hashlib.sha256()
    m.update((nonce + x).encode('ascii'))
    h = m.digest().hex()
    if not h.startswith('0000000'):
        client.send(f'Bad PoW. Got hash: {h}\n')
        return
    client.send('PoW accepted, thank you\n')


    client.send("Please send your team code (64 chars): ")
    code = client.recvline().rstrip()
    client.team_id = validate_team_code(code)
    if client.team_id < 0:
        client.send('Invalid team code. Goodbye\n')
        return

    
    print('Team %d wants to join the queue' % client.team_id)
    if check_for_duplicate_team(client.team_id):
        client.send('Sorry your team is already in the queue. Goodbye\n')
        return

    with queue_lock:
        wait_queue.append(client)
        client.send('You are in position %d in the queue\n' % len(wait_queue))

    # some basic keepalive
    while True:
        time.sleep(60)
        with client:
            if client.closed:
                break
            client.send('\x7f') # send an invisible character

def handle_client_thread(client):
    try:
        handle_client(client)
    finally:
        client.close()

def client_go(client):
    # client must be locked!!
    client.send('Alright buddy your time to shine!\n')
    ip = client.addr[0]
    client.send('It appears to me your IP address is ' + ip + '\n')
    for c in ip:
        if c not in '1234567890.':
            client.send('Your ip address is weird so goodbye')
            return
    assert os.system('iptables -t nat -I PREROUTING -p tcp -s ' + ip + ' --dport 1338 -j DNAT --to-destination 127.0.0.1:10001') == 0
    client.send('This IP has been temporarily whitelisted for access to the VNC port (port 1338).\n')
    client.conn.setblocking(True) # !!! important
    assert os.getcwd() == '/root/vm'
    try:
        subprocess.run(['/usr/bin/python3', './runner.py'], stdin=client.conn, stdout=client.conn, stderr=client.conn, timeout=120)
    except subprocess.TimeoutExpired:
        print('TIMEOUT! on client', client.addr)

def process_queue_thread():
    print('Process queue thread is running')
    while True:
        with queue_lock:
            empty = not wait_queue
        if empty:
            time.sleep(1.0)
            continue

        with queue_lock:
            print('Current queue depth: ', len(wait_queue))
            print('Current queued teams: ', end='')
            for c in wait_queue:
                print(c.team_id, end=' ')
            print('')

            client = wait_queue.popleft()

            for i, remaining_client in enumerate(wait_queue):
                with remaining_client:
                    try:
                        remaining_client.send('You are in position %d in the queue\n' % (i+1))
                    except:
                        remaining_client.close()

        with client:
            print('Processing', client.addr)
            assert os.system('iptables -t nat -F PREROUTING') == 0
            try:
                client_go(client)
            except Exception as e:
                print(e)
            finally:
                client.close()
                assert os.system('iptables -t nat -F PREROUTING') == 0
            print('Finished processing', client.addr)

t = threading.Thread(target=process_queue_thread)
t.daemon=True
t.start()

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

socket.setdefaulttimeout(30) # i'm not here to wait on some fucking slowpokes to bring the whole show down.

s.bind(('0.0.0.0', 1337))
s.listen(5)
print('Listen server is running')
while True:
    conn, addr = s.accept()
    print('Connected by', addr)
    client = Client(conn, addr)
    t = threading.Thread(target=handle_client_thread, args=(client,))
    t.daemon=True
    t.start()
