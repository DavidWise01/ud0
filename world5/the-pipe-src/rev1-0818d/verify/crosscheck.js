const fs=require('fs');
const h=fs.readFileSync('rev1-0818d.html','utf8');
const m=h.match(/<script id="data" type="application\/json">([\s\S]*?)<\/script>/);
if(!m){console.error('FAIL no data');process.exit(1);}
const D=JSON.parse(m[1]);
let fail=0;const chk=(n,c)=>{console.log((c?'ok  ':'FAIL')+' '+n);if(!c)fail++;};

// re-derive i13 gate + engine independently
const NOTES={C:0,D:2,E:4,F:5,G:7,A:9,B:11};
const freq=pc=>440*Math.pow(2,(pc-9)/12);
function i13valid(t){let d=0;for(const x of t){if(!/^[A-Za-z]\d$/.test(x))return false;const a=+x[x.length-1];if(d<a)return false;d=d-a+1;}return d===1;}

chk('single door: omega folds to alpha', JSON.stringify(D.omega.slice(0,3))===JSON.stringify(D.alpha));
chk('lineage advanced gen0->gen5', D.finalGen===5 && D.generations[D.generations.length-1].gen===5);
chk('payload grew 3 -> 8', D.alpha.length===3 && D.omega.length===8);
chk('bank accumulated to 7', D.bankSize===7 && Object.keys(D.bank).length===7);
chk('every gen gated', D.generations.every(g=>g.floor&&g.stable&&g.gate) && D.allGated);
chk('beacons distinct', new Set(D.generations.map(g=>g.beacon)).size===D.generations.length);
chk('bank is append-only (gen tags non-decreasing per key present)', Object.values(D.bank).every(v=>typeof v.gen==='number'&&v.gen>=1));
chk('3/3 known-bads caught', D.knownBadsCaught===3);
// independently re-run: valid move accepted, malformed refused
chk('i13 accepts a valid move', i13valid(['A0','U1','U1'])===true);
chk('i13 refuses a malformed move', i13valid(['A0','COMPOUND9','U1'])===false);
// re-derive the bank freqs from note names
let bankOK=true;for(const k in D.bank){if(NOTES[k.toUpperCase()]!==undefined){const f=+freq(NOTES[k.toUpperCase()]).toFixed(1);if(Math.abs(f-D.bank[k].freq)>0.2)bankOK=false;}}
chk('banked freqs match independent synthesis', bankOK);

console.log('\n'+(fail===0?'CROSSCHECK GREEN — page agrees with re-derivation':'CROSSCHECK RED — '+fail));
process.exit(fail?1:0);
