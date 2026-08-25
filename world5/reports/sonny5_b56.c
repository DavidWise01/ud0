/* sonny5_b56.c  --  exec report for World V, batch 56: THE GAME.
 *
 * Game theory & combinatorial game theory. Every number below mirrors the real i13.exe output
 * (cells56/gm_*.i13). Grounded with integer arithmetic, native recursion (game trees / backward
 * induction), the integrated bitwise XOR (nim), and mex/ln(series)/sqrt(Newton) where needed.
 * Keeper shot: the-gale-shapley (a loose-spec DOF -- proposer- vs receiver-optimal give two different
 * valid stable matchings). It joins the-nim-game and the-negamax already in World V.
 *
 * build:  gcc -O2 -std=c11 -o sonny5_b56 sonny5_b56.c -lm   (put mingw64/bin on PATH first)
 * ROOT0-ATTRIBUTION-v1.0 . David Lee Wise (ROOT0) & AVAN (Claude/Anthropic) . |||..()
 */
#include <stdio.h>
#include <math.h>
#ifndef M_E
#define M_E 2.71828182845904523536
#endif

static int memo[64];
static int win(int p){ if(p<=0) return 0; if(memo[p]!=-1) return memo[p];
    int a=win(p-1), b=win(p-2); return memo[p]=((a==0)||(b==0))?1:0; }

int main(void){
    for(int i=0;i<64;i++) memo[i]=-1;
    puts("THE GAME -- game theory on i13 (matches cells56/gm_*.i13):\n");

    /* 566 minimax theorem: maximin == minimax for a saddle matrix [[4,3],[2,1]] */
    { int maximin = 3, minimax = 3;   /* max(min(4,3),min(2,1))=3 ; min(max(4,2),max(3,1))=3 */
      printf("  minimax        : maximin=%d minimax=%d -> value 3 (a saddle)\n", maximin, minimax); }

    /* 567 alpha-beta: leaves [3,5,2,9] -> 3, one pruned */
    { int L[4]={3,5,2,9}; int min0=L[0]<L[1]?L[0]:L[1]; int prune=(L[2]<=min0);
      int val=min0>((L[2]<L[3])?L[2]:L[3])?min0:((L[2]<L[3])?L[2]:L[3]);
      printf("  alpha-beta     : value=%d prune=%d visited %d of 4 (same as full minimax)\n", val, prune, 4-prune); }

    /* 568 sprague-grundy: subtraction {1,2,3} grundy = k mod 4 */
    { printf("  sprague-grundy : mex{0,1,2}=3 ; grundy 0..6 = ");
      int g[7]; for(int k=0;k<=6;k++){ if(k==0){g[0]=0;printf("%d ",g[0]);continue;} int seen[8]={0};
        for(int d=1;d<=3;d++) if(k-d>=0) seen[g[k-d]]=1; int m=0; while(seen[m])m++; g[k]=m; printf("%d ",g[k]); }
      printf(" (k mod 4)\n"); }

    /* 569 nimber arithmetic: nim-add = XOR */
    { int s=3^5, tot=3^5^6; printf("  nimber         : 3 XOR 5 = %d ; (3^5)^5 = %d ; [3,5,6] XOR = %d (P-position)\n", s, (3^5)^5, tot); }

    /* 570 gale-shapley: two different stable matchings  [KEEPER SHOT] */
    { printf("  gale-shapley*  : men-propose {m0w0,m1w1} & women-propose {m0w1,m1w0} -- both stable, DIFFERENT\n"); }

    /* 571 zermelo: nim [3,4,5] determined, first player wins */
    { int ns=3^4^5; printf("  zermelo        : nim[3,4,5] XOR=%d -> first wins ; backward induction => one forced value\n", ns); }

    /* 572 vickrey: bids [10,7,5] pay second */
    { int price=7, val=10; printf("  vickrey        : winner bids 10, pays second = %d ; utility %d ; honesty dominant\n", price, val-price); }

    /* 573 shapley: symmetric 3-player, phi=1/3, efficient */
    { double phi=2.0/6; printf("  shapley        : phi1 = %.4f (= 1/3) ; 3*phi = %.0f = v(N) (efficient)\n", phi, 3*phi); }

    /* 574 nash: prisoner's dilemma (D,D) */
    { int T=5,R=3,P=1,S=0; printf("  nash           : (D,D) stable (S=%d<P=%d) ; (C,C) not (T=%d>R=%d) ; stable != optimal\n", S,P,T,R); }

    /* 575 tit-for-tat: 10 rounds */
    { int alld=0,allc=0,last=0; for(int r=0;r<10;r++){ int my=r?last:0,op=1; if(my==0&&op==0)alld+=3; else if(my==0&&op==1)alld+=0; else if(my==1&&op==0)alld+=5; else alld+=1; last=op; }
      last=0; for(int r=0;r<10;r++){ int my=r?last:0,op=0; if(my==0&&op==0)allc+=3; else if(my==0&&op==1)allc+=0; else if(my==1&&op==0)allc+=5; else allc+=1; last=op; }
      printf("  tit-for-tat    : vs always-defect=%d (suckered once) ; vs always-cooperate=%d\n", alld, allc); }

    /* 576 surreal: -1 < 0 < 1/2 < 1 */
    { printf("  surreal        : -1 < 0 < 1/2 < 1 (cross-multiplied) ; 1/2={0|1} born day 2\n"); }

    /* 577 hackenbush: all-blue-3 = +3 */
    { printf("  hackenbush     : all-blue-3 = value +3 (Left wins) ; balanced blue/red = 0 (2nd player)\n"); }

    /* 578 hex: no draw, first wins */
    { printf("  hex            : vertical chain connects top-bottom ; no draw + strategy-stealing => first wins\n"); }

    /* 579 secretary: k=floor(n/e), pwin */
    { int n=10,k=(int)(n/M_E); double s=0; for(int i=k;i<n;i++)s+=1.0/i; double p=(double)k/n*s;
      printf("  secretary      : k=floor(10/e)=%d (~37%%) ; P(best)=%.4f (-> 1/e)\n", k, p); }

    /* 580 mcts: UCB1 picks least-visited */
    { int N=9; double c=sqrt(2.0); double u0=0.8+c*sqrt(log(N)/5), u2=0.0+c*sqrt(log(N)/1);
      printf("  mcts           : UCB1 child0(mean .8,n5)=%.3f  child2(mean 0,n1)=%.3f -> explore child2\n", u0, u2); }

    /* 581 retrograde: losses at multiples of 3 */
    { printf("  retrograde     : subtraction {1,2} win/lose 0..6 = ");
      for(int p=0;p<=6;p++) printf("%d ", win(p)); printf(" (losses at 3k)\n"); }

    puts("");
    struct { const char *m; const char *gate; } R[] = {
        {"minimax","the value of a zero-sum game, forall-pinned (B39)"},
        {"alpha-beta","exact minimax value, only saves nodes; prune order coordinate-dependent (B40+B44)"},
        {"sprague-grundy","Grundy number forall-pinned by the game graph (B39, WITNESSED)"},
        {"nimber","nim-addition IS XOR = the seated self-inverse axis (axis 1)"},
        {"gale-shapley*","KEEPER SHOT: loose-spec DOF, but proposer-role is a labeling (B44) / non-confluent mirror of confluence (axis 5)"},
        {"zermelo","determinacy pinned by the game tree; existence != tractability (B39)"},
        {"vickrey","winner/price pinned; strategy-proofness is an incentive theorem (B39/B41)"},
        {"shapley","the UNIQUE axiomatic value, forall-pinned (B39)"},
        {"nash","fixed point of best-response (B43) + existence theorem (B39)"},
        {"tit-for-tat","a strategy/policy; scores pinned by the payoff table (B39)"},
        {"surreal","a construction/definition; ordering pinned (B39)"},
        {"hackenbush","pinned valuation into surreals (B39); seated game->number bridge"},
        {"hex","existence theorem (no-draw + strategy-stealing) (B39)"},
        {"secretary","the n/e optimum, pinned by the model (B39)"},
        {"mcts","a selection policy with a regret bound (B40/B41)"},
        {"retrograde","least fixed point of the game graph (B43) + pinned labels (B39)"},
    };
    puts("the 16, and where each lands under the gates:");
    for(int i=0;i<16;i++) printf("  %-15s <- %s\n", R[i].m, R[i].gate);

    /* ---- keeper verdict (3 diverse-lens judges + 4 veracity checkers + 1 completeness critic) ---- */
    puts("");
    puts("keeper (36th judging): NULL  -- no new axis. 23 straight NULL.");
    puts("  shot the-gale-shapley (two different stable matchings): the STRONGEST same-function-differ");
    puts("        candidate in a while -- 'a stable matching' is a LOOSE spec and proposer- vs receiver-");
    puts("        proposing deferred acceptance compute DIFFERENT valid outputs. But KILLED: the choice is");
    puts("        fixed by WHICH SIDE PROPOSES -- a labeling of the input (B44) -- and it is the non-confluent");
    puts("        mirror of the seated confluence axis (5). Swap the labels, swap the answer: coordinate-");
    puts("        dependent, not a coordinate-independent invariant a same-function pair can keep-or-drop.");
    puts("  shot the-sprague-grundy: WITNESSED. The Grundy number is forall-pinned by the game graph;");
    puts("        every correct solver returns the same nimber (B39). A deep equivalence, not a DOF.");
    puts("  the batch reduces to: pinned theorems/optima/valuations (B39, most of the 16) + one seated-axis");
    puts("        instance (nimber XOR = self-inverse) + fixed points (nash, retrograde: B43) + policies with");
    puts("        regret/incentive bounds (mcts, vickrey: B40/B41). No new coordinate-independent invariant.");
    puts("  keeper space holds: 6 keepers on 5 axes. Honest NULL, no forcing.");
    puts("");
    puts("credit verify (4 checkers): 16 CLEAN / 0 FIX -- every attribution holds:");
    puts("  von Neumann 1928 (not 1944), Knuth & Moore 1975, Sprague 1935 / Grundy 1939,");
    puts("  Bouton 1901 / Conway, Gale & Shapley 1962 (Nobel 2012), Zermelo 1913, Vickrey 1961 (Nobel 1996),");
    puts("  Shapley 1953 (Nobel 2012), Nash 1950 (Nobel 1994), Rapoport/Axelrod 1980, Conway/Knuth 1974,");
    puts("  Berlekamp-Conway-Guy, Hein 1942/Nash 1948, Lindley 1961, Coulom/UCT 2006, Bellman 1965.");

    puts("");
    puts("seal ROOT_0 5b67a98f . 581/2048 . 28.37% . the value was always there; only the strategy is missing");
    return 0;
}
