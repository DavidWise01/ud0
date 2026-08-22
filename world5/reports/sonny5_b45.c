/* sonny5_b45.c  --  exec report for World V, batch 45: THE SYMMETRY.
 *
 * Equivariance: f(g.x) = g.f(x), the output COMMUTING with a transformation. The genuine
 * remaining shot at a 6th axis -- it settles the enacted-vs-witnessed line batch 38 left open:
 * invariance (f(g.x)=f(x)) was ruled WITNESSED; is ENACTED equivariance (guaranteed by the
 * mechanism's construction, so a correct-but-different mechanism LACKS it) any different?
 *
 * build:  gcc -O2 -std=c11 -o sonny5_b45 sonny5_b45.c
 * ROOT0-ATTRIBUTION-v1.0 . David Lee Wise (ROOT0) & AVAN (Claude/Anthropic)
 */
#include <stdio.h>

/* the keeper-shot contrast: double is reflection-equivariant, prefix-sum is not */
static void dbl(const int*a,int*o,int n){ for(int i=0;i<n;i++)o[i]=a[i]*2; }
static void pfx(const int*a,int*o,int n){ int s=0; for(int i=0;i<n;i++){s+=a[i];o[i]=s;} }
static void rev(const int*a,int*o,int n){ for(int i=0;i<n;i++)o[i]=a[n-1-i]; }
static int eq(const int*a,const int*b,int n){ for(int i=0;i<n;i++)if(a[i]!=b[i])return 0; return 1; }

int main(void){
    int x[3]={1,2,3}, t1[3],t2[3],t3[3],t4[3];
    /* double: f(rev x) == rev(f x) ? */
    rev(x,t1,3); dbl(t1,t2,3);   dbl(x,t3,3); rev(t3,t4,3);
    int equi_double = eq(t2,t4,3);
    /* prefix-sum: f(rev x) == rev(f x) ? */
    rev(x,t1,3); pfx(t1,t2,3);   pfx(x,t3,3); rev(t3,t4,3);
    int equi_prefix = eq(t2,t4,3);

    puts("THE SYMMETRY -- equivariance f(g.x) = g.f(x):\n");
    printf("  double     : f(reverse x) == reverse(f x) ?  %d   (equivariant)\n", equi_double);
    printf("  prefix-sum : f(reverse x) == reverse(f x) ?  %d   (a correct map that LACKS it)\n", equi_prefix);
    puts("  -> equivariance is a property a correct-but-different map can lack.\n");

    /* convolution: shared kernel -> shift-equivariance */
    int a[4]={1,2,3,4}, as[4]={9,1,2,3}, y[3], ys[3];
    for(int i=0;i<3;i++){ y[i]=a[i]+a[i+1]; ys[i]=as[i]+as[i+1]; }
    printf("  convolution: conv([1,2,3,4],[1,1]) = [%d,%d,%d]; shift the input, output shifts: %s\n\n",
           y[0],y[1],y[2], (y[0]==ys[1]&&y[1]==ys[2])?"shift-equivariant":"no");

    struct { const char *m; const char *v; } R[] = {
        {"equivariance",     "f(g.x)=g.f(x) -- KEEPER SHOT (enacted, or witnessed like B38?)"},
        {"invariance",       "f(g.x)=f(x) -- batch 38: WITNESSED"},
        {"convolution",      "weight-sharing -> shift-equivariance -- KEEPER SHOT (by construction)"},
        {"translation-equiv","no special position; Noether -> momentum"},
        {"permutation-inv",  "a function of a SET (Deep Sets) -- witnessed"},
        {"symmetric-function","Vieta: coefficients ARE symmetric fns of roots -- witnessed"},
        {"antisymmetry",     "swap -> negate; equal -> 0 (Pauli) -- self-inverse flavour"},
        {"group-action",     "g.(h.x)=(g.h).x -- a group axiom (witnessed)"},
        {"orbit",            "|orbit||stab|=|G| -- a counting theorem (witnessed)"},
        {"reflection-sym",   "x=reverse(x) -- a recognizer + the self-inverse group Z2"},
        {"rotational-sym",   "rot(x,k)=x -- witnessed; crystallographic restriction"},
        {"parity-operator",  "P^2=1 (involution) -- self-inverse; Wu 1956 broke it"},
        {"gauge-invariance", "potential+C -> same force -- invariance-of-observable + relabeling"},
        {"scale-invariance", "f(lx)=l^k f(x) -- homogeneity, witnessed"},
        {"self-duality",     "own dual -- a fixed point of duality (B43) + involution"},
        {"homomorphism",     "f(a.b)=f(a).f(b) -- structure-preservation, witnessed (B38 shape)"},
    };
    puts("the 16, and where each lands under the gates:");
    for(int i=0;i<16;i++) printf("  %-18s <- %s\n", R[i].m, R[i].v);

    /* keeper verdict (adversarial panel, 3 judges) */
    puts("");
    puts("keeper (25th judging): NULL, UNANIMOUS 3-0 -- the twelfth straight, and it DECISIVELY");
    puts("  closes the enacted-vs-witnessed line batch 38 left open. Equivariance is WITNESSED:");
    puts("  the decider -- do a convolution and a per-position filter that agree on ALL inputs have");
    puts("  the same equivariance? YES, necessarily; they ARE the same function, and f(g.x)=g.f(x)");
    puts("  is a for-all-x predicate on the input-output GRAPH, pinned by the function alone with");
    puts("  zero reference to the mechanism. (The 'tune a filter on the tested input' escape is a");
    puts("  DIFFERENT function.) It survives the mechanism-swap-holding-correctness-fixed test ->");
    puts("  witnessed, exactly as B38's invariance did.");
    puts("");
    puts("  THE FUNCTION-PROPERTY GATE (the campaign's sharpest characterization of the boundary):");
    puts("  a property is WITNESSED if preserved by every mechanism computing the SAME function");
    puts("  (for-all-x over the graph). A KEEPER is precisely a property two mechanisms computing");
    puts("  the SAME function can DIFFER on -- extra structure carried alongside the output, free to");
    puts("  keep or drop while the output stays bit-fixed (reversibility, remainder, the dual channel).");
    puts("  Everything that commutes-with / is-fixed-by a group action (invariance, equivariance,");
    puts("  homomorphism, symmetry) is graph-pinned -> witnessed -> NULL. The 5 keepers survive");
    puts("  because they live under a LOOSE-but-correct spec (a family of acceptable outputs).");
    puts("  Keepers stay 6 on 5 axes; near-complete read now has a crisp boundary, not just a tally.");

    puts("");
    puts("seal ROOT_0 d0475a0b . 405/2048 . 19.78% . f(g.x) = g.f(x)");
    return 0;
}
