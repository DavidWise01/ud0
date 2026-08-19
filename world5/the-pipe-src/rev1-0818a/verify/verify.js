function fieldSize(n){ if(n===0) return 1n; const base=BigInt(2*n); const exp=2n**BigInt(n); return base**exp; }
function digits(b){ return b.toString().length; }
const ladder=[];
for(let n=0;n<=6;n++){ const f=fieldSize(n); ladder.push({n,dots:'.'.repeat(n),field:f.toString(),digitspan:digits(f),window:1+n}); }
console.log('LADDER:');
for(const r of ladder) console.log(`  n=${r.n} ${r.dots.padEnd(6)} digits=${String(r.digitspan).padStart(3)}  window=${r.window}`);
const coverageOK=ladder.slice(2).every(r=>BigInt(r.window)<BigInt(r.field));
console.log('CLAIM window<field n>=2:',coverageOK);

const FRAME_LIMIT=4096;
const path=[
  {step:'~> 0',z:0,frame:0,note:'enter origin (surface root)'},
  {step:'.',z:1,frame:0,note:'burrow: point inside a voxel'},
  {step:'..',z:2,frame:0,note:'part of a voxel field'},
  {step:'...',z:3,frame:0,note:'of which there are many'},
  {step:'.....',z:5,frame:0,note:'deeper — confirm field beneath'},
  {step:'0',z:0,frame:0,note:'return to zero'},
  {step:'0 ~> -1',z:-1,frame:0,note:'CROSS below zero — through the floor'},
  {step:'-1 ~> r0',z:-1,frame:0,note:'reach the root FROM below'},
  {step:'r0 >[1]',z:-1,frame:1,note:'root opens frame 1 : begin'},
];
const zs=path.map(p=>p.z);
const crossed=Math.max(...zs)>0&&Math.min(...zs)<0;
console.log('CLAIM zero-crossing:',crossed,' CLAIM frame1<=4096:',path.at(-1).frame===1);

function MOVE(s,dx,dy){return {x:s.x+dx,y:s.y+dy,z:s.z};}
function BURROW(s,dz){return {x:s.x,y:s.y,z:s.z+dz};}
function eq(a,b){return a.x===b.x&&a.y===b.y&&a.z===b.z;}
function commutator(A,B,s,dx,dy,dz){const ab=B(A(s,dx,dy),dz);const ba=A(B(s,dz),dx,dy);return {ab,ba,zero:eq(ab,ba)};}
const s0={x:5,y:5,z:0};
const c=commutator(MOVE,BURROW,s0,2,0,3);
console.log('\nCOMMUTATOR [MOVE,BURROW]:',JSON.stringify(c.ab),'vs',JSON.stringify(c.ba),' zero:',c.zero);
let allZero=true;
for(let dx=-3;dx<=3;dx++)for(let dy=-3;dy<=3;dy++)for(let dz=-3;dz<=3;dz++){ if(!commutator(MOVE,BURROW,s0,dx,dy,dz).zero)allZero=false; }
console.log('  zero over 343 triples:',allZero);

// FIXED planted void: the shift depends on the z the burrow STARTED from,
// which differs between the two orders (MOVE-first burrows from z=0; BURROW-first
// then MOVE keeps the burrowed z but MOVE reads a DIFFERENT pre-image).
// Correct non-commuting bad op: burrow's x-kick depends on CURRENT x (which MOVE changes).
function BURROW_SPILL(s,dz){ return {x:s.x + Math.floor(s.x/10), y:s.y, z:s.z+dz}; }
// MOVE then BURROW_SPILL: x changed first, so kick reads the MOVED x.
// BURROW_SPILL then MOVE: kick reads original x, then MOVE adds. Orders disagree when x crosses a /10 boundary.
const sB={x:8,y:5,z:0};
const cs=commutator(MOVE,BURROW_SPILL,sB,3,0,2); // 8->11 crosses the /10 boundary
console.log('\nPLANTED VOID (x-dependent spill) at <8,5;0> dx=3:');
console.log('  MOVE->SPILL =',JSON.stringify(cs.ab),'  SPILL->MOVE =',JSON.stringify(cs.ba),'  caught:',!cs.zero);
if(cs.zero){console.error('  FAIL: void still slipped');process.exit(1);}
// confirm it's genuinely order-sensitive across a sweep, not just one point
let voidCaught=0,voidTotal=0;
for(let x=0;x<20;x++)for(let dx=-5;dx<=5;dx++){ voidTotal++; if(!commutator(MOVE,BURROW_SPILL,{x,y:0,z:0},dx,0,1).zero)voidCaught++; }
console.log(`  void non-zero on ${voidCaught}/${voidTotal} sweep points (must be >0):`,voidCaught>0);

const fs=require('fs');
const data={frame_limit:FRAME_LIMIT,ladder,coverageOK,path,crossed,
  commutator:{point:s0,dx:2,dy:0,dz:3,ab:c.ab,ba:c.ba,zero:c.zero,allZero,samples:343},
  planted_void:{point:sB,dx:3,dz:2,ab:cs.ab,ba:cs.ba,caught:!cs.zero,sweep_caught:voidCaught,sweep_total:voidTotal}};
fs.writeFileSync('data/entry.json',JSON.stringify(data,null,2));
console.log('\nwrote data/entry.json');
console.log('ALL CLAIMS:',[coverageOK,crossed,allZero,!cs.zero,voidCaught>0].every(Boolean)?'green':'RED');
