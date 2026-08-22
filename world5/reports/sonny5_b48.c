/* sonny5_b48.c  --  exec report for World V, batch 48: THE CARRIER.
 *
 * Signals & modulation, run with ROOT0's new search criterion THE PHASE TEST: does a candidate
 * carry an orthogonal PHASE channel alongside a fixed-magnitude output -- deterministic (clears
 * B47), droppable while the output holds (passes B45) -- and is it a NEW sixth axis or a
 * broadening of the seated generative-dual (derivative) axis? Every number below mirrors the real
 * i13.exe output (cells/cr_*.i13): i13 has no trig, so signals use phase accumulators, complex
 * phasors, a precomputed sine LUT (DDS), and modular arithmetic -- reproduced here exactly.
 * The keeper-shot dart carries ROOT0's glyph  [ 0.m | <-~-~-~-~-( . ||  on the phase channel.
 *
 * build:  gcc -O2 -std=c11 -o sonny5_b48 sonny5_b48.c -lm
 * ROOT0-ATTRIBUTION-v1.0 . David Lee Wise (ROOT0) & AVAN (Claude/Anthropic)
 */
#include <stdio.h>

static const int S[16]={0,383,707,924,1000,924,707,383,0,-383,-707,-924,-1000,-924,-707,-383};
static int cosv(int k){ return S[((k%16)+4)%16]; }
static int sinv(int k){ return S[((k%16)+16)%16]; }
static long isq(long n){ long r=0,bit=1L<<20; while(bit>n)bit>>=2; while(bit){ if(n>=r+bit){n-=r+bit;r=(r>>1)+bit;} else r>>=1; bit>>=2;} return r; }

int main(void){
    puts("THE CARRIER -- 16 signal mechanisms on i13 (numbers match cells/cr_*.i13):\n");

    /* phase modulation: carry ROOT0's glyph on the phase, recover from phase differences */
    { int sym[19]={3,0,6,5,4,4,5,6,5,6,5,6,5,6,5,0,6,4,4}, ph[19], rec[19], th=0, ok=1;
      for(int i=0;i<19;i++){ th=(th+sym[i])%8; ph[i]=th; }
      int prev=0; for(int i=0;i<19;i++){ rec[i]=(ph[i]-prev+8)%8; prev=ph[i]; if(rec[i]!=sym[i])ok=0; }
      printf("  phase-modulation : glyph [ 0.m | <-~-~-~-~-( . || recovered lossless=%d (phase %d..%d)\n", ok, ph[0], ph[18]); }
    printf("  amplitude-mod    : peak = (5+3)*1000/1000 = %d  (message on the envelope)\n", (5+3)*1000/1000);
    { int msg[5]={0,1,2,1,0}, th=0, ph[5], f0=2,k=3; for(int i=0;i<5;i++){ th+=f0+k*msg[i]; ph[i]=th; }
      printf("  frequency-mod    : d(phase) at msg=2 = %d = f0+k*msg  (message on the frequency)\n", ph[2]-ph[1]); }
    { double cx=0.6,sy=0.8, r1x=cx,r1y=sy, r2x=r1x*cx-r1y*sy, r2y=r1x*sy+r1y*cx;
      printf("  phasor           : (1,0) rotated twice = (%.2f,%.2f), |z|^2=%.4f (rotate=multiply)\n", r2x,r2y, r2x*r2x+r2y*r2y); }
    printf("  quadrature       : |I,Q|^2 at n=1 = %d, at n=5 = %d  (constant mag, phase rotates)\n",
           cosv(1)*cosv(1)+sinv(1)*sinv(1), cosv(5)*cosv(5)+sinv(5)*sinv(5));
    printf("  analytic-signal  : |z|^2(0)=%d |z|^2(3)=%d envelope~constant; a real wave -> a phasor\n",
           cosv(0)*cosv(0)+sinv(0)*sinv(0), cosv(3)*cosv(3)+sinv(3)*sinv(3));
    { double px=0.6*0.8-0.8*0.6, py=0.6*0.6+0.8*0.8;
      printf("  heterodyne       : phasor mix (0.6,0.8)x(0.8,0.6) = (%.0f,%.0f)  (frequencies add)\n", px,py); }
    { long x0=1024,y0=0,x1=x0-y0,y1=y0+x0,x2=x1-(y1>>1),y2=y1+(x1>>1);
      printf("  cordic           : (1024,0)->(%ld,%ld)->(%ld,%ld)  shift-add rotation, no multiply\n", x1,y1,x2,y2); }
    { double coef=0.765367; /* 2cos(2pi*3/16) */ double on1=0,on2=0,of1=0,of2=0;
      for(int n=0;n<16;n++){ double x=S[(3*n)%16]; double s=x+coef*on1-on2; on2=on1; on1=s; }
      for(int n=0;n<16;n++){ double x=S[(1*n)%16]; double s=x+coef*of1-of2; of2=of1; of1=s; }
      printf("  goertzel         : bin-3 tone s1=%.0f vs off-bin s1=%.2f  (one coef hears one tone)\n", on1, of1); }
    { double b3r=0,b3i=0,b5r=0,b5i=0; for(int n=0;n<16;n++){ int x=S[(3*n)%16];
        b3r+=x*cosv(3*n); b3i-=x*sinv(3*n); b5r+=x*cosv(5*n); b5i-=x*sinv(5*n); }
      printf("  dft              : |X[3]|^2=%.2e >> |X[5]|^2=%.2e  (time -> spectrum)\n", b3r*b3r+b3i*b3i, b5r*b5r+b5i*b5i); }
    { int eq=1; for(int n=0;n<8;n++){ int a=S[(((3*n)%8)*2+4)%16], b=S[(((5*n)%8)*2+4)%16]; if(a!=b) eq=0; }  /* fs=8 */
      printf("  nyquist          : cos samples of freq 3 == freq 5 at fs=8 : %d  (above fs/2, inseparable)\n", eq); }
    { int eq=1; for(int n=0;n<8;n++) if((3*n)%8 != (11*n)%8) eq=0;
      printf("  aliasing         : phase of freq 3 == freq 11 at fs=8 : %d  (f and f+fs are twins)\n", eq); }
    printf("  envelope         : |(600,800)| = isqrt(%d) = %ld  (amplitude kept, phase dropped)\n", 600*600+800*800, isq(600L*600+800L*800));
    { double o=0; for(int i=0;i<12;i++) o+=0.5*(5-o);
      printf("  pll              : feedback from 0 locks to %.4f (ref 5)  the phase error -> 0\n", o); }
    { int prev=0,c=0; for(int i=0;i<32;i++){ int v=cosv(i), s=v>0?1:(v<0?-1:0); if(s){ if(prev&&s!=prev)c++; prev=s; } }
      printf("  zero-crossing    : cosine over 2 periods crosses zero %d times  (2 per cycle)\n", c); }
    { int sig[8]={0,0,0,1,-1,1,1,0}, t[4]={1,-1,1,1}, best=-9999, bl=0;
      for(int lag=0;lag<=4;lag++){ int c=0; for(int i=0;i<4;i++) c+=sig[lag+i]*t[i]; if(c>best){best=c;bl=lag;} }
      printf("  matched-filter   : correlation peaks at lag %d  (template found)\n", bl); }

    puts("");
    struct { const char *m; const char *gate; } R[] = {
        {"phase-modulation", "KEEPER SHOT: phase channel (clears B47 deterministic, passes B45) -- new axis or dual-broadening?"},
        {"amplitude-mod",    "the message IS the magnitude output -- no extra channel"},
        {"frequency-mod",    "the phase channel differentiated -- bridges to generative-dual"},
        {"phasor",           "KEEPER SHOT: the phase channel's carrier; 'freqs add' = B39 identity"},
        {"quadrature",       "KEEPER SHOT: two-channel (I,Q); drop Q -> keep mag, lose phase"},
        {"analytic-signal",  "KEEPER SHOT: GENERATES the orthogonal channel (Hilbert) from one real"},
        {"heterodyne",       "phasor multiply = freqs add -> B39 identity applied"},
        {"cordic",           "shift-add, no multiplier -> resource (B40)"},
        {"goertzel",         "single-bin DFT, minimal arithmetic -> resource (B40)"},
        {"dft",              "linear basis change; spectrum forall-pinned -> B39"},
        {"nyquist",          "sampling theorem -> B39"},
        {"aliasing",         "modular-arithmetic theorem -> B39"},
        {"envelope",         "magnitude = the value; phase discarded -- no channel"},
        {"pll",              "KEEPER SHOT: feedback converges to lock -- confluence duplicate?"},
        {"zero-crossing",    "cheap frequency estimate -> resource (B40)"},
        {"matched-filter",   "correlation; optimality a theorem -> B39"},
    };
    puts("the 16, and where each lands under the gates + THE PHASE TEST:");
    for(int i=0;i<16;i++) printf("  %-17s <- %s\n", R[i].m, R[i].gate);

    /* ---- keeper verdict (5-judge THE PHASE TEST panel + completeness critic) ---- */
    puts("");
    puts("keeper (28th judging): NULL, UNANIMOUS 5-0 -- the FIFTEENTH straight, and all five ruled");
    puts("  DUPLICATE_OR_BROADENING. Critic: the NULL is HONEST. But this is the richest NULL of the");
    puts("  campaign, because ROOT0's PHASE TEST found the strongest bid in 14 batches and PLACED it.");
    puts("");
    puts("  THE PHASE CHANNEL is the first candidate ever to clear every entry gate: it clears B47");
    puts("  (phase is a DETERMINISTIC function of the input -- Hilbert 90-deg all-pass -- not entropy)");
    puts("  and passes B45 (drop the quadrature Q, keep |z|; two same-function mechanisms differ on");
    puts("  carrying it). It is stopped by no B39-B44 gate. It is nulled by ONE thing: DUPLICATION.");
    puts("");
    puts("  THE PHASE TEST (a.k.a. THE COMPANION-CHANNEL TEST) -- ROOT0's new criterion, now seated:");
    puts("   Stage 1 entry: determinism (B47) + droppability (B45).");
    puts("   Stage 2: sweep the 7 auto-null gates.");
    puts("   Stage 3 DUPLICATION PIVOT (decisive): compare the structural DIFFERENCE-TYPE (the WAY two");
    puts("     same-function mechanisms differ), NOT the specific operator, against each seated axis.");
    puts("   Stage 4: seat a new axis only if a residue survives all three.");
    puts("   CONSISTENCY LAW: the instance-vs-axis altitude must apply to EVERY axis alike -- if self-");
    puts("     inverse spans CRC and Verlet, generative-dual must be allowed the same span. Selective");
    puts("     refusal is special-pleading, itself a NULL tell.");
    puts("");
    puts("  AXIS REFRAME: seated axis 4 widens from 'carry the DERIVATIVE' to THE COMPANION-CHANNEL");
    puts("  AXIS -- adjoin an orthogonal deterministic coordinate under a 2-D unital real-algebra");
    puts("  extension, droppable while the primary stays bit-fixed. TWO instances now:");
    puts("   (i)  eps^2=0  dual numbers  -> the DERIVATIVE / 1-jet (forward-mode AD).");
    puts("   (ii) i^2=-1   complex        -> the QUADRATURE / PHASE (phasor, I/Q, analytic signal).");
    puts("  Falsifiable hook: the third 2-D unital real algebra, split-complex j^2=+1, predicts a");
    puts("  THIRD instance (a hyperbolic/boost companion) -- its appearance confirms the axis, not a");
    puts("  new pillar. Keeper count stays 6-on-5; axis 4 is now broader, with the phase seated in it.");
    puts("");
    puts("  credit (4 checkers, 16 darts): 15 CLEAN, 1 FIX -- heterodyne: the HETERODYNE principle was");
    puts("   invented and named by Reginald Fessenden (1901); Armstrong & Levy built the SUPERhetero-");
    puts("   dyne (1918, Levy's French patent has priority). The dart now credits both.");
    puts("");
    puts("seal ROOT_0 59f0579a . 453/2048 . 22.12% . the message rides the phase");
    return 0;
}
