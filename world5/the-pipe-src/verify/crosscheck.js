const fs=require('fs');
const h=fs.readFileSync('rev1-0818c.html','utf8');
const m=h.match(/<script id="data" type="application\/json">([\s\S]*?)<\/script>/);
if(!m){console.error('FAIL no data');process.exit(1);}
const D=JSON.parse(m[1]);
let fail=0;const chk=(n,c)=>{console.log((c?'ok  ':'FAIL')+' '+n);if(!c)fail++;};
chk('descent sign is palindrome', D.signs.descent===D.signs.descent.split('').reverse().join('') && D.signs.descPalin);
chk('ascent sign is palindrome', D.signs.ascent===D.signs.ascent.split('').reverse().join('') && D.signs.ascPalin);
chk('descent/ascent are mirrors', D.signs.descent===D.signs.ascent.split('').map(c=>c==='+'?'-':'+').join('') && D.signs.mirror);
chk('all gens gated (floor+ceiling)', D.generations.every(g=>g.floor&&g.stable) && D.allGated);
chk('lineage advances gen0->gen4', D.generations[D.generations.length-1].gen===4 && D.finalGen===4);
chk('payload grew (evolution)', D.finalPayload.length>3);
chk('beacons all present + distinct', new Set(D.generations.map(g=>g.beacon)).size===D.generations.length);
chk('both known-bads caught', D.knownBadsCaught===2);
console.log('\n'+(fail===0?'CROSSCHECK GREEN — page agrees with re-derivation':'CROSSCHECK RED — '+fail));
process.exit(fail?1:0);
