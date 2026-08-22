/* sonny5_b44.c  --  exec report for World V, batch 44: THE CARRY.
 *
 * The sequel to THE THREAD, whose #1 principle was the carry ("a fold without a carry is
 * one-way"). The batch traces one signal: BORN in the half adder's AND, propagated (ripple),
 * parallelized (lookahead/Kogge-Stone), deferred (carry-save), speculated (carry-select),
 * wrapped (end-around), re-based (BCD), pinned (saturating), REFUSED (gray code), DISCARDED
 * (parity). This report enacts the arc from creation to destruction of the carry.
 *
 * build:  gcc -O2 -std=c11 -o sonny5_b44 sonny5_b44.c
 * ROOT0-ATTRIBUTION-v1.0 . David Lee Wise (ROOT0) & AVAN (Claude/Anthropic)
 */
#include <stdio.h>

static int  hcarry(int a,int b){ return a & b; }                 /* the carry is born */
static int  ripple(int a,int b){ int c=0,s=0; for(int i=0;i<8;i++){ int ai=(a>>i)&1,bi=(b>>i)&1;
    int su=ai^bi^c; c=(ai&bi)|(c&(ai^bi)); s|=su<<i; } return s; } /* carry walks */
static int  gray(int x){ return x ^ (x>>1); }                     /* count with no carry */
static int  popc(int x){ int c=0; while(x){c+=x&1;x>>=1;} return c; }
static int  parity(int x){ return popc(x)&1; }                    /* carry discarded */

int main(void){
    puts("THE CARRY, from birth to death:\n");
    printf("  BORN     half adder 1+1  -> sum 0, carry %d        (the AND: 1+1 overflows one bit)\n", hcarry(1,1));
    printf("  WALKS    ripple 11+6     -> %d                     (carry propagates low->high)\n", ripple(11,6));
    printf("  DEFERRED carry-save 5+6+7-> %d                    (kept in a 2nd number, added later)\n",
           (5^6^7) + (((5&6)|(6&7)|(5&7))<<1));
    printf("  WRAPS    end-around 6+12 -> %d                     (top carry folds into the bottom)\n",
           ((6+12)&15) + ((6+12)>>4));
    printf("  RE-BASED bcd 9+1         -> 0x%X  (decimal 10)     (+6 forces the carry at ten)\n", (9+1)+6);
    printf("  PINNED   saturating 200+100 -> %d                  (clamp, don't wrap to 44)\n", 300>255?255:300);
    printf("  REFUSED  gray g(5),g(6)  -> %d,%d  (differ in %d bit)  (count with NO carry)\n",
           gray(5), gray(6), popc(gray(5)^gray(6)));
    printf("  DISCARDED parity(11)     -> %d                     (XOR-fold: sum mod 2, carry gone)\n\n", parity(11));

    /* the 16, and where each lands under the four gates */
    struct { const char *m; const char *v; } R[] = {
        {"half-adder",     "the carry born (correctness)"},
        {"full-adder",     "the arithmetic cell (correctness)"},
        {"ripple-carry",   "O(n) carry walk (resource)"},
        {"carry-lookahead","parallel carries, bit-identical sum (resource, B40)"},
        {"carry-save",     "deferred carry, redundant rep (resource)"},
        {"kogge-stone",    "parallel-prefix scan (resource; scan covered in B40)"},
        {"carry-select",   "speculate both, select (resource)"},
        {"carry-flag",     "unsigned-overflow status bit (recognizer, B41)"},
        {"overflow-flag",  "signed-overflow status bit (recognizer)"},
        {"borrow",         "subtract via ~b+1 (two's complement = self-inverse)"},
        {"ones-complement","negate by flip; ~~x=x (self-inverse, Verlet)"},
        {"end-around-carry","the fold-back loop (self-inverse ride; the thread's ring)"},
        {"bcd",            "carry at ten (+6 convention)"},
        {"saturating-add", "clamp = the idempotent projection (dart 366)"},
        {"gray-code",      "unit-Hamming ordering -- KEEPER SHOT (reflected = self-inverse?)"},
        {"parity",         "XOR-fold = sum mod 2 (self-inverse + a recognizer)"},
    };
    puts("the 16, and where each lands under the four gates:");
    for(int i=0;i<16;i++) printf("  %-16s <- %s\n", R[i].m, R[i].v);

    /* keeper verdict (adversarial panel, 3 judges) */
    puts("");
    puts("keeper (24th judging): NULL, UNANIMOUS 3-0 -- the eleventh straight. the-gray-code dies");
    puts("  THREE ways, any one sufficient: (1) ENCODING/RESOURCE -- Gray is a relabeling of the");
    puts("  identical value-set, an ordering chosen to minimize transitions (a hardware concern);");
    puts("  (2) SELF-INVERSE -- the reflected binary code is built by mirroring (an involution)");
    puts("  and encoded via XOR (the self-inverse op), so unit-Hamming folds into the Verlet/CRC");
    puts("  axis; (3) HAMILTONIAN-PATH WITNESS -- a minimal-change ordering is a Hamiltonian path");
    puts("  on the n-cube, a combinatorial existence theorem the code merely instances. And the");
    puts("  whole batch decomposes with no remainder into the four seated categories: RESOURCE");
    puts("  (lookahead/carry-save/kogge-stone/carry-select, bit-identical sums), SELF-INVERSE");
    puts("  (ones-complement/borrow/end-around/parity/gray), RECOGNIZER (carry & overflow flags),");
    puts("  and CORRECTNESS/CONVENTION (adders/bcd/saturating). NEW GATE -- THE ENCODING/");
    puts("  REPRESENTATION BAN: a property of the CODEWORDS (labels, numeral scheme, ordering)");
    puts("  rather than the computed VALUES is a representation choice, witnessed by the encoding,");
    puts("  not a structural output-relation invariant. TELL: relabel and the property moves while");
    puts("  the computed function stays bit-identical. KILLS: Gray, BCD, one-hot, any nice numbering.");
    puts("  Keepers stay 6 on 5 axes; the near-complete read strengthens again.");

    puts("");
    puts("seal ROOT_0 49b661cb . 389/2048 . 18.99% . the carry, born in an AND, buried in an XOR");
    return 0;
}
