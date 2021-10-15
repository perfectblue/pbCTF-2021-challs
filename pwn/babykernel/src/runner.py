#!/usr/bin/env python3
import os
import sys
import time
import subprocess
import urllib.request

assert sys.version_info.major == 3

os.chdir('/root/vm')
assert os.getcwd() == '/root/vm'

def system(cmd):
    assert os.getcwd() == '/root/vm'
    time.sleep(0.05) # FIX RACE CONDITIONS BY JUST WAITING!!!!!! :-)
    code = os.system(cmd)
    time.sleep(0.05) # FIX RACE CONDITIONS BY JUST WAITING!!!!!! :-)
    return code

def cleanup():
    # This function just tries to fix any fucking problems there might be.

    assert os.getcwd() == '/root/vm'

    system('vboxmanage controlvm pbctf-pwn poweroff >/dev/null 2>/dev/null')

    for i in range(3):
        system('umount /root/vm/tmp >/dev/null 2>/dev/null')
        system('rm -rfd /root/vm/tmp')
        if not os.path.exists('/root/vm/tmp'):
            break
        time.sleep(1.0)
    else:
        raise RuntimeError('failed to cleanup /root/vm/tmp CONTACT ADMIN.')

    for i in range(10):
        system('umount /root/vm/mnt >/dev/null 2>/dev/null')
        system('losetup -d /dev/loop123 >/dev/null 2>/dev/null')
        if b'loop123' not in subprocess.check_output('lsblk') and b'/root/vm/mnt' not in subprocess.check_output('mount'):
            break
        time.sleep(3.0)
    else:
        raise RuntimeError('failed to cleanup loop device CONTACT ADMIN.')

    assert system('shred -vzn1 /root/vm/loop.img >/dev/null 2>/dev/null') == 0
    assert system('cp -f /root/vm/loop.img.orig /root/vm/loop.img') == 0

    for i in range(3):
        if system('vboxmanage controlvm pbctf-pwn poweroff >/dev/null 2>/dev/null') == 0:
            break
    assert system('VBoxManage snapshot pbctf-pwn restore my-snapshot >/dev/null 2>/dev/null') == 0

def run():
    TIMEOUT=60

    print('Setting things up...')
    cleanup()
    print("Okay let's rock and roll!")

    os.mkdir('/root/vm/tmp')
    assert system('mount -t tmpfs -o size=2M chaltmp /root/vm/tmp') == 0
    assert os.listdir('/root/vm/tmp') == []

    print('For your convenience, you may specify a URL of a file that will be available on the VM on a removable disk (at A:\\, 2 MB max).')
    print('Enter your URL (or leave empty to opt-out): ', end='')
    url = input().rstrip()
    if url:
        if len(url) > 300:
            print('URL too long')
            return 1
        for i, c in enumerate(url):
            if c not in '1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ.:/%:':
                print('Weird URL. Offending character: "' + c + '"" at position ' + str(i) + '. Goodbye')
                return 1
        if not url.startswith('http://') and not url.startswith('https://'):
            print("URL scheme isn't http or https. Goodbye")
            return 1
        with open('/root/vm/tmp/url.txt', 'w') as f:
            f.write(url + '\n')
        code = system('timeout 15 wget -P /root/vm/tmp -i /root/vm/tmp/url.txt >/dev/null 2>/dev/null')
        if code == 124:
            print('Download timed out. Goodbye')
            return code
        elif code == 3:
            print('Download failed. Check file size. Goodbye')
            return code
        elif code:
            print('Download failed. Goodbye')
            return code
        print('Download succeeded')

    system('umount /root/vm/mnt >/dev/null 2>/dev/null') # in case still mounted
    system('losetup -d /dev/loop123 >/dev/null 2>/dev/null')
    assert system('cp -f /root/vm/loop.img.orig /root/vm/loop.img') == 0
    assert system('losetup /dev/loop123 /root/vm/loop.img') == 0
    assert system('partprobe /dev/loop123') == 0
    assert system('mount /dev/loop123p1 /root/vm/mnt') == 0
    
    if url:
        assert system('cp -f /root/vm/tmp/* /root/vm/mnt/') == 0
        assert len(os.listdir('/root/vm/mnt')) == 3
    assert system('umount /root/vm/tmp') == 0

    assert system('sync -f /root/vm/mnt') == 0
    assert system('umount /root/vm/mnt') == 0

    system('vboxmanage controlvm pbctf-pwn poweroff >/dev/null 2>/dev/null') # in case its somehow still hanging
    assert system('VBoxManage snapshot pbctf-pwn restore my-snapshot') == 0

    system('vboxheadless --startvm pbctf-pwn >/dev/null 2>/dev/null &')

    print('Waiting for vm to start ', end='')
    time.sleep(0.5)
    for i in range(30):
        code = system('timeout 1 vboxmanage showvminfo pbctf-pwn | grep -i State | grep -i running')
        if code == 0:
            break
        print('.', end='')
        sys.stdout.flush()
        time.sleep(0.1)
    else:
        print('VM failed to start! Contact admin!!')
        return 1

    # this will error but it still works. attach USB drive
    system('vboxmanage storageattach pbctf-pwn --storagectl "USB Controller" --port 1 --medium /root/vm/loop2.vmdk --mtype writethrough --hotpluggable on --type hdd >/dev/null 2>/dev/null')

    print('')
    print(f"Okay, you're all set. You have {TIMEOUT} seconds on the VM, make it count.")
    print(f"Connect to port 1338 on VNC. If prompted for password, leave empty.")
    print("")

    for i in range(TIMEOUT+1):
        print(TIMEOUT-i, end=' ')
        sys.stdout.flush()
        time.sleep(1)
    print('')
    print("Alright time's up. Hope you got the flag.")

    system('vboxmanage controlvm pbctf-pwn poweroff >/dev/null 2>/dev/null')
    assert system('VBoxManage snapshot pbctf-pwn restore my-snapshot >/dev/null 2>/dev/null') == 0
    assert system('shred -vzn1 /root/vm/loop.img') == 0

    cleanup()

    print('Goodbye')

try:
    run()
finally:
    cleanup()
