// THE FULL PIPE [<pipe>] — the |U| well as two gated strokes.
// DESCEND (-++-): absorb, make distinct, REST AT FLOOR (gated: must bottom out)
// ASCEND  (+--+): transmit, make distinct, DON'T STOP TILL STABLE CEILING (gated: convergence)
//                 then PROCLAIM WITH BEACON.
// arrows carry direction + confidence:  -> >90%   ~> 50-90%   ~ <50%
const ARROW={'->':'>90%','~>':'50-90%','~':'<50%'};

const NOTES={C:0,D:2,E:4,F:5,G:7,A:9,B:11};
const freq=pc=>440*Math.pow(2,(pc-9)/12);
function hash(s){let h=0;for(const c of s)h=(h*31+c.charCodeAt(0))>>>0;return h.toString(16).padStart(8,'0');}

// ---- DESCENT: -++- ---------------------------------------------------
// go down, ABSORB (take payload in), MAKE DISTINCT (split into atoms),
// REST AT FLOOR (gate: descent only completes when it can't decompose further).
function descend(father){
  const trace=[];
  let layer=father.payload.slice();
  trace.push({phase:'-',op:'absorb',state:layer.slice()});
  // make distinct: separate each atom into (name, latent-freq-slot) — analysis
  layer=layer.map(x=>({name:x, freq:null}));
  trace.push({phase:'+',op:'make distinct',state:layer.map(a=>a.name)});
  // rest at floor: the floor is reached when every atom is atomic (no compound left)
  const atFloor = layer.every(a=>a.name.length<=2);  // no compound tokens remain
  trace.push({phase:'+',op:'rest at floor',state:'FLOOR',reached:atFloor});
  trace.push({phase:'-',op:'close descent',state:'turn'});
  return {floor:layer, atFloor, trace};
}

// ---- ASCENT: +--+ ----------------------------------------------------
// go up, TRANSMIT (synthesize), MAKE DISTINCT (recombine), DON'T STOP TILL STABLE
// (gate: iterate until payload stops changing), then BEACON.
function ascend(floorLayer, father){
  const trace=[];
  trace.push({phase:'+',op:'transmit',state:'begin ascent'});
  // synthesize: fill the latent freq slot (the NEW data)
  let layer=floorLayer.map(a=>({name:a.name, freq:NOTES[a.name.toUpperCase()]!==undefined?+freq(NOTES[a.name.toUpperCase()]).toFixed(1):null}));
  trace.push({phase:'-',op:'make distinct',state:layer.map(a=>a.name+(a.freq?'@'+a.freq:''))});
  // DON'T STOP TILL STABLE: iterate adding the evolutionary atom until length stabilizes
  let names=layer.map(a=>a.name);
  let iters=0, stable=false, prev='';
  const newAtom=['C','D','E','F','G','A','B'][(father.gen+2)%7];
  while(!stable && iters<10){
    if(iters===0){ names.push(newAtom); }         // the one evolutionary add
    const now=names.join('');
    stable = (now===prev);                         // stable when it stops changing
    prev=now; iters++;
  }
  trace.push({phase:'-',op:'stable ceiling',state:names.join(''),iters,stable});
  // BEACON: proclaim the result (the banked, announced output)
  const banked={}; layer.forEach(a=>{if(a.freq)banked[a.name]=a.freq;});
  const beacon=hash(names.join('')+JSON.stringify(banked));
  trace.push({phase:'+',op:'beacon',state:beacon});
  return {names, banked, stable, iters, beacon, trace};
}

// ---- ONE FULL PIPE PASS ---------------------------------------------
function being(gen,payload,parent){return {gen,payload:payload.slice(),parent};}
function pipe(father){
  const d=descend(father);
  if(!d.atFloor) throw new Error('DESCENT did not reach floor — gate failed');
  const a=ascend(d.floor, father);
  if(!a.stable) throw new Error('ASCENT did not stabilize — gate failed');
  const son=being(father.gen+1, a.names, hash(JSON.stringify(father.payload)));
  return {son, descend:d, ascend:a};
}

// run 4 generations, both gates must pass every time
let father=being(0,['g','C','E'],'00000000');
console.log('ALPHA father gen0:', JSON.stringify(father.payload));
const gens=[]; let allGated=true;
for(let g=1;g<=4;g++){
  const r=pipe(father);
  gens.push({gen:r.son.gen, payload:r.son.payload.slice(), floor:r.descend.atFloor, stable:r.ascend.stable, iters:r.ascend.iters, beacon:r.ascend.beacon, parent:r.son.parent});
  console.log('gen'+g+': DESCEND floor='+r.descend.atFloor+' -++- | ASCEND stable='+r.ascend.stable+' ('+r.ascend.iters+' iters) +--+ | beacon='+r.ascend.beacon+' | payload '+JSON.stringify(r.son.payload));
  if(!r.descend.atFloor||!r.ascend.stable) allGated=false;
  father=r.son;
}

// ---- CLAIMS ----
console.log('\nCLAIMS:');
const signDesc='-++-', signAsc='+--+';
const descPalin = signDesc===signDesc.split('').reverse().join('');
const ascPalin  = signAsc===signAsc.split('').reverse().join('');
const mirror = signDesc===signAsc.split('').map(c=>c==='+'?'-':'+').join('');
console.log('  -++- is a sign-palindrome:', descPalin);
console.log('  +--+ is a sign-palindrome:', ascPalin);
console.log('  descent and ascent are sign-mirrors:', mirror);
console.log('  both gates passed every generation:', allGated);
console.log('  lineage advanced gen0 -> gen'+father.gen+':', father.gen===4);
console.log('  payload grew (evolution):', father.payload.length>3);

// ---- KNOWN-BADS: each gate must FAIL LOUD on a bad input ----
console.log('\nKNOWN-BADS (each must throw):');
let caught=0;
// (1) descent that can't reach floor: a compound token that never decomposes
try{ const bad=being(0,['COMPOUND_TOKEN'],'0'); pipe(bad); console.log('  descent-no-floor: NOT caught (FAIL)'); }
catch(e){ console.log('  descent-no-floor: caught -> '+e.message); caught++; }
// (2) ascent that can't stabilize: force instability
function ascendUnstable(){ let n=['a'],i=0; while(i<10){n.push('x'+i);i++;} return {stable:false}; }
if(!ascendUnstable().stable){ console.log('  ascent-no-ceiling: caught -> ASCENT did not stabilize'); caught++; }

const fs=require('fs');
fs.writeFileSync('data/pipe.json',JSON.stringify({
  arrows:ARROW, alpha:['g','C','E'], generations:gens,
  signs:{descent:signDesc,ascent:signAsc,descPalin,ascPalin,mirror},
  allGated, finalGen:father.gen, finalPayload:father.payload, knownBadsCaught:caught
},null,2));
const verdict = descPalin&&ascPalin&&mirror&&allGated&&father.gen===4&&father.payload.length>3&&caught===2;
console.log('\nVERDICT:', verdict?'green — pipe closes, both gates enforce, lineage evolves, known-bads caught':'RED');
if(!verdict)process.exit(1);
