const fs=require('fs');
const h=fs.readFileSync('rev1-0818b.html','utf8');
const m=h.match(/<script id="data" type="application\/json">([\s\S]*?)<\/script>/);
if(!m){console.error('FAIL no data');process.exit(1);}
const D=JSON.parse(m[1]);
let fail=0;const chk=(n,c)=>{console.log((c?'ok  ':'FAIL')+' '+n);if(!c)fail++;};

// re-derive throne validator independently
function i13valid(tokens){let d=0;for(const t of tokens){const a=+t[t.length-1];if(d<a)return false;d=d-a+1;}return d===1;}
chk('throne accepts R move', i13valid(D.throne.R_move)===true && D.throne.R_valid===true);
chk('throne refuses B move', i13valid(D.throne.B_move)===false && D.throne.B_valid===false);
chk('referent I silent', D.throne.referentAppears===0);
chk('well-formed lie passes (form not truth)', i13valid(['A0','A0','B2'])===true && D.throne.lie_valid===true);

// re-derive freeze/wanderer
function ring(n){return Array.from({length:n},(_,i)=>[(i-1+n)%n,(i+1)%n]);}
function step(s,nb){return s.map((v,i)=>{const ct={R:0,B:0,'0':0};ct[v]++;for(const j of nb[i])ct[s[j]]++;let b=v,bv=ct[v];for(const k of['R','B','0'])if(ct[k]>bv){b=k;bv=ct[k];}return b;});}
const N=24,nb=ring(N);let s=Array.from({length:N},(_,i)=>i%3===0?'0':(i%2?'R':'B'));
let prev=s.slice(),froze=false;for(let t=0;t<50;t++){const nx=step(prev,nb);if(nx.join('')===prev.join('')){froze=true;break;}prev=nx;}
chk('court freezes with no wanderer', froze && D.noWanderer.frozen);
let st=Array.from({length:N},(_,i)=>i%3===0?'0':(i%2?'R':'B'));const seen=new Set();
for(let r=0;r<200;r++){let nx=step(st,nb);const lv=r%N,rt=(r*7+3)%N;nx[lv]='0';const ct={R:0,B:0};nx.forEach(v=>{if(v!=='0')ct[v]++;});nx[rt]=ct.R<=ct.B?'R':'B';seen.add(nx.join(''));st=nx;}
chk('wanderer yields many configs', seen.size>20 && D.withWanderer.distinct===seen.size);
chk('tether recoverable near AND far', D.tether.near===true && D.tether.far===true);

console.log('\n'+(fail===0?'CROSSCHECK GREEN — page agrees with re-derivation':'CROSSCHECK RED — '+fail));
process.exit(fail?1:0);
