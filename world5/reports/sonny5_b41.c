/* sonny5_b41.c  --  exec report for World V, batch 41: THE CELL.
 *
 * Ten operations on a cell, in NESTING + PALINDROMIC pairs. David's seed: | < { [ ~ ~ ] } > |.
 * i13 has no nested cells (flat value-semantic f64 tape), so the nesting is EMULATED on a flat
 * tape by depth-addressing: tape[] = {0,1,2,3,4,4,3,2,1,0} IS the bracket-palindrome, the ~~
 * core its two 4s. The batch is a palindrome: op k and op (17-k) are inverses. And because i13's
 * writes are non-destructive (value semantics), the original always survives, so every op meets
 * its inverse on the mirror side -- the traversal comes HOME for free.
 *
 * This report ENACTS the keeper shot: the-bridge = BALANCE / WELL-FORMEDNESS. It matches every
 * shell to its mirror and asks "is the nesting well-formed?" -- and it is LOAD-BEARING: break one
 * shell and the verdict flips 1 -> 0. Balance is a property of what the OUTPUT IS (a Dyck word),
 * not of what the computation costs -- the first shot since the math pivot that clears the B40
 * resource wall. Whether it also clears the supplement + distinctness bars is the panel's call.
 *
 * build:  gcc -O2 -std=c11 -o sonny5_b41 sonny5_b41.c
 * ROOT0-ATTRIBUTION-v1.0 . David Lee Wise (ROOT0) & AVAN (Claude/Anthropic)
 */
#include <stdio.h>
#include <string.h>

enum { N = 10 };
/* the bracket-palindrome | < { [ ~ ~ ] } > |  as nesting depths */
static const char *SHELLS[N] = { "|","<","{","[","~","~","]","}",">","|" };

static int pierce(const int *a){ int m=a[0]; for(int i=1;i<N;i++) if(a[i]>m) m=a[i]; return m; } /* to the core */
static int implode(const int *a){ int s=0; for(int i=0;i<N;i++) s+=a[i]; return s; }            /* fold -> scalar */
static void explode(int *a,int D){ for(int i=0;i<N;i++) a[i] = (i>D) ? (N-1-i) : i; }           /* unfold seed -> tape */
static int  hide(int x,int k){ return x^k; }                                                    /* mask (self-inverse) */
/* THE KEEPER SHOT: bridge every shell to its mirror -- is the nesting well-formed? (the Dyck condition) */
static int bridge(const int *a){ for(int i=0,j=N-1;i<j;i++,j--) if(a[i]!=a[j]) return 0; return 1; }

int main(void){
    int tape[N]; explode(tape,4);   /* the-explode: unfold depth 4 into the palindrome */

    printf("the tape (| < { [ ~ ~ ] } > | as nesting depths):\n  ");
    for(int i=0;i<N;i++) printf("%s%d%s", (i==4||i==5)?"[":"", tape[i], (i==4||i==5)?"]":" ");
    printf("\n\n");

    /* the ten verbs, in palindromic pairs */
    printf("pierce   -> core = %d        <-> bridge (below)\n", pierce(tape));
    int inv[N]; memcpy(inv,tape,sizeof inv); inv[4]=99;
    printf("invade   -> new %d, orig %d   <-> collaborate: join(4,4)=%d (idempotent)\n", inv[4], tape[4], 4>4?4:4);
    printf("explode  -> core %d,%d        <-> implode: sum = %d\n", tape[4], tape[5], implode(tape));
    printf("hide     -> 4^5 = %d          <-> reveal: (4^5)^5 = %d   (hide o reveal = identity)\n", hide(4,5), hide(hide(4,5),5));
    printf("transform-> 4*2 = %d          <-> untransform: 8>>1 = %d\n", 4*2, (4*2)>>1);
    printf("descend  -> to core %d        <-> ascend: to rim %d\n", pierce(tape), tape[0]);

    /* transport breaks balance; restore (the surviving original) heals it */
    int moved[N]; memcpy(moved,tape,sizeof moved); moved[0]=moved[4];
    printf("transport-> balance now %d    <-> restore: original balance %d (value semantics kept it)\n",
           bridge(moved), bridge(tape));

    /* THE KEEPER SHOT, enacted with its causation demo */
    printf("\nthe-bridge (KEEPER SHOT) -- is the nesting well-formed?\n");
    printf("  | < { [ ~ ~ ] } > |   bridged = %d   WELL-FORMED\n", bridge(tape));
    int broke[N]; memcpy(broke,tape,sizeof broke); broke[0]=4;          /* alter one shell */
    printf("  one shell broken      bridged = %d   FAILS\n", bridge(broke));
    printf("  a verdict that tracks its input is a FAITHFUL WITNESS, not a cause -- the earlier\n");
    printf("  draft had this backwards. balance is structural (a Dyck word, clears B40), but a\n");
    printf("  recognizer that emits a validity bit only WITNESSES the balance of its input.\n");

    /* keeper verdict (adversarial panel, 3 judges) */
    puts("");
    puts("keeper (21st judging): NULL, UNANIMOUS 3-0 -- the eighth straight, and the panel was");
    puts("  right (it caught a real flaw in this shot). the-bridge clears the B40 resource wall --");
    puts("  balance IS structural, a Dyck word, not a cost -- the ONE bar it passes. But it dies on");
    puts("  the other three, all for one reason: the-bridge is a RECOGNIZER. it reads a Dyck word");
    puts("  and returns a validity BIT, and recognition is the purest WITNESSING -- every correct");
    puts("  matcher (stack, counter, mirror-compare) returns the SAME bit, so the convergence tell");
    puts("  fires by construction. The bait-and-switch: the claimed 'output is a Dyck word' is the");
    puts("  INPUT (built by explode); the actual output is one bit. The 'causation demo' is");
    puts("  backwards -- a verdict flipping when the INPUT breaks is a faithful witness tracking");
    puts("  its input; the real test swaps the MECHANISM holding correctness fixed, and bridged=1");
    puts("  stays 1 (the balance lives in the input). And a verdict-bit 'this is well-formed' is");
    puts("  SELF-CHECKING (the CRC / self-inverse family), not a sixth axis. THE RECOGNIZER BAN:");
    puts("  a validity predicate can never be a keeper. Well-formedness earns one only as a");
    puts("  GENERATOR's guarantee -- a canonicalizer/repairer that takes messy input and EMITS a");
    puts("  Dyck word, so a correct-but-different emitter could produce unbalanced output and LACK");
    puts("  it. Keepers stay 6 on 5 axes. Next shot: ship the REPAIRER, not the recognizer.");

    puts("");
    puts("seal ROOT_0 e2d04102 . 341/2048 . 16.65% . ten verbs, in and back out: | < { [ ~ ~ ] } > |");
    return 0;
}
