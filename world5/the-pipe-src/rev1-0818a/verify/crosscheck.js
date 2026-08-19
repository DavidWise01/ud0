// Pull the data BACK OUT of the shipped page and re-derive. Proves the artifact
// the user opens agrees with the source computation — not just that the JSON is self-consistent.
const fs=require('fs');
const h=fs.readFileSync('rev1-0818.html','utf8');
const m=h.match(/<script id="data" type="application\/json">([\s\S]*?)<\/script>/);
if(!m){console.error('FAIL: no embedded data');process.exit(1);}
const D=JSON.parse(m[1]);
let fail=0;
function chk(name,cond){console.log((cond?'ok  ':'FAIL')+' '+name);if(!cond)fail++;}

// re-derive the ladder independently
function fieldSize(n){if(n===0)return 1n;return BigInt(2*n)**(2n**BigInt(n));}
chk('ladder field sizes match', D.ladder.every(r=>fieldSize(r.n).toString()===r.field));
chk('windows are 1+n', D.ladder.every(r=>r.window===1+r.n));
chk('coverage flag true', D.coverageOK===true);
chk('window<field n>=2', D.ladder.slice(2).every(r=>BigInt(r.window)<BigInt(r.field)));

// re-derive commutator
function MOVE(s,dx,dy){return {x:s.x+dx,y:s.y+dy,z:s.z};}
function BURROW(s,dz){return {x:s.x,y:s.y,z:s.z+dz};}
function eq(a,b){return a.x===b.x&&a.y===b.y&&a.z===b.z;}
let allZero=true;const s0={x:5,y:5,z:0};
for(let dx=-3;dx<=3;dx++)for(let dy=-3;dy<=3;dy++)for(let dz=-3;dz<=3;dz++){
  if(!eq(BURROW(MOVE(s0,dx,dy),dz),MOVE(BURROW(s0,dz),dx,dy)))allZero=false;}
chk('commutator zero over 343 (re-derived)', allZero && D.commutator.zero && D.commutator.allZero);

// re-derive planted void MUST be non-zero
function SPILL(s,dz){return {x:s.x+Math.floor(s.x/10),y:s.y,z:s.z+dz};}
const sB=D.planted_void.point;
const ab=SPILL(MOVE(sB,D.planted_void.dx,0),D.planted_void.dz);
const ba=MOVE(SPILL(sB,D.planted_void.dz),D.planted_void.dx,0);
chk('planted void non-zero (re-derived)', !eq(ab,ba) && D.planted_void.caught);
chk('planted void matches shipped values', ab.x===D.planted_void.ab.x && ba.x===D.planted_void.ba.x);

// zero crossing
chk('zero-crossing present', Math.max(...D.path.map(p=>p.z))>0 && Math.min(...D.path.map(p=>p.z))<0 && D.crossed);
chk('root opens frame 1 below zero', (()=>{const r=D.path.find(p=>p.frame>0);return r&&r.frame===1&&r.z<0;})());
chk('frame 1 <= limit', 1<=D.frame_limit);

console.log('\n'+(fail===0?'CROSSCHECK GREEN — shipped page agrees with re-derivation':'CROSSCHECK RED — '+fail+' disagreements'));
process.exit(fail===0?0:1);
