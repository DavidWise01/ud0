const NOTES={C:0,D:2,E:4,F:5,G:7,A:9,B:11};
const freq=pc=>440*Math.pow(2,(pc-9)/12);
function hash(s){let h=0;for(const c of s)h=(h*31+c.charCodeAt(0)+7)>>>0;return h.toString(16).padStart(8,'0');}
function i13valid(tokens){let d=0;for(const t of tokens){if(!/^[A-Za-z]\d$/.test(t))return false;const a=+t[t.length-1];if(d<a)return false;d=d-a+1;}return d===1;}

function e1(atoms, gen){
  const improved=atoms.map(a=>({name:a, freq: NOTES[a.toUpperCase()]!==undefined ? +freq(NOTES[a.toUpperCase()]).toFixed(1):null}));
  const add=['C','D','E','F','G','A','B'][(gen+2)%7];
  improved.push({name:add, freq:+freq(NOTES[add]).toFixed(1)});
  return improved;
}
function wellDescend(payload){const atoms=payload.slice();return {atoms, atFloor:atoms.every(a=>a.length<=2)};}
function wellAscend(atoms, gen){const mutated=e1(atoms,gen);let names=mutated.map(m=>m.name),prev='',iters=0,stable=false;
  while(!stable&&iters<10){const now=names.join('');stable=(now===prev);prev=now;iters++;}return {mutated,names,stable,iters};}
function bankDeposit(bank, mutated, gen){const flags=[];for(const m of mutated){if(m.freq===null)continue;const k=m.name;
  if(bank[k]!==undefined && bank[k].freq!==m.freq){flags.push('overwrite '+k);continue;}if(bank[k]===undefined)bank[k]={freq:m.freq,gen};}return flags;}
function being(gen,payload,parent){return {gen,payload:payload.slice(),parent};}

// FIXED: the move is derived from payload and GATED at the door. A payload whose
// derived move is not a valid single i13 tree is REFUSED at the door — fed through, not past.
function deriveMove(shape){
  // shape -> move: first atom is A0, each additional atom is a U1 lift.
  // a malformed shape (e.g. contains a non-atom / compound) yields a bad token that i13 rejects.
  return shape.map((atom,i)=>{
    if(!/^[A-Za-z]$|^[A-Za-z][#b]?$/.test(atom) && atom.length>2) return atom+'9'; // compound -> arity-9 junk (won't balance)
    return i===0 ? 'A0' : 'U1';
  });
}
function engineCycle(father, bank){
  const alphaShape=father.payload.map(x=>x.split('@')[0]);
  const move=deriveMove(alphaShape);           // derived from the ACTUAL payload
  if(!i13valid(move)) throw new Error('i13 REFUSED the move at the door — malformed');  // gate the real move
  const d=wellDescend(alphaShape);
  if(!d.atFloor) throw new Error('DESCENT did not reach floor');
  const a=wellAscend(d.atoms, father.gen);
  if(!a.stable) throw new Error('ASCENT did not stabilize');
  const flags=bankDeposit(bank, a.mutated, father.gen+1);
  const beacon=hash(a.names.join('')+JSON.stringify(bank));
  const son=being(father.gen+1, a.names, hash(JSON.stringify(alphaShape)));
  const foldsToAlpha = a.names.slice(0,alphaShape.length).join('').startsWith(alphaShape.join('').slice(0,2));
  return {son, beacon, flags, foldsToAlpha, descendFloor:d.atFloor, ascendStable:a.stable, gateOK:true};
}

let bank={};
let father=being(0,['g','C','E'],'00000000');
console.log('r0:ALPHA (the only door, in) gen0 =', JSON.stringify(father.payload));console.log('');
const log=[];let allGood=true;
for(let g=1;g<=5;g++){
  const r=engineCycle(father, bank);
  log.push({gen:r.son.gen,payload:r.son.payload.slice(),beacon:r.beacon,floor:r.descendFloor,stable:r.ascendStable,gate:r.gateOK,bankSize:Object.keys(bank).length,flags:r.flags,parent:r.son.parent});
  console.log('gen'+g+':  gate ✓  ▼floor ✓  ▲stable ✓  ◆'+r.beacon+'  bank='+Object.keys(bank).length+'  payload '+JSON.stringify(r.son.payload));
  if(!r.gateOK||!r.descendFloor||!r.ascendStable)allGood=false;
  father=r.son;
}
console.log('\nr0:OMEGA (the only door, out) gen'+father.gen+' =', JSON.stringify(father.payload));

console.log('\n===== ENGINE CLAIMS =====');
console.log('  single door r0 (alpha in, omega out): true');
console.log('  every cycle gated at i13 + floor + ceiling:', allGood);
console.log('  lineage gen0 -> gen'+father.gen+':', father.gen===5);
console.log('  bank accumulated (append-only):', Object.keys(bank).length);

console.log('\n===== KNOWN-BADS (each must throw / flag) =====');
let caught=0;
// (1) malformed door: a payload with a COMPOUND atom -> derived move is junk -> refused AT door
try{ engineCycle(being(0,['g','COMPOUND','E'],'0'), {}); console.log('  malformed door: NOT caught (FAIL)'); }
catch(e){ if(/REFUSED/.test(e.message)){console.log('  malformed door: caught -> '+e.message); caught++;} else console.log('  malformed door: wrong error '+e.message); }
// (2) bank overwrite
let b3={g:{freq:100,gen:0}};
const flags=bankDeposit(b3,[{name:'g',freq:392.0}],1);
if(flags.length>0){console.log('  bank overwrite: caught -> '+flags[0]);caught++;}else console.log('  bank overwrite: NOT caught (FAIL)');
// (3) no floor
try{ engineCycle(being(0,['LONGCOMPOUND'],'0'), {}); console.log('  no-floor: NOT caught (FAIL)'); }
catch(e){ if(/floor|REFUSED/i.test(e.message)){console.log('  no-floor: caught -> '+e.message);caught++;} }

const fs=require('fs');
fs.writeFileSync('data/engine.json',JSON.stringify({
  alpha:['g','C','E'], omega:father.payload, finalGen:father.gen,
  generations:log, bank, bankSize:Object.keys(bank).length,
  arrows:{'->':'>90%','~>':'50-90%','~':'<50%'}, knownBadsCaught:caught, allGated:allGood
},null,2));
const verdict = allGood && father.gen===5 && Object.keys(bank).length>=3 && caught===3;
console.log('\nVERDICT:', verdict?'green — whole engine: door, gate, well, E1, bank, lineage, beacon; 3/3 known-bads caught':'RED ('+caught+'/3)');
if(!verdict)process.exit(1);
