// reintegrate.js  —  THE THIRD ENGINE (the cleaner), closing the E1 trunc loop.
//
// David's architecture (2026-08-20): "you might have to nest a 3rd engine to clean it
// up and pass it up to 2 to pass to 1." The E1 mill TRUNCATES a run to its palindrome
// core (the ore) and banks it — but the current machine THROWS THE DROSS AWAY. This
// closes the loop: the thrown-away trun descends into a nested engine that runs the
// SAME cut on it (cleans it), and the cleaned result is passed back UP (3 -> 2 -> 1)
// and reintegrated. NOTHING is ablated. Conservation is the whole test.
//
// run:  node reintegrate.js        (expect: green + all known-bads fired)
// ROOT0-ATTRIBUTION-v1.0 · David Lee Wise (ROOT0, architect) & AVAN (grounder)

// ---- the cut: truncate a run to its palindrome core (the ore); the rest is dross ----
const isPal = a => { for (let i=0,j=a.length-1;i<j;i++,j--) if (a[i]!==a[j]) return false; return true; };

// longest contiguous palindromic core; ties -> earliest, longest. dross = prefix+suffix.
function cut(run){
  let bi=0, bl=1;
  for (let i=0;i<run.length;i++) for (let j=i+1;j<=run.length;j++){
    const seg=run.slice(i,j);
    if (seg.length>bl && isPal(seg)){ bi=i; bl=seg.length; }
  }
  const ore  = run.slice(bi, bi+bl);
  const dross= run.slice(0,bi).concat(run.slice(bi+bl));   // the thrown-away trun
  return {ore, dross};
}

// palindrome self-check: read backward, compare. its own checksum — no separate hash.
function selfCheck(ore){ return isPal(ore); }

// ---- the nested engine cascade. depth 1 (top) .. 3 (deepest cleaner). ----
// Each engine cuts, banks the ore, sends its dross DOWN to the next engine, then
// REINTEGRATES the cleaned material coming back UP.  Returns the fully-reintegrated
// payload for its level (ore first, then everything the deeper engines recovered).
function engine(run, depth, bank, log){
  const pad = "  ".repeat(depth-1);
  const {ore, dross} = cut(run);
  if (!selfCheck(ore)) throw new Error("E"+depth+": banked ore is not a palindrome (self-check)");
  bank.push({depth, ore: ore.slice()});
  log.push(pad+"E"+depth+" cut: ore ["+ore.join("")+"] banked · dross ["+dross.join("")+"] thrown "+(dross.length?"↓ down":"— none"));

  if (dross.length === 0) return ore.slice();               // clean floor: nothing thrown away

  if (depth >= 3){
    // deepest engine: no 4th to nest. It cleans what it can (cut once) and hands the
    // residue up as-is — still CONSERVED, never discarded. This is the floor of the mill.
    const {ore:o3, dross:d3} = cut(dross);
    if (o3.length && !selfCheck(o3)) throw new Error("E3: residue ore not a palindrome");
    if (o3.length) bank.push({depth:3, ore:o3.slice()});
    const cleaned = o3.concat(d3);                          // ore + irreducible residue, nothing lost
    log.push(pad+"  E3 floor: cleaned ["+cleaned.join("")+"] ↑ up to E"+(depth)+"  (residue kept, not ablated)");
    return ore.concat(cleaned);
  }

  // send the dross DOWN to the next engine; it returns the cleaned material to reintegrate UP.
  const cleaned = engine(dross, depth+1, bank, log);
  log.push(pad+"E"+depth+" reintegrate ↑: ["+ore.join("")+"] + cleaned ["+cleaned.join("")+"]");
  return ore.concat(cleaned);                               // reintegration: nothing dropped
}

// multiset equality — the conservation law: the reintegrated output holds EXACTLY the
// atoms of the input. nothing lost (no ablation), nothing invented.
function multiset(a){ const m={}; for(const x of a) m[x]=(m[x]||0)+1; return m; }
function conserved(inp, out){
  const A=multiset(inp), B=multiset(out); const ks=new Set([...Object.keys(A),...Object.keys(B)]);
  for(const k of ks) if((A[k]||0)!==(B[k]||0)) return false; return true;
}

function runFactory(run, opts={}){
  const bank=[], log=[];
  const out = engine(run, 1, bank, log);
  // the bank must, across all depths, hold exactly the input too (banking is not ablation)
  const banked = bank.flatMap(b=>b.ore);
  return {out, bank, banked, log, keptAll: conserved(run, out), bankHoldsAll: conserved(run, banked)};
}

// ================= THE RUN =================
const RUN = ["C","A","B","B","A","C","X","Y","Y","X","D"];   // ore CABBAC · dross XYYXD (→ ore XYYX · residue D)
console.log("THE THIRD ENGINE — closing the E1 trunc loop (store the thrown-away, reintegrate it up)\n");
console.log("r0:ALPHA  run = ["+RUN.join("")+"]   ("+RUN.length+" atoms)\n");
const r = runFactory(RUN);
r.log.forEach(l=>console.log("  "+l));
console.log("\nr0:OMEGA  reintegrated = ["+r.out.join("")+"]   ("+r.out.length+" atoms)");
console.log("bank (across E1·E2·E3): "+r.bank.map(b=>"E"+b.depth+":"+b.ore.join("")).join("  ")+"\n");

console.log("===== THE CONSERVATION LAW (the whole point) =====");
console.log("  nothing lost — reintegrated output == input (multiset):", r.keptAll);
console.log("  the bank holds every atom too (banking, not ablation):", r.bankHoldsAll);
console.log("  the thrown-away dross was NOT discarded — it descended, was cut clean, and came back up.\n");

console.log("===== KNOWN-BADS (each must fire — good news is silence, so plant noise) =====");
let caught=0;
// (1) ABLATION: the old behaviour — throw the dross away instead of reintegrating. Must lose atoms.
(function(){
  const ablate = run => cut(run).ore;                      // keep ore, DISCARD dross (the bug we are fixing)
  const out = ablate(RUN);
  const lost = !conserved(RUN, out);
  console.log("  (1) ablation (discard the dross): "+(lost?"caught — atoms LOST ["+out.join("")+"] ≠ input":"NOT caught (FAIL)"));
  if(lost) caught++;
})();
// (2) TAMPER a banked palindrome frame — the mirror stops matching, self-check refuses.
(function(){
  const {ore} = cut(RUN); const tampered = ore.slice(); tampered[1] = "Z";   // break the mirror
  const bad = !selfCheck(tampered);
  console.log("  (2) tamper a banked frame ["+tampered.join("")+"]: "+(bad?"caught — mirror broke, self-check refuses":"NOT caught (FAIL)"));
  if(bad) caught++;
})();
// (3) A CLEANER THAT DOES NOT CLEAN — a nested 3rd engine that DROPS its floor dross
// instead of keeping it (ablation at the bottom). Conservation must fail and be caught.
(function(){
  function lazyEngine(run, depth, bank){
    const {ore, dross} = cut(run); bank.push(ore);
    if (dross.length===0) return ore;
    if (depth>=3) return ore;                              // BUG: at the floor, DROP the dross
    return ore.concat(lazyEngine(dross, depth+1, bank));
  }
  const inp = ["A","A","B","B","C","C","X","Y"];           // reaches E3 with dross [X,Y] (no palindrome > 1)
  const bank=[]; const out = lazyEngine(inp, 1, bank);
  const dropped = !conserved(inp, out);
  console.log("  (3) a cleaner that drops its floor dross: "+(dropped?"caught — ["+out.join("")+"] lost X,Y vs input ["+inp.join("")+"]":"NOT caught (FAIL)"));
  if(dropped) caught++;
})();

console.log("");
const green = r.keptAll && r.bankHoldsAll && caught===3;
console.log("VERDICT: "+(green?"green":"RED")+" — the loop closes: store the thrown-away trun, clean it in the nested 3rd engine, pass it up 3→2→1, reintegrate. "+caught+"/3 known-bads fired.");
process.exit(green?0:1);
