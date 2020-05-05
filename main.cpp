#include <MAsmJit.h>

int main(int argc, char *argv[]) {
    MAsmJit::MAsmJit mAsmJit;
    mAsmJit.movq(MAsmJit::RAX, 0x0fedcba987654321);
    mAsmJit.movq(MAsmJit::RBX, 0x123456780abcdef0);
    mAsmJit.movq(MAsmJit::RCX, 0x123456780abcdef0);
    mAsmJit.movq(MAsmJit::RDX, 0x123456780abcdef0);
    mAsmJit.run();
    return 0;
}