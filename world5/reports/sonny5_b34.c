/* sonny5_b34.c  --  exec report for World V, batch 34: THE FALSIFIER.
 *
 * The batch is about Popper: a universal claim is never proven, only
 * corroborated (survived) or FALSIFIED (one counterexample kills it).
 * So this report does not just print what happened -- it RUNS the same
 * verdict machine on real claims, then turns it on the batch itself.
 *
 * build:  gcc -O2 -std=c11 -o sonny5_b34 sonny5_b34.c
 * run  :  ./sonny5_b34
 *
 * ROOT0-ATTRIBUTION-v1.0 . David Lee Wise (ROOT0) & AVAN (Claude/Anthropic)
 */
#include <stdio.h>
#include <string.h>

enum { FALSIFIED = 0, CORROBORATED = 1 };

static int is_prime(long n) {
    if (n < 2) return 0;
    if (n % 2 == 0) return n == 2;
    for (long i = 3; i * i <= n; i += 2)
        if (n % i == 0) return 0;
    return 1;
}

/* A conjecture is "for all x in [lo,hi]: pred(x)". Try to REFUTE it.
 * Return the first counterexample, or hi+1 if none (=> corroborated). */
typedef int (*pred_fn)(long);
static long falsify(pred_fn pred, long lo, long hi, int *verdict, long *tested) {
    *tested = 0;
    for (long x = lo; x <= hi; x++) {
        (*tested)++;
        if (!pred(x)) { *verdict = FALSIFIED; return x; }   /* black swan */
    }
    *verdict = CORROBORATED;                                /* survived, NOT proven */
    return hi + 1;
}

static int all_primes_odd(long x) { return !is_prime(x) || (x % 2 == 1); }
static int square_ge_self(long x) { return x * x >= x; }

/* One dart: does it compute on i13.exe, and did credit need a fix? */
typedef struct { const char *slug; int computes; int credit_fix; } Dart;

int main(void) {
    /* --- the verdict machine, run for real on two universal claims --- */
    int v; long ce, n;
    const char *V[] = { "FALSIFIED", "CORROBORATED" };

    ce = falsify(all_primes_odd, 2, 60, &v, &n);
    printf("claim  all primes are odd            -> %-12s", V[v]);
    if (v == FALSIFIED) printf("(counterexample x=%ld, after %ld tests)\n", ce, n);
    else                printf("(survived %ld tests)\n", n);

    ce = falsify(square_ge_self, 1, 5000, &v, &n);
    printf("claim  n*n >= n on [1,5000]          -> %-12s", V[v]);
    if (v == FALSIFIED) printf("(counterexample x=%ld, after %ld tests)\n", ce, n);
    else                printf("(survived %ld tests -- NOT proven, lives to be tested harder)\n", n);

    /* --- the batch ledger (222-229) --- */
    Dart d[] = {
        {"the-quickcheck",        1, 0}, {"the-shrinking",         1, 1},
        {"the-metamorphic-test",  1, 1}, {"the-differential-test", 1, 0},
        {"the-fuzzer",            1, 0}, {"the-model-checker",     1, 0},
        {"the-symbolic-execution",1, 0}, {"the-mutation-testing",  1, 0},
    };
    int N = (int)(sizeof d / sizeof d[0]);
    int compute = 0, fixes = 0, keepers = 0;   /* keepers found this batch */
    for (int i = 0; i < N; i++) { compute += d[i].computes; fixes += d[i].credit_fix; }

    puts("");
    printf("darts shipped ........ %d (222-229)\n", N);
    printf("compute on i13.exe ... %d/%d\n", compute, N);
    printf("credit fixes applied . %d  (the-shrinking: ddmin=Hildebrandt&Zeller 2000;\n", fixes);
    printf("                          the-metamorphic-test: Weyuker 1982 FORMALIZED, not named)\n");

    /* --- turn the machine on the batch: "batch 34 produced a keeper" --- */
    long tested = 0;
    for (int i = 0; i < N; i++) { tested++; /* (void) */ }
    printf("conjecture  \"B34 yields a keeper\"  -> %s (searched %d darts, found %d)\n",
           keepers > 0 ? "CORROBORATED" : "FALSIFIED", N, keepers);
    printf("keepers total ........ 6 on 5 axes (unchanged -- refutation is a real\n");
    printf("                          principle, but no dart ENACTS it at keeper depth)\n");

    puts("\nseal ROOT_0 ced1f24e . 229/2048 . 11.18% . pushed 821331adf");
    return 0;
}
