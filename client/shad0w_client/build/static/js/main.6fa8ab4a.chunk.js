(window.webpackJsonp=window.webpackJsonp||[]).push([[0],{16:function(e,t,n){e.exports=n.p+"static/media/logo.e7a463a5.png"},21:function(e,t,n){"use strict";(function(e){n.d(t,"a",function(){return v});var a=n(6),o=n(7),i=n(9),s=n(8),r=n(10),l=n(0),c=n.n(l),u=n(11),m=n.n(u),d=(n(32),n(15)),h=n.n(d),p=n(4),g=n.n(p),v=function(t){function n(){var e,t;Object(a.a)(this,n);for(var o=arguments.length,r=new Array(o),l=0;l<o;l++)r[l]=arguments[l];return(t=Object(i.a)(this,(e=Object(s.a)(n)).call.apply(e,[this].concat(r)))).state={},t.containerRef=c.a.createRef(),t.goldenLayoutInstance=void 0,t}return Object(r.a)(n,t),Object(o.a)(n,[{key:"render",value:function(){var e=Array.from(this.state.renderPanels||[]);return c.a.createElement("div",Object.assign({ref:this.containerRef},this.props.htmlAttrs),e.map(function(e,t){return m.a.createPortal(e._getReactComponent(),e._container.getElement()[0])}))}},{key:"componentRender",value:function(e){this.setState(function(t){var n=new Set(t.renderPanels);return n.add(e),{renderPanels:n}})}},{key:"componentDestroy",value:function(e){this.setState(function(t){var n=new Set(t.renderPanels);return n.delete(e),{renderPanels:n}})}},{key:"componentDidMount",value:function(){this.goldenLayoutInstance=new h.a(this.props.config||{},g()("#layoutContainer")),e.myMainLayout=this.goldenLayoutInstance,this.props.registerComponents instanceof Function&&this.props.registerComponents(this.goldenLayoutInstance),this.goldenLayoutInstance.reactContainer=this,this.goldenLayoutInstance.init();var t=this.goldenLayoutInstance;window.addEventListener("resize",function(e){t.updateSize("100%",g()(window).height())})}}]),n}(c.a.Component),k=function(e){function t(){return Object(a.a)(this,t),Object(i.a)(this,Object(s.a)(t).apply(this,arguments))}return Object(r.a)(t,e),Object(o.a)(t,[{key:"_render",value:function(){var e=this._container.layoutManager.reactContainer;e&&e.componentRender&&e.componentRender(this)}},{key:"_destroy",value:function(){this._container.off("open",this._render,this),this._container.off("destroy",this._destroy,this)}},{key:"_getReactComponent",value:function(){var e={glEventHub:this._container.layoutManager.eventHub,glContainer:this._container},t=g.a.extend(e,this._container._config.props);return c.a.createElement(this._reactClass,t)}}]),t}(h.a.__lm.utils.ReactComponentHandler);h.a.__lm.utils.ReactComponentHandler=k}).call(this,n(14))},22:function(e,t,n){"use strict";(function(e){n.d(t,"a",function(){return v});var a=n(6),o=n(7),i=n(9),s=n(8),r=n(10),l=n(12),c=n(0),u=n.n(c),m=n(13),d=n(4),h=n.n(d),p=n(5),g=n(35),v=function(t){function c(e){var t;return Object(a.a)(this,c),(t=Object(i.a)(this,Object(s.a)(c).call(this,e))).returnBeaconData=t.returnBeaconData.bind(Object(l.a)(Object(l.a)(t))),t.state={isLoaded:!1,focused:!1,agents:[],dragSources:[],activeIndex:{},allIndexes:{}},t}return Object(r.a)(c,t),Object(o.a)(c,[{key:"render",value:function(){var t=this,a=this.state,o=a.isLoaded,i=a.agents,s=a.dragSources,r=a.allIndexes;if(!o){var l=n(20);return u.a.createElement("div",{className:"loading"},u.a.createElement(l,{name:"spinner enlargeSpinner",className:"rotate"}),u.a.createElement("br",null),"LOADING")}for(var c=0;c<localStorage.length;c++){var d=localStorage.key(c);"savedConnections"===d||d.includes("Host")||d.includes("Nick")||d.includes("Token")||d.includes("gl-")||d.includes("_latest")||d.includes("token")||d in i||(localStorage.removeItem(d),localStorage.removeItem(d+"_latest"))}var p=Object.keys(i).map(function(t,n){var a=this,o=i[t],s=t;function l(){this.selectRow(n);var t={title:i[s].ip_addr+"@"+s,type:"react-component",component:"agents-menu-component",props:{agentuuid:s,returnBeaconData:this.returnBeaconData}};null===e.myMainLayout.selectedItem?e.myMainLayout.root.contentItems[0].addChild(t):e.myMainLayout.selectedItem.addChild(t)}return r[n]=s,u.a.createElement(m.b,{key:s,id:s+"shad0wMENU",renderTag:"tr",attributes:{id:s+"shad0w",className:"pointer "+(n in this.state.activeIndex&&"active-index")}},u.a.createElement("td",{onContextMenu:function(e){return a.selectRowRight(n,s)},onClick:function(e){return a.selectRow(n,s)},onDoubleClick:l.bind(this)},s),u.a.createElement("td",{onContextMenu:function(e){return a.selectRowRight(n,s)},onClick:function(e){return a.selectRow(n,s)},onDoubleClick:l.bind(this)},o.ip_addr),u.a.createElement("td",{onContextMenu:function(e){return a.selectRowRight(n,s)},onClick:function(e){return a.selectRow(n,s)},onDoubleClick:l.bind(this)},o.username),u.a.createElement("td",{onContextMenu:function(e){return a.selectRowRight(n,s)},onClick:function(e){return a.selectRow(n,s)},onDoubleClick:l.bind(this)},o.machine),u.a.createElement("td",{onContextMenu:function(e){return a.selectRowRight(n,s)},onClick:function(e){return a.selectRow(n,s)},onDoubleClick:l.bind(this)},o.arch),u.a.createElement("td",{onContextMenu:function(e){return a.selectRowRight(n,s)},onClick:function(e){return a.selectRow(n,s)},onDoubleClick:l.bind(this)},o.os),u.a.createElement("td",{onContextMenu:function(e){return a.selectRowRight(n,s)},onClick:function(e){return a.selectRow(n,s)},onDoubleClick:l.bind(this)},o.domain),u.a.createElement("td",{onContextMenu:function(e){return a.selectRowRight(n,s)},onClick:function(e){return a.selectRow(n,s)},onDoubleClick:l.bind(this)},!0===o.secure?"True":"False"),u.a.createElement("td",{onContextMenu:function(e){return a.selectRowRight(n,s)},onClick:function(e){return a.selectRow(n,s)},onDoubleClick:l.bind(this)},o.last_checkin_raw))}.bind(this)),g=!1;if((/(android|bb\d+|meego).+mobile|avantgo|bada\/|blackberry|blazer|compal|elaine|fennec|hiptop|iemobile|ip(hone|od)|ipad|iris|kindle|Android|Silk|lge |maemo|midp|mmp|netfront|opera m(ob|in)i|palm( os)?|phone|p(ixi|re)\/|plucker|pocket|psp|series(4|6)0|symbian|treo|up\.(browser|link)|vodafone|wap|windows (ce|phone)|xda|xiino/i.test(navigator.userAgent)||/1207|6310|6590|3gso|4thp|50[1-6]i|770s|802s|a wa|abac|ac(er|oo|s\-)|ai(ko|rn)|al(av|ca|co)|amoi|an(ex|ny|yw)|aptu|ar(ch|go)|as(te|us)|attw|au(di|\-m|r |s )|avan|be(ck|ll|nq)|bi(lb|rd)|bl(ac|az)|br(e|v)w|bumb|bw\-(n|u)|c55\/|capi|ccwa|cdm\-|cell|chtm|cldc|cmd\-|co(mp|nd)|craw|da(it|ll|ng)|dbte|dc\-s|devi|dica|dmob|do(c|p)o|ds(12|\-d)|el(49|ai)|em(l2|ul)|er(ic|k0)|esl8|ez([4-7]0|os|wa|ze)|fetc|fly(\-|_)|g1 u|g560|gene|gf\-5|g\-mo|go(\.w|od)|gr(ad|un)|haie|hcit|hd\-(m|p|t)|hei\-|hi(pt|ta)|hp( i|ip)|hs\-c|ht(c(\-| |_|a|g|p|s|t)|tp)|hu(aw|tc)|i\-(20|go|ma)|i230|iac( |\-|\/)|ibro|idea|ig01|ikom|im1k|inno|ipaq|iris|ja(t|v)a|jbro|jemu|jigs|kddi|keji|kgt( |\/)|klon|kpt |kwc\-|kyo(c|k)|le(no|xi)|lg( g|\/(k|l|u)|50|54|\-[a-w])|libw|lynx|m1\-w|m3ga|m50\/|ma(te|ui|xo)|mc(01|21|ca)|m\-cr|me(rc|ri)|mi(o8|oa|ts)|mmef|mo(01|02|bi|de|do|t(\-| |o|v)|zz)|mt(50|p1|v )|mwbp|mywa|n10[0-2]|n20[2-3]|n30(0|2)|n50(0|2|5)|n7(0(0|1)|10)|ne((c|m)\-|on|tf|wf|wg|wt)|nok(6|i)|nzph|o2im|op(ti|wv)|oran|owg1|p800|pan(a|d|t)|pdxg|pg(13|\-([1-8]|c))|phil|pire|pl(ay|uc)|pn\-2|po(ck|rt|se)|prox|psio|pt\-g|qa\-a|qc(07|12|21|32|60|\-[2-7]|i\-)|qtek|r380|r600|raks|rim9|ro(ve|zo)|s55\/|sa(ge|ma|mm|ms|ny|va)|sc(01|h\-|oo|p\-)|sdk\/|se(c(\-|0|1)|47|mc|nd|ri)|sgh\-|shar|sie(\-|m)|sk\-0|sl(45|id)|sm(al|ar|b3|it|t5)|so(ft|ny)|sp(01|h\-|v\-|v )|sy(01|mb)|t2(18|50)|t6(00|10|18)|ta(gt|lk)|tcl\-|tdg\-|tel(i|m)|tim\-|t\-mo|to(pl|sh)|ts(70|m\-|m3|m5)|tx\-9|up(\.b|g1|si)|utst|v400|v750|veri|vi(rg|te)|vk(40|5[0-3]|\-v)|vm40|voda|vulc|vx(52|53|60|61|70|80|81|83|85|98)|w3c(\-| )|webc|whit|wi(g |nc|nw)|wmlb|wonu|x700|yas\-|your|zeto|zte\-/i.test(navigator.userAgent.substr(0,4)))&&(g=!0),!g){var v=this.state.agents,k=this;h()(".pointer").each(function(){var t=h()(this).find("td").eq(0).text(),n={title:v[t]?v[t].ip_addr+"@"+t:"undefined",type:"react-component",component:"agents-menu-component",props:{agentuuid:t,returnBeaconData:k.returnBeaconData}};s.includes(t)||(e.myMainLayout.createDragSource(h()(this),n),s.push(t))}),s.forEach(function(e){if(0===h()("#"+e+"shad0w").length){var t=s.indexOf(e);-1!==t&&s.splice(t,1)}})}var f=Object.keys(i).map(function(n){var a=t.state.activeIndex,o=t;return u.a.createElement(m.a,{key:n+"shad0wMENU",id:n+"shad0wMENU"},u.a.createElement(m.c,null,u.a.createElement("strong",null,Object.keys(t.state.activeIndex).length>1?"MULTIPLE":n+":"+t.state.agents[n].username)),u.a.createElement(m.c,{onClick:function(){for(var t in a){var n={title:i[a[t]]?i[a[t]].ip_addr+"@"+a[t]:"undefined",type:"react-component",component:"agents-menu-component",props:{agentuuid:a[t],returnBeaconData:o.returnBeaconData}};null===e.myMainLayout.selectedItem?e.myMainLayout.root.contentItems[0].addChild(n):e.myMainLayout.selectedItem.addChild(n)}}},"Interact"))});return u.a.createElement("div",{tabIndex:"1",onKeyDown:function(e){return t.selectMany(e)},className:"scrollable-noflex-nopadding table-responsive",id:"Agents_main",ref:function(e){return t.node=e}},f,u.a.createElement("table",{className:"table"},u.a.createElement("thead",null,u.a.createElement("tr",{className:"noHover"},u.a.createElement("th",null,"Agent"),u.a.createElement("th",null,"IP"),u.a.createElement("th",null,"User"),u.a.createElement("th",null,"Hostname"),u.a.createElement("th",null,"Architecture"),u.a.createElement("th",null,"OS"),u.a.createElement("th",null,"Domain"),u.a.createElement("th",null,"Secure"),u.a.createElement("th",null,"Last Checkin"))),u.a.createElement("tbody",null,p)))}},{key:"selectRow",value:function(e,t){document.getElementById("Agents_main").focus();var n=this.state.activeIndex;e in this.state.activeIndex?delete n[e]:n[e]=t,this.setState({activeIndex:n})}},{key:"selectRowRight",value:function(e,t){document.getElementById("Agents_main").focus();var n=this.state.activeIndex;n[e]=t,this.setState({activeIndex:n})}},{key:"selectMany",value:function(e){var t=this.state,n=t.activeIndex,a=t.allIndexes;if(a={},27===e.keyCode)return n={},void this.setState({activeIndex:n});if(16===e.keyCode)for(var o in this.state.allIndexes){var i=[];for(var s in n)i.push(s);var r=Math.max.apply(Math,i),l=Math.min.apply(Math,i);parseInt(o)<r&&parseInt(o)>=l&&r!==l&&(n[o]=this.state.allIndexes[o])}this.setState({activeIndex:n,allIndexes:a})}},{key:"handleClick",value:function(e){if("Agents_main"===e.target.id)this.setState({activeIndex:{}});else if(this.node.contains(e.target))return;this.setState({activeIndex:{}})}},{key:"getBeacons",value:function(){var e;try{return(e=g(Object(p.b)("Host")+"/beacons",{method:"GET",credentials:"include",headers:{Cookie:Object(p.b)("tokenName")+"="+Object(p.b)("tokenValue")}}).json()).failed&&(localStorage.removeItem("Nick"),localStorage.removeItem("Host"),localStorage.removeItem("Token"),localStorage.removeItem("tokenName"),localStorage.removeItem("tokenValue"),document.cookie="SDWAuth= ; expires = Thu, 01 Jan 1970 00:00:00 GMT",window.location.reload()),e}catch(t){return this.state.agents}}},{key:"getBeaconData",value:function(){var e;try{return(e=g(Object(p.b)("Host")+"/poll",{method:"GET",credentials:"include",headers:{Cookie:Object(p.b)("tokenName")+"="+Object(p.b)("tokenValue")}}).json()).failed&&(localStorage.removeItem("Nick"),localStorage.removeItem("Host"),localStorage.removeItem("Token"),localStorage.removeItem("tokenName"),localStorage.removeItem("tokenValue"),document.cookie="SDWAuth= ; expires = Thu, 01 Jan 1970 00:00:00 GMT",window.location.reload()),e}catch(t){return this.state.allAgentData}}},{key:"returnBeaconData",value:function(){return this.state.allAgentData}},{key:"componentDidMount",value:function(){var e=this,t=this.state.agents;t=this.getBeacons(),document.addEventListener("mousedown",function(t){return e.handleClick(t)},!1),this.setState({agents:t,isLoaded:!0}),setInterval(function(t){var n=e.state.agents;n=e.getBeacons(),e.setState({agents:n})},1e3),setInterval(function(t){var n=e.state.allAgentData;n=e.getBeaconData(),e.setState({allAgentData:n})},1e3)}},{key:"componentWillUnmount",value:function(){var e=this;document.removeEventListener("mousedown",function(t){return e.handleClick(t)},!1)}},{key:"componentDidUpdate",value:function(){}}]),c}(u.a.Component)}).call(this,n(14))},23:function(e,t,n){"use strict";(function(e){n.d(t,"a",function(){return m});var a=n(6),o=n(7),i=n(9),s=n(8),r=n(10),l=n(0),c=n.n(l),u=n(5),m=function(t){function n(e){var t;return Object(a.a)(this,n),(t=Object(i.a)(this,Object(s.a)(n).call(this,e))).time=null,t.scrollToBottom=function(){t.messagesEnd.current.scrollIntoView({behavior:"smooth"})},t.state={results:"",agentuuid:t.props.agentuuid,value:"",history:[],historyval:0,historylatest:"",scroll:!0,displayDrop:!1},t.messagesEnd=c.a.createRef(),t.messageInput=c.a.createRef(),t.bg=c.a.createRef(),t}return Object(r.a)(n,t),Object(o.a)(n,[{key:"render",value:function(){var e,t=this,n=this.state,a=n.agentuuid,o=n.results,i=(0,this.props.returnBeaconData)();try{e=i[a]}catch(s){e=[]}return c.a.createElement("div",{className:"wrap agent-terminal",ref:this.bg,onClick:function(e){return t.onBgClick(e)},onDoubleClick:function(e){return t.onBgClickDouble(e)},onContextMenu:function(e){return t.handleClick(e)}},c.a.createElement("div",{className:"output"},c.a.createElement("div",null,"[+] Agent ",a," ",e?"Connected.":"Disconnected."),c.a.createElement("div",null,"Welcome to Shad0w C2."),c.a.createElement("br",null),c.a.createElement("div",null,"Enter a command."),c.a.createElement("div",null,"Click the window to scroll to the bottom.  To select text, right click to disable this feature and right click again to re-enable."),c.a.createElement("br",null),c.a.createElement("div",{className:"pre"},o),c.a.createElement("div",{style:{display:"flex"}},c.a.createElement("div",{style:{whiteSpace:"nowrap"}},this.state.scroll&&"*"," Shad0w ","> "," \xa0"),c.a.createElement("input",{ref:this.messageInput,maxLength:"512",onChange:function(e){this.handleChange(e)}.bind(this),value:this.state.value,className:"input clear",onKeyDown:function(e){this.keyPress(e)}.bind(this)})),c.a.createElement("div",{style:{height:"0"},ref:this.messagesEnd})))}},{key:"print_result_newline",value:function(e){var t=this,n=this.state.agentuuid,a=this.state,o=a.results,i=a.latest_result;if(localStorage.getItem(n)&&(o=localStorage.getItem(n)),localStorage.getItem(n+"_latest")&&(i=localStorage.getItem(n+"_latest")),i!==e){o+="Shad0w > "+e+"\n";t.setState({results:o},function(){t.bottomScroll()}),localStorage.setItem(n,o)}}},{key:"print_result",value:function(e){var t=this,n=this.state,a=n.agentuuid,o=n.latest_result,i=e.length,s=this.state.results;if(localStorage.getItem(a)&&(s=localStorage.getItem(a)),s+="LOAD"===e?"":"Results >>\n"+e+"\n[+] Host responded with ["+i+"] bytes.\n",o!==e){t.setState({results:s,latest_result:e,dedup:!0},function(){localStorage.setItem(a,s),t.bottomScroll()})}}},{key:"onBgClick",value:function(e){this.state.scroll&&(!1===this.isInViewport()?this.bottomScroll():this.messageInput.current.focus())}},{key:"onBgClickDouble",value:function(e){this.state.scroll&&(!1===this.isInViewport()?this.bottomScroll():this.messageInput.current.focus())}},{key:"handleClick",value:function(e){e.preventDefault();var t=this.state.scroll;2===e.button&&(t=!t,this.setState({scroll:t}))}},{key:"handleChange",value:function(e){this.setState({value:e.target.value})}},{key:"keyPress",value:function(e){if(38===e.keyCode){var t=this.state,n=t.value,a=t.history,o=t.historyval,i=t.historylatest;return o===a.length&&(i=n),o>0&&o--,n=a[o],e.preventDefault(),this.HAXLOL=!0,this.setState({value:n,historyval:o,historylatest:i}),!1}if(40===e.keyCode){var s=this.state,r=s.value,l=s.history,c=s.historyval,u=s.historylatest;return c!==l.length&&c++,r=c===l.length+1?l[c-1]:c===l.length?u:l[c],e.preventDefault(),this.HAXLOL=!0,this.setState({value:r,historyval:c}),!1}if(13===e.keyCode){this.bottomScroll();var m=this.state.agentuuid;if(""===this.state.value)return this.print_result_newline(" "),!1;"clear"===this.state.value||"clear"===this.state.value.split(" ")[0]?(this.setState({value:"",results:"",latest_result:""}),localStorage.removeItem(m),localStorage.removeItem(m+"_latest")):(this.print_result_newline("".concat(this.state.value,'\n[+] Tasking agent to run "').concat(this.state.value,'"')),this.sendAgentData())}}},{key:"sendAgentData",value:function(t){var n=this,a=this.state,o=a.history,i=a.historyval,s=a.value,r=(a.historylatest,a.agentuuid);if("",o.length>1024&&(o=o.slice(o.length-1024)),!0===t){o=o.concat(s);var l=s.lastIndexOf(" "),c=s.substring(0,l);o=o.concat(c)}else o=o.concat(s);i=o.length,this.setState({history:o,historyval:i,value:s,historylatest:""});var m=e.from(s).toString("base64");fetch(Object(u.b)("Host")+"/cmd",{method:"POST",headers:{Accept:"application/json","Content-Type":"application/json"},body:JSON.stringify({beacon_id:r,cmd:m}),credentials:"include"}).then(function(e){return e.json()}).then(function(e){console.log(document.cookie),!0===e.success&&console.log("Success")}).catch(function(e){n.print_result("Invalid Command.\n"),console.log(e)}),this.setState({value:""})}},{key:"bottomScroll",value:function(){this.scrollToBottom();var e=this.messageInput.current;if(e&&this.HAXLOL){this.HAXLOL=!1;var t=this.state.value;null!=t&&e.setSelectionRange(t.length,t.length)}}},{key:"isInViewport",value:function(){var e=arguments.length>0&&void 0!==arguments[0]?arguments[0]:0;if(!this.messageInput.current)return!1;var t=this.messageInput.current.getBoundingClientRect().top;return t+e>=0&&t-e<=window.innerHeight}},{key:"componentDidMount",value:function(){var e=this;this.scrollToBottom();var t=this.state.agentuuid,n=(0,this.props.returnBeaconData)();void 0!==n&&t in n&&this.print_result("LOAD"),setInterval(function(t){return e.forceUpdate()},1e3)}},{key:"componentDidUpdate",value:function(){var e=this.state.agentuuid,t=(0,this.props.returnBeaconData)();if(t[e].length>0){var n=t[e];this.print_result(n.join("\n"))}}}]),n}(c.a.Component)}).call(this,n(19).Buffer)},24:function(e,t,n){e.exports=n.p+"static/media/background.977e3fd0.jpg"},25:function(e){e.exports={name:"shad0w-gui",version:"1.0.0",description:"First version of shad0w gui",private:!0,dependencies:{"bootstrap-css-only":"^4.3.1",enzyme:"^3.10.0","golden-layout":"^1.5.9",grpc:"^1.24.2","js-file-download":"^0.4.8",md5:"^2.2.1",react:"^16.12.0","react-confirm-alert":"^2.4.1","react-contextmenu":"^2.13.0","react-dom":"^16.12.0","react-file-drop":"^0.2.8","react-fontawesome":"^1.7.1","react-icons":"^3.8.0","react-router-dom":"^4.3.1","react-vis-network":"^1.0.0","sync-fetch":"^0.3.0"},homepage:"./",main:"electron-starter.js",scripts:{start:"react-scripts start",build:"react-scripts build",test:"react-scripts test",eject:"react-scripts eject",electron:"electron .","electron-build":"npm run build && electron ."},eslintConfig:{extends:"react-app"},browserslist:[">0.2%","not dead","not ie <= 11","not op_mini all"],devDependencies:{electron:"^5.0.13","electron-packager":"^14.1.1","react-scripts":"^2.1.8"}}},26:function(e,t,n){e.exports=n(64)},31:function(e,t,n){},32:function(e,t,n){"use strict";var a=n(4),o=n.n(a);window.$=o.a,window.jQuery=o.a,window.jquery=o.a},5:function(e,t,n){"use strict";n.d(t,"b",function(){return f}),n.d(t,"a",function(){return b});var a=n(6),o=n(7),i=n(9),s=n(8),r=n(10),l=n(0),c=n.n(l),u=n(21),m=n(22),d=n(23),h=(n(58),n(59),n(60),n(61),n(62),n(16)),p=n.n(h),g=n(24),v=n.n(g),k=n(25);function f(e){return localStorage.getItem(e)}var b=function(e){function t(e){var n;return Object(a.a)(this,t),(n=Object(i.a)(this,Object(s.a)(t).call(this,e))).timer=void 0,n.state={isLoaded:!1,nickValue:"admin",hostValue:"http://127.0.0.1:1337",tokenValue:"shad0w",commState:{}},n}return Object(r.a)(t,e),Object(o.a)(t,[{key:"handleChangeNick",value:function(e){this.setState({nickValue:e.target.value})}},{key:"handleChangeHost",value:function(e){this.setState({hostValue:e.target.value})}},{key:"handleChangeToken",value:function(e){this.setState({tokenValue:e.target.value})}},{key:"setCookie",value:function(e,t){localStorage.setItem(e,t)}},{key:"setAllCookies",value:function(){var e=this;this.setCookie("Host",this.state.hostValue),this.setCookie("LastHost",this.state.hostValue),this.setCookie("Nick",this.state.nickValue),this.setCookie("LastNick",this.state.nickValue),this.setCookie("Token",this.state.tokenValue),this.setCookie("LastToken",this.state.tokenValue);var t=this.state,n=t.hostValue,a=t.tokenValue,o=t.nickValue,i={};if(i[n]={nick:o,token:a},localStorage.getItem("savedConnections")){var s=localStorage.getItem("savedConnections");this.setCookie("savedConnections",JSON.stringify(Object.assign({},JSON.parse(s),i)))}else this.setCookie("savedConnections",JSON.stringify(i));fetch(f("Host")+"/login",{method:"POST",headers:{Accept:"application/json","Content-Type":"application/json"},body:JSON.stringify({username:f("Nick"),password:f("Token")}),credentials:"include"}).then(function(e){return e.json()}).then(function(t){if(!1===t.success)alert("BAD CREDENTIALS.  Try again!");else{console.log("OK");var n=new Date,a=n.getTime()+36e6;n.setTime(a),document.cookie=t.tokenName+"="+t.tokenValue+";expires="+n.toUTCString()+";path=/",e.setCookie("tokenName",t.tokenName),e.setCookie("tokenValue",t.tokenValue),window.location.reload()}}).catch(function(e){alert("Connection failed."),console.log(e)})}},{key:"eraseCookie",value:function(e){localStorage.removeItem(e)}},{key:"eraseAllCookies",value:function(){this.eraseCookie("Nick"),this.eraseCookie("Host"),this.eraseCookie("Token"),this.eraseCookie("tokenName"),this.eraseCookie("tokenValue"),document.cookie="SDWAuth= ; expires = Thu, 01 Jan 1970 00:00:00 GMT",window.location.reload()}},{key:"UNSAFE_componentWillMount",value:function(){f("LastHost")&&this.setState({hostValue:f("LastHost")}),f("LastNick")&&this.setState({nickValue:f("LastNick")}),f("LastToken")&&this.setState({tokenValue:f("LastToken")}),f("tokenName")&&f("tokenValue")&&this.setState({isLoaded:!0})}},{key:"change",value:function(e){this.setState({hostValue:e.target.value}),this.setState({nickValue:JSON.parse(localStorage.getItem("savedConnections"))[e.target.value].nick}),this.setState({tokenValue:JSON.parse(localStorage.getItem("savedConnections"))[e.target.value].token}),this.setState({connOption:e.target.value})}},{key:"clearAllConnections",value:function(){this.eraseCookie("savedConnections"),this.eraseCookie("LastHost"),this.eraseCookie("LastNick"),this.eraseCookie("LastToken")}},{key:"clearConnection",value:function(){var e=this.state.connOption,t=JSON.parse(f("savedConnections"));delete t[e],this.setCookie("savedConnections",JSON.stringify(t))}},{key:"showSidebar",value:function(e){if("LI"===e.target.tagName||"UL"===e.target.tagName||"SPAN"===e.target.tagName)return!1;var t=!1;(/(android|bb\d+|meego).+mobile|avantgo|bada\/|blackberry|blazer|compal|elaine|fennec|hiptop|iemobile|ip(hone|od)|ipad|iris|kindle|Android|Silk|lge |maemo|midp|mmp|netfront|opera m(ob|in)i|palm( os)?|phone|p(ixi|re)\/|plucker|pocket|psp|series(4|6)0|symbian|treo|up\.(browser|link)|vodafone|wap|windows (ce|phone)|xda|xiino/i.test(navigator.userAgent)||/1207|6310|6590|3gso|4thp|50[1-6]i|770s|802s|a wa|abac|ac(er|oo|s\-)|ai(ko|rn)|al(av|ca|co)|amoi|an(ex|ny|yw)|aptu|ar(ch|go)|as(te|us)|attw|au(di|\-m|r |s )|avan|be(ck|ll|nq)|bi(lb|rd)|bl(ac|az)|br(e|v)w|bumb|bw\-(n|u)|c55\/|capi|ccwa|cdm\-|cell|chtm|cldc|cmd\-|co(mp|nd)|craw|da(it|ll|ng)|dbte|dc\-s|devi|dica|dmob|do(c|p)o|ds(12|\-d)|el(49|ai)|em(l2|ul)|er(ic|k0)|esl8|ez([4-7]0|os|wa|ze)|fetc|fly(\-|_)|g1 u|g560|gene|gf\-5|g\-mo|go(\.w|od)|gr(ad|un)|haie|hcit|hd\-(m|p|t)|hei\-|hi(pt|ta)|hp( i|ip)|hs\-c|ht(c(\-| |_|a|g|p|s|t)|tp)|hu(aw|tc)|i\-(20|go|ma)|i230|iac( |\-|\/)|ibro|idea|ig01|ikom|im1k|inno|ipaq|iris|ja(t|v)a|jbro|jemu|jigs|kddi|keji|kgt( |\/)|klon|kpt |kwc\-|kyo(c|k)|le(no|xi)|lg( g|\/(k|l|u)|50|54|\-[a-w])|libw|lynx|m1\-w|m3ga|m50\/|ma(te|ui|xo)|mc(01|21|ca)|m\-cr|me(rc|ri)|mi(o8|oa|ts)|mmef|mo(01|02|bi|de|do|t(\-| |o|v)|zz)|mt(50|p1|v )|mwbp|mywa|n10[0-2]|n20[2-3]|n30(0|2)|n50(0|2|5)|n7(0(0|1)|10)|ne((c|m)\-|on|tf|wf|wg|wt)|nok(6|i)|nzph|o2im|op(ti|wv)|oran|owg1|p800|pan(a|d|t)|pdxg|pg(13|\-([1-8]|c))|phil|pire|pl(ay|uc)|pn\-2|po(ck|rt|se)|prox|psio|pt\-g|qa\-a|qc(07|12|21|32|60|\-[2-7]|i\-)|qtek|r380|r600|raks|rim9|ro(ve|zo)|s55\/|sa(ge|ma|mm|ms|ny|va)|sc(01|h\-|oo|p\-)|sdk\/|se(c(\-|0|1)|47|mc|nd|ri)|sgh\-|shar|sie(\-|m)|sk\-0|sl(45|id)|sm(al|ar|b3|it|t5)|so(ft|ny)|sp(01|h\-|v\-|v )|sy(01|mb)|t2(18|50)|t6(00|10|18)|ta(gt|lk)|tcl\-|tdg\-|tel(i|m)|tim\-|t\-mo|to(pl|sh)|ts(70|m\-|m3|m5)|tx\-9|up(\.b|g1|si)|utst|v400|v750|veri|vi(rg|te)|vk(40|5[0-3]|\-v)|vm40|voda|vulc|vx(52|53|60|61|70|80|81|83|85|98)|w3c(\-| )|webc|whit|wi(g |nc|nw)|wmlb|wonu|x700|yas\-|your|zeto|zte\-/i.test(navigator.userAgent.substr(0,4)))&&(t=!0);var n=this.state.menuVisible;(n=!n)&&!t?this.restartTimer():t||this.timer&&(clearTimeout(this.timer),this.timer=void 0),this.setState({menuVisible:n})}},{key:"toggleSidebar",value:function(){this.timer=void 0,this.setState({menuVisible:!this.state.menuVisible})}},{key:"restartTimer",value:function(){var e=!1;if((/(android|bb\d+|meego).+mobile|avantgo|bada\/|blackberry|blazer|compal|elaine|fennec|hiptop|iemobile|ip(hone|od)|ipad|iris|kindle|Android|Silk|lge |maemo|midp|mmp|netfront|opera m(ob|in)i|palm( os)?|phone|p(ixi|re)\/|plucker|pocket|psp|series(4|6)0|symbian|treo|up\.(browser|link)|vodafone|wap|windows (ce|phone)|xda|xiino/i.test(navigator.userAgent)||/1207|6310|6590|3gso|4thp|50[1-6]i|770s|802s|a wa|abac|ac(er|oo|s\-)|ai(ko|rn)|al(av|ca|co)|amoi|an(ex|ny|yw)|aptu|ar(ch|go)|as(te|us)|attw|au(di|\-m|r |s )|avan|be(ck|ll|nq)|bi(lb|rd)|bl(ac|az)|br(e|v)w|bumb|bw\-(n|u)|c55\/|capi|ccwa|cdm\-|cell|chtm|cldc|cmd\-|co(mp|nd)|craw|da(it|ll|ng)|dbte|dc\-s|devi|dica|dmob|do(c|p)o|ds(12|\-d)|el(49|ai)|em(l2|ul)|er(ic|k0)|esl8|ez([4-7]0|os|wa|ze)|fetc|fly(\-|_)|g1 u|g560|gene|gf\-5|g\-mo|go(\.w|od)|gr(ad|un)|haie|hcit|hd\-(m|p|t)|hei\-|hi(pt|ta)|hp( i|ip)|hs\-c|ht(c(\-| |_|a|g|p|s|t)|tp)|hu(aw|tc)|i\-(20|go|ma)|i230|iac( |\-|\/)|ibro|idea|ig01|ikom|im1k|inno|ipaq|iris|ja(t|v)a|jbro|jemu|jigs|kddi|keji|kgt( |\/)|klon|kpt |kwc\-|kyo(c|k)|le(no|xi)|lg( g|\/(k|l|u)|50|54|\-[a-w])|libw|lynx|m1\-w|m3ga|m50\/|ma(te|ui|xo)|mc(01|21|ca)|m\-cr|me(rc|ri)|mi(o8|oa|ts)|mmef|mo(01|02|bi|de|do|t(\-| |o|v)|zz)|mt(50|p1|v )|mwbp|mywa|n10[0-2]|n20[2-3]|n30(0|2)|n50(0|2|5)|n7(0(0|1)|10)|ne((c|m)\-|on|tf|wf|wg|wt)|nok(6|i)|nzph|o2im|op(ti|wv)|oran|owg1|p800|pan(a|d|t)|pdxg|pg(13|\-([1-8]|c))|phil|pire|pl(ay|uc)|pn\-2|po(ck|rt|se)|prox|psio|pt\-g|qa\-a|qc(07|12|21|32|60|\-[2-7]|i\-)|qtek|r380|r600|raks|rim9|ro(ve|zo)|s55\/|sa(ge|ma|mm|ms|ny|va)|sc(01|h\-|oo|p\-)|sdk\/|se(c(\-|0|1)|47|mc|nd|ri)|sgh\-|shar|sie(\-|m)|sk\-0|sl(45|id)|sm(al|ar|b3|it|t5)|so(ft|ny)|sp(01|h\-|v\-|v )|sy(01|mb)|t2(18|50)|t6(00|10|18)|ta(gt|lk)|tcl\-|tdg\-|tel(i|m)|tim\-|t\-mo|to(pl|sh)|ts(70|m\-|m3|m5)|tx\-9|up(\.b|g1|si)|utst|v400|v750|veri|vi(rg|te)|vk(40|5[0-3]|\-v)|vm40|voda|vulc|vx(52|53|60|61|70|80|81|83|85|98)|w3c(\-| )|webc|whit|wi(g |nc|nw)|wmlb|wonu|x700|yas\-|your|zeto|zte\-/i.test(navigator.userAgent.substr(0,4)))&&(e=!0),e)return!1;this.timer&&clearTimeout(this.timer),this.timer=setTimeout(this.toggleSidebar.bind(this),1e3)}},{key:"stopTimer",value:function(){this.timer&&(clearTimeout(this.timer),this.timer=void 0)}},{key:"render",value:function(){var e,t=n(20),a=this.state.menuVisible;return localStorage.getItem("savedConnections")&&(e=Object.keys(JSON.parse(localStorage.getItem("savedConnections"))).map(function(e){return c.a.createElement("option",{key:Math.random(),value:e},e)})),null===f("Host")||null===f("Nick")||null===f("Token")||null===f("tokenName")||null===f("tokenValue")?c.a.createElement("div",{className:"wrapper fadeInDown fill-cont",style:{backgroundImage:"url("+v.a+")"}},c.a.createElement("div",{id:"formContent",className:"fadeIn first"},c.a.createElement("form",null,c.a.createElement("br",null),c.a.createElement("h1",{style:{color:"white"}},"shad0w",c.a.createElement("br",null),c.a.createElement("span",{style:{fontSize:"20px"}},"v",k.version)),c.a.createElement("img",{src:p.a,style:{width:"10%"},alt:"shad0w"}),c.a.createElement("br",null),c.a.createElement("br",null),c.a.createElement("p",{style:{color:"white"}},c.a.createElement("strong",null,"Enter a host, nickname and token to login.")),c.a.createElement("span",{className:"custom-dropdown"},c.a.createElement("select",{defaultValue:"",value:this.state.connOption,id:"connection",onChange:this.change.bind(this)},c.a.createElement("option",{disabled:!0,value:""},"Saved Connections"),e)),c.a.createElement("br",null),c.a.createElement("input",{onChange:function(e){this.handleChangeHost(e)}.bind(this),value:this.state.hostValue,type:"text",id:"Host",className:"fadeIn third",name:"Host",placeholder:"Host"}),c.a.createElement("input",{onChange:function(e){this.handleChangeNick(e)}.bind(this),value:this.state.nickValue,type:"text",id:"Nick",className:"fadeIn second",name:"Nick",placeholder:"Nick"}),c.a.createElement("input",{onChange:function(e){this.handleChangeToken(e)}.bind(this),value:this.state.tokenValue,type:"text",id:"Token",className:"fadeIn third",name:"Token",placeholder:"Token"}),c.a.createElement("br",null),c.a.createElement("br",null),c.a.createElement("button",{type:"button",className:"fadeIn fourth btn btn-light",onClick:function(){this.setAllCookies()}.bind(this)},"ENTER")," ",c.a.createElement("button",{className:"fadeIn fourth btn btn-light",onClick:function(){this.clearAllConnections()}.bind(this)},"CLEAR ALL"),c.a.createElement("br",null),c.a.createElement("br",null),c.a.createElement("button",{className:"fadeIn fourth btn btn-light",onClick:function(){this.clearConnection()}.bind(this)},"DELETE SELECTED CONNECTION"),c.a.createElement("br",null),c.a.createElement("br",null)))):this.state.isLoaded?c.a.createElement("div",{onClick:a?this.showSidebar.bind(this):function(){return!1}},c.a.createElement("div",{id:"wrapper"},c.a.createElement("ul",{id:"menuContainer",onMouseEnter:this.stopTimer.bind(this),onMouseLeave:this.restartTimer.bind(this),className:"scrollable "+(a?"visible":"")},c.a.createElement("li",null,c.a.createElement("img",{src:p.a,style:{width:"30%"},alt:"jian yang"}),c.a.createElement("span",{style:{fontSize:"20px"}},"\xa0",c.a.createElement("strong",null,c.a.createElement("br",null),"shad0w"))),c.a.createElement("li",{onClick:function(){this.eraseAllCookies()}.bind(this)},"Logout"),c.a.createElement("li",{onClick:function(){window.location.reload()}},"Reload")),c.a.createElement("div",{className:"drawerScroll"},c.a.createElement("img",{style:{position:"relative",width:"25px"},src:p.a,alt:"jian yang"}),c.a.createElement("div",{className:"pad"}),c.a.createElement("div",{title:"expand",onClick:this.showSidebar.bind(this),className:"drawerBtn "+(a?"visibleBtn":"")},c.a.createElement("strong",null,"...")),c.a.createElement("div",{className:"pad"}),c.a.createElement("div",{title:"logout",onClick:function(){this.eraseAllCookies()}.bind(this),className:"drawerBtn "+(a?"visibleBtn":"")},c.a.createElement(t,{name:"sign-out-alt"})),c.a.createElement("div",{className:"pad"}),c.a.createElement("div",{title:"refresh",onClick:function(){window.location.reload()},className:"drawerBtn "+(a?"visibleBtn":"")},c.a.createElement(t,{name:"sync"})),c.a.createElement("div",{className:"pad"})),c.a.createElement("div",{id:"layoutContainer"})),c.a.createElement(u.a,{config:{settings:{selectionEnabled:!0},content:[{type:"column",content:[{type:"react-component",title:"Agents",component:"agents-component",isClosable:!1,activeItemIndex:1,props:{}}]}]},registerComponents:function(e){e.registerComponent("agents-component",m.a),e.registerComponent("agents-menu-component",d.a)}})):c.a.createElement("div",null,"Loading...")}}]),t}(c.a.Component)},62:function(e,t,n){},63:function(e,t,n){(function(e){e.myMainLayout=void 0}).call(this,n(14))},64:function(e,t,n){"use strict";n.r(t);var a=n(0),o=n.n(a),i=n(11),s=n.n(i),r=(n(31),n(5));Boolean("localhost"===window.location.hostname||"[::1]"===window.location.hostname||window.location.hostname.match(/^127(?:\.(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)){3}$/));n(63);window.React=o.a,window.ReactDOM=s.a,s.a.render(o.a.createElement(r.a,null),document.getElementById("root")),"serviceWorker"in navigator&&navigator.serviceWorker.ready.then(function(e){e.unregister()})}},[[26,1,2]]]);
//# sourceMappingURL=main.6fa8ab4a.chunk.js.map