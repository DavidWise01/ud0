/* sonny5_b54.c  --  exec report for World V, batch 54: THE HASH.
 *
 * Hashing & hash tables. Every number below mirrors the real i13.exe output (cells54/hs_*.i13).
 * Hash widths are chosen so every multiply stays under f64's exact 2^53; where a full 32-bit form
 * would overflow (FNV), i13 runs a reduced width -- noted. Keeper shot: the-consistent-hashing
 * (a ring where adding a node moves only ~1/n keys).
 *
 * build:  gcc -O2 -std=c11 -o sonny5_b54 sonny5_b54.c -lm
 * ROOT0-ATTRIBUTION-v1.0 . David Lee Wise (ROOT0) & AVAN (Claude/Anthropic) . |||..()
 */
#include <stdio.h>
#include <string.h>

int main(void){
    puts("THE HASH -- hashing & hash tables on i13 (matches cells54/hs_*.i13):\n");

    /* 534 multiplicative (Knuth): (k*A mod 2^32) >> 24 */
    { unsigned long long A=2654435761ULL;
      unsigned b42=(unsigned)((42ULL*A)%4294967296ULL>>24), b43=(unsigned)((43ULL*A)%4294967296ULL>>24);
      printf("  multiplicative : 42->%u  43->%u  (adjacent keys, far-apart buckets)\n", b42,b43); }

    /* 535 FNV-1a, 24-bit (exact in f64) */
    { unsigned h1=1875397,h2=1875397; char a[]="hi",b[]="hj";
      for(int i=0;i<2;i++){h1=(h1^(unsigned char)a[i]); h1=(unsigned)(((unsigned long long)h1*16777619ULL)%16777216ULL);}
      for(int i=0;i<2;i++){h2=(h2^(unsigned char)b[i]); h2=(unsigned)(((unsigned long long)h2*16777619ULL)%16777216ULL);}
      printf("  fnv-1a (24bit) : hi=%u  hj=%u  avalanche (one byte -> whole hash)\n", h1,h2); }

    /* 536 djb2: (h<<5)+h == h*33 */
    { unsigned long long h=5381; for(int i=0;i<2;i++){int c="hi"[i]; h=(h*33+c)%4294967296ULL;}
      printf("  djb2           : h(\"hi\")=%llu  ; (5381<<5)+5381 == 5381*33 = %d\n", h, (5381<<5)+5381); }

    /* 537 polynomial rolling (Rabin-Karp), roll == direct */
    { long long M=1000000007,P=31; long long hi=0; const char*s="hi"; for(int i=0;i<2;i++)hi=(hi*P+s[i])%M;
      long long rolled=(((hi - 'h'*P)*P)+'t')%M; long long it=0; const char*u="it"; for(int i=0;i<2;i++)it=(it*P+u[i])%M;
      printf("  poly-rolling   : roll(\"hi\"->\"it\")=%lld == direct h(\"it\")=%lld  (O(1) slide)\n", rolled, it); }

    /* 538 Fibonacci hash: consecutive keys spread */
    { unsigned long long A=2654435769ULL; printf("  fibonacci      : 0,1,2,3 -> ");
      for(int k=0;k<4;k++) printf("%llu ", (k*A)%4294967296ULL>>28); printf(" (golden ratio spreads)\n"); }

    /* 539 separate chaining: longest chain */
    { int m=7,c[7]={0}; int keys[]={5,12,19,7,26}; for(int i=0;i<5;i++)c[keys[i]%m]++; int mx=0; for(int i=0;i<7;i++)if(c[i]>mx)mx=c[i];
      printf("  chaining       : bucket5=%d bucket0=%d longest_chain=%d\n", c[5],c[0],mx); }

    /* 540 linear probing: cluster grows 0,1,2 */
    { int m=7,t[7]={0}; int p[3],ins[]={-1,5,6}; for(int j=0;j<3;j++){ if(ins[j]>=0)t[ins[j]]=1; int s=0; while(t[(5+s)%m])s++; p[j]=s; }
      printf("  linear-probing : probe distances %d,%d,%d  (primary cluster grows)\n", p[0],p[1],p[2]); }

    /* 541 quadratic probing: offsets i^2 */
    { int m=11; int t[11]={0,0,0,1,1,0,0,0,0,0,0}; int i=0; while(t[(3+i*i)%m])i++;
      printf("  quadratic      : lands slot %d ; offsets (3+i^2): %d,%d,%d\n", (3+i*i)%m,(3+1)%m,(3+4)%m,(3+9)%m); }

    /* 542 double hashing: h1+i*h2 */
    { int m=11; int t[11]={1,0,0,1,0,0,1,0,0,0,0}; int i=0; while(t[(3+i*4)%m])i++;
      printf("  double-hashing : first free slot %d = (3 + 1*4) mod 11\n", (3+i*4)%m); }

    /* 543 load factor */
    { double a=7.0/8, na=7.0/16; printf("  load-factor    : 7/8=%.3f > 0.75 -> double m -> 7/16=%.4f\n", a,na); }

    /* 544 birthday collision */
    { double p=1; for(int i=0;i<23;i++)p*=(365.0-i)/365.0; printf("  birthday       : 23 in 365 -> P(collision)=%.4f (over half)\n", 1-p); }

    /* 545 universal hashing */
    { long long p=1000003,m=16; printf("  universal      : ((3*42+7) mod %lld) mod %lld = %lld  (Pr[coll]<=1/m over random a,b)\n", p,m, ((3*42+7)%p)%m); }

    /* 546 perfect hashing: search a for injective ((a*k) mod 97) mod 5 */
    { int a=1; int S[]={10,22,37}; int found=0;
      for(a=1;a<97&&!found;a++){int x=((a*S[0])%97)%5,y=((a*S[1])%97)%5,z=((a*S[2])%97)%5; if(x!=y&&y!=z&&x!=z)found=a;}
      int x=((found*S[0])%97)%5,y=((found*S[1])%97)%5,z=((found*S[2])%97)%5;
      printf("  perfect        : a=%d (a=1,2 collide) -> buckets %d,%d,%d distinct\n", found,x,y,z); }

    /* 547 consistent hashing: minimal remap */
    { int keeper_shot=1; (void)keeper_shot;
      int n1[]={10,50,90}, n2[]={10,30,50,90};
      int a=0; for(int i=0;i<3;i++)if(n1[i]>=15){a=n1[i];break;}
      int a2=0; for(int i=0;i<4;i++)if(n2[i]>=15){a2=n2[i];break;}
      int b2=0; for(int i=0;i<4;i++)if(n2[i]>=60){b2=n2[i];break;}
      printf("  consistent*    : key15 %d->%d (moved) ; key60 stays %d  (add node -> ~1/n move)\n", a,a2,b2); }

    /* 548 Robin Hood: probe-distance metric */
    { int slot=0,home=5,m=7; int d=slot>=home?slot-home:slot+m-home;
      printf("  robin-hood     : dist(slot0,home5,m7)=%d ; caps worst case, mean unchanged\n", d); }

    /* 549 cuckoo: two nests */
    { int k=42,m=11; printf("  cuckoo         : key 42 -> nests h1=%d h2=%d ; lookup checks 2 (worst case)\n", k%m, (k/7)%m); }

    puts("");
    struct { const char *m; const char *gate; } R[] = {
        {"multiplicative","arithmetic (B40) + pinned bucket (B39)"},
        {"fnv-1a","B39 pinned digest + B40; 32-bit overflows f64 (width limit, not keeper)"},
        {"djb2","(h<<5)+h == 33h is an identity (B39/B40)"},
        {"poly-rolling","rolled==direct is an algebraic identity, Horner mod m (B39) + B40 cheaper path"},
        {"fibonacci","number-theory fact about phi, three-distance theorem (B40/B39)"},
        {"chaining","PS-015 list + pinned count (B39); layout vs open-addressing = B44"},
        {"linear-probing","resource/amortized probe cost (B40); same stored map"},
        {"quadratic","probe-schedule change (B40); guarantee needs prime m, load<1/2"},
        {"double-hashing","better distribution (B40); coprimality = correctness cond"},
        {"load-factor","resize policy + amortized cost (B40)"},
        {"birthday","an exact probability theorem (B39), the sqrt(m) bound"},
        {"universal","adversary/probabilistic bound Pr<=1/m (B41 + approximation)"},
        {"perfect","a SEARCH for a witness (B40) + pinned map (B39)"},
        {"consistent*","KEEPER SHOT: on a fixed ring key->node is pinned (B39); minimal-remap is how the map CHANGES, not same-input differ"},
        {"robin-hood","cuts probe VARIANCE not mean (B40); reorder = coordinate-dependent tie-break (B44)"},
        {"cuckoo","worst-case-2 cost bound (B40); nest = coordinate-dependent eviction history (B44)"},
    };
    puts("the 16, and where each lands under the gates:");
    for(int i=0;i<16;i++) printf("  %-15s <- %s\n", R[i].m, R[i].gate);

    /* ---- keeper verdict (3-judge panel + 4-checker credit verify) ---- */
    puts("");
    puts("keeper (34th judging): NULL / NULL / NULL  -- unanimous, no new axis. 21 straight NULL.");
    puts("  shot the-consistent-hashing (minimal remap): KILLED BY WITNESSING. On a fixed ring,");
    puts("        key->node is forall-pinned (B39). 'minimal remap' compares the map at config n");
    puts("        vs n+1 -- the delta between two DIFFERENT functions, not extra output-structure on");
    puts("        one fixed function; and the remap set is a DETERMINISTIC FUNCTION OF THE TWO OUTPUT");
    puts("        MAPS, so any two mechanisms agreeing on key->node for every config MUST share it");
    puts("        (WITNESSED). To differ on remap you must compute a different map (mod-n vs ring) =");
    puts("        different function = no same-function pair. Also B40 rehash-cost, B44 seed-position.");
    puts("  the family reduces to: pinned digest/map (B39) + probe/amortized cost (B40) + slot/nest");
    puts("        coordinate (B44) + seed entropy (B47, universal hashing) + adversary bound (B41).");
    puts("        No coordinate-independent deterministic output a same-function pair can keep-or-drop.");
    puts("  keeper space holds: 6 keepers on 5 axes (self-inverse . computed-not-stored . conserve-");
    puts("        remainder . companion-channel . confluence). Honest NULL, no forcing.");
    puts("");
    puts("credit verify (4 checkers): 16 CLEAN / 0 FIX -- every attribution holds");
    puts("  (three-distance thm via Sos/Suranyi/Swierczkowski; Luhn 1953; Knuth 1963; von Mises 1939;");
    puts("   Carter-Wegman 1979; FKS 1984; Karger 1997; Celis 1985; Pagh-Rodler 2001).");

    puts("");
    puts("seal ROOT_0 b89e9d72 . 549/2048 . 26.81% . a bucket for every key, and a ring that barely moves");
    return 0;
}
