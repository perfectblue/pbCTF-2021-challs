#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <climits>
#include <cstring>
#include <cassert>
#include <bitset>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <bit>

int sha1digest(uint8_t *digest, const uint8_t *data, size_t databytes);

void meme(bool* x, bool* out);

constexpr uint64_t invmod(uint64_t a) noexcept {
    uint64_t x = (((a+2u)&4u)<<1)+a;    // low  4 bits of inverse
    x =    (2u-1u*a*x)*x;               // low  8 bits of inverse
    x =    (2u-1u*a*x)*x;               // low 16 bits of inverse
    x =    (2u-1u*a*x)*x;               // low 32 bits of inverse
    return (2u-1u*a*x)*x;               // low 64 bits of inverse
}

static uint64_t decrypt_block_meme(uint64_t x)
{
    std::bitset<64> tmp(x);
    bool input[64];
    for (int i = 0; i < 64; i++)
        input[i] = tmp[i];
    bool output[64];
    meme(input, output);
    for (int i = 0; i < 64; i++)
        tmp[i] = output[i];
    return tmp.to_ulong();
}

uint64_t decrypt_block(uint64_t x) {
    uint64_t tmp = x;
    tmp = tmp ^ 0x9445827458d1e38f;
    tmp = tmp + 0xf053cfc591ae5a85;
    tmp = tmp * 0xf61475c34efa5845;
    tmp = std::rotr(tmp, 46);
    tmp = tmp ^ 0x2fc55b45c61ebe31;
    tmp = tmp + 0x4e317ba1cc4318f6;
    tmp = tmp * 0x248ce601f038a07;
    tmp = std::rotr(tmp, 15);
    tmp = tmp ^ 0x31a9687a17608c12;
    tmp = tmp + 0xed0c25e75b89aced;
    tmp = tmp * 0x975bf348d59b263b;
    tmp = std::rotr(tmp, 41);
    tmp = tmp ^ 0xc6ef667a38bdce09;
    tmp = tmp + 0x71122c73403c6eb9;
    tmp = tmp * 0xcccb0c9343f99e2b;
    tmp = std::rotr(tmp, 29);
    tmp = tmp ^ 0xb6d5b0156ab6173c;
    tmp = tmp + 0xc1dc47474a382c47;
    tmp = tmp * 0x684a6a759995e6bd;
    tmp = std::rotr(tmp, 4);
    tmp = tmp ^ 0xf7717a58c7bb2d9e;
    tmp = tmp + 0xfc8209b7ca52282a;
    tmp = tmp * 0x2613486a2bb075b9;
    tmp = std::rotr(tmp, 44);
    tmp = tmp ^ 0xf665597571bc48d;
    tmp = tmp + 0x85334c2b17e4655a;
    tmp = tmp * 0x91e7d7118a7c3b1f;
    tmp = std::rotr(tmp, 1);
    tmp = tmp ^ 0x473f56b61d111668;
    tmp = tmp + 0x60fd0697f9c18b48;
    tmp = tmp * 0xe741548ac2acdc99;
    tmp = std::rotr(tmp, 44);
    return tmp;
}

uint64_t encrypt_block(uint64_t x) {
    uint64_t tmp = x;
    tmp = std::rotl(tmp, 44);
    tmp = tmp * invmod(0xe741548ac2acdc99);
    tmp = tmp - 0x60fd0697f9c18b48;
    tmp = tmp ^ 0x473f56b61d111668;
    tmp = std::rotl(tmp, 1);
    tmp = tmp * invmod(0x91e7d7118a7c3b1f);
    tmp = tmp - 0x85334c2b17e4655a;
    tmp = tmp ^ 0xf665597571bc48d;
    tmp = std::rotl(tmp, 44);
    tmp = tmp * invmod(0x2613486a2bb075b9);
    tmp = tmp - 0xfc8209b7ca52282a;
    tmp = tmp ^ 0xf7717a58c7bb2d9e;
    tmp = std::rotl(tmp, 4);
    tmp = tmp * invmod(0x684a6a759995e6bd);
    tmp = tmp - 0xc1dc47474a382c47;
    tmp = tmp ^ 0xb6d5b0156ab6173c;
    tmp = std::rotl(tmp, 29);
    tmp = tmp * invmod(0xcccb0c9343f99e2b);
    tmp = tmp - 0x71122c73403c6eb9;
    tmp = tmp ^ 0xc6ef667a38bdce09;
    tmp = std::rotl(tmp, 41);
    tmp = tmp * invmod(0x975bf348d59b263b);
    tmp = tmp - 0xed0c25e75b89aced;
    tmp = tmp ^ 0x31a9687a17608c12;
    tmp = std::rotl(tmp, 15);
    tmp = tmp * invmod(0x248ce601f038a07);
    tmp = tmp - 0x4e317ba1cc4318f6;
    tmp = tmp ^ 0x2fc55b45c61ebe31;
    tmp = std::rotl(tmp, 46);
    tmp = tmp * invmod(0xf61475c34efa5845);
    tmp = tmp - 0xf053cfc591ae5a85;
    tmp = tmp ^ 0x9445827458d1e38f;
    return tmp;
}

static uint8_t* encrypt(uint8_t* buf, size_t len, uint64_t iv, size_t* len_out)
{
    size_t out_len = len;
    if (len % 8)   
        out_len += 8 - (len % 8);
    assert(out_len % 8 == 0);
    out_len += 24;

    uint8_t* out = (uint8_t*) calloc(1, out_len);
    memcpy(out, buf, len);
    sha1digest(out + out_len - 24, out, out_len - 24);

    auto block = (uint64_t*) out;
    for (size_t i = 0; i < out_len/8; i++, block++) {
        uint64_t next_iv = *block;
        *block = encrypt_block(*block) ^ iv;
        iv = next_iv;
    }

    *len_out = out_len;
    return out;
}

static bool decrypt(uint8_t* buf, size_t len, uint64_t iv)
{
    if (len < 24)
        return false;

    if (len % 8)
        return false;

    auto block = (uint64_t*) buf;
    for (size_t i = 0; i < len/8; i++, block++) {
        *block = iv = decrypt_block(*block ^ iv);
    }

    uint8_t digest[20];
    sha1digest(digest, buf, len - 24);

    if (memcmp(digest, buf + len - 24, 20))
        return false;

    return true;
}

void hexdump(uint8_t* x, size_t len) {
    for (size_t i = 0; i < len; i++)
        printf("%02x ", x[i]);
    printf("\n");
}

int main(int argc, char** argv) {
    for (uint64_t i = 0; i < 1000; i++) {
        assert (i != decrypt_block(i));
        assert (decrypt_block_meme(i) == decrypt_block(i));
        assert (i == encrypt_block(decrypt_block(i)));
    }
    puts("Test vectors OK");

    if (argc < 2)
        exit(1);

    auto f = fopen(argv[1], "rb");
    fseek(f, 0, SEEK_END);
    auto filesz = ftell(f);
    fseek(f, 0, SEEK_SET);
    auto buf = (uint8_t*) malloc(filesz);
    for (size_t n = 0; n < filesz; n += fread(buf+n, 1, filesz-n, f));
    fclose(f);

    hexdump(buf, filesz);

    uint64_t iv = 0;
    size_t ciphertext_len;
    auto ciphertext = encrypt(buf, filesz, iv, &ciphertext_len);
    hexdump(ciphertext, ciphertext_len);

    char* out_name;
    asprintf(&out_name, "%s.enc", argv[1]);
    f = fopen(out_name, "wb");
    for (size_t n = 0; n < ciphertext_len; n += fwrite(ciphertext+n, 1, ciphertext_len-n, f));
    fclose(f);

    bool ok = decrypt(ciphertext, ciphertext_len, iv);
    hexdump(ciphertext, ciphertext_len);

    assert (ok);
    assert (!memcmp(buf, ciphertext, filesz));
    puts("OK");
}
