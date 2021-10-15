#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <functional>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <optional>
#include <span>

#include <Windows.h>

#define assert(what) { if (!(what)) abort(); }

template <typename T>
__forceinline T offset(void* base, ptrdiff_t offset)
{
	return reinterpret_cast<T>(((uintptr_t)base) + offset);
}

BYTE* read_file(const char* file, size_t* len_out = nullptr)
{
	auto f = fopen(file, "rb");
	fseek(f, 0, SEEK_END);
	size_t len = ftell(f);
	fseek(f, 0, SEEK_SET);
	auto data = (BYTE*)malloc(len);
	assert(data);
	for (size_t n = 0; n < len; n += fread(data + n, 1, len - n, f));
	fclose(f);
	if (len_out) *len_out = len;
	return data;
}

void write_file(const char* file, BYTE* data, size_t len)
{
	auto f = fopen(file, "wb");
	for (size_t n = 0; n < len; n += fwrite(data + n, 1, len - n, f));
	fclose(f);
}

struct section_t
{
	std::string name;
	std::span<BYTE> data;
	uintptr_t va;
	DWORD rva;
	DWORD fo;
	size_t size;
	PIMAGE_SECTION_HEADER hdr;
};

typedef std::function<void(const section_t& section)> pe_section_callback_t;

struct pefile
{
	// todo: unique_ptr and make it own the raw data
	
	PIMAGE_DOS_HEADER image;
	PIMAGE_NT_HEADERS pe;
	WORD num_sections;
	ULONGLONG image_base;
	DWORD oep;

	std::vector<section_t> sections;
	std::unordered_map<std::string, DWORD> exports; // name to rva

	pefile(void* pefile)
	{
		image = (PIMAGE_DOS_HEADER)pefile;
		pe = offset<PIMAGE_NT_HEADERS>(image, image->e_lfanew);
		num_sections = pe->FileHeader.NumberOfSections;
		image_base = pe->OptionalHeader.ImageBase;
		oep = pe->OptionalHeader.AddressOfEntryPoint;

		// parse sections
		auto hdr = offset<PIMAGE_SECTION_HEADER>(&pe->OptionalHeader, pe->FileHeader.SizeOfOptionalHeader);
		for (int i = 0; i < num_sections; i++, hdr++) {
			auto fo = hdr->PointerToRawData;
			auto src = offset<BYTE*>(image, fo);
			auto size = hdr->SizeOfRawData;
			auto rva = hdr->VirtualAddress;
			auto va = image_base + rva;

			auto name = std::string((char*)hdr->Name, strnlen((char*)hdr->Name, 8));

			printf("section %s (fo %x, va 0x%llx, size 0x%x)\n", name.c_str(), fo, va, size);
			sections.push_back({name, { src, size }, va, rva, fo, size, hdr});
		}

		// parse exports
		IMAGE_DATA_DIRECTORY exports_dir_info = pe->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
		printf("exports dir rva = %x\n", exports_dir_info.VirtualAddress);
		assert(exports_dir_info.VirtualAddress);

		auto exports_dir = rva_offset<PIMAGE_EXPORT_DIRECTORY>(exports_dir_info.VirtualAddress);
		auto funcs = rva_offset<DWORD*>(exports_dir->AddressOfFunctions);
		auto names = rva_offset<DWORD*>(exports_dir->AddressOfNames);
		printf("num exports: %d\n", exports_dir->NumberOfFunctions);
		for (DWORD i = 0; i < exports_dir->NumberOfFunctions; i++)
		{
			auto func_rva = funcs[i];
			std::string func_name = rva_offset<char*>(names[i]);
			printf("export: %s at 0x%x\n", func_name.c_str(), func_rva);
			exports[func_name] = func_rva;
		}
	}

	std::optional<section_t> find_section(std::string name)
	{
		for (auto const& section : sections)
			if (section.name == name)
				return section;
		return {};
	}

	std::optional<uint64_t> rva_to_fo(uint64_t rva)
	{
		for (auto const& section : sections)
			if (section.rva <= rva && rva < section.rva + section.size)
				return section.fo + rva - section.rva;
		return {};
	}

	template <typename T>
	__forceinline T rva_offset(uint64_t rva)
	{
		return fo_offset<T>(rva_to_fo(rva).value());
	}

	template <typename T>
	__forceinline T fo_offset(uint64_t fo)
	{
		return ::offset<T>(image, fo);
	}

	std::optional<DWORD> find_export(std::string name)
	{
		if (exports.find(name) == exports.end())
			return {};
		else
			return exports[name];
	}
};

void process_text(section_t section)
{
	printf("Processing .text , as if it were mapped at 0x%llx\n", section.va);
	uint64_t x = 0;
	for (uint64_t i = 0, va = section.va; i < section.size; i += 8, va += 8, x++)
	{
		*reinterpret_cast<uint64_t*>(&section.data[i]) ^= va ^ ~(x & 0x1ff);
	}
	// Remove any rwx permission
	section.hdr->Characteristics &= ~IMAGE_SCN_MEM_EXECUTE;
	section.hdr->Characteristics &= ~IMAGE_SCN_MEM_WRITE;
	section.hdr->Characteristics &= ~IMAGE_SCN_MEM_READ;
	puts("Done");
}

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		puts("No filename given!");
		exit(1);
	}

	auto filename = std::string(argv[1]);

	size_t filelen;
	auto raw_data = read_file(filename.c_str(), &filelen);

	auto my_pe = pefile(raw_data);

	process_text(my_pe.find_section(".text").value());

	// rewrite entrypoint
	auto oep = my_pe.image_base + my_pe.oep;
	auto ItsNerfOrNothing = my_pe.find_export("ItsNerfOrNothing").value();
	my_pe.pe->OptionalHeader.AddressOfEntryPoint = ItsNerfOrNothing;
	*my_pe.fo_offset<uint64_t*>(0x369) = oep;

	auto out_file = filename + ".out.exe";
	write_file(out_file.c_str(), raw_data, filelen);

	puts("OK");
}
