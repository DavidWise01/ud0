/* sonny5_b35.c  --  exec report for World V, batch 35: THE OPTIMIZER.
 *
 * 16 classic compiler-optimization passes, judged against i13 -- a single-pass,
 * correctness-first validator whose creed is EXECUTION != COMMIT. So this report
 * is itself a tiny pass manager: it RUNS the one pass that fits i13's grain
 * (bounds-check elimination), then sorts all 16 by the creed and prints the
 * honest pipeline that survives.
 *
 * build:  gcc -O2 -std=c11 -o sonny5_b35 sonny5_b35.c
 * ROOT0-ATTRIBUTION-v1.0 . David Lee Wise (ROOT0) & AVAN (Claude/Anthropic)
 */
#include <stdio.h>

/* Each pass falls into one bucket once you ask "does it survive EXECUTION != COMMIT?" */
enum verdict { FIT, FREE, REPORT, NA };
static const char *VNAME[] = { "FIT   ", "FREE  ", "REPORT", "N/A   " };

typedef struct { const char *pass; enum verdict v; const char *why; } Pass;

/* THE ONE PASS I-13 SHOULD RUN, executed for real: remove a bounds check only
 * where the index is a compile-time literal proven in [0,len). The runtime index
 * (-1 here) keeps its guard. This is dart 241, and its number is 4 of 5. */
static int bce_removable(const int *idx, int n, int len) {
    int removable = 0;
    for (int i = 0; i < n; i++)
        if (idx[i] >= 0 && idx[i] < len)   /* proven safe -> discharge the check */
            removable++;
    return removable;                       /* idx[i] < 0 == runtime -> stays guarded */
}

int main(void) {
    /* run the star pass */
    int idx[] = { 0, 1, 2, 3, -1 };
    int removable = bce_removable(idx, 5, /*len=*/4);
    printf("bounds-check elimination (dart 241): %d of 5 checks removable by proof\n\n", removable);

    Pass P[] = {
        {"bounds-check elimination", FIT,   "only deletes a check the validator PROVED redundant"},
        {"peephole (dead-op)",       FIT,   "removes an op that can never run (Op::Attr already gone)"},
        {"def-use chains",           FREE,  "single-assignment -> near-SSA, chains exact"},
        {"escape analysis",          FREE,  "value semantics -> nothing escapes, by construction"},
        {"scalar replacement",       FREE,  "no aliasing -> load-once is safe and idiomatic"},
        {"basic block",              FREE,  "i13 already reports regions (its block unit)"},
        {"reaching definitions",     FREE,  "one-bit version already runs (return-totality)"},
        {"common-subexpr elim",      REPORT,"warn, do not rewrite: the ops that run are the ops you wrote"},
        {"available expressions",    REPORT,"diagnose redundancy, never silently reuse"},
        {"cyclomatic complexity",    REPORT,"a waste metric for the ledger"},
        {"loop-invariant motion",    NA,    "no loops -- the invariant is a threaded recursion arg"},
        {"induction variable",       NA,    "no loops -- the counter is an explicit arg"},
        {"interference graph",       NA,    "no registers -- the stack IS the allocation"},
        {"branch prediction",        NA,    "no speculation -- ARRIVAL != EXECUTION"},
        {"inlining",                 NA,    "opt-in only -- it hides a frame the spot-log witnesses"},
        {"sea of nodes",             NA,    "opposite IR on purpose -- i13 fixes the schedule to verify it"},
    };
    int N = (int)(sizeof P / sizeof P[0]);

    int tally[4] = {0,0,0,0};
    puts("the 16, sorted by the creed:");
    for (int i = 0; i < N; i++) {
        printf("  [%s] %-24s -- %s\n", VNAME[P[i].v], P[i].pass, P[i].why);
        tally[P[i].v]++;
    }
    printf("\n  FIT %d   FREE %d   REPORT %d   N/A %d   (total %d)\n\n",
           tally[FIT], tally[FREE], tally[REPORT], tally[NA], N);

    /* the pipeline that survives = the FIT passes */
    printf("the multipass i13 would ship:  check");
    for (int i = 0; i < N; i++)
        if (P[i].v == FIT) printf(" -> %s", P[i].pass);
    printf(" -> emit\n");
    puts("  (both only ever delete what can never run)\n");

    /* keeper judgment */
    puts("keeper (14th judging): NULL -- unanimous 3 judges.");
    puts("  bounds-check elimination is the near-miss, but \"a proof discharges the");
    puts("  runtime guard\" is i13's OWN identity restated, not a new axis. keepers stay 6.");

    puts("\nseal ROOT_0 88dd52f1 . 245/2048 . 11.96% . 16 darts + THE MULTIPASS milestone");
    return 0;
}
