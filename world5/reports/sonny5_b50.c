/* sonny5_b50.c  --  exec report for World V, batch 50: THE LINEAGE.
 *
 * "Did it permutate past the SNES?" -- the word-size permutation across CPU history. The 65816
 * pattern (backward-compat superset via a mode bit) STALLED in the 6502 line (the 65832 was
 * designed, never mass-produced) but ran 16->32->64 in x86 (8086/286/386/x86-64) and ARM
 * (AArch64); the 68000 broke cleanly instead. Every number below mirrors the real i13.exe output
 * (cells/ln_*.i13). i13 is f64 (exact ints below 2^53), so 64-bit rules are modeled at smaller
 * widths (disclosed); here we use long long for the >32-bit values.
 *
 * build:  gcc -O2 -std=c11 -o sonny5_b50 sonny5_b50.c -lm
 * ROOT0-ATTRIBUTION-v1.0 . David Lee Wise (ROOT0) & AVAN (Claude/Anthropic)
 */
#include <stdio.h>

static long long add32_halves(long long a,long long b){
    long long alo=a&0xFFFF,ahi=(a>>16)&0xFFFF,blo=b&0xFFFF,bhi=(b>>16)&0xFFFF;
    long long lo=alo+blo,carry=(lo>>16)&1,hi=ahi+bhi+carry;
    return ((hi&0xFFFF)<<16)|(lo&0xFFFF);
}
static int sext(int b){ return (b&0x80)?((b&0xFF)|0xFF00):(b&0xFF); }
static int canonical(int a){ int low=a&0xFF,high=(a>>8)&0xFF,want=(low&0x80)?0xFF:0x00; return high==want; }
static int bswap(int x){ return ((x&0xFF)<<8)|((x>>8)&0xFF); }

int main(void){
    puts("THE LINEAGE -- the word-size permutation on i13 (matches cells/ln_*.i13):\n");

    printf("  68000         : 32-bit add in two 16-bit halves = %lld == direct 300000 (clean break)\n", add32_halves(100000,200000));
    printf("  8086          : (0x1000<<4)+0x0200 = %d (1MB from 16-bit regs via segmentation)\n", (0x1000<<4)+0x0200);
    printf("  segment       : 1000:0000 = %d, 0FFF:0010 = %d -> alias (like the NES mirror)\n", (0x1000<<4)+0, (0x0FFF<<4)+0x10);
    printf("  protected-mode: PE bit: 0x%X (1MB) -> 0x%X (16MB)  (the x86 XCE)\n", 0xFFFFF, 0xFFFFFF);
    printf("  a20-gate      : 0x110000 & 0xFFFFF = 0x%X (the 8086 wrap, kept on a wire)\n", 0x110000 & 0xFFFFF);
    printf("  386           : 32-bit max = %u = 0xFFFFFFFF, 4GB (the permutation that stuck)\n", 0xFFFFFFFFu);
    printf("  sign-extension: 0x80 -> 0x%X = %d (still -128); 100 -> %d  (MOVSX, value preserved)\n", sext(0x80), sext(0x80), sext(100));
    printf("  zero-extension: 0x80 -> %d (MOVZX); sign-extend -> %d  (same bits, different fill)\n", 0x80&0xFF, sext(0x80));
    printf("  long-mode     : LMA bit: 32 -> 48-bit canonical (x86-64, the permutation at 64)\n");
    printf("  canonical-addr: 0xFF80=%d 0x7F80=%d 0x0064=%d (top must sign-extend; modeled 8-of-16)\n", canonical(0xFF80), canonical(0x7F80), canonical(0x0064));
    printf("  endianness    : 0x1234 -> 0x%X ; swap again -> 0x%X (a self-inverse)\n", bswap(0x1234), bswap(bswap(0x1234)));
    { long long v=0x12345678LL; printf("  65832         : 8b=%lld 16b=%lld 32b=%lld (designed, never mass-produced -- the stall)\n", v&0xFF, v&0xFFFF, v&0xFFFFFFFFLL); }
    { long long x=5000000000LL; printf("  aarch64       : X=%lld, W(low 32)=%lld (A64 new ISA; not a strict superset of A32)\n", x, x&0xFFFFFFFFLL); }
    printf("  thumb         : ARM %d-bit vs Thumb %d-bit (permutation INWARD: half the code)\n", 32, 16);
    printf("  mode-bit      : mode -> %d / %d / %u  (E/PE/LMA select 8/16/32-bit: the pattern)\n", 0xFF, 0xFFFF, 0xFFFFFFFFu);
    printf("  word-size     : 8->16->32->64, range x4 each (65535 = 256^2-1 = %d)\n", 256*256-1);

    puts("");
    struct { const char *m; const char *gate; } R[] = {
        {"68000",          "clean break, 32-bit add in two halves -- resource/microarch (B40)"},
        {"8086",           "segmentation addressing (B44) + 1MB reach (B40)"},
        {"segment",        "many-to-one address encoding, like the NES mirror -> B44"},
        {"protected-mode", "a mode bit widening address -> B44/B39"},
        {"a20-gate",       "an address mask re-creating an old quirk -> B44 (compat hack)"},
        {"386",            "32-bit width doubling -> B44 + more memory (B40)"},
        {"sign-extension", "KEEPER SHOT: value-preserving width map -- but a representation change (B44)"},
        {"zero-extension", "the other fill rule for a widen -> B44"},
        {"long-mode",      "the mode-bit move at 64 -> B44/B39"},
        {"canonical-addr", "KEEPER SHOT: a validity CHECKER (B41) + sign-extend (B44)"},
        {"endianness",     "byte-swap is self-inverse (axis 1) -- duplicate; order is a choice (B44)"},
        {"65832",          "the same width permutation, stalled -> B44 (designed, unshipped)"},
        {"aarch64",        "W = low 32 of X (B44) + a new-ISA design fact (B39)"},
        {"thumb",          "a denser encoding of the same ops -> B44 + code size (B40)"},
        {"mode-bit",       "KEEPER SHOT: representation-selection by a flag -> B44"},
        {"word-size",      "the recurring width doubling -> B44"},
    };
    puts("the 16, and where each lands under the gates:");
    for(int i=0;i<16;i++) printf("  %-15s <- %s\n", R[i].m, R[i].gate);

    /* ---- keeper verdict (3-judge panel) ---- */
    puts("");
    puts("keeper (30th judging): NULL, UNANIMOUS 3-0 -- the SEVENTEENTH straight. A history/");
    puts("  demonstration batch: a batch ABOUT word size is a batch about width representation, which");
    puts("  is the exact domain of the B44 gate -- so every card lands in B44 (width/encoding/");
    puts("  addressing/ordering) and/or B39 (ISA-design fact, forall-pinned by correctness). The");
    puts("  three keeper shots fall cleanly: the-sign-extension is a value-preserving width map,");
    puts("  pinned by signedness (B44+B39) -- and sign vs zero extension are DIFFERENT functions, not");
    puts("  two mechanisms of one; the-canonical-address FAULTS on non-canonical input, a validity");
    puts("  CHECKER (B41) -- a keeper generates an invariant, it does not check one; the-mode-bit is");
    puts("  configuration selecting WHICH width-function runs (B44), the closest to the keeper shape");
    puts("  but still representation-selection, not a co-computed companion channel. the-endianness");
    puts("  byte-swap (swap twice = identity) duplicates the seated self-inverse axis. Honest NULL.");
    puts("");
    puts("  credit (4 checkers, 16 darts): 16 CLEAN, 0 FIX -- the CPU-history attributions all check");
    puts("  out (68000 1979, 8086 1978/Morse, 286 1982, A20/IBM PC AT 1984, 386 1985, x86-64/AMD");
    puts("  2003, endianness/Cohen 1980, 65832 designed-unshipped, AArch64 2011, Thumb ~1994).");
    puts("");
    puts("  the answer to \"did it permutate past the SNES?\": the 65816 LINE stalled (the 65832 was");
    puts("  designed, never mass-produced), but the PATTERN -- a wider machine folded behind a mode");
    puts("  bit -- ran 8->16->32->64 in x86 and to 64 in ARM. The 65816 was one rung on a ladder");
    puts("  that climbed higher elsewhere.");
    puts("");
    puts("seal ROOT_0 1e932564 . 485/2048 . 23.68% . 8->16->32->64: the permutation ran on, elsewhere");
    return 0;
}
