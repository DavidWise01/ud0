/* sonny5_b47.c  --  exec report for World V, batch 47: THE ZERO DAY.
 *
 * Security / cryptography / cracking mechanisms, run as a GENUINE fresh keeper hunt on a new
 * topic ("new topic for all keepers, zero point, hack the planet"). Every number below is the
 * real i13.exe output (cells/zd_*.i13). i13 is f64 (integers exact below 2^53), so all moduli
 * are small (23, 100003, 65536, 16). Keeper shots: the-secret-sharing (any k of n rebuild the
 * secret, k-1 reveal nothing) and the-bloom-filter (one-sided error).
 *
 * build:  gcc -O2 -std=c11 -o sonny5_b47 sonny5_b47.c -lm
 * ROOT0-ATTRIBUTION-v1.0 . David Lee Wise (ROOT0) & AVAN (Claude/Anthropic)
 */
#include <stdio.h>

static long H2(long a,long b){ return (a*131 + b*137 + 7) % 100003; }   /* pair hash */
static long H1(long x){ return (x*137 + 331) % 100003; }                /* chain hash */
static long HMIX(long x){ return (x*40503) % 65536; }                   /* 16-bit mix */
static long modexp(long base,long e,long m){ long a=1; base%=m; while(e>0){ if(e&1) a=(a*base)%m; base=(base*base)%m; e>>=1; } return a; }
static int  pc(long n){ int c=0; while(n){ c+=n&1; n>>=1; } return c; }
static long chain(long x,int n){ while(n-->0) x=H1(x); return x; }
static int  lfsr_period(void){ int s=1,cur=1,c=0; do{ int nb=((cur>>3)^(cur>>2))&1; cur=((cur<<1)|nb)&15; c++; }while(cur!=s&&c<40); return c; }
static double lag3(double x1,double y1,double x2,double y2,double x3,double y3){
    return y1*((0-x2)*(0-x3))/((x1-x2)*(x1-x3)) + y2*((0-x1)*(0-x3))/((x2-x1)*(x2-x3)) + y3*((0-x1)*(0-x2))/((x3-x1)*(x3-x2)); }
static int  h1b(long x){ return x%16; }
static int  h2b(long x){ return (x*7+3)%16; }
static long setb(long b,long x){ return b | (1L<<h1b(x)) | (1L<<h2b(x)); }
static int  testb(long b,long x){ return (int)(((b>>h1b(x))&1) & ((b>>h2b(x))&1)); }
static int  birthday16(void){ int seen[16]={0}; for(int x=1;x<40;x++){ int b=(int)((x*x+2*x)%16); if(seen[b]) return x; seen[b]=1; } return -1; }

int main(void){
    puts("THE ZERO DAY -- 16 security mechanisms on i13 (numbers match cells/zd_*.i13):\n");

    printf("  xor-cipher      : 77^42=%ld -> ^42=%ld (enc==dec, self-inverse)\n", 77L^42, (77L^42)^42);
    printf("  one-time-pad    : ct 9 -> plaintext %ld (key 12) or %ld (key 5): perfect secrecy\n", 9L^12, 9L^5);
    printf("  secret-sharing  : Lagrange {1,2,3}=%g , {2,3,4}=%g  (any 3 -> secret 42)\n", lag3(1,50,2,64,3,84), lag3(2,64,3,84,4,110));
    printf("  diffie-hellman  : A=5^6=%ld B=5^15=%ld ; B^a=%ld A^b=%ld (shared 2, never sent)\n",
           modexp(5,6,23), modexp(5,15,23), modexp(modexp(5,15,23),6,23), modexp(modexp(5,6,23),15,23));
    printf("  lfsr            : 4-bit taps(4,3) period = %d (all 2^4-1 nonzero states)\n", lfsr_period());
    { long h0=7*961+2*31+5, h1=(h0-7*961)*31+9, hr=2*961+5*31+9;
      printf("  rolling-hash    : roll[2,5,9]=%ld == recompute %ld (O(1) window update)\n", h1, hr); }
    { long b=setb(setb(0,42),100);
      printf("  bloom-filter    : 42=%d 100=%d (no false neg) ; 26=%d (FALSE POS) 7=%d (true neg)\n",
             testb(b,42), testb(b,100), testb(b,26), testb(b,7)); }
    { long n01=H2(11,22), n23=H2(33,44), root=H2(n01,n23);
      printf("  merkle-tree     : root=%ld ; leaf-2 proof ok=%d ; tamper -> root %ld (detected=%d)\n",
             root, (H2(n01,H2(33,44))==root), H2(H2(11,99),n23), (H2(H2(11,99),n23)!=root)); }
    { long c=H2(42,31337); printf("  commitment      : c=%ld verify=%d ; commit(43)=%ld binding=%d\n",
             c, (H2(42,31337)==c), H2(43,31337), (H2(43,31337)!=c)); }
    { int xa[4]={1,2,3,4}, xb[4]={1,2,9,4}; long d1=0,d2=0;
      for(int i=0;i<4;i++){ d1|=xa[i]^xb[i]; d2|=xa[i]^xa[i]; }   /* no early exit */
      printf("  constant-time   : [1,2,3,4]vs[1,2,9,4] eq=%d ; vs itself eq=%d (all elts visited)\n",
             d1==0, d2==0); }
    { long y=modexp(5,9,23); int x=-1; for(int t=0;t<23;t++) if(modexp(5,t,23)==y){x=t;break;}
      printf("  one-way-fn      : 5^9 mod 23 = %ld (cheap) ; inverse found by search: x=%d\n", y, x); }
    { long anchor=chain(7,5), r4=chain(7,4);
      printf("  hash-chain      : anchor=H^5(7)=%ld ; reveal H^4=%ld ; H(reveal)=anchor (%d)\n",
             anchor, r4, (H1(r4)==anchor)); }
    printf("  nonce           : reuse key 42: c1^c2 = %ld = 77^88 = %ld (key cancels)\n", (77L^42)^(88L^42), 77L^88);
    printf("  salt            : bare H(pw)=%ld,%ld collide ; salted %ld != %ld\n", H2(1234,0),H2(1234,0),H2(1234,55),H2(1234,88));
    printf("  birthday-bound  : 16 buckets -> first collision at item %d (~sqrt(16)=4)\n", birthday16());
    { long a=HMIX(12345), b=HMIX(12345^1); printf("  avalanche       : H(x) vs H(x^1) differ in %d of 16 bits (~half)\n", pc(a^b)); }

    puts("");
    struct { const char *m; const char *gate; } R[] = {
        {"xor-cipher",     "SELF-INVERSE (seated axis 1) -- duplicate"},
        {"one-time-pad",   "information-theoretic (key distribution); needs randomness i13 lacks"},
        {"secret-sharing", "CANDIDATE: any-k-agree (confluence?) + loose-spec family (B44?)"},
        {"diffie-hellman", "CANDIDATE: agree-in-open; commuting exponents = theorem (B39) / confluence"},
        {"lfsr",           "generator -> resource (B40); maximal period = theorem (B39)"},
        {"rolling-hash",   "incremental recomputation -> resource (B40)"},
        {"bloom-filter",   "CANDIDATE: one-sided loose spec, or approximation (different function)"},
        {"merkle-tree",    "verifier -> B41 (the corpus's own .dlw seal)"},
        {"commitment",     "hiding+binding = recognizer-side -> B41"},
        {"constant-time",  "timing side-channel -> resource (B40)"},
        {"one-way-fn",     "complexity asymmetry -> B39/B41"},
        {"hash-chain",     "verify-forward/hard-back -> B41"},
        {"nonce",          "vulnerability demo on axis-1 XOR -- not an axis"},
        {"salt",           "relabeling of the input -> B44 encoding"},
        {"birthday-bound", "counting theorem -> B39"},
        {"avalanche",      "function-property (distinguishes functions) -> B39"},
    };
    puts("the 16, and where each lands under the gates:");
    for(int i=0;i<16;i++) printf("  %-16s <- %s\n", R[i].m, R[i].gate);

    /* ---- keeper verdict (5-judge panel + completeness critic) ---- */
    puts("");
    puts("keeper (27th judging): NULL, UNANIMOUS 5-0 -- the FOURTEENTH straight. A genuine hunt");
    puts("  on a fresh topic (asked for), not a reflexive streak: judges ranked candidates by");
    puts("  closeness-to-clearing-gates, and the completeness critic confirmed the NULL is HONEST.");
    puts("");
    puts("  THE SHARPENING (why THE ZERO DAY is keeper-hostile): a seated keeper's extra output is");
    puts("  a DETERMINISTIC function of the inputs (Kahan's remainder, the dual-number derivative).");
    puts("  Every crypto 'extra output' -- secret-sharing's shares, the commitment nonce, the salt --");
    puts("  is ENTROPY: non-reproducible randomness for security. A non-reproducible extra output");
    puts("  can never be the structural invariant two same-function mechanisms DIFFER on; it just");
    puts("  varies. And crypto's headline guarantees ARE adversary-inability claims (B41) and cost /");
    puts("  hardness asymmetries (B40) -- two auto-NULL gates by construction.");
    puts("");
    puts("  the candidates: 424 secret-sharing (best shot) = any-k-agree is Lagrange-uniqueness,");
    puts("    forall-pinned -> B39, and duplicates CONFLUENCE; k-1-reveal-nothing = adversary-can't");
    puts("    (B41) needing randomness i13 lacks; the loose-spec latitude is entropy, non-structural.");
    puts("  428 bloom-filter (most novel-feeling) = one-sided error is genuinely distinct from the 5");
    puts("    axes, but a Bloom filter computes a RELAXED/different function -> APPROXIMATION gate: no");
    puts("    same-function pair to compare. Novel-but-gated, not a seat.");
    puts("  425 diffie-hellman = commuting exponents converge -> duplicates CONFLUENCE + B39 + B41.");
    puts("  430 commitment = hiding+binding are both adversary-can't statements -> B41.");
    puts("");
    puts("  credit (4 checkers, 16 darts): 13 CLEAN, 3 FIX -- all priority corrections:");
    puts("   - one-time-pad: Frank Miller (1882) is the earliest known concept (Bellovin 2011);");
    puts("     Vernam (1917) built the XOR cipher, Joseph Mauborgne (~1918) added the one-use key.");
    puts("   - diffie-hellman: add GCHQ prior art -- James Ellis (1969), Malcolm Williamson (1974);");
    puts("     Cocks (1973) belongs to RSA, not DH.");
    puts("   - one-way-function: Roger Needham (~1967, Wilkes 1968) predates the D-H formalization.");
    puts("");
    puts("seal ROOT_0 dac620c5 . 437/2048 . 21.34% . hack the planet");
    return 0;
}
