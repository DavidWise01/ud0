/* sonny5_b58.c  --  exec report for World V, batch 58: THE MIND.
 *
 * The mechanics of learning. Every number below mirrors the real i13.exe output
 * (cells58/nn_*.i13). Grounded with f64 arithmetic, native recursion (the training loops),
 * and Taylor/Newton series for exp, ln, sqrt (activations, losses, Adam) - here C uses <math.h>.
 * Keeper shot: the-attention (softmax(QK^T/sqrt d)V - routing information by content).
 * The engine under the AI the corpus is built around.
 *
 * build:  gcc -O2 -std=c11 -o sonny5_b58 sonny5_b58.c -lm   (put mingw64/bin on PATH first)
 * ROOT0-ATTRIBUTION-v1.0 . David Lee Wise (ROOT0) & AVAN (Claude/Anthropic) . |||..()
 */
#include <stdio.h>
#include <math.h>

int main(void){
    puts("THE MIND -- the mechanics of learning on i13 (matches cells58/nn_*.i13):\n");

    /* 598 gradient descent: minimize (x-3)^2 */
    { double x=0,lr=0.1; for(int i=0;i<60;i++) x=x-lr*2*(x-3);
      printf("  gradient-descent : 60 steps on (x-3)^2 -> x = %.5f (the minimum 3)\n", x); }

    /* 599 chain rule: d/dx (2x+1)^2 at x=1 */
    { int x=1; int g=2*x+1; int chain=(2*g)*2, direct=8*x+4;
      printf("  chain-rule       : d/dx(2x+1)^2 = %d (chain) = %d (direct) -- backprop\n", chain, direct); }

    /* 600 relu */
    { printf("  relu             : relu(-2)=%d relu(3)=%d ; grad(3)=%d no vanishing\n", 0, 3, 1); }

    /* 601 sigmoid */
    { double s0=1/(1+exp(0)), s2=1/(1+exp(-2)); printf("  sigmoid          : sigma(0)=%.1f sigma(2)=%.3f ; max deriv %.2f (vanishing culprit)\n", s0, s2, s0*(1-s0)); }

    /* 602 softmax([1,2,3]) */
    { double e1=exp(1),e2=exp(2),e3=exp(3),Z=e1+e2+e3;
      printf("  softmax          : [%.3f, %.3f, %.3f] sum %.0f -- a distribution\n", e1/Z,e2/Z,e3/Z,(e1+e2+e3)/Z); }

    /* 603 cross-entropy */
    { printf("  cross-entropy    : -ln(0.665)=%.3f (unsure) vs -ln(0.95)=%.3f (confident) ; punishes confident-wrong\n", -log(0.665), -log(0.95)); }

    /* 604 perceptron learns AND */
    { int y=1, yhat0=(0>0)?1:0; int err=y-yhat0; int w0=err,w1=err,b=err; int yhat1=((w0*1+w1*1+b)>0)?1:0;
      printf("  perceptron       : AND (1,1): predict %d wrong -> update w=(%d,%d) b=%d -> predict %d correct\n", yhat0,w0,w1,b,yhat1); }

    /* 605 momentum terminal velocity */
    { double v=0; for(int i=0;i<90;i++) v=0.9*v+1; printf("  momentum         : steady gradient, beta 0.9 -> v = %.3f (1/(1-0.9)=10)\n", v); }

    /* 606 adam scale-invariance */
    { double s2=2.0/sqrt(4.0), s100=100.0/sqrt(10000.0); printf("  adam             : m/sqrt(v) = %.0f (g=2) = %.0f (g=100) -- scale-invariant\n", s2, s100); }

    /* 607 weight init */
    { int n=100; int bad=n*1*1, he=(n*2*1)/n; printf("  weight-init      : naive Var(out)=%d (explodes) ; He 2/n -> %d (stable)\n", bad, he); }

    /* 608 batch norm */
    { double b[4]={2,4,6,8}; double mu=(b[0]+b[1]+b[2]+b[3])/4; double var=0; for(int i=0;i<4;i++)var+=(b[i]-mu)*(b[i]-mu); var/=4;
      double nm=0; for(int i=0;i<4;i++)nm+=(b[i]-mu)/sqrt(var); nm/=4;
      printf("  batch-norm       : [2,4,6,8] mean %.0f var %.0f std %.3f -> new mean %.0f\n", mu, var, sqrt(var), nm); }

    /* 609 attention  [KEEPER SHOT] */
    { double d=2,sc=sqrt(d); double s1=1/sc,s2=0/sc; double e1=exp(s1),e2=exp(s2);
      printf("  attention*       : scaled %.3f ; softmax weights [%.3f, %.3f] -- attends to the matching key\n", s1, e1/(e1+e2), e2/(e1+e2)); }

    /* 610 embedding analogy */
    { int rx=3-2+0, ry=1-0+2; printf("  embedding        : king(3,1)-man(2,0)+woman(0,2) = (%d,%d) = queen -- meaning is a direction\n", rx, ry); }

    /* 611 vanishing gradient */
    { printf("  vanishing-grad   : sigmoid (1/4)^10 = %.1e (vanished) vs ReLU 1^10 = %.0f (survives)\n", pow(0.25,10), pow(1.0,10)); }

    /* 612 universal approximation: a bump from 3 ReLUs */
    { double bump0=fmax(0,0)-2*fmax(0,-1)+fmax(0,-2);
      double bump1=fmax(0,1)-2*fmax(0,0)+fmax(0,-1);
      double bump2=fmax(0,2)-2*fmax(0,1)+fmax(0,0);
      printf("  universal-approx : bump(0)=%.0f bump(1)=%.0f bump(2)=%.0f -- 3 ReLUs; sums trace any function\n", bump0,bump1,bump2); }

    /* 613 residual connection */
    { int df=0; int res=1+df, plain=df; printf("  residual         : f' vanished -> residual grad 1+%d = %d (survives), plain %d (dead) ; 1^50 = %.0f through 50 layers\n", df, res, plain, pow(1.0,50)); }

    puts("");
    struct { const char *m; const char *gate; } R[] = {
        {"gradient-descent","an iteration to a local min (B39/B40)"},
        {"chain-rule","exact calculus identity, backprop's bookkeeping (B39)"},
        {"relu","a pinned function max(0,x) + subgradient (B39)"},
        {"sigmoid","a pinned function; the <=1/4 derivative is analytic (B39)"},
        {"softmax","a pinned map to a distribution (B39)"},
        {"cross-entropy","a pinned information measure -ln(p_true) (B39)"},
        {"perceptron","a learning rule; convergence for separable data (B39)"},
        {"momentum","a pinned recurrence; 1/(1-beta) geometric limit (B39)"},
        {"adam","a pinned update; scale-invariance is algebra (B39)"},
        {"weight-init","a variance identity; He/Xavier scale (B39)"},
        {"batch-norm","a pinned normalization (x-mu)/sigma (B39)"},
        {"attention*","KEEPER SHOT: softmax(QK^T/sqrt d)V is a pinned function of Q,K,V (B39); content-routing computed identically by all"},
        {"embedding","pinned vector arithmetic over learned coords (B39/B44)"},
        {"vanishing-gradient","a product identity -> 0 (B39)"},
        {"universal-approximation","an existence theorem (B39)"},
        {"residual","an identity path; 1+f' is pinned calculus (B39)"},
    };
    puts("the 16, and where each lands under the gates:");
    for(int i=0;i<16;i++) printf("  %-18s <- %s\n", R[i].m, R[i].gate);

    /* ---- keeper verdict (3 diverse-lens judges + 4 veracity checkers + 1 completeness critic) ---- */
    puts("");
    puts("keeper (38th judging): NULL  -- no new axis. 25 straight NULL.");
    puts("  shot the-attention (softmax(QK^T/sqrt d)V): the most STRUCTURAL card -- it routes information by");
    puts("        CONTENT, no fixed wiring, any token reaching any other. But the attention weights are a PINNED");
    puts("        function of Q,K,V: every correct implementation computes the identical weights (B39/WITNESSED).");
    puts("        Content-based routing is an architecture, not a same-function DOF. NULL.");
    puts("  shot the-residual-connection (y = x + f(x)): the +1 identity term keeps the gradient from vanishing to");
    puts("        zero (1+f' -> 1, not 0, as f' vanishes) -- pinned calculus, and the identity is ADDED (not an");
    puts("        involution, so not the self-inverse");
    puts("        axis); a fixed architecture choice. NULL.");
    puts("  the batch reduces to: pinned functions/identities (B39, all 16) -- activations, losses, updates, and");
    puts("        theorems. Learning's mechanics are exact computations; every correct net computes them the same.");
    puts("  keeper space holds: 6 keepers on 5 axes. Honest NULL, no forcing.");
    puts("");
    puts("credit verify (4 checkers): 15 clean + 1 refined -- the residual card claimed 1+f' >= 1; corrected");
    puts("  (f' can be negative, so not bounded below by 1; the +1 identity term keeps the gradient from vanishing");
    puts("  to zero, which is the real point). Attributions all hold:");
    puts("  Cauchy 1847, Leibniz/chain rule (Werbos 1974, RHW 1986), Nair & Hinton 2010,");
    puts("  the logistic curve, Bridle 1990, Shannon, Rosenblatt 1958 (Minsky-Papert 1969), Polyak 1964,");
    puts("  Kingma & Ba 2014, Glorot 2010 / He 2015, Ioffe & Szegedy 2015, Bahdanau 2014 / Vaswani 2017,");
    puts("  Mikolov 2013 (word2vec), Hochreiter 1991, Cybenko 1989 / Hornik 1991, He et al. 2015 (ResNet).");

    puts("");
    puts("seal ROOT_0 1bde6e39 . 592/2048 . 28.91% . the whole of learning, in one repeated step");
    return 0;
}
