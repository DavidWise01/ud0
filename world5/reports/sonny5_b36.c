/* sonny5_b36.c  --  exec report for World V, batch 36: THE TYPE.
 *
 * 16 type-system mechanisms, each in dialogue with i13's most explicit gap: its
 * `check` ledger literally lists "types" under NOT COVERED. i13 is dynamically
 * typed (f64 only). So this report is a tiny TYPE CHECKER: it runs a bidirectional
 * check on a couple of terms (dart 255), then shows the batch's punchline -- a
 * phantom tag costs nothing at runtime -- and sorts the 16 by what i13 does.
 *
 * build:  gcc -O2 -std=c11 -o sonny5_b36 sonny5_b36.c
 * ROOT0-ATTRIBUTION-v1.0 . David Lee Wise (ROOT0) & AVAN (Claude/Anthropic)
 */
#include <stdio.h>

/* type tags */
enum { ILL = 0, T_INT = 1, T_BOOL = 2 };
static const char *tname(int t){ return t==T_INT?"Int":t==T_BOOL?"Bool":"<ill-typed>"; }

/* bidirectional application typing (dart 255): synthesize f : arg->res, check x
 * against arg, conclude res -- or ILL if the argument does not match. */
static int synth_app(int argT, int resT, int xT){
    return (xT == argT) ? resT : ILL;
}

/* the batch's punchline (dart 260): a phantom tag is erased -- two differently
 * "typed" values share one runtime representation, so their difference is 0. */
static double phantom_diff(double meters, double feet){ return meters - feet; }

/* each dart: does i13 get it FREE (by construction) or FORGO it (no type layer)? */
enum verdict { FREE, FORGO };
static const char *VN[] = { "FREE ", "FORGO" };
typedef struct { const char *feature; enum verdict v; } Dart;

int main(void) {
    /* run the type checker on two terms: f : Int -> Bool */
    int ok  = synth_app(T_INT, T_BOOL, T_INT);   /* f applied to an Int  */
    int bad = synth_app(T_INT, T_BOOL, T_BOOL);  /* f applied to a Bool  */
    printf("bidirectional check  f:Int->Bool\n");
    printf("  (f x), x:Int   -> %s\n", tname(ok));    /* Bool */
    printf("  (f x), x:Bool  -> %s\n\n", tname(bad)); /* ill-typed, rejected */

    /* phantom erasure: types cost nothing at runtime */
    printf("phantom  Length<Meters>100  vs  Length<Feet>100  -> runtime diff %.0f (tag erased)\n\n",
           phantom_diff(100.0, 100.0));

    Dart D[] = {
        {"parametric polymorphism", FREE},   /* one type -> uniform by necessity */
        {"effect system",           FREE},   /* no heap/IO -> effect set is {} */
        {"linear types",            FREE},    /* value semantics -> no aliasing (partial shadow) */
        {"algebraic data types",    FORGO},   /* encoded, but no exhaustiveness check */
        {"System F",                FORGO},   /* value half only; no type abstraction */
        {"subtyping",               FORGO},
        {"typeclasses",             FORGO},
        {"dependent types",         FORGO},   /* runtime bounds-check is the weak cousin */
        {"refinement types",        FORGO},   /* <- the one i13 could most plausibly adopt */
        {"existential types",       FORGO},
        {"row polymorphism",        FORGO},
        {"higher-kinded types",     FORGO},
        {"bidirectional typing",    FORGO},   /* <- the algorithm i13 would use if typed */
        {"progress & preservation", FORGO},   /* i13's ledger IS this, minus types */
        {"phantom types",           FORGO},
        {"the lambda cube",         FORGO},   /* i13 sits one step below its origin */
    };
    int N = (int)(sizeof D / sizeof D[0]), nfree = 0, nforgo = 0;
    puts("the 16, by what i13 does with each:");
    for (int i = 0; i < N; i++) {
        printf("  [%s] %s\n", VN[D[i].v], D[i].feature);
        if (D[i].v == FREE) nfree++; else nforgo++;
    }
    printf("\n  FREE (by construction) %d   FORGONE (no type layer) %d   (total %d)\n", nfree, nforgo, N);
    puts("  closest-to-adopt: refinement types + bidirectional typing (single-pass, prove-a-predicate).\n");

    /* keeper judgment */
    puts("keeper (16th judging): NULL -- unanimous 3 judges.");
    puts("  i13 is untyped BY DESIGN, so type features are forgone, not enacted. The deep");
    puts("  near-misses fail: dependent/refinement/soundness RESTATE i13's verify-before-run");
    puts("  identity (auto-NULL, per B35); parametricity is a property that HOLDS, not a");
    puts("  mechanism i13 enacts; linear-types i13 only shadows (no-aliasing), never enforces.");
    puts("  keepers stay 6 on 5 axes.");

    puts("\nseal ROOT_0 35b24394 . 261/2048 . 12.74% . the ledger says: types NOT COVERED");
    return 0;
}
