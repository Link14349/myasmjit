#ifndef MYASMJIT_MASMJIT_H
#define MYASMJIT_MASMJIT_H

#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <check-is-win.h>
#ifdef I_OS_WIN32
#include <Windows.h>
#else
#include <sys/mman.h>
#endif

namespace MAsmJit {
    enum REG8BIT {
        AH = 0xb4, AL = 0xb0, BH = 0xb7, BL = 0xb3, CH = 0xb5, CL = 0xb1, DH = 0xb6, DL = 0xb2,
    };
    enum REG16BIT {
        AX = 0xb8, BX = 0xbb, CX = 0xb9, DX = 0xba,
    };
    enum REG32BIT {
        EAX = 0xb8, EBX = 0xbb, ECX = 0xb9, EDX = 0xba,
    };
    enum REG64BIT {
        RAX = 0xb8, RBX = 0xbb, RCX = 0xb9, RDX = 0xba,
    };
    class MAsmJit {
    public:
#define MAJ_APP machineCodeAdr[machineCodeIndex++]
        typedef void (*JIT_FUNC)();

#ifdef I_OS_WIN32
        MAsmJit(size_t cl = 1024) : codeLength(cl), machineCodeAdr((uint8_t*)VirtualAlloc(NULL, sizeof(codeLength), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE)), machineCodeIndex(0) { }
#else
        MAsmJit(size_t cl = 1024) : codeLength(cl), machineCodeAdr((uint8_t*) mmap(
                NULL,
                codeLength,
                PROT_READ | PROT_WRITE | PROT_EXEC,
                MAP_ANONYMOUS | MAP_PRIVATE,
                0,
                0)), machineCodeIndex(0) {

        }
#endif

#define MAJ_APP_8(source) MAJ_APP = source;
#define MAJ_APP_16(source) { MAJ_APP = source >> 8; \
        MAJ_APP = source & 0xff; }
#define MAJ_APP_32(source) { MAJ_APP = source >> 24; \
        MAJ_APP = (source >> 16) & 0xff; \
        MAJ_APP = (source >> 8) & 0xff; \
        MAJ_APP = source & 0xff; }
#define MAJ_APP_64(source) { MAJ_APP = source >> 56; \
        MAJ_APP = (source >> 48) & 0xff; \
        MAJ_APP = (source >> 40) & 0xff; \
        MAJ_APP = (source >> 32) & 0xff; \
        MAJ_APP = (source >> 24) & 0xff; \
        MAJ_APP = (source >> 16) & 0xff; \
        MAJ_APP = (source >> 8) & 0xff; \
        MAJ_APP = source & 0xff; }
        void movb(REG8BIT reg, uint8_t source) {
            MAJ_APP = reg;
            MAJ_APP_8(source)
        }
        void movw(REG16BIT reg, uint16_t source) {
            MAJ_APP = 0x66;
            MAJ_APP = reg;
            MAJ_APP_16(source)
        }
        void movl(REG32BIT reg, uint32_t source) {
            MAJ_APP = reg;
            MAJ_APP_32(source)
        }
        void movq(REG64BIT reg, uint64_t source) {
            MAJ_APP = 0x48;
            MAJ_APP = reg;
            // Note, low byte order!
            MAJ_APP_64(source)
        }
        void db(std::initializer_list<uint8_t> il) {
            for (auto& i : il) MAJ_APP_8(i)
        }
        void dw(std::initializer_list<uint16_t> il) {
            for (auto& i : il) MAJ_APP_16(i)
        }
        void dd(std::initializer_list<uint32_t> il) {
            for (auto& i : il) MAJ_APP_32(i)
        }
        void dq(std::initializer_list<uint64_t> il) {
            for (auto& i : il) MAJ_APP_64(i)
        }

        void run() {
            MAJ_APP = 0xc3;// ret, MUSTN'T lose it!!! Otherwise, you will not be able to return to the place where the machine code is called
            auto func = (JIT_FUNC) machineCodeAdr;
            func();
        }
        template <class Func>
        decltype(auto) run_as(){
            MAJ_APP = 0xc3;
            auto func = (Func*)machineCodeAdr;
            return func();
        }
        void clear() {
            machineCodeIndex = 0;
        }
        ~MAsmJit() {
#ifdef I_OS_WIN32
            VirtualFree(machineCodeAdr, 0, MEM_RELEASE);
#else
            munmap(machineCodeAdr, codeLength);
#endif
        }
    private:
        size_t machineCodeIndex;
        size_t codeLength;
        uint8_t *machineCodeAdr;
    };
}


#endif //MYASMJIT_MASMJIT_H
