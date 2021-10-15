#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cstring>

#include <vector>
#include <unordered_map>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <bitset>
#include <intrin.h>

#include <Windows.h>

#include "json.hpp"

#define assert(what) { if (!(what)) abort(); }
#define require(what) { __halt(); if (!(JustALilFYI_IfYoureReadingThisYouAreGettingTrolled_Lmao(), __halt(), anti_debug(), what)) { puts("Wrong"); __halt(); return 1; } __halt(); }

template <typename T>
__forceinline T offset(void* base, ptrdiff_t offset)
{
	return reinterpret_cast<T>(((uintptr_t)base) + offset);
}

__forceinline void CryptPage(void* page)
{
	auto end = offset<uint64_t*>(page, 0x1000);
	uint64_t i = 0;
	for (uint64_t* ptr = (uint64_t*)page; ptr < end; ptr++, i++)
	{
		*ptr ^= (uint64_t)ptr ^ ~i;
	}
}

__declspec(dllexport) volatile void JustALilFYI_IfYoureReadingThisYouAreGettingTrolled_Lmao();

// Crash violently
void __forceinline __declspec(noreturn) crash()
{
	// No stack trace! Wow!
	void* x = alloca(1);
	x = (void*)((uintptr_t)x & ~0xfff);
	memset(x, 0xff, 0xffffffffffff);
}

// Anti VM
__forceinline float measure_cpuid()
{
	auto tsc1 = __rdtsc();
	unsigned int cpuInfo[4];
	const unsigned int n = 10000;
	for (unsigned int i = 0; i < n; i++)
	{
		__cpuid((int*)cpuInfo, 1);
	}
	auto tsc2 = __rdtsc();
	auto delta = tsc2 - tsc1;
	float latency = delta / float(n);
	//printf("%f\n", latency);
	return latency;
}

__forceinline void anti_vm()
{
	for (int i = 0; i < 5; i++)
		if (measure_cpuid() < 250.f)
			return;
	crash();
}

__forceinline void anti_debug()
{
	static ULONGLONG prev_timestamp = __rdtsc();
	static int anti_debug_failure = 0;
	ULONGLONG cur_timestamp = __rdtsc();
	auto delta = cur_timestamp - prev_timestamp;
	if (delta > 100000000ULL)
		anti_debug_failure++;
	if (anti_debug_failure >= 3)
		crash();
	prev_timestamp = __rdtsc();
}

void anti_vm();

bool crt_is_ready = false;

__declspec(noinline) uint64_t ExtremelySpookyFunction(uint64_t Rcx)
{
	__nop(); // Make room for hook
	return Rcx;
}

__declspec(noinline) bool ExtremelySpookyFunction2(bool Rcx)
{
	__nop(); // Make room for hook
	return Rcx;
}

int main()
{
	crt_is_ready = true;

	__halt();

	printf("Enter your flag: ");

	std::string flag;
	std::getline(std::cin, flag);

	std::stringstream bits;
	for (std::size_t i = 0; i < flag.size(); ++i)
		bits << std::bitset<8>(flag.c_str()[i]);

	auto grab = [&](int n) {
		unsigned int x = 0;
		std::vector<int> asdf;
		for (int i = 0; i < n; i++)
			asdf.push_back(bits.get() - '0');
		std::reverse(asdf.begin(), asdf.end());
		for (int b : asdf) {
			x |= b;
			x <<= 1;
		}
		//printf("%d\n", x >> 1);
		return x >> 1;
	};

	bits.seekg(0, std::ios::end);
	int size = bits.tellg();
	bits.seekg(0, std::ios::beg);
	auto remaining = [&]() {
		return size - bits.tellg();
	};

	nlohmann::json obj;
	nlohmann::json result;

	while (remaining() >= 15) {
		require(1);
		unsigned int n = grab(2);
		if (n == 0b00) { // number
			n = grab(12);
			obj = n;
		}
		else if (n == 0b01) { // Null
			obj = nullptr;
		}
		else if (n == 0b10) { // bool
			n = grab(1);
			if (n == 1)
				obj = true;
			else
			obj = false;
		}
		else if (n == 0b11) { // string
			n = grab(6);
			obj = std::string(1, (char)(n + 0x20));
		}

		n = grab(1);
		if (n == 0) {
			result = nlohmann::json::array({ result, obj });
		} else if (n == 1) {
			result = {
				{ obj.dump(), result},
			};
		}
	}
	
	{
		require(result.is_array());
		require(result.size() == 2);
		obj = result[1];
		result = result[0];
		require(obj.is_string());
		require(obj.get<std::string>()[0] == 'K');
		require(result.is_object());
		require(result.size() == 1);
		obj = nlohmann::json::parse(result.items().begin().key());
		result = result.items().begin().value();
		require(obj.is_null());
		require(result.is_object());
		require(result.size() == 1);
		obj = nlohmann::json::parse(result.items().begin().key());
		result = result.items().begin().value();
		require(obj.is_string());
		require(obj.get<std::string>()[0] == '6');
		require(result.is_array());
		require(result.size() == 2);
		obj = result[1];
		result = result[0];
		require(obj.is_null());
		require(result.is_array());
		require(result.size() == 2);
		obj = result[1];
		result = result[0];
		require(obj.is_number_integer());
		require(obj.get<int>() == ExtremelySpookyFunction(1609));
		require(result.is_object());
		require(result.size() == 1);
		obj = nlohmann::json::parse(result.items().begin().key());
		result = result.items().begin().value();
		require(obj.is_number_integer());
		require(obj.get<int>() == ExtremelySpookyFunction(1390));
		require(result.is_object());
		require(result.size() == 1);
		obj = nlohmann::json::parse(result.items().begin().key());
		result = result.items().begin().value();
		require(obj.is_number_integer());
		require(obj.get<int>() == ExtremelySpookyFunction(2175));
		require(result.is_array());
		require(result.size() == 2);
		obj = result[1];
		result = result[0];
		require(obj.is_number_integer());
		require(obj.get<int>() == ExtremelySpookyFunction(1665));
		require(result.is_object());
		require(result.size() == 1);
		obj = nlohmann::json::parse(result.items().begin().key());
		result = result.items().begin().value();
		require(obj.is_boolean());
		require(obj.get<bool>() == ExtremelySpookyFunction2(false));
		require(result.is_array());
		require(result.size() == 2);
		obj = result[1];
		result = result[0];
		require(obj.is_string());
		require(obj.get<std::string>()[0] == 'L');
		require(result.is_array());
		require(result.size() == 2);
		obj = result[1];
		result = result[0];
		require(obj.is_null());
		require(result.is_object());
		require(result.size() == 1);
		obj = nlohmann::json::parse(result.items().begin().key());
		result = result.items().begin().value();
		require(obj.is_number_integer());
		require(obj.get<int>() == ExtremelySpookyFunction(2286));
		require(result.is_array());
		require(result.size() == 2);
		obj = result[1];
		result = result[0];
		require(obj.is_boolean());
		require(obj.get<bool>() == ExtremelySpookyFunction2(false));
		require(result.is_array());
		require(result.size() == 2);
		obj = result[1];
		result = result[0];
		require(obj.is_null());
		require(result.is_array());
		require(result.size() == 2);
		obj = result[1];
		result = result[0];
		require(obj.is_number_integer());
		require(obj.get<int>() == ExtremelySpookyFunction(758));
		require(result.is_object());
		require(result.size() == 1);
		obj = nlohmann::json::parse(result.items().begin().key());
		result = result.items().begin().value();
		require(obj.is_string());
		require(obj.get<std::string>()[0] == '$');
		require(result.is_array());
		require(result.size() == 2);
		obj = result[1];
		result = result[0];
		require(obj.is_null());
		require(result.is_array());
		require(result.size() == 2);
		obj = result[1];
		result = result[0];
		require(obj.is_null());
		require(result.is_object());
		require(result.size() == 1);
		obj = nlohmann::json::parse(result.items().begin().key());
		result = result.items().begin().value();
		require(obj.is_string());
		require(obj.get<std::string>()[0] == 'J');
		require(result.is_array());
		require(result.size() == 2);
		obj = result[1];
		result = result[0];
		require(obj.is_number_integer());
		require(obj.get<int>() == ExtremelySpookyFunction(1734));
		require(result.is_array());
		require(result.size() == 2);
		obj = result[1];
		result = result[0];
		require(obj.is_number_integer());
		require(obj.get<int>() == ExtremelySpookyFunction(3484));
		require(result.is_object());
		require(result.size() == 1);
		obj = nlohmann::json::parse(result.items().begin().key());
		result = result.items().begin().value();
		require(obj.is_string());
		require(obj.get<std::string>()[0] == '2');
		require(result.is_object());
		require(result.size() == 1);
		obj = nlohmann::json::parse(result.items().begin().key());
		result = result.items().begin().value();
		require(obj.is_string());
		require(obj.get<std::string>()[0] == 'L');
		require(result.is_object());
		require(result.size() == 1);
		obj = nlohmann::json::parse(result.items().begin().key());
		result = result.items().begin().value();
		require(obj.is_boolean());
		require(obj.get<bool>() == ExtremelySpookyFunction2(true));
		require(result.is_array());
		require(result.size() == 2);
		obj = result[1];
		result = result[0];
		require(obj.is_boolean());
		require(obj.get<bool>() == ExtremelySpookyFunction2(false));
		require(result.is_object());
		require(result.size() == 1);
		obj = nlohmann::json::parse(result.items().begin().key());
		result = result.items().begin().value();
		require(obj.is_null());
		require(result.is_array());
		require(result.size() == 2);
		obj = result[1];
		result = result[0];
		require(obj.is_null());
		require(result.is_array());
		require(result.size() == 2);
		obj = result[1];
		result = result[0];
		require(obj.is_number_integer());
		require(obj.get<int>() == ExtremelySpookyFunction(304));
		require(result.is_array());
		require(result.size() == 2);
		obj = result[1];
		result = result[0];
		require(obj.is_number_integer());
		require(obj.get<int>() == ExtremelySpookyFunction(576));
		require(result.is_array());
		require(result.size() == 2);
		obj = result[1];
		result = result[0];
		require(obj.is_boolean());
		require(obj.get<bool>() == ExtremelySpookyFunction2(true));
		require(result.is_object());
		require(result.size() == 1);
		obj = nlohmann::json::parse(result.items().begin().key());
		result = result.items().begin().value();
		require(obj.is_null());
		require(result.is_object());
		require(result.size() == 1);
		obj = nlohmann::json::parse(result.items().begin().key());
		result = result.items().begin().value();
		require(obj.is_string());
		require(obj.get<std::string>()[0] == '2');
		require(result.is_object());
		require(result.size() == 1);
		obj = nlohmann::json::parse(result.items().begin().key());
		result = result.items().begin().value();
		require(obj.is_number_integer());
		require(obj.get<int>() == ExtremelySpookyFunction(1144));
		require(result.is_object());
		require(result.size() == 1);
		obj = nlohmann::json::parse(result.items().begin().key());
		result = result.items().begin().value();
		require(obj.is_string());
		require(obj.get<std::string>()[0] == '.');
		require(result.is_object());
		require(result.size() == 1);
		obj = nlohmann::json::parse(result.items().begin().key());
		result = result.items().begin().value();
		require(obj.is_boolean());
		require(obj.get<bool>() == ExtremelySpookyFunction2(false));
		require(result.is_object());
		require(result.size() == 1);
		obj = nlohmann::json::parse(result.items().begin().key());
		result = result.items().begin().value();
		require(obj.is_string());
		require(obj.get<std::string>()[0] == '4');
		require(result.is_array());
		require(result.size() == 2);
		obj = result[1];
		result = result[0];
		require(obj.is_null());
		require(result.is_object());
		require(result.size() == 1);
		obj = nlohmann::json::parse(result.items().begin().key());
		result = result.items().begin().value();
		require(obj.is_number_integer());
		require(obj.get<int>() == ExtremelySpookyFunction(2987));
		require(result.is_array());
		require(result.size() == 2);
		obj = result[1];
		result = result[0];
		require(obj.is_boolean());
		require(obj.get<bool>() == ExtremelySpookyFunction2(false));
		require(result.is_object());
		require(result.size() == 1);
		obj = nlohmann::json::parse(result.items().begin().key());
		result = result.items().begin().value();
		require(obj.is_number_integer());
		require(obj.get<int>() == ExtremelySpookyFunction(1388));
		require(result.is_object());
		require(result.size() == 1);
		obj = nlohmann::json::parse(result.items().begin().key());
		result = result.items().begin().value();
		require(obj.is_boolean());
		require(obj.get<bool>() == ExtremelySpookyFunction2(false));
		require(result.is_object());
		require(result.size() == 1);
		obj = nlohmann::json::parse(result.items().begin().key());
		result = result.items().begin().value();
		require(obj.is_null());
		require(result.is_array());
		require(result.size() == 2);
		obj = result[1];
		result = result[0];
		require(obj.is_string());
		require(obj.get<std::string>()[0] == '[');
		require(result.is_object());
		require(result.size() == 1);
		obj = nlohmann::json::parse(result.items().begin().key());
		result = result.items().begin().value();
		require(obj.is_number_integer());
		require(obj.get<int>() == ExtremelySpookyFunction(3843));
		require(result.is_array());
		require(result.size() == 2);
		obj = result[1];
		result = result[0];
		require(obj.is_null());
		require(result.is_object());
		require(result.size() == 1);
		obj = nlohmann::json::parse(result.items().begin().key());
		result = result.items().begin().value();
		require(obj.is_null());
		require(result.is_object());
		require(result.size() == 1);
		obj = nlohmann::json::parse(result.items().begin().key());
		result = result.items().begin().value();
		require(obj.is_string());
		require(obj.get<std::string>()[0] == 'Z');
		require(result.is_object());
		require(result.size() == 1);
		obj = nlohmann::json::parse(result.items().begin().key());
		result = result.items().begin().value();
		require(obj.is_string());
		require(obj.get<std::string>()[0] == ',');
		require(result.is_object());
		require(result.size() == 1);
		obj = nlohmann::json::parse(result.items().begin().key());
		result = result.items().begin().value();
		require(obj.is_null());
		require(result.is_object());
		require(result.size() == 1);
		obj = nlohmann::json::parse(result.items().begin().key());
		result = result.items().begin().value();
		require(obj.is_null());
		require(result.is_array());
		require(result.size() == 2);
		obj = result[1];
		result = result[0];
		require(obj.is_null());
		require(result.is_array());
		require(result.size() == 2);
		obj = result[1];
		result = result[0];
		require(obj.is_number_integer());
		require(obj.get<int>() == ExtremelySpookyFunction(2486));
		require(result.is_array());
		require(result.size() == 2);
		obj = result[1];
		result = result[0];
		require(obj.is_string());
		require(obj.get<std::string>()[0] == ':');
		require(result.is_array());
		require(result.size() == 2);
		obj = result[1];
		result = result[0];
		require(obj.is_boolean());
		require(obj.get<bool>() == ExtremelySpookyFunction2(false));
		require(result.is_array());
		require(result.size() == 2);
		obj = result[1];
		result = result[0];
		require(obj.is_null());
		require(result.is_object());
		require(result.size() == 1);
		obj = nlohmann::json::parse(result.items().begin().key());
		result = result.items().begin().value();
		require(obj.is_string());
		require(obj.get<std::string>()[0] == ';');
		require(result.is_object());
		require(result.size() == 1);
		obj = nlohmann::json::parse(result.items().begin().key());
		result = result.items().begin().value();
		require(obj.is_boolean());
		require(obj.get<bool>() == ExtremelySpookyFunction2(true));
		require(result.is_object());
		require(result.size() == 1);
		obj = nlohmann::json::parse(result.items().begin().key());
		result = result.items().begin().value();
		require(obj.is_null());
		require(result.is_object());
		require(result.size() == 1);
		obj = nlohmann::json::parse(result.items().begin().key());
		result = result.items().begin().value();
		require(obj.is_null());
		require(result.is_object());
		require(result.size() == 1);
		obj = nlohmann::json::parse(result.items().begin().key());
		result = result.items().begin().value();
		require(obj.is_null());
		require(result.is_array());
		require(result.size() == 2);
		obj = result[1];
		result = result[0];
		require(obj.is_string());
		require(obj.get<std::string>()[0] == '6');
		require(result.is_object());
		require(result.size() == 1);
		obj = nlohmann::json::parse(result.items().begin().key());
		result = result.items().begin().value();
		require(obj.is_boolean());
		require(obj.get<bool>() == ExtremelySpookyFunction2(false));
		require(result.is_object());
		require(result.size() == 1);
		obj = nlohmann::json::parse(result.items().begin().key());
		result = result.items().begin().value();
		require(obj.is_null());
		require(result.is_array());
		require(result.size() == 2);
		obj = result[1];
		result = result[0];
		require(obj.is_number_integer());
		require(obj.get<int>() == ExtremelySpookyFunction(3627));
		require(result.is_array());
		require(result.size() == 2);
		obj = result[1];
		result = result[0];
		require(obj.is_boolean());
		require(obj.get<bool>() == ExtremelySpookyFunction2(true));
		require(result.is_object());
		require(result.size() == 1);
		obj = nlohmann::json::parse(result.items().begin().key());
		result = result.items().begin().value();
		require(obj.is_boolean());
		require(obj.get<bool>() == ExtremelySpookyFunction2(false));
		require(result.is_object());
		require(result.size() == 1);
		obj = nlohmann::json::parse(result.items().begin().key());
		result = result.items().begin().value();
		require(obj.is_null());
		require(result.is_array());
		require(result.size() == 2);
		obj = result[1];
		result = result[0];
		require(obj.is_null());
		require(result.is_object());
		require(result.size() == 1);
		obj = nlohmann::json::parse(result.items().begin().key());
		result = result.items().begin().value();
		require(obj.is_null());
		require(result.is_array());
		require(result.size() == 2);
		obj = result[1];
		result = result[0];
		require(obj.is_string());
		require(obj.get<std::string>()[0] == '>');
		require(result.is_object());
		require(result.size() == 1);
		obj = nlohmann::json::parse(result.items().begin().key());
		result = result.items().begin().value();
		require(obj.is_string());
		require(obj.get<std::string>()[0] == 'T');
		require(result.is_object());
		require(result.size() == 1);
		obj = nlohmann::json::parse(result.items().begin().key());
		result = result.items().begin().value();
		require(obj.is_null());
		require(result.is_object());
		require(result.size() == 1);
		obj = nlohmann::json::parse(result.items().begin().key());
		result = result.items().begin().value();
		require(obj.is_boolean());
		require(obj.get<bool>() == ExtremelySpookyFunction2(true));
		require(result.is_object());
		require(result.size() == 1);
		obj = nlohmann::json::parse(result.items().begin().key());
		result = result.items().begin().value();
		require(obj.is_string());
		require(obj.get<std::string>()[0] == '<');
		require(result.is_array());
		require(result.size() == 2);
		obj = result[1];
		result = result[0];
		require(obj.is_null());
		require(result.is_object());
		require(result.size() == 1);
		obj = nlohmann::json::parse(result.items().begin().key());
		result = result.items().begin().value();
		require(obj.is_null());
		require(result.is_object());
		require(result.size() == 1);
		obj = nlohmann::json::parse(result.items().begin().key());
		result = result.items().begin().value();
		require(obj.is_string());
		require(obj.get<std::string>()[0] == 'Z');
		require(result.is_object());
		require(result.size() == 1);
		obj = nlohmann::json::parse(result.items().begin().key());
		result = result.items().begin().value();
		require(obj.is_string());
		require(obj.get<std::string>()[0] == ',');
		require(result.is_object());
		require(result.size() == 1);
		obj = nlohmann::json::parse(result.items().begin().key());
		result = result.items().begin().value();
		require(obj.is_null());
		require(result.is_object());
		require(result.size() == 1);
		obj = nlohmann::json::parse(result.items().begin().key());
		result = result.items().begin().value();
		require(obj.is_boolean());
		require(obj.get<bool>() == ExtremelySpookyFunction2(true));
		require(result.is_array());
		require(result.size() == 2);
		obj = result[1];
		result = result[0];
		require(obj.is_string());
		require(obj.get<std::string>()[0] == 'L');
		require(result.is_array());
		require(result.size() == 2);
		obj = result[1];
		result = result[0];
		require(obj.is_number_integer());
		require(obj.get<int>() == ExtremelySpookyFunction(3131));
		require(result.is_array());
		require(result.size() == 2);
		obj = result[1];
		result = result[0];
		require(obj.is_string());
		require(obj.get<std::string>()[0] == 'M');
		require(result.is_object());
		require(result.size() == 1);
		obj = nlohmann::json::parse(result.items().begin().key());
		result = result.items().begin().value();
		require(obj.is_number_integer());
		require(obj.get<int>() == ExtremelySpookyFunction(2528));
		require(result.is_array());
		require(result.size() == 2);
		obj = result[1];
		result = result[0];
		require(obj.is_null());
		require(result.is_object());
		require(result.size() == 1);
		obj = nlohmann::json::parse(result.items().begin().key());
		result = result.items().begin().value();
		require(obj.is_string());
		require(obj.get<std::string>()[0] == ')');
		require(result.is_object());
		require(result.size() == 1);
		obj = nlohmann::json::parse(result.items().begin().key());
		result = result.items().begin().value();
		require(obj.is_number_integer());
		require(obj.get<int>() == ExtremelySpookyFunction(2090));
		require(result.is_object());
		require(result.size() == 1);
		obj = nlohmann::json::parse(result.items().begin().key());
		result = result.items().begin().value();
		require(obj.is_string());
		require(obj.get<std::string>()[0] == 'D');
		require(result.is_object());
		require(result.size() == 1);
		obj = nlohmann::json::parse(result.items().begin().key());
		result = result.items().begin().value();
		require(obj.is_null());
		require(result.is_array());
		require(result.size() == 2);
		obj = result[1];
		result = result[0];
		require(obj.is_string());
		require(obj.get<std::string>()[0] == '&');
		require(result.is_array());
		require(result.size() == 2);
		obj = result[1];
		result = result[0];
		require(obj.is_string());
		require(obj.get<std::string>()[0] == '\\');
		require(result.is_array());
		require(result.size() == 2);
		obj = result[1];
		result = result[0];
		require(obj.is_null());
		require(result.is_object());
		require(result.size() == 1);
		obj = nlohmann::json::parse(result.items().begin().key());
		result = result.items().begin().value();
		require(obj.is_number_integer());
		require(obj.get<int>() == ExtremelySpookyFunction(750));
		require(result.is_object());
		require(result.size() == 1);
		obj = nlohmann::json::parse(result.items().begin().key());
		result = result.items().begin().value();
		require(obj.is_number_integer());
		require(obj.get<int>() == ExtremelySpookyFunction(2995));
		require(result.is_object());
		require(result.size() == 1);
		obj = nlohmann::json::parse(result.items().begin().key());
		result = result.items().begin().value();
		require(obj.is_boolean());
		require(obj.get<bool>() == ExtremelySpookyFunction2(false));
	}

	puts("You win");
	fflush(stdout);
	TerminateProcess(GetCurrentProcess(), 0);

	return 0;
}

// Malloc memes
std::unordered_map<void*, BYTE> stolen_bytes(32);
std::unordered_map<uint64_t, void*> fake_heap(1000000);
uint64_t fake_heap_ptr = 0x12340000000;

BYTE PatchCode(void* location, BYTE value)
{
	DWORD oldProtect;
	auto ret = VirtualProtect(location, 1, PAGE_READWRITE, &oldProtect);
	assert(ret);
	auto ptr = (BYTE*)location;
	BYTE oldValue = *ptr;
	*ptr = value;
	DWORD dontCare;
	ret = VirtualProtect(location, 2, oldProtect, &dontCare);
	assert(ret);
	FlushInstructionCache(GetCurrentProcess(), location, 1);
	return oldValue;
}

void PlaceHook(void* location)
{
	stolen_bytes[location] = PatchCode(location, 0xF4); // HLT
}

void UndoHook(void* location)
{
	PatchCode(location, stolen_bytes[location]);
}

// Puts() that will NOT alloc. (stdio may cause allocation due to buffering)
__forceinline void safe_puts(const char* s)
{
	WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), s, strlen(s), NULL, NULL);
	WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), "\n", 1, NULL, NULL);
}

// PAGE HEAP??? SO SECURE!!!!
void HookCallback(void* ip, PCONTEXT context)
{
	if (ip == HeapAlloc)
	{
		// Fake alloc
		// safe_puts("FakeAlloc");
		auto size = (int64_t) context->R8;
		uint64_t fake_alloc = fake_heap_ptr;
		while (size > 0)
		{
			void* alloced_page = VirtualAlloc(0, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
			assert(alloced_page);
			size -= 0x1000;
			anti_debug();
			fake_heap[fake_heap_ptr] = alloced_page;
			fake_heap_ptr += 0x1000;
		}
		context->Rax = fake_heap_ptr;
	}
	else if (ip == HeapFree)
	{
		anti_debug();
		// safe_puts("FakeFree");
		// Freeing memory is for losers.
	}
	else if (ip == ExtremelySpookyFunction)
	{
		static int global_spooky = 0xabc;
		anti_vm();
		global_spooky += 0x123;
		anti_debug();
		context->Rcx = (context->Rcx - global_spooky) & 0xfff;
	}
	else if (ip == ExtremelySpookyFunction2)
	{
		context->Rcx = !context->Rcx;
		anti_debug();
	}
}

LONG CALLBACK MyVeh2(struct _EXCEPTION_POINTERS* ExceptionInfo)
{
	static bool hooks_applied = 0;
	static void* SingleStepHook = nullptr;
	static void* SingleStepFakeHeap = nullptr;

	if (!crt_is_ready)
	{
		return EXCEPTION_CONTINUE_SEARCH;
	}

	auto ip = (void*) ExceptionInfo->ContextRecord->Rip;
	if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_PRIV_INSTRUCTION)
	{
		BYTE fault_instr[16];
		memcpy(fault_instr, ip, sizeof(fault_instr));

		// HLT
		if (!memcmp(fault_instr, "\xF4", 1))
		{
			// printf("HLT at %p\n", ip);
			if (!hooks_applied)
			{
				hooks_applied = 1;
				PlaceHook(HeapAlloc);
				PlaceHook(HeapFree);
				PlaceHook(ExtremelySpookyFunction);
				PlaceHook(ExtremelySpookyFunction2);
				ExceptionInfo->ContextRecord->Rip += 1;
				// safe_puts("  Placed hooks and continuing");
				return EXCEPTION_CONTINUE_EXECUTION;
			}

			if (stolen_bytes.find(ip) != stolen_bytes.end())
			{
				// safe_puts("  Unhooking and single stepping");
				UndoHook(ip);
				ExceptionInfo->ContextRecord->EFlags |= 0x0100; // single step
				SingleStepHook = ip;

				HookCallback(ip, ExceptionInfo->ContextRecord);

				return EXCEPTION_CONTINUE_EXECUTION;
			}

			ExceptionInfo->ContextRecord->Rip += 1;
			// safe_puts("  Simply skipping over and continuing");
			return EXCEPTION_CONTINUE_EXECUTION;
		}
	}

	anti_debug();

	if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_SINGLE_STEP)
	{
		// printf("Single step at %p\n", ip);
		if (SingleStepHook)
		{
			assert(stolen_bytes.find(SingleStepHook) != stolen_bytes.end());
			// printf("  Re-hooking %p\n", SingleStepLocation);
			PlaceHook(SingleStepHook);
			SingleStepHook = 0;
			return EXCEPTION_CONTINUE_EXECUTION;
		}
		if (SingleStepFakeHeap)
		{
			// printf("Free fake page %p\n", SingleStepFakeHeap);
			auto fault_page = (uint64_t)SingleStepFakeHeap;
			assert (fake_heap.find(fault_page) != fake_heap.end());
			// write-back results
			CryptPage(SingleStepFakeHeap);
			memcpy(fake_heap[fault_page], SingleStepFakeHeap, 0x1000);
			auto succ = VirtualFree(SingleStepFakeHeap, 0x1000, MEM_RELEASE);
			assert(succ);
			SingleStepFakeHeap = 0;
			return EXCEPTION_CONTINUE_EXECUTION;
		}
	}
	
	if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION)
	{
		anti_vm();

		auto fault_addr = (void*)ExceptionInfo->ExceptionRecord->ExceptionInformation[1];
		auto fault_page = (uint64_t)fault_addr & ~0xfff;
		if (fake_heap.find(fault_page) != fake_heap.end())
		{
			// printf("Fake heap access to %p\n", fault_addr);
			SingleStepFakeHeap = (void*) fault_page;
			// LOL FAULT THIS PAGE IN FOR ONE INSTRUCTION THEN PAGE IT BACK OUT. BYE
			void* alloced_page = VirtualAlloc(SingleStepFakeHeap, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
			assert (alloced_page == SingleStepFakeHeap);
			memcpy(alloced_page, fake_heap[fault_page], 0x1000);
			CryptPage(alloced_page);
			ExceptionInfo->ContextRecord->EFlags |= 0x0100; // single step
		}
	}

	return EXCEPTION_CONTINUE_SEARCH;
}

// Memory obfuscation nonsense
#pragma code_seg(".bingus")

constexpr void* ToPage(void* x)
{
	return (void*)(((uintptr_t)x) & ~0xfff);
}

// Real abort.
__forceinline void __declspec(noreturn) abort()
{
	TerminateProcess(GetCurrentProcess(), 1);
}

void ProcessCodePage(void* fault_page, DWORD newProtect)
{
	DWORD oldProtect;
	auto ret = VirtualProtect(fault_page, 0x1000, PAGE_READWRITE, &oldProtect);
	assert(ret);

	CryptPage(fault_page);

	ret = VirtualProtect(fault_page, 0x1000, newProtect, &oldProtect);
	assert(ret);

	FlushInstructionCache(GetCurrentProcess(), fault_page, 0x1000);
}

volatile ULONGLONG reentrancy_guard;

// Simple non-recursive RAII lock guard.
class LockGuard
{
	volatile ULONGLONG * mutex;
	LockGuard() = delete;
	LockGuard(const LockGuard&) = delete;
	LockGuard(const LockGuard&&) = delete;
	LockGuard& operator=(const LockGuard&) = delete;
	LockGuard& operator=(const LockGuard&&) = delete;
public:
	__forceinline LockGuard(volatile ULONGLONG* mutex) : mutex(mutex)
	{
		while (InterlockedCompareExchange(mutex, 1, 0));
	};
	__forceinline ~LockGuard()
	{
		assert(InterlockedAnd(mutex, 0));
	}
};

LONG CALLBACK MyVeh(struct _EXCEPTION_POINTERS* ExceptionInfo)
{
	JustALilFYI_IfYoureReadingThisYouAreGettingTrolled_Lmao();

	auto addr = ExceptionInfo->ExceptionRecord->ExceptionAddress;
	if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION)
	{
		auto fault_type = (ULONG)ExceptionInfo->ExceptionRecord->ExceptionInformation[0];
		auto fault_addr = (void*)ExceptionInfo->ExceptionRecord->ExceptionInformation[1];
		if (fault_type == 8 || fault_type == 0) // NX failure or Read failure
		{
			// Take the lock.
			LockGuard lock(&reentrancy_guard);

			auto fault_page = ToPage(fault_addr);
			MEMORY_BASIC_INFORMATION page_info;
			auto ret = VirtualQuery(fault_page, &page_info, sizeof(page_info));
			if (!ret) abort();
			if (page_info.RegionSize < 0x1000) abort();
			if (page_info.BaseAddress != fault_page) abort();
			if (page_info.State == MEM_COMMIT)
			{
				DWORD newProtect = 0;
				if (page_info.AllocationProtect == PAGE_NOACCESS || page_info.AllocationProtect == PAGE_EXECUTE_WRITECOPY)
					newProtect = PAGE_EXECUTE_READ;
				if (newProtect)
				{
					ProcessCodePage(fault_page, newProtect);
					return EXCEPTION_CONTINUE_EXECUTION;
				}
			}
		}
	}
	return EXCEPTION_CONTINUE_SEARCH;
}

__declspec(noinline) __declspec(dllexport) volatile void JustALilFYI_IfYoureReadingThisYouAreGettingTrolled_Lmao() {}

extern "C" __declspec(dllexport) void ItsNerfOrNothing()
{
	AddVectoredExceptionHandler(TRUE, MyVeh2);
	AddVectoredExceptionHandler(TRUE, MyVeh);
	(*offset<void(**)()>(GetModuleHandle(NULL), 0x369))();
}
