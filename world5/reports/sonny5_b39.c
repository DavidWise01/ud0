/* sonny5_b39.c  --  exec report for World V, batch 39: THE RECONSTRUCTION.
 *
 * Recover a whole from parts, where a STRUCTURE is LOAD-BEARING: the recovery
 * works ONLY because the structure holds, and altering it makes recovery FAIL.
 * This is the B38 path forward -- a principle that CAUSES the computation, not one
 * it witnesses. So this report ENACTS the causation test: it computes the
 * Vandermonde determinant (the solvability certificate) and reconstructs a
 * polynomial ONLY when det != 0; collide a node and it refuses -- there is
 * nothing to recover.
 *
 * build:  gcc -O2 -std=c11 -o sonny5_b39 sonny5_b39.c
 * ROOT0-ATTRIBUTION-v1.0 . David Lee Wise (ROOT0) & AVAN (Claude/Anthropic)
 */
#include <stdio.h>

/* THE LOAD-BEARING STRUCTURE: Vandermonde det = prod_{i<j}(xj - xi).
 * != 0  iff the nodes are distinct  iff the interpolant exists and is unique. */
static double vdet(const double *x, int n) {
    double p = 1.0;
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            p *= (x[j] - x[i]);
    return p;
}

/* Newton reconstruction at t -- but ONLY if the structure permits it. Returns 1
 * on success (fills *out), 0 if the determinant vanished (recovery impossible). */
static int reconstruct(const double *x, const double *y, int n, double t, double *out) {
    if (vdet(x, n) == 0.0) return 0;               /* the recovery is CONTINGENT on det != 0 */
    /* n=3 Newton divided differences */
    double dd0 = y[0];
    double dd1 = (y[1] - y[0]) / (x[1] - x[0]);
    double d12 = (y[2] - y[1]) / (x[2] - x[1]);
    double dd2 = (d12 - dd1) / (x[2] - x[0]);
    *out = dd0 + dd1 * (t - x[0]) + dd2 * (t - x[0]) * (t - x[1]);
    return 1;
}

enum kind { INTERP, CODE, SIGNAL, ESTIMATE };
static const char *KN[] = { "INTERP  ", "CODE    ", "SIGNAL  ", "ESTIMATE" };
typedef struct { const char *m; enum kind k; const char *structure; } M;

int main(void) {
    /* ENACT the causation test: reconstruct iff the structure holds */
    double x[3]  = {0, 1, 2}, y[3] = {1, 3, 9};    /* samples of 2x^2 + 1 */
    double got;
    int ok = reconstruct(x, y, 3, 3.0, &got);
    printf("distinct nodes {0,1,2}: det = %.0f  -> reconstruct p(3) = %.0f  (recovery PROCEEDS)\n",
           vdet(x, 3), got);

    double xbad[3] = {1, 2, 2};                     /* a collided node */
    int ok2 = reconstruct(xbad, y, 3, 3.0, &got);
    printf("collided nodes {1,2,2}: det = %.0f  -> reconstruct %s  (recovery IMPOSSIBLE)\n\n",
           vdet(xbad, 3), ok2 ? "?!" : "REFUSED -- nothing to recover");
    (void)ok;

    /* an erasure recovered from a low-degree fit (the structure IS the redundancy) */
    double slope = (5.0 - 1.0) / (2.0 - 0.0), erased = 1.0 + slope * (1.0 - 0.0);
    printf("erasure: line y=2x+1, middle sample lost -> recovered %.0f from the two survivors\n\n", erased);

    M R[] = {
        {"Vandermonde (solvability)",  INTERP,   "det = prod(xj-xi) != 0 : the certificate"},
        {"Newton divided differences", INTERP,   "unique interpolant (distinct nodes)"},
        {"Neville tableau",            INTERP,   "unique interpolant"},
        {"barycentric",                INTERP,   "unique interpolant, stable weights"},
        {"erasure code",               CODE,     "data is a low-degree polynomial"},
        {"syndrome decoding",          CODE,     "syndrome depends on the error alone"},
        {"error locator",              CODE,     "positions encoded as polynomial roots"},
        {"fountain code",              CODE,     "XOR mixtures stay independent"},
        {"Nyquist-Shannon",            SIGNAL,   "signal is band-limited (< B)"},
        {"compressed sensing",         SIGNAL,   "signal is sparse"},
        {"Pade approximant",           SIGNAL,   "the function is rational (has poles)"},
        {"pseudoinverse",              ESTIMATE, "least-squares projection geometry"},
        {"normal equations",           ESTIMATE, "residual orthogonal to the model"},
        {"Kalman filter",              ESTIMATE, "certainty-weighted optimal gain"},
        {"Wiener filter",              ESTIMATE, "known signal/noise statistics"},
        {"Aitken delta^2",             ESTIMATE, "the error tail is geometric"},
    };
    int N = (int)(sizeof R / sizeof R[0]);
    puts("the 16, and the structure each recovery is CAUSED by:");
    for (int i = 0; i < N; i++)
        printf("  [%s] %-26s <- %s\n", KN[R[i].k], R[i].m, R[i].structure);
    puts("\n  alter any structure -- collide the nodes, raise the degree, drop the sparsity --");
    puts("  and the SAME parts recover nothing. the structure is load-bearing, not witnessed.\n");

    /* keeper verdict (adversarial panel, 3 judges) */
    puts("keeper (19th judging): NULL, unanimous 3-0 -- and the most valuable NULL yet.");
    puts("  Reconstruction clears distinctness + foreign-import but dies on causation, like");
    puts("  cross-ratio (B38). THE definitive keeper rule the panel drew: a keeper's principle");
    puts("  must be a SUPPLEMENT TO CORRECTNESS -- a property a correct-but-DIFFERENT mechanism");
    puts("  can LACK (Euler is correct but not reversible -> Verlet; a table is correct but");
    puts("  stores -> Stern-Brocot; naive sum loses the remainder -> Kahan). But 'recovery");
    puts("  succeeds iff the structure holds' is COEXTENSIVE WITH correctness -- every correct");
    puts("  reconstructor has it -- so i13 WITNESSES the theorem, cannot cause it. The tell:");
    puts("  Newton, Neville, barycentric ALL reconstruct 19 -- multiple correct mechanisms");
    puts("  agreeing IS the holds-not-enacted signature (uniqueness = implementation-independence).");
    puts("  The structure bears the load of the PROBLEM (the truss), not i13's mechanism (the");
    puts("  welder); gate (c) is about the welder. keepers stay 6 on 5 axes.");
    puts("  Next: a MECHANISM PROPERTY a correct alternative lacks (in-place, streaming,");
    puts("  reversible, idempotent) -- not a theorem coextensive with correctness.");

    puts("\nseal ROOT_0 93372801 . 309/2048 . 15.09% . recovery is CAUSED by structure");
    return 0;
}
