STMBLPN=window.STMBLPN||{};
(function(){if(STMBLPN&&STMBLPN.hasHelperFunctions){return
}STMBLPN.isHttps=location.protocol.match(/https/);
STMBLPN.browser=function(){var a=navigator.userAgent;
return{ie:a.match(/MSIE\s([^;]*)/)}
}();
STMBLPN.trim=function(a){return a.replace(/^\s+|\s+$/g,"")
};
STMBLPN.byId=function(a){if(typeof a=="string"){return document.getElementById(a)
}return a
};
STMBLPN.loadStyleSheet=function(c,d,b){if(!STMBLPN.Namespace.loadingStyleSheet){STMBLPN.Namespace.loadingStyleSheet=true;
var a=document.createElement("link");
a.href=c;
a.rel="stylesheet";
a.type="text/css";
document.getElementsByTagName("head")[0].appendChild(a);
var e=setInterval(function(){var f=getStyle(b,"position");
if(f=="relative"){clearInterval(e);
e=null;
STMBLPN.Namespace.hasLoadedStyleSheet=true
}},50)
}};
STMBLPN.jsonP=function(b,d){var a=document.createElement("script");
var c=document.getElementsByTagName("head")[0];
a.type="text/javascript";
a.src=b;
c.insertBefore(a,c.firstChild);
d(a);
return a
};
STMBLPN.classes={has:function(a,b){return new RegExp("(^|\\s)"+b+"(\\s|$)").test(STMBLPN.byId(a).className)
},add:function(a,b){if(!this.has(a,b)){STMBLPN.byId(a).className=STMBLPN.trim(STMBLPN.byId(a).className)+" "+b
}},remove:function(a,b){if(this.has(a,b)){STMBLPN.byId(a).className=STMBLPN.byId(a).className.replace(new RegExp("(^|\\s)"+b+"(\\s|$)","g"),"")
}}};
STMBLPN.events={add:function(c,b,a){if(c.addEventListener){c.addEventListener(b,a,false)
}else{c.attachEvent("on"+b,function(){a.call(c,window.event)
})
}},remove:function(c,b,a){if(c.removeEventListener){c.removeEventListener(b,a,false)
}else{c.detachEvent("on"+b,a)
}}};
STMBLPN.wasBadgeDataColleted=false;
STMBLPN.collectBadgeData=function(){try{if(!window._gat){return
}var j=window._gat._getTrackerByName()._getAccount();
if(j.indexOf("UA-XXXX")==0){j=window.pageTracker._getAccount()
}var c={utmn:(new Date()).getTime(),utmhn:document.location.hostname,utmt:"event",utmr:"http://www.stumbleupon.com/refer.php",utmp:document.location.pathname,utmac:j,url:document.URL,utmcc:""};
var a={__utma:true,__utmb:true,__utmc:true,__utmz:true};
var m=document.cookie.split(";");
for(var g=0;
g<m.length;
g++){var l=m[g].indexOf("=");
if(l==-1){continue
}var d=m[g].substring(0,l);
var h=m[g].substring(l);
if(a[d]===true){c.utmcc+=(c.utmcc?";":"")+d+"="+h
}}var f=[];
for(var b in c){f.push('"'+b+'": "'+String(c[b]).replace('"','\\"')+'"')
}return'{"type": "SU_BADGEMESSAGE", "params": {'+f.join(", ")+"}}"
}catch(k){}};
STMBLPN.createIframe=function(d,b,a){var c=document.createElement("iframe");
c.scrolling="no";
c.frameBorder="0";
c.setAttribute("allowTransparency","true");
c.style.overflow="hidden";
c.style.margin=0;
c.style.padding=0;
c.style.border=0;
c.src=d;
if(b){c.width=b
}if(a){c.height=a
}return c
};
STMBLPN.isNode=function(a){return(typeof Node==="object"?a instanceof Node:typeof a==="object"&&typeof a.nodeType==="number"&&typeof a.nodeName==="string")
};
STMBLPN.wasProcessLoaded=false;
STMBLPN.processWidgets=function(){STMBLPN.wasProcessLoaded=true;
var g=document.getElementsByTagName("su:badge");
if(g){var d=["layout","location","id","domain"];
var h=[];
for(var f=0;
f<g.length;
f++){var c=g[f];
var k={container:c,type:"badge"};
for(var e=0;
e<d.length;
e++){var b=c.getAttribute(d[e]);
if(b){k[d[e]]=b
}}h.push(new STMBLPN.Widget(k))
}for(var f=0;
f<h.length;
f++){h[f].render()
}STMBLPN.Widget.sendBadgeData()
}g=document.getElementsByTagName("su:follow");
if(g){var h=[];
for(var f=0;
f<g.length;
f++){var c=g[f];
var k={container:c,type:"follow"};
k.ref=c.getAttribute("ref");
if(!k.ref){continue
}var d=["layout","id","domain"];
for(var e=0;
e<d.length;
e++){var b=c.getAttribute(d[e]);
if(b){k[d[e]]=b
}}h.push(new STMBLPN.Widget(k))
}for(var f=0;
f<h.length;
f++){h[f].render()
}}};
STMBLPN.hasHelperFunction=true;
return true
})();
(function(){if(STMBLPN&&STMBLPN.Widget){return
}STMBLPN.Widget=function(a){this.init(a)
};
(function(){STMBLPN.Widget.sendBadgeData=function(){try{if(STMBLPN.wasBadgeDataColleted){return
}STMBLPN.wasBadgeDataColleted=true;
var a=STMBLPN.collectBadgeData();
if(!a){return
}top.postMessage(a,"http://www.stumbleupon.com")
}catch(b){}};
STMBLPN.Widget.NUMBER=0;
STMBLPN.Widget.prototype=function(){var b=STMBLPN.isHttps?"https://":"http://";
var c="/badge/embed/";
var a="/widgets/follow_badge.php";
var d="/widgets/get.php";
return{init:function(m){var l=this;
this._badgeNumber=++STMBLPN.Widget.NUMBER;
this.selfContainer=false;
this.domain=m.domain?m.domain.replace(/^\.+/g,""):"stumbleupon.com";
this.type=m.type||"badge";
this.layout=m.layout?parseInt(m.layout):1;
this.layout=this.layout||1;
this.id=m.id||"stmblpn-widget-"+this._badgeNumber;
this.container=m.container;
if(m.id){var n=STMBLPN.byId(m.id);
if(n){this.selfContainer=true
}}if(!this.container&&!this.selfContainer){document.write('<div id="'+this.id+'"></div>');
this.container=STMBLPN.byId(this.id)
}switch(this.type){case"badge":this.location=m.location||document.URL;
break;
case"follow":this.ref=m.ref;
break;
case"bestof":this.title=m.title||"";
this.request=[];
var o=["usernames","channels","topics","sites"];
for(var f=0;
f<o.length;
f++){var h=o[f];
if(!m[h]){continue
}var j=m[h];
if(typeof j=="object"&&j instanceof Array){for(var g=0;
g<j.length;
g++){this.request.push(h+"[]="+encodeURIComponent(j[g]))
}}else{this.request.push(h+"="+encodeURIComponent(j))
}}break
}return this
},render:function(){var h=this;
var i=this._getIframeSrc();
var g=this._getIframeDimensions();
var f=null;
var e=STMBLPN.createIframe(i,g.width,g.height);
e.id="iframe-"+this.id;
if(this.container&&STMBLPN.isNode(this.container)){f=this.container.parentNode
}if(this.selfContainer){STMBLPN.byId(this.id).appendChild(e)
}else{if(f){f.insertBefore(e,this.container)
}}if(this.container&&STMBLPN.isNode(this.container)){f.removeChild(this.container);
delete this.container
}return this
},_getIframeSrc:function(){if(this.type=="badge"){return b+"www."+this.domain+c+this.layout+"/?url="+encodeURIComponent(this.location)
}else{if(this.type=="follow"){return b+"www."+this.domain+a+"?id="+this.ref+"&l="+this.layout
}else{if(this.type=="bestof"){return b+"www."+this.domain+d+"?"+this.request.join("&")+"&l="+this.layout+"&title="+encodeURIComponent(this.title)
}}}},_getIframeDimensions:function(){var e;
if(this.type=="badge"){e={1:{width:74,height:18},2:{width:65,height:18},4:{width:18,height:18},5:{width:50,height:60},6:{width:30,height:31},200:{width:108,height:22},310:{width:128,height:22}};
e[3]=e[2];
e[210]=e[200];
e[300]=e[200];
e[this.layout]=e[this.layout]||e[1];
return e[this.layout]
}else{if(this.type=="follow"){e={1:{width:154,height:21},2:{width:210,height:28},3:{width:160,height:105}};
e[this.layout]=e[this.layout]||e[1];
return e[this.layout]
}else{if(this.type=="bestof"){e={1:{width:300,height:250},2:{width:600,height:250},3:{width:160,height:600}};
e[this.layout]=e[this.layout]||e[1];
return e[this.layout]
}}}return{width:null,height:null}
}}
}()
})()
})();
(function(){if(STMBLPN.wasProcessLoaded==false){STMBLPN.events.add(window,"load",STMBLPN.processWidgets);
STMBLPN.wasProcessLoaded=true
}})();