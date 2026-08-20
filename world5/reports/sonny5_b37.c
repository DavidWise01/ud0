/* sonny5_b37.c  --  exec report for World V, batch 37: THE MACHINE.
 *
 * 16 abstract machines / execution models. i13's runtime IS a stack machine (the
 * IVM), so this report is itself one: a tiny stack VM runs a bytecode program
 * (like i13's IVM, tracking peak stack), then a trampoline demonstrates the one
 * REAL i13 frontier -- constant-stack tail recursion past i13's 4096-frame ceiling.
 *
 * build:  gcc -O2 -std=c11 -o sonny5_b37 sonny5_b37.c
 * ROOT0-ATTRIBUTION-v1.0 . David Lee Wise (ROOT0) & AVAN (Claude/Anthropic)
 */
#include <stdio.h>

/* ---- a stack VM (dart 262/264): push operands, an operator pops the top ---- */
enum { PUSH, ADD, SUB, MUL };
typedef struct { int op, arg; } Insn;

static int vm_run(const Insn *code, int n, int *peak_stack) {
    int st[64], sp = 0; *peak_stack = 0;
    for (int i = 0; i < n; i++) {
        switch (code[i].op) {
            case PUSH: st[sp++] = code[i].arg; break;
            case ADD:  { int b = st[--sp], a = st[--sp]; st[sp++] = a + b; } break;
            case SUB:  { int b = st[--sp], a = st[--sp]; st[sp++] = a - b; } break;
            case MUL:  { int b = st[--sp], a = st[--sp]; st[sp++] = a * b; } break;
        }
        if (sp > *peak_stack) *peak_stack = sp;
    }
    return st[0];
}

/* ---- a trampoline (dart 275): sum 1..n in ONE stack frame ----
 * each bounce returns the next thunk instead of calling it, so the C stack
 * never grows -- exactly the trick that lifts i13's E0503 4096-frame ceiling. */
static int t_i, t_n, t_acc, t_bounces;
typedef void (*Thunk)(void);
static Thunk t_next;
static void t_step(void) {
    if (t_i > t_n) { t_next = 0; return; }
    t_acc += t_i; t_i++; t_bounces++; t_next = t_step;   /* return, don't recurse */
}
static int trampoline_sum(int n, int *bounces) {
    t_i = 1; t_n = n; t_acc = 0; t_bounces = 0; t_next = t_step;
    while (t_next) t_next();                              /* the bounce loop */
    *bounces = t_bounces; return t_acc;
}

enum rel { IS, FORGO, FRONTIER };
static const char *RN[] = { "IS      ", "FORGO   ", "FRONTIER" };
typedef struct { const char *machine; enum rel r; } M;

int main(void) {
    /* run the stack VM on (2 3 +) 4 *  = 20 */
    Insn prog[] = { {PUSH,2},{PUSH,3},{ADD,0},{PUSH,4},{MUL,0} };
    int peak, result = vm_run(prog, 5, &peak);
    printf("stack VM  2 3 + 4 *  = %d   (peak stack %d, like i13's IVM report)\n", result, peak);

    /* the trampoline frontier */
    int bounces, s = trampoline_sum(100, &bounces);
    printf("trampoline  sum 1..100 = %d   (%d bounces, 1 C frame; i13 nests to depth 101, ceiling 4096)\n\n",
           s, bounces);

    M mm[] = {
        {"stack machine",              IS},   /* i13's IVM literally is one */
        {"bytecode",                   IS},   /* i13 compiles to ~19 IVM opcodes */
        {"three-address code",         IS},   /* i13's IVM stream is TAC-shaped */
        {"A-normal form",              IS},   /* i13's syntax is essentially ANF */
        {"closure conversion",         IS},   /* i13 starts post-conversion (no closures) */
        {"supercombinator",            IS},   /* every i13 function is one */
        {"register machine",           FORGO},
        {"CEK machine",                FORGO}, /* control stays implicit, no call/cc */
        {"CESK machine",               FORGO}, /* no store -> value semantics */
        {"categorical abstract machine",FORGO},
        {"ZINC machine",               FORGO}, /* no currying to optimize */
        {"spineless tagless G-machine",FORGO}, /* i13 is strict, not lazy */
        {"Warren abstract machine",    FORGO}, /* evaluates, does not search */
        {"normalization by evaluation",FORGO},
        {"explicit substitution",      FORGO}, /* i13 does it as a lookup */
        {"the trampoline",             FRONTIER},
    };
    int N = (int)(sizeof mm / sizeof mm[0]), n_is=0, n_fo=0, n_fr=0;
    puts("the 16, by how i13's machine relates:");
    for (int i = 0; i < N; i++) {
        printf("  [%s] %s\n", RN[mm[i].r], mm[i].machine);
        if (mm[i].r==IS) n_is++; else if (mm[i].r==FORGO) n_fo++; else n_fr++;
    }
    printf("\n  IS %d   FORGOES %d   FRONTIER %d   (total %d)\n", n_is, n_fo, n_fr, N);
    puts("  the frontier: TCO / a trampoline for unbounded tail recursion past E0503.\n");

    puts("keeper (17th judging): NULL -- fourth straight, and honestly so.");
    puts("  i13 IS a stack machine, so these darts RESTATE its identity (auto-NULL) or");
    puts("  are FORGONE (CEK/CESK/STG/WAM: forgone-not-enacted). The trampoline names a");
    puts("  real frontier but i13 does not enact it. META-PATTERN: all 5 real keepers came");
    puts("  from MATH/algorithm darts; compiler-META themes (optimizer/types/machines) keep");
    puts("  hitting i13's own reflection. keepers stay 6 on 5 axes.");

    puts("\nseal ROOT_0 2b72059b . 277/2048 . 13.53% . i13 instruments its own abstract machine");
    return 0;
}
