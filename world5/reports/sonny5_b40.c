/* sonny5_b40.c  --  exec report for World V, batch 40: THE ONE PASS.
 *
 * Streaming / bounded-memory mechanisms. The B39 panel asked for the opposite of a
 * theorem: a MECHANISM PROPERTY a correct-but-different impl LACKS. The property this
 * batch tests is the STREAMING DISCIPLINE -- touch each input ONCE, fold it into a
 * BOUNDED SUMMARY, and forget it; a correct BATCH alternative RETAINS the whole input.
 *
 * This report ENACTS the supplement test rather than asserting it: it runs Welford's
 * one-pass variance beside a batch variance. They must AGREE on the answer (both are
 * correct) yet DIFFER in what they retain -- the streaming reducer carries two numbers,
 * the batch reducer carries all n. That gap -- same result, different retention -- is
 * exactly the "property a correct-but-different mechanism lacks" the criterion demands.
 * It also runs the two other keeper shots i13 grounded: Morris (count by exponent) and
 * Frugal-1 (a median in one integer).
 *
 * build:  gcc -O2 -std=c11 -o sonny5_b40 sonny5_b40.c
 * ROOT0-ATTRIBUTION-v1.0 . David Lee Wise (ROOT0) & AVAN (Claude/Anthropic)
 */
#include <stdio.h>
#include <math.h>

/* ---- the streaming reducer: one pass, carry only [mean, M2] (Welford 1962) ---- */
typedef struct { double mean, M2; long n; } Welford;   /* the ENTIRE retained state */
static void welford_push(Welford *w, double x) {
    w->n++;
    double d = x - w->mean;
    w->mean += d / w->n;
    w->M2   += d * (x - w->mean);
}
static double welford_var(const Welford *w) { return w->n ? w->M2 / w->n : 0.0; }

/* ---- the batch reducer: correct, but it must RETAIN every sample ---- */
static double batch_var(const double *a, int n) {
    double m = 0; for (int i = 0; i < n; i++) m += a[i];   m /= n;
    double s = 0; for (int i = 0; i < n; i++) s += (a[i]-m)*(a[i]-m);
    return s / n;                                          /* two passes over stored a[] */
}

/* ---- Morris: the count lives in an exponent (sub-log memory, sized here) ---- */
static int morris_exponent(long N) { int c = 0; long v = 1; while (v < N) { v <<= 1; c++; } return c; }

/* ---- Frugal-1: a median estimate in ONE integer, nudged +/-1 ---- */
static long frugal1(const double *a, int n) {
    long est = 0;
    for (int i = 0; i < n; i++) { if (a[i] > est) est++; else if (a[i] < est) est--; }
    return est;
}

int main(void) {
    double data[8] = {2,4,4,4,5,5,7,9};   /* the same stream i13 grounded */
    int n = 8;

    /* enact the supplement: same answer, different retention */
    Welford w = {0,0,0};
    for (int i = 0; i < n; i++) welford_push(&w, data[i]);
    double vs = welford_var(&w), vb = batch_var(data, n);

    printf("stream of %d samples -> variance\n", n);
    printf("  streaming (Welford):  %.1f   retains [mean, M2] = 2 numbers   (state size: %zu bytes, independent of n)\n",
           vs, sizeof(Welford));
    printf("  batch (textbook)   :  %.1f   retains all %d samples           (state size: %d doubles, grows with n)\n",
           vb, n, n);
    printf("  same answer (%s), different retention  <-- the property a correct batch LACKS\n\n",
           fabs(vs - vb) < 1e-12 ? "agree" : "DISAGREE");

    /* the honest bound the panel was handed: i13 has no loops, so its recursion STACK is
       O(n) -- what is bounded is the SUMMARY carried, not total memory. In C the loop makes
       the summary genuinely O(1); on i13 the [mean,M2] state is O(1) but the call depth is n. */
    printf("honest bound: i13's only loop is recursion -> call stack O(n). i13 enacts the\n");
    printf("  bounded SUMMARY (two numbers for any n) and the single touch, not O(1) total memory.\n\n");

    long N = 1023;
    int c = morris_exponent(N + 1);
    printf("Morris: count ~%ld stored as exponent c=%d  -> estimate 2^%d-1 = %ld   (%d bits vs %d exact)\n",
           N, c, c, (1L << c) - 1, (int)ceil(log2(c + 1)), (int)ceil(log2(N + 1)));
    printf("  (the sub-log memory is bought with a coin i13 cannot flip -- sized here, not earned)\n\n");

    printf("Frugal-1: median of the stream in ONE integer -> %ld   (an exact median stores all %d)\n\n",
           frugal1(data, n), n);

    /* the 16, by what each one bounds */
    puts("the 16, and the bounded thing each one carries:");
    struct { const char *m; const char *carries; } R[] = {
        {"online-mean",           "one running number"},
        {"welford-variance",      "[mean, M2] -- KEEPER SHOT (single-pass, stable)"},
        {"exponential-moving-avg","one register, infinite fading memory"},
        {"simple-moving-average", "a window of k"},
        {"monotonic-queue",       "a deque of undominated candidates (near-miss)"},
        {"sliding-window-maximum","nothing new -- a WITNESS (all solvers agree)"},
        {"monotonic-stack",       "a stack of the undecided"},
        {"two-pointer",           "two indices, the sortedness pays"},
        {"morris-counting",       "an exponent -- KEEPER SHOT (sub-log, needs a coin)"},
        {"frugal-streaming",      "one integer -- KEEPER SHOT (fully enacted)"},
        {"lossy-counting",        "bounded counters + an error bound"},
        {"cusum",                 "one clamped accumulator"},
        {"prefix-sum",            "a built table (the discrete integral)"},
        {"difference-array",      "two marks (the discrete derivative)"},
        {"sliding-window",        "two forward-only bounds"},
        {"tumbling-window",       "one pane at a time"},
    };
    for (int i = 0; i < 16; i++) printf("  %-24s <- %s\n", R[i].m, R[i].carries);

    /* keeper verdict (adversarial panel, 3 judges) */
    puts("");
    puts("keeper (20th judging): NULL, UNANIMOUS 3-0 -- the seventh straight, and a new mode of");
    puts("  death. B38/B39 died by WITNESSING a theorem; B40 dies as a RESOURCE-PROPERTY masquerade.");
    puts("  The batch did aim at a mechanism property, not a theorem -- Welford's [mean,M2] is real");
    puts("  and a correct batch variance lacks it. But all three judges drew the same new wall:");
    puts("  THE RESOURCE-PROPERTY EXCLUSION -- the B39 supplement test is NECESSARY but NOT");
    puts("  SUFFICIENT. Every resource property (bounded memory, bounded time, fewer passes,");
    puts("  in-place, faster) passes 'a correct-but-different mechanism can lack it' trivially, yet");
    puts("  none is a keeper. A keeper must be a STRUCTURAL/ALGEBRAIC property of the OUTPUT-RELATION");
    puts("  -- what the output IS or how it relates to the input (reversible, generative, info-");
    puts("  conserving, dual-carrying, order-confluent) -- NOT how much the machine SPENDS.");
    puts("  THE TELL: Welford variance is BIT-IDENTICAL to batch variance; when the only delta is");
    puts("  the cost ledger, it is efficiency, not structure (the B39 convergence tell again).");
    puts("  Welford is squeezed both ways: its structural core (numerical stability) re-instantiates");
    puts("  Kahan; its streaming core is a resource property i13 cannot even enact -- no loops means");
    puts("  an O(n) recursion stack, so i13 threads a bounded SUMMARY but always pays O(n) memory,");
    puts("  cancelling the very saving that was the point. Morris is worse (its sub-log memory is");
    puts("  bought with an RNG i13 lacks -- sized, not run). GENERAL FORM: when the advantage is");
    puts("  bought with a resource i13 lacks (RNG, O(1)-loop space), i13 SIZES/SHAPES but never RUNS");
    puts("  it. The whole streaming/space family is dead on this machine. Keepers stay 6 on 5 axes.");
    puts("  Next: a property VISIBLE IN THE OUTPUT -- a semantic guarantee, not a cheaper path to");
    puts("  the same value. STOP firing streaming/bounded-memory/in-place/constant-time darts.");

    puts("");
    puts("seal ROOT_0 f2e80003 . 325/2048 . 15.87% . one pass, a bounded summary carried onward");
    return 0;
}
