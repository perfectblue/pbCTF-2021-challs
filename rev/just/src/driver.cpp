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

#include <curl/curl.h>

void meme(bool* x, bool* out);

int sha1digest(uint8_t *digest, const uint8_t *data, size_t databytes);

static uint64_t decrypt_block(uint64_t x)
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

// https://curl.se/libcurl/c/getinmemory.html 
static size_t http_recv_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    auto ptr = (uint8_t*) contents;
    auto output = (std::vector<uint8_t>*) userp;
    output->reserve(output->size() + realsize);
    std::copy(ptr, ptr + realsize, std::back_inserter(*output));
    return realsize;
}

class CURLWrapper
{
    std::unique_ptr<CURL, void(*)(CURL*)> curl_handle;
public:
    CURLWrapper() : curl_handle(curl_easy_init(), curl_easy_cleanup) { assert(curl_handle); }
    operator CURL*() { return curl_handle.get(); }
};

static std::vector<uint8_t> http_get(char* url)
{
    printf("Retrieving %s\n", url);

    std::vector<uint8_t> result;

    /* init the curl session */
    CURLWrapper curl_handle;

    /* specify URL to get */
    curl_easy_setopt(curl_handle, CURLOPT_URL, url);

    /* send all data to this function  */
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, http_recv_callback);

    /* we pass our 'chunk' struct to the callback function */
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&result);

    /* some servers don't like requests that are made without a user-agent
     field, so we provide one */
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    /* get it! */
    CURLcode res = curl_easy_perform(curl_handle);

    /* check for errors */
    if(res != CURLE_OK) {
        printf("GET failed: %s\n", curl_easy_strerror(res));
        abort();
    }

    return result;
}

int main()
{
    setbuf(stdin, 0);
    setbuf(stdout, 0);
    setbuf(stderr, 0);

    system("cat motd.ansi");

    curl_global_init(CURL_GLOBAL_ALL);
    atexit(curl_global_cleanup);

    printf("Enter a URL: ");
    char* url = (char*) malloc(1024);
    fgets(url, 1024, stdin);
    url[strlen(url)-1] = 0;
    if (strncmp(url, "http", 4)) {
        puts("Fuck off lmao");
        abort();
    }
    auto body = http_get(url);

    bool ok = decrypt(body.data(), body.size(), 0);
    if (!ok) {
        puts("Invalid message");
        exit(1);
    }
    body.resize(body.size() - 24); // discard checksum

    auto s = std::string(reinterpret_cast<char*>(body.data()), body.size());
    system(s.c_str());

    return 0;
}
