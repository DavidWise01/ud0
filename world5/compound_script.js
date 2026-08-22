
"use strict";
/* THE GRADE IS A GATE. Real numbers from I13_CANONICAL_GRADE_v0.15.json; arithmetic reproduces 81.167% exactly. */
(function(){
  var CLASSES=[
    {n:"Public Conformance",        p:68,t:68,w:25},
    {n:"False-Positive Controls",   p:24,t:24,w:10},
    {n:"Full Regression",           p:54,t:54,w:15},
    {n:"Prior-Version Differential",p:11,t:12,w:10},
    {n:"Separated Holdout",         p:0, t:48,w:15},
    {n:"Mutation Kill",             p:48,t:60,w:15},
    {n:"Reproducibility & Transport",p:9, t:9, w:10}
  ];
  var out=document.getElementById("gout"), say=document.getElementById("gsay"), maxed=false;
  var HOLD="Separated Holdout";
  function pad(s,n){ s=String(s); while(s.length<n) s+=" "; return s; }
  function lpad(s,n){ s=String(s); while(s.length<n) s=" "+s; return s; }
  function render(){
    var tot=0, s="";
    for(var i=0;i<CLASSES.length;i++){ var c=CLASSES[i], p=c.p, t=c.t;
      if(maxed && c.n!==HOLD){ p=t; }                 // holdout can't be maxed: the evidence is ABSENT, not failing
      var rate=p/t, pts=rate*c.w; tot+=pts;
      var flag = (c.n===HOLD && p===0) ? "<span class='bad'>ABSENT</span>" : (rate<1 ? "<span class='g'>partial</span>" : "<span class='hit'>full</span>");
      s+= pad(c.n,28)+" "+lpad(p+"/"+t,7)+"  w"+lpad(c.w,2)+"  = "+lpad(pts.toFixed(3),6)+" pts   "+flag+"\n";
    }
    s+="\nweighted score = <span class='"+(tot>=85?"hit":"g")+"'>"+tot.toFixed(3)+"%</span>    independence = <span class='bad'>L0 (self-eval)</span>";
    s+="\ngate result    = <span class='bad'>INCOMPLETE</span>   (holdout class ABSENT + L0) &mdash; score cannot change this";
    out.innerHTML=s;
    say.innerHTML = maxed
      ? "<span class='err'>"+tot.toFixed(1)+"% &mdash; still INCOMPLETE</span>"
      : "<span class='err'>81.167% &mdash; INCOMPLETE by precedence</span>";
  }
  document.getElementById("gcalc").onclick=function(){ maxed=false; render(); };
  document.getElementById("gmax").onclick=function(){ maxed=!maxed; this.className=maxed?"on":""; this.textContent=maxed?"restore the real numbers":"fill every provable class to 100%"; render(); };
  render();
})();
/* THE DIFFERENTIAL SPINE. Outputs captured from the two shipped engines via a headless Node harness this session. */
(function(){
  var CASES=[
    { id:"P0-012-D01", src:"def f(I x){ -> x }\nI z <- f()",
      v14:{ok:false, observed:"reached VM execution boundary", msg:"(no diagnostic)", coord:"(none)"},
      v15:{ok:true,  observed:"compile rejection", msg:"function arity mismatch: f expects 1, got 0", coord:"line 2, col 8"} },
    { id:"P0-012-B01 (nested call)", src:"def f(I x){ -> x }\nI z <- g( f() )",
      v14:{ok:false, observed:"reached VM execution boundary", msg:"(no diagnostic)", coord:"(none)"},
      v15:{ok:true,  observed:"compile rejection", msg:"function arity mismatch: f expects 1, got 0", coord:"line 2, col 16"} }
  ];
  var ci=0, ver="v14";
  var out=document.getElementById("vout"), lbl=document.getElementById("vcase"), btn=document.getElementById("vtog");
  function render(){
    var c=CASES[ci], r=c[ver];
    var vtag = ver==="v14" ? "<span class='bad'>v0.14 (predecessor)</span>" : "<span class='hit'>v0.15 (fix)</span>";
    var s="";
    s+="case "+c.id+"\n";
    s+="source:\n  "+c.src.replace("\n","\n  ")+"\n\n";
    s+="engine "+vtag+"\n";
    s+="  outcome  : "+(r.ok? "<span class='hit'>REJECTED at compile (correct)</span>" : "<span class='bad'>accepted -> "+r.observed+"</span>")+"\n";
    s+="  message  : "+(r.ok? "<span class='hit'>"+r.msg+"</span>" : "<span class='dim'>"+r.msg+"</span>")+"\n";
    s+="  location : "+(r.ok? "<span class='g'>"+r.coord+"</span>" : "<span class='dim'>"+r.coord+"</span>")+"\n";
    if(ver==="v15") s+="\n<span class='hit'>caught before the WASM VM ever ran.</span>";
    else s+="\n<span class='bad'>the wrong-arity call slipped through to the VM boundary.</span>";
    out.innerHTML=s;
    lbl.innerHTML = "case "+(ci+1)+" / "+CASES.length;
  }
  btn.onclick=function(){
    if(ver==="v14"){ ver="v15"; this.textContent="◈ run on v0.15"; this.className="dartbtn on"; }
    else { ver="v14"; ci=(ci+1)%CASES.length; this.textContent="◈ run on v0.14"; this.className="dartbtn"; }
    render();
  };
  render();
})();
