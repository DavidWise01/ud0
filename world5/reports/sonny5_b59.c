/* sonny5_b59.c  --  exec report for World V, batch 59: THE MIND II.
 *
 * Deep learning, part 2. Every number below mirrors the real i13.exe output (cells59/n2_*.i13).
 * Grounded with f64 arithmetic, native recursion (training/decoding loops), and Taylor/Newton
 * series for exp, ln, sqrt, sin - here C uses <math.h>. Keeper shot: the-lstm (the constant
 * error carousel). Continues THE MIND (batch 58).
 *
 * build:  gcc -O2 -std=c11 -o sonny5_b59 sonny5_b59.c -lm   (put mingw64/bin on PATH first)
 * ROOT0-ATTRIBUTION-v1.0 . David Lee Wise (ROOT0) & AVAN (Claude/Anthropic) . |||..()
 */
#include <stdio.h>
#include <math.h>

int main(void){
    puts("THE MIND II -- deep learning, part 2, on i13 (matches cells59/n2_*.i13):\n");

    /* 614 exploding gradient */
    { printf("  exploding-grad   : 4^10 = %.0f (explodes) vs 1^10 = %.0f (stable) -- dual of vanishing\n", pow(4,10), pow(1,10)); }

    /* 615 gradient clipping */
    { double gx=60,gy=80,norm=hypot(gx,gy),t=5; double cx=gx*t/norm,cy=gy*t/norm;
      printf("  grad-clipping    : ||g||=%.0f -> clipped (%.0f,%.0f) norm %.0f -- direction kept\n", norm, cx, cy, hypot(cx,cy)); }

    /* 616 dropout */
    { double p=0.5,x=10,sc=1/(1-p); printf("  dropout          : p=0.5, scale %.0f -> E[out] = %.0f = x (inverted dropout)\n", sc, (1-p)*sc*x); }

    /* 617 layer norm */
    { double f[4]={2,4,6,8},mu=(f[0]+f[1]+f[2]+f[3])/4,v=0; for(int i=0;i<4;i++)v+=(f[i]-mu)*(f[i]-mu); v/=4;
      double nm=0; for(int i=0;i<4;i++)nm+=(f[i]-mu)/sqrt(v); nm/=4;
      printf("  layer-norm       : one sample [2,4,6,8] -> mean %.0f (batch size 1, no running stats)\n", nm); }

    /* 618 gelu */
    { double g0=0, g2=2/(1+exp(-1.702*2)), gn=-1/(1+exp(1.702));
      printf("  gelu             : GELU(0)=%.0f GELU(2)=%.3f GELU(-1)=%.3f (soft leak, not hard 0)\n", g0, g2, gn); }

    /* 619 weight decay */
    { double w=100; for(int i=0;i<200;i++)w*=0.99; printf("  weight-decay     : w 100 -> %.2f (200 steps x0.99) -- shrinks toward 0\n", w); }

    /* 620 positional encoding */
    { printf("  positional-enc   : PE(pos)=sin(pos): pe(0)=%.0f pe(1)=%.3f pe(2)=%.3f -- distinct per position\n", sin(0.0), sin(1.0), sin(2.0)); }

    /* 621 multi-head attention */
    { int d=8,h=2; printf("  multi-head-attn  : d=%d h=%d -> head_dim %d ; concat restores %d = d\n", d, h, d/h, (d/h)*h); }

    /* 622 lstm  [KEEPER SHOT] */
    { double c=100; for(int i=0;i<50;i++)c=1*c; printf("  lstm*            : forget=1 -> cell %.0f survives 50 steps (constant error carousel) ; 3 gates\n", c); }

    /* 623 gru */
    { double keep=(1-0)*100+0*5, upd=(1-1)*100+1*5; printf("  gru              : z=0 keep %.0f ; z=1 update %.0f ; 2 gates (vs LSTM 3)\n", keep, upd); }

    /* 624 beam search */
    { double greedy=0.6*0.3, beam=0.4*0.9; printf("  beam-search      : greedy %.2f vs beam-2 %.2f -- lookahead recovers the better sequence\n", greedy, beam); }

    /* 625 temperature */
    { double z[3]={1,2,3}; double lo=exp(3/0.5)/(exp(1/0.5)+exp(2/0.5)+exp(3/0.5)), hi=exp(3/2.0)/(exp(1/2.0)+exp(2/2.0)+exp(3/2.0));
      printf("  temperature      : p(top) low-T %.3f (sharp) vs high-T %.3f (flat) -- the dial\n", lo, hi); }

    /* 626 kl divergence */
    { double p[2]={0.5,0.5},q[2]={0.9,0.1}; double pq=0,qp=0; for(int i=0;i<2;i++){pq+=p[i]*log(p[i]/q[i]); qp+=q[i]*log(q[i]/p[i]);}
      printf("  kl-divergence    : D(p||p)=0  D(p||q)=%.3f  D(q||p)=%.3f -- >=0, asymmetric\n", pq, qp); }

    /* 627 label smoothing */
    { double eps=0.1; int K=10; double t=(1-eps)+eps/K, o=eps/K; printf("  label-smoothing  : true %.2f, others %.2f (x9), sum %.2f -- never 100%% sure\n", t, o, t+9*o); }

    /* 628 teacher forcing */
    { int target[4]={1,2,3,4}; printf("  teacher-forcing  : step2 input %d, step3 input %d (ground truth) ; exposure bias at inference\n", target[0], target[1]); }

    /* 629 lr warmup */
    { int w=10; printf("  lr-warmup        : lr(2)=%d (ramp) lr(10)=%d (peak) lr(50)=%d (decay)\n", 2*100/w, w*100/w, w*100/50); }

    puts("");
    struct { const char *m; const char *gate; } R[] = {
        {"exploding-gradient","a product identity -> infinity (B39)"},
        {"gradient-clipping","a pinned rescale, direction-preserving (B39)"},
        {"dropout","entropy mask (B47) + pinned 1/(1-p) expectation (B39)"},
        {"layer-norm","a pinned normalization over features (B39)"},
        {"gelu","a pinned activation x*Phi(x) (B39)"},
        {"weight-decay","a pinned multiplicative recurrence (B39)"},
        {"positional-encoding","a pinned function of position (B39/B44)"},
        {"multi-head-attention","a pinned reshape of attention (B39)"},
        {"lstm*","KEEPER SHOT: gated recurrence, the carousel is a fixed structure like the residual identity (B39)"},
        {"gru","a pinned gated blend (B39)"},
        {"beam-search","a search heuristic over pinned scores (B40)"},
        {"temperature-sampling","a pinned reshape (B39) + entropy draw (B47)"},
        {"kl-divergence","a pinned information functional (B39)"},
        {"label-smoothing","a pinned target transform (B39)"},
        {"teacher-forcing","a training protocol (B39)"},
        {"learning-rate-warmup","a pinned schedule of the step (B39)"},
    };
    puts("the 16, and where each lands under the gates:");
    for(int i=0;i<16;i++) printf("  %-20s <- %s\n", R[i].m, R[i].gate);

    /* ---- keeper verdict (self-judged + 1 veracity checker; lean-verify per David's budget) ---- */
    puts("");
    puts("keeper (39th judging): NULL  -- no new axis. 26 straight NULL.");
    puts("  shot the-lstm (the constant error carousel): the forget gate carries the cell state (and gradient)");
    puts("        down a nearly-untouched channel -- close in spirit to the residual identity. But the cell");
    puts("        update c_t = f*c_(t-1) + i*c~ is a PINNED function of the gates; every correct LSTM computes it");
    puts("        identically (B39). A gated recurrence, a fixed structure, not a same-function DOF. NULL.");
    puts("  the batch reduces to: pinned functions/normalizations/schedules (B39, most of 16) + two with an");
    puts("        entropy draw (dropout mask, temperature sampling: B47) + one search heuristic (beam: B40).");
    puts("        None expose a new coordinate-independent same-function-differ axis. Honest NULL, no forcing.");
    puts("  (Lean verify this batch -- keeper self-judged, one veracity checker -- to conserve budget toward 2048;");
    puts("   the keeper space is near-complete and NULL is the calibrated default after 26 straight.)");
    puts("");
    puts("credit verify (1 checker, lean): 15 clean + 1 refined -- weight-decay stated penalty lambda*||w||^2 but");
    puts("  gradient lambda*w / update w(1-eta*lambda); a penalty of lambda*||w||^2 has gradient 2*lambda*w, so the");
    puts("  penalty is written (1/2)*lambda*||w||^2 for consistency. Attributions all hold:");
    puts("  Pascanu et al. 2013, Srivastava & Hinton 2014, Ba/Kiros/Hinton 2016, Hendrycks & Gimpel");
    puts("  2016, L2/Tikhonov, Vaswani 2017 (pos-enc, multi-head, warmup), Hochreiter & Schmidhuber 1997,");
    puts("  Cho et al. 2014, Kullback & Leibler 1951, Szegedy et al. 2016, Williams & Zipser 1989.");

    puts("");
    puts("seal ROOT_0 3be80268 . 608/2048 . 29.69% . the mind, kept from vanishing and from exploding");
    return 0;
}
