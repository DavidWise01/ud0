/* sonny5_b38.c  --  exec report for World V, batch 38: THE INVARIANT.
 *
 * 16 quantities preserved under a transformation group. Steered to MATH/PHYSICS
 * after four straight compiler-meta NULLs, to import a FOREIGN shape (symmetry)
 * i13 can enact. So this report ENACTS invariance: it computes a cross-ratio,
 * pushes the four points through a genuine Mobius transform, re-measures -- and
 * the number survives to machine precision. Then the same for a determinant
 * (shear) and a conic discriminant (rotation), and sorts the 16 by their group.
 *
 * build:  gcc -O2 -std=c11 -o sonny5_b38 sonny5_b38.c -lm
 * ROOT0-ATTRIBUTION-v1.0 . David Lee Wise (ROOT0) & AVAN (Claude/Anthropic)
 */
#include <stdio.h>

/* the projective invariant: cross-ratio of four points */
static double cross_ratio(double a, double b, double c, double d) {
    return ((c - a) * (d - b)) / ((c - b) * (d - a));
}
static double mobius(double z) { return (2.0 * z + 1.0) / (z + 3.0); }

/* the linear invariant: 2x2 determinant */
static double det2(double a, double b, double c, double d) { return a * d - b * c; }

/* the invariant-theory invariant: conic discriminant */
static double disc(double A, double B, double C) { return B * B - 4.0 * A * C; }

typedef struct { const char *invariant; const char *group; } Inv;

int main(void) {
    /* ENACT projective invariance: transform, re-measure */
    double p[4] = { 0, 1, 2, 3 };
    double cr0 = cross_ratio(p[0], p[1], p[2], p[3]);
    double cr1 = cross_ratio(mobius(p[0]), mobius(p[1]), mobius(p[2]), mobius(p[3]));
    printf("cross-ratio   before %.16f\n", cr0);
    printf("  after Mobius (2z+1)/(z+3)  %.16f   diff %.1e  (projective invariant)\n", cr1, cr0 - cr1);

    /* ENACT linear invariance: shear, re-measure (exact) */
    double d0 = det2(2, 1, 1, 3);
    double d1 = det2(2 + 2 * 1, 1 + 2 * 3, 1, 3);
    printf("determinant   before %.0f   after shear %.0f   diff %.0f  (linear invariant)\n", d0, d1, d0 - d1);

    /* ENACT rotation invariance of the conic classifier */
    double c0 = disc(2, 0, 1), c1 = disc(1, 0, 2);
    printf("conic B^2-4AC before %.0f  after 90-rotation %.0f  diff %.0f  (still an ellipse)\n\n", c0, c1, c0 - c1);

    Inv I[] = {
        {"cross-ratio",         "projective (Mobius)"},
        {"determinant",         "linear (shear / basis)"},
        {"conic discriminant",  "rotation (invariant theory)"},
        {"Gauss-Bonnet chi",    "any bending (topological)"},
        {"genus",               "any stretching (topological)"},
        {"mapping degree",      "homotopy (integer, cannot jump)"},
        {"Poincare-Hopf sum",   "any vector field (Euler class)"},
        {"argument winding",    "loop deformation (Cauchy)"},
        {"rotation number",     "smooth re-coordinate (dynamics)"},
        {"writhe (Lk=Tw+Wr)",   "ribbon deformation"},
        {"holonomy",            "path inside the loop (only area)"},
        {"orbit x stabilizer",  "choice of element (= |G|)"},
        {"Cauchy-Frobenius",    "the symmetry group (orbit count)"},
        {"Noether momentum",    "time (from space symmetry)"},
        {"Liouville volume",    "time (Hamiltonian flow)"},
        {"first integral E",    "time (along the orbit)"},
    };
    int N = (int)(sizeof I / sizeof I[0]);
    puts("the 16 invariants, by the transformation that cannot change them:");
    for (int i = 0; i < N; i++)
        printf("  %-20s invariant under  %s\n", I[i].invariant, I[i].group);

    puts("");
    /* KEEPER VERDICT (adversarial panel, 3 judges) */
    puts("keeper (18th judging): NULL, 2-1 -- the CLOSEST miss yet.");
    puts("  the candidate axis INVARIANCE-UNDER-A-GROUP is genuinely DISTINCT from all 5");
    puts("  (a non-involution group fixing a scalar != self-inverse; != Kahan accounting;");
    puts("  != crdt join) and IMPORTS a foreign shape (projective geometry) -- clearing the");
    puts("  two gates that killed B34-B37. It dies only on ENACTMENT: the LOAD-BEARING test.");
    puts("  Alter i13's mechanism and the invariance SURVIVES -- it is a theorem of the Mobius");
    puts("  group, true of any correct implementation; i13 WITNESSES it, does not CAUSE it.");
    puts("  (crdt's join CAUSES confluence; eps^2=0 CAUSES the derivative -- remove them and");
    puts("  the principle dies.) A property that HOLDS, not a mechanism ENACTED. keepers stay 6.");
    puts("  Path forward: a dart that USES an invariant to RECONSTRUCT (un-distort an image via");
    puts("  the cross-ratio) would make the invariance load-bearing -- and could clear the bar.");

    puts("\nseal ROOT_0 f7e1e286 . 293/2048 . 14.31% . invariance ENACTED: transform, re-measure, unchanged");
    return 0;
}
