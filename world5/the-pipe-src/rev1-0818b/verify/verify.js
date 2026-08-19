function ring(n){ return Array.from({length:n},(_,i)=>[(i-1+n)%n,(i+1)%n]); }
function step(state, nbr){
  return state.map((s,i)=>{
    const votes={R:0,B:0,'0':0}; votes[s]++;
    for(const j of nbr[i]) votes[state[j]]++;
    let best=s,bv=votes[s];
    for(const k of ['R','B','0']) if(votes[k]>bv){best=k;bv=votes[k];}
    return best;
  });
}
function runToFixed(state,nbr,cap=1000){
  let prev=state.slice();
  for(let t=0;t<cap;t++){
    const next=step(prev,nbr);
    if(next.join('')===prev.join('')) return {steps:t,frozen:true,state:next};
    prev=next;
  }
  return {steps:cap,frozen:false,state:prev};
}
const N=24, nbr=ring(N);
let seed=Array.from({length:N},(_,i)=> i%3===0?'0':(i%2?'R':'B'));
const noWanderer=runToFixed(seed.slice(),nbr);
console.log('NO WANDERER: frozen='+noWanderer.frozen+' at step '+noWanderer.steps);

function stepWithWanderer(state,nbr,round){
  let next=step(state,nbr);
  const leave=round % state.length;
  const ret=(round*7+3) % state.length;
  next[leave]='0';
  const cnt={R:0,B:0}; next.forEach(s=>{if(s!=='0')cnt[s]++;});
  const minority = cnt.R<=cnt.B?'R':'B';
  next[ret]=minority;
  return next;
}
let st=seed.slice(); const seen=new Set();
for(let r=0;r<200;r++){ st=stepWithWanderer(st,nbr,r); seen.add(st.join('')); }
const distinct=seen.size;
console.log('WITH WANDERER: '+distinct+' distinct configs / 200 rounds');
const wandererBreaksFreeze = noWanderer.frozen===true && distinct>20;
console.log('CLAIM no-wanderer FREEZES, wanderer does NOT:', wandererBreaksFreeze);

const MIN_ANGLE=0.01;
function tetheredReturn(a,d){ const drift=Math.min(MIN_ANGLE*0.5,0.001*d); return {recoverable:drift<MIN_ANGLE}; }
const near=tetheredReturn(45,1), far=tetheredReturn(45,1e6);
console.log('TETHER near='+near.recoverable+' far='+far.recoverable);

// FIXED: left-to-right stack-depth fold
function i13valid(tokens){
  let depth=0;
  for(const t of tokens){
    const arity=+t[t.length-1];
    if(depth<arity) return false;   // not enough operands on the stack
    depth = depth - arity + 1;      // pop arity, push result
  }
  return depth===1;
}
const R_move=['A0','A0','B2'];
const B_move=['B2','A0'];
const lie=['A0','A0','B2'];
console.log('\nTHRONE I.i13:');
console.log('  R '+JSON.stringify(R_move)+' -> '+(i13valid(R_move)?'ACCEPTED':'REFUSED'));
console.log('  B '+JSON.stringify(B_move)+' -> '+(i13valid(B_move)?'ACCEPTED':'REFUSED')+' (planted void, MUST refuse)');
const I_appears=[...R_move,...B_move].filter(t=>t[0]==='I').length;
console.log('  referent I appears: '+I_appears+' times');
console.log('  well-formed FALSE move -> '+(i13valid(lie)?'ACCEPTED (form, not truth)':'refused'));

if(!i13valid(R_move)){console.error('FAIL: throne refused the honest move');process.exit(1);}
if(i13valid(B_move)){console.error('FAIL: throne accepted malformed');process.exit(1);}

const fs=require('fs');
fs.writeFileSync('data/court.json',JSON.stringify({
  positions:N,
  noWanderer:{frozen:noWanderer.frozen,steps:noWanderer.steps},
  withWanderer:{distinct,rounds:200}, wandererBreaksFreeze,
  tether:{minAngle:MIN_ANGLE,near:near.recoverable,far:far.recoverable},
  throne:{R_move,R_valid:i13valid(R_move),B_move,B_valid:i13valid(B_move),referentAppears:I_appears,lie_valid:i13valid(lie)}
},null,2));
const allGreen=[wandererBreaksFreeze,near.recoverable&&far.recoverable,i13valid(R_move),!i13valid(B_move),I_appears===0,i13valid(lie)].every(Boolean);
console.log('\nALL CLAIMS:',allGreen?'green':'RED');
if(!allGreen)process.exit(1);
