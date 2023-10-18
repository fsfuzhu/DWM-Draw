#pragma once
/*

这个库是可以在无crt的情况下使用的
参考了https://github.com/JustasMasiulis/lazy_importer

可以解决系统库的导出函数转发问题


*/
















#include <windows.h>
#include <type_traits>
#pragma push_macro("INLINE")
#pragma push_macro("TOLOWER")
#undef INLINE
#undef TOLOWER
#define INLINE __forceinline

#define CASE_INSENSITIVE
#ifdef CASE_INSENSITIVE
#define TOLOWER(c) (c >= 'A' && c <= 'Z' ? (c | (1 << 5)) : c)
#else
#define TOLOWER(c) (c)
#endif

namespace misc {
    namespace importer {
        namespace detail {
            namespace win {

                struct LIST_ENTRY_T {
                    const char* Flink;
                    const char* Blink;
                };

                struct UNICODE_STRING_T {
                    unsigned short Length;
                    unsigned short MaximumLength;
                    wchar_t* Buffer;
                };

                struct PEB_LDR_DATA_T {
                    unsigned long Length;
                    unsigned long Initialized;
                    const char* SsHandle;
                    LIST_ENTRY_T  InLoadOrderModuleList;
                };

                struct PEB_T {
                    unsigned char   Reserved1[2];
                    unsigned char   BeingDebugged;
                    unsigned char   Reserved2[1];
                    const char* Reserved3[2];
                    PEB_LDR_DATA_T* Ldr;
                };

                struct LDR_DATA_TABLE_ENTRY_T {
                    LIST_ENTRY_T InLoadOrderLinks;
                    LIST_ENTRY_T InMemoryOrderLinks;
                    LIST_ENTRY_T InInitializationOrderLinks;
                    const char* DllBase;
                    const char* EntryPoint;
                    union {
                        unsigned long SizeOfImage;
                        const char* _dummy;
                    };
                    UNICODE_STRING_T FullDllName;
                    UNICODE_STRING_T BaseDllName;

                    INLINE const LDR_DATA_TABLE_ENTRY_T*
                        load_order_next() const noexcept
                    {
                        return reinterpret_cast<const LDR_DATA_TABLE_ENTRY_T*>(
                            InLoadOrderLinks.Flink);
                    }
                };

                struct IMAGE_DOS_HEADER { // DOS .EXE header
                    unsigned short e_magic; // Magic number
                    unsigned short e_cblp; // Bytes on last page of file
                    unsigned short e_cp; // Pages in file
                    unsigned short e_crlc; // Relocations
                    unsigned short e_cparhdr; // Size of header in paragraphs
                    unsigned short e_minalloc; // Minimum extra paragraphs needed
                    unsigned short e_maxalloc; // Maximum extra paragraphs needed
                    unsigned short e_ss; // Initial (relative) SS value
                    unsigned short e_sp; // Initial SP value
                    unsigned short e_csum; // Checksum
                    unsigned short e_ip; // Initial IP value
                    unsigned short e_cs; // Initial (relative) CS value
                    unsigned short e_lfarlc; // File address of relocation table
                    unsigned short e_ovno; // Overlay number
                    unsigned short e_res[4]; // Reserved words
                    unsigned short e_oemid; // OEM identifier (for e_oeminfo)
                    unsigned short e_oeminfo; // OEM information; e_oemid specific
                    unsigned short e_res2[10]; // Reserved words
                    long           e_lfanew; // File address of new exe header
                };

                struct IMAGE_FILE_HEADER {
                    unsigned short Machine;
                    unsigned short NumberOfSections;
                    unsigned long  TimeDateStamp;
                    unsigned long  PointerToSymbolTable;
                    unsigned long  NumberOfSymbols;
                    unsigned short SizeOfOptionalHeader;
                    unsigned short Characteristics;
                };

                struct IMAGE_EXPORT_DIRECTORY {
                    unsigned long  Characteristics;
                    unsigned long  TimeDateStamp;
                    unsigned short MajorVersion;
                    unsigned short MinorVersion;
                    unsigned long  Name;
                    unsigned long  Base;
                    unsigned long  NumberOfFunctions;
                    unsigned long  NumberOfNames;
                    unsigned long  AddressOfFunctions; // RVA from base of image
                    unsigned long  AddressOfNames; // RVA from base of image
                    unsigned long  AddressOfNameOrdinals; // RVA from base of image
                };

                struct IMAGE_DATA_DIRECTORY {
                    unsigned long VirtualAddress;
                    unsigned long Size;
                };

                struct IMAGE_OPTIONAL_HEADER64 {
                    unsigned short       Magic;
                    unsigned char        MajorLinkerVersion;
                    unsigned char        MinorLinkerVersion;
                    unsigned long        SizeOfCode;
                    unsigned long        SizeOfInitializedData;
                    unsigned long        SizeOfUninitializedData;
                    unsigned long        AddressOfEntryPoint;
                    unsigned long        BaseOfCode;
                    unsigned long long   ImageBase;
                    unsigned long        SectionAlignment;
                    unsigned long        FileAlignment;
                    unsigned short       MajorOperatingSystemVersion;
                    unsigned short       MinorOperatingSystemVersion;
                    unsigned short       MajorImageVersion;
                    unsigned short       MinorImageVersion;
                    unsigned short       MajorSubsystemVersion;
                    unsigned short       MinorSubsystemVersion;
                    unsigned long        Win32VersionValue;
                    unsigned long        SizeOfImage;
                    unsigned long        SizeOfHeaders;
                    unsigned long        CheckSum;
                    unsigned short       Subsystem;
                    unsigned short       DllCharacteristics;
                    unsigned long long   SizeOfStackReserve;
                    unsigned long long   SizeOfStackCommit;
                    unsigned long long   SizeOfHeapReserve;
                    unsigned long long   SizeOfHeapCommit;
                    unsigned long        LoaderFlags;
                    unsigned long        NumberOfRvaAndSizes;
                    IMAGE_DATA_DIRECTORY DataDirectory[16];
                };

                struct IMAGE_OPTIONAL_HEADER32 {
                    unsigned short       Magic;
                    unsigned char        MajorLinkerVersion;
                    unsigned char        MinorLinkerVersion;
                    unsigned long        SizeOfCode;
                    unsigned long        SizeOfInitializedData;
                    unsigned long        SizeOfUninitializedData;
                    unsigned long        AddressOfEntryPoint;
                    unsigned long        BaseOfCode;
                    unsigned long        BaseOfData;
                    unsigned long        ImageBase;
                    unsigned long        SectionAlignment;
                    unsigned long        FileAlignment;
                    unsigned short       MajorOperatingSystemVersion;
                    unsigned short       MinorOperatingSystemVersion;
                    unsigned short       MajorImageVersion;
                    unsigned short       MinorImageVersion;
                    unsigned short       MajorSubsystemVersion;
                    unsigned short       MinorSubsystemVersion;
                    unsigned long        Win32VersionValue;
                    unsigned long        SizeOfImage;
                    unsigned long        SizeOfHeaders;
                    unsigned long        CheckSum;
                    unsigned short       Subsystem;
                    unsigned short       DllCharacteristics;
                    unsigned long        SizeOfStackReserve;
                    unsigned long        SizeOfStackCommit;
                    unsigned long        SizeOfHeapReserve;
                    unsigned long        SizeOfHeapCommit;
                    unsigned long        LoaderFlags;
                    unsigned long        NumberOfRvaAndSizes;
                    IMAGE_DATA_DIRECTORY DataDirectory[16];
                };

                struct IMAGE_NT_HEADERS {
                    unsigned long     Signature;
                    IMAGE_FILE_HEADER FileHeader;
#ifdef _WIN64
                    IMAGE_OPTIONAL_HEADER64 OptionalHeader;
#else
                    IMAGE_OPTIONAL_HEADER32 OptionalHeader;
#endif
                };

            } // namespace win

            struct hash_t {
                using value_type = unsigned long;
                constexpr static value_type         offset = 2166136261;
                constexpr static value_type         prime = 16777619;
                constexpr static unsigned long long prime64 = prime;

                INLINE constexpr static value_type single(value_type value,
                    char c) noexcept
                {
                    return static_cast<hash_t::value_type>(
                        (value ^ TOLOWER(c)) *
                        static_cast<unsigned long long>(prime));
                }
            };

            template<class CharT = char>
            INLINE constexpr hash_t::value_type
                khash(const CharT* str, hash_t::value_type value = hash_t::offset) noexcept
            {
                return (*str ? khash(str + 1, hash_t::single(value, *str)) : value);
            }

            template<class CharT = char>
            INLINE hash_t::value_type hash(const CharT* str) noexcept
            {
                hash_t::value_type value = hash_t::offset;

                for (;;) {
                    char c = *str++;
                    if (!c)
                        return value;
                    value = hash_t::single(value, c);
                }
            }

            INLINE hash_t::value_type hash(
                const win::UNICODE_STRING_T& str) noexcept
            {
                auto       first = str.Buffer;
                const auto last = first + (str.Length / sizeof(wchar_t));
                auto       value = hash_t::offset;
                for (; first != last; ++first)
                    value = hash_t::single(value, static_cast<char>(*first));

                return value;
            }



            INLINE const win::PEB_T* get_peb() noexcept {
#if defined(_WIN64)
                return reinterpret_cast<const win::PEB_T*>(__readgsqword(0x60));
#else
                return reinterpret_cast<const win::PEB_T*>(__readfsdword(0x30));
#endif
            }

            INLINE const win::IMAGE_NT_HEADERS* nt_headers(const char* base) noexcept {
                return reinterpret_cast<const win::IMAGE_NT_HEADERS*>(
                    base + reinterpret_cast<const win::IMAGE_DOS_HEADER*>(base)->e_lfanew);
            }

            INLINE const win::IMAGE_EXPORT_DIRECTORY* image_export_dir(const char* base) noexcept {
                return reinterpret_cast<const win::IMAGE_EXPORT_DIRECTORY*>(
                    base + nt_headers(base)->OptionalHeader.DataDirectory->VirtualAddress);
            }

            INLINE const win::PEB_LDR_DATA_T* get_ldr()
            {
                return reinterpret_cast<const win::PEB_LDR_DATA_T*>(get_peb()->Ldr);
            }

            INLINE const win::LDR_DATA_TABLE_ENTRY_T* get_ldr_data_entry() noexcept
            {
                return reinterpret_cast<const win::LDR_DATA_TABLE_ENTRY_T*>(
                    get_ldr()->InLoadOrderModuleList.Flink);
            }


            struct module_enumerator {
                using value_type = const win::LDR_DATA_TABLE_ENTRY_T;
                value_type* value;
                value_type* head;

                INLINE module_enumerator() noexcept
                    : module_enumerator(get_ldr_data_entry())
                {}

                INLINE module_enumerator(const win::LDR_DATA_TABLE_ENTRY_T* ldr) noexcept
                    : value(ldr->load_order_next()), head(value)
                {}

                INLINE void reset() noexcept {
                    value = head->load_order_next();
                }

                INLINE bool next() noexcept {
                    value = value->load_order_next();
                    return value != head && value->DllBase;
                }
            };

            struct exports_directory {
                const char* _base;
                const win::IMAGE_EXPORT_DIRECTORY* _ied;
                unsigned long                      _ied_size;

            public:
                using size_type = unsigned long;

                INLINE exports_directory(const char* base) noexcept : _base(base)
                {
                    const auto ied_data_dir = nt_headers(base)->OptionalHeader.DataDirectory[0];
                    _ied = reinterpret_cast<const win::IMAGE_EXPORT_DIRECTORY*>(
                        base + ied_data_dir.VirtualAddress);
                    _ied_size = ied_data_dir.Size;
                }

                INLINE explicit operator bool() const noexcept
                {
                    return reinterpret_cast<const char*>(_ied) != _base;
                }

                INLINE size_type size() const noexcept
                {
                    return _ied->NumberOfNames;
                }

                INLINE const char* base() const noexcept { return _base; }
                INLINE const win::IMAGE_EXPORT_DIRECTORY* ied() const noexcept {
                    return _ied;
                }

                INLINE const char* name(size_type index) const noexcept {
                    return reinterpret_cast<const char*>(
                        _base + reinterpret_cast<const unsigned long*>(
                            _base + _ied->AddressOfNames)[index]);
                }

                INLINE const char* address(size_type index) const noexcept {
                    const auto* const rva_table =
                        reinterpret_cast<const unsigned long*>(_base + _ied->AddressOfFunctions);

                    const auto* const ord_table = reinterpret_cast<const unsigned short*>(
                        _base + _ied->AddressOfNameOrdinals);

                    return _base + rva_table[ord_table[index]];
                }

                INLINE bool is_forwarded(const char* export_address) const noexcept {
                    const auto ui_ied = reinterpret_cast<const char*>(_ied);
                    return (export_address > ui_ied && export_address < ui_ied + _ied_size);
                }
            };


            template<class First, class Second>
            struct pair {
                First  first;
                Second second;
            };

            INLINE pair<hash_t::value_type, hash_t::value_type> hash_forwarded(
                const char* str) noexcept
            {
                pair<hash_t::value_type, hash_t::value_type> module_and_function{
                    hash_t::offset, hash_t::offset
                };

                for (; *str != '.'; ++str)
                    module_and_function.first = hash_t::single(module_and_function.first, *str);

                ++str;

                for (; *str; ++str)
                    module_and_function.second = hash_t::single(module_and_function.second, *str);

                return module_and_function;
            }


        };


        INLINE void* find_module(detail::hash_t::value_type hash_value, bool forwarded = false) {

            detail::module_enumerator modules;

            do {
                detail::win::UNICODE_STRING_T dll_name = (modules.value->BaseDllName);


                if (detail::hash(dll_name) == hash_value)
                    return (void*)(modules.value->DllBase);
                if (forwarded && dll_name.Length > 8) {
                    dll_name.Length -= 8;
                    if (detail::hash(dll_name) == hash_value) {
                        return (void*)(modules.value->DllBase);
                    }
                }


            } while (modules.next());

            return 0;
        }


        INLINE void* find_function(void* dllbase, detail::hash_t::value_type hash_value) {

            const detail::exports_directory exports((const char*)dllbase);

            if (exports) {
                auto export_index = exports.size();
                while (export_index--)
                    if (detail::hash(exports.name(export_index)) == hash_value) {
                        void* addr = (void*)(exports.address(export_index));

                        if (exports.is_forwarded((const char*)addr)) {

                            auto hashes = detail::hash_forwarded(reinterpret_cast<const char*>(addr));


                            auto real_module = find_module(hashes.first, true);
                            auto real_function = find_function(real_module, hashes.second);

                            addr = (void*)(real_function);

                        }

                        return addr;
                    }

            }

            return 0;
        }


        INLINE void* find_function(detail::hash_t::value_type hash_value) {

            detail::module_enumerator modules;

            do {

                void* address = find_function((void*)(modules.value->DllBase), hash_value);
                if (address)
                    return address;

            } while (modules.next());

            return 0;
        }



    };

};

#define quick_import_function(moudle_name, function)\
constexpr misc::importer::detail::hash_t::value_type hash_##moudle##function = misc::importer::detail::khash(moudle_name);\
constexpr misc::importer::detail::hash_t::value_type hash_##function         = misc::importer::detail::khash(#function);\
typedef decltype(function) type_##function;\
type_##function * function = (type_##function *)misc::importer::find_function(misc::importer::find_module(hash_##moudle##function),hash_##function)





#define function_ptr(function)\
typedef decltype(function) type_##function##_ptr; \
type_##function##_ptr * function = nullptr;
#define static_function_ptr(function)\
typedef decltype(function) type_##function##_ptr; \
static type_##function##_ptr * function = nullptr;




template <misc::importer::detail::hash_t::value_type moudle_hash, misc::importer::detail::hash_t::value_type function_hash>
struct _get_function_ptr {
    template<typename T>
    operator T() {
        return (T)misc::importer::find_function(misc::importer::find_module(moudle_hash), function_hash);
    }
};



#define get_function_ptr(moudle_name,function)\
_get_function_ptr<misc::importer::detail::khash(moudle_name),misc::importer::detail::khash(#function)>()

#if 0
    extern "C"
    {
#pragma function(memset)
        INLINE void* __cdecl memset(void* dest, int value, size_t num)
        {
            __stosb(static_cast<unsigned char*>(dest),
                static_cast<unsigned char>(value), num);
            return dest;
        }

#pragma function(memcpy)
        INLINE void* __cdecl memcpy(void* dest, const void* src, size_t num)
        {
            __movsb(static_cast<unsigned char*>(dest),
                static_cast<const unsigned char*>(src), num);
            return dest;
        }
    }


   /* void* operator new(size_t n)
    {
        static_function_ptr(HeapAlloc);
        static_function_ptr(GetProcessHeap);

        if(!HeapAlloc)
            HeapAlloc = get_function_ptr("kernel32.dll", HeapAlloc);
        if (!GetProcessHeap)
            GetProcessHeap = get_function_ptr("kernel32.dll", GetProcessHeap);


        return HeapAlloc(GetProcessHeap(), NULL, n);
    }

    void* operator new[](size_t n)
    {
        static_function_ptr(HeapAlloc);
        static_function_ptr(GetProcessHeap);

        if (!HeapAlloc)
            HeapAlloc = get_function_ptr("kernel32.dll", HeapAlloc);
        if (!GetProcessHeap)
            GetProcessHeap = get_function_ptr("kernel32.dll", GetProcessHeap);

        return HeapAlloc(GetProcessHeap(), NULL, n);
    }

    void operator delete(void* p)
    {
        static_function_ptr(HeapFree);
        static_function_ptr(GetProcessHeap);

        if (!HeapFree)
            HeapFree = get_function_ptr("kernel32.dll", HeapFree);
        if (!GetProcessHeap)
            GetProcessHeap = get_function_ptr("kernel32.dll", GetProcessHeap);


        HeapFree(GetProcessHeap(), NULL, p);
    }

    void operator delete[](void* p)
    {
        static_function_ptr(HeapFree);
        static_function_ptr(GetProcessHeap);

        if (!HeapFree)
            HeapFree = get_function_ptr("kernel32.dll", HeapFree);
        if (!GetProcessHeap)
            GetProcessHeap = get_function_ptr("kernel32.dll", GetProcessHeap);

        HeapFree(GetProcessHeap(), NULL, p);
    }*/

#endif // 1



#pragma pop_macro("TOLOWER")
#pragma pop_macro("INLINE")