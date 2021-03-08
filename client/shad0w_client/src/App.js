import React from "react";
import { GoldenLayoutComponent } from "./GoldenLayout/goldenLayoutComponent";
import { AgentsPanel } from "./GoldenLayout/AgentsPanel";
import { AgentsMenuPanel } from "./GoldenLayout/AgentsMenuPanel";
import 'react-confirm-alert/src/react-confirm-alert.css'
import 'golden-layout/src/css/goldenlayout-base.css';
import 'golden-layout/src/css/goldenlayout-dark-theme.css';
import 'bootstrap-css-only/css/bootstrap.min.css';
import './App.css';
import logo from "./logo.png";
import background from "./background.jpg";
import packageJson from "../package.json";

export function getCookie(name) {
  var cookie = localStorage.getItem(name);
  return cookie;
}

export default class App extends React.Component {
  constructor(props) {
    super(props);
    this.state = {
      isLoaded: false, nickValue: 'admin', hostValue: 'http://127.0.0.1:1337', tokenValue: 'shad0w', commState: {}
    };
  }

  handleChangeNick(e) {
    this.setState({ nickValue: e.target.value });
  }
  handleChangeHost(e) {
    this.setState({ hostValue: e.target.value });
  }
  handleChangeToken(e) {
    this.setState({ tokenValue: e.target.value });
  }

  setCookie(name,value) {
    localStorage.setItem(name, value);
  }

  setAllCookies() {
    this.setCookie("Host", this.state.hostValue);
    this.setCookie("LastHost", this.state.hostValue);
    this.setCookie("Nick", this.state.nickValue);
    this.setCookie("LastNick", this.state.nickValue);
    this.setCookie("Token", this.state.tokenValue);
    this.setCookie("LastToken", this.state.tokenValue);

    let {hostValue, tokenValue, nickValue} = this.state;
    const newLogin = {};
    newLogin[hostValue] = {"nick":nickValue,"token":tokenValue};
    if ( localStorage.getItem("savedConnections") ) {
      const tmp = localStorage.getItem("savedConnections");
      this.setCookie("savedConnections", JSON.stringify(Object.assign({}, JSON.parse(tmp), newLogin)));
    }
    else {
      this.setCookie("savedConnections", JSON.stringify(newLogin));
    }
    fetch(getCookie("Host")+"/login", {
      method: 'POST',
      headers: {
        'Accept': 'application/json',
        'Content-Type': 'application/json'
      },
      body: JSON.stringify({"username": getCookie("Nick"), "password": getCookie("Token")}),
      credentials: 'include'
    })
      .then((response) => response.json())
      .then((json) => {
        console.log(document.cookie);
        if (json["success"] === false) {
          alert("BAD CREDENTIALS.  Try again!")
        } else {
          console.log('OK');
          window.location.reload();
        }
      }).catch((err) => {
        alert("Connection failed.");
        console.log(err);
    });
  }

  eraseCookie(name) {   
    localStorage.removeItem(name);
  }

  eraseAllCookies() {
    this.eraseCookie("Nick");
    this.eraseCookie("Host");
    this.eraseCookie("Token");
    document.cookie = "SDWAuth= ; expires = Thu, 01 Jan 1970 00:00:00 GMT"
    window.location.reload();
  }
  
  //SDWAuth
  UNSAFE_componentWillMount() {
    if (getCookie("LastHost")) {
      this.setState({ hostValue: getCookie("LastHost")});
    }
    if (getCookie("LastNick")) {
      this.setState({ nickValue: getCookie("LastNick")});
    }
    if (getCookie("LastToken")) {
      this.setState({ tokenValue: getCookie("LastToken")});
    }
    if (document.cookie.indexOf('SDWAuth') !== -1) {
      this.setState({isLoaded: true});
    }
  }

  change(event) {
    this.setState({hostValue: event.target.value});
    this.setState({nickValue: JSON.parse(localStorage.getItem("savedConnections"))[event.target.value]["nick"]});
    this.setState({tokenValue: JSON.parse(localStorage.getItem("savedConnections"))[event.target.value]["token"]});
    this.setState({connOption: event.target.value});
  }

  clearAllConnections() {
    this.eraseCookie("savedConnections");
    this.eraseCookie("LastHost");
    this.eraseCookie("LastNick");
    this.eraseCookie("LastTicken");
  }

  clearConnection() {
    const strUser = this.state.connOption;
    const current = JSON.parse(getCookie("savedConnections"));
    delete current[strUser];
    this.setCookie("savedConnections", JSON.stringify(current));
  }

  showSidebar(e) {
    if (e.target.tagName === "LI" || e.target.tagName === "UL" || e.target.tagName === "SPAN") {
      return false;
    }
    
    let deviceIsMobile = false; //At the beginning we set this flag as false. If we can detect the device is a mobile device in the next line, then we set it as true.
    if(/(android|bb\d+|meego).+mobile|avantgo|bada\/|blackberry|blazer|compal|elaine|fennec|hiptop|iemobile|ip(hone|od)|ipad|iris|kindle|Android|Silk|lge |maemo|midp|mmp|netfront|opera m(ob|in)i|palm( os)?|phone|p(ixi|re)\/|plucker|pocket|psp|series(4|6)0|symbian|treo|up\.(browser|link)|vodafone|wap|windows (ce|phone)|xda|xiino/i.test(navigator.userAgent) //eslint-disable-line
        || /1207|6310|6590|3gso|4thp|50[1-6]i|770s|802s|a wa|abac|ac(er|oo|s\-)|ai(ko|rn)|al(av|ca|co)|amoi|an(ex|ny|yw)|aptu|ar(ch|go)|as(te|us)|attw|au(di|\-m|r |s )|avan|be(ck|ll|nq)|bi(lb|rd)|bl(ac|az)|br(e|v)w|bumb|bw\-(n|u)|c55\/|capi|ccwa|cdm\-|cell|chtm|cldc|cmd\-|co(mp|nd)|craw|da(it|ll|ng)|dbte|dc\-s|devi|dica|dmob|do(c|p)o|ds(12|\-d)|el(49|ai)|em(l2|ul)|er(ic|k0)|esl8|ez([4-7]0|os|wa|ze)|fetc|fly(\-|_)|g1 u|g560|gene|gf\-5|g\-mo|go(\.w|od)|gr(ad|un)|haie|hcit|hd\-(m|p|t)|hei\-|hi(pt|ta)|hp( i|ip)|hs\-c|ht(c(\-| |_|a|g|p|s|t)|tp)|hu(aw|tc)|i\-(20|go|ma)|i230|iac( |\-|\/)|ibro|idea|ig01|ikom|im1k|inno|ipaq|iris|ja(t|v)a|jbro|jemu|jigs|kddi|keji|kgt( |\/)|klon|kpt |kwc\-|kyo(c|k)|le(no|xi)|lg( g|\/(k|l|u)|50|54|\-[a-w])|libw|lynx|m1\-w|m3ga|m50\/|ma(te|ui|xo)|mc(01|21|ca)|m\-cr|me(rc|ri)|mi(o8|oa|ts)|mmef|mo(01|02|bi|de|do|t(\-| |o|v)|zz)|mt(50|p1|v )|mwbp|mywa|n10[0-2]|n20[2-3]|n30(0|2)|n50(0|2|5)|n7(0(0|1)|10)|ne((c|m)\-|on|tf|wf|wg|wt)|nok(6|i)|nzph|o2im|op(ti|wv)|oran|owg1|p800|pan(a|d|t)|pdxg|pg(13|\-([1-8]|c))|phil|pire|pl(ay|uc)|pn\-2|po(ck|rt|se)|prox|psio|pt\-g|qa\-a|qc(07|12|21|32|60|\-[2-7]|i\-)|qtek|r380|r600|raks|rim9|ro(ve|zo)|s55\/|sa(ge|ma|mm|ms|ny|va)|sc(01|h\-|oo|p\-)|sdk\/|se(c(\-|0|1)|47|mc|nd|ri)|sgh\-|shar|sie(\-|m)|sk\-0|sl(45|id)|sm(al|ar|b3|it|t5)|so(ft|ny)|sp(01|h\-|v\-|v )|sy(01|mb)|t2(18|50)|t6(00|10|18)|ta(gt|lk)|tcl\-|tdg\-|tel(i|m)|tim\-|t\-mo|to(pl|sh)|ts(70|m\-|m3|m5)|tx\-9|up(\.b|g1|si)|utst|v400|v750|veri|vi(rg|te)|vk(40|5[0-3]|\-v)|vm40|voda|vulc|vx(52|53|60|61|70|80|81|83|85|98)|w3c(\-| )|webc|whit|wi(g |nc|nw)|wmlb|wonu|x700|yas\-|your|zeto|zte\-/i.test(navigator.userAgent.substr(0,4))) { //eslint-disable-line
      deviceIsMobile = true;
    }

    let {menuVisible} = this.state;
    menuVisible = !menuVisible;
    if (menuVisible && !deviceIsMobile) {
      this.restartTimer();
    }
    else if (!deviceIsMobile) {
      if (this.timer) {
        clearTimeout(this.timer);
        this.timer = undefined;
      }
    }
    this.setState({ menuVisible });
    return;
  }

  toggleSidebar() {
    this.timer = undefined;
    this.setState({ menuVisible: !this.state.menuVisible });
    return;
  }

  timer = undefined;
  restartTimer() {
    let deviceIsMobile = false; //At the beginning we set this flag as false. If we can detect the device is a mobile device in the next line, then we set it as true.
    if(/(android|bb\d+|meego).+mobile|avantgo|bada\/|blackberry|blazer|compal|elaine|fennec|hiptop|iemobile|ip(hone|od)|ipad|iris|kindle|Android|Silk|lge |maemo|midp|mmp|netfront|opera m(ob|in)i|palm( os)?|phone|p(ixi|re)\/|plucker|pocket|psp|series(4|6)0|symbian|treo|up\.(browser|link)|vodafone|wap|windows (ce|phone)|xda|xiino/i.test(navigator.userAgent) //eslint-disable-line
        || /1207|6310|6590|3gso|4thp|50[1-6]i|770s|802s|a wa|abac|ac(er|oo|s\-)|ai(ko|rn)|al(av|ca|co)|amoi|an(ex|ny|yw)|aptu|ar(ch|go)|as(te|us)|attw|au(di|\-m|r |s )|avan|be(ck|ll|nq)|bi(lb|rd)|bl(ac|az)|br(e|v)w|bumb|bw\-(n|u)|c55\/|capi|ccwa|cdm\-|cell|chtm|cldc|cmd\-|co(mp|nd)|craw|da(it|ll|ng)|dbte|dc\-s|devi|dica|dmob|do(c|p)o|ds(12|\-d)|el(49|ai)|em(l2|ul)|er(ic|k0)|esl8|ez([4-7]0|os|wa|ze)|fetc|fly(\-|_)|g1 u|g560|gene|gf\-5|g\-mo|go(\.w|od)|gr(ad|un)|haie|hcit|hd\-(m|p|t)|hei\-|hi(pt|ta)|hp( i|ip)|hs\-c|ht(c(\-| |_|a|g|p|s|t)|tp)|hu(aw|tc)|i\-(20|go|ma)|i230|iac( |\-|\/)|ibro|idea|ig01|ikom|im1k|inno|ipaq|iris|ja(t|v)a|jbro|jemu|jigs|kddi|keji|kgt( |\/)|klon|kpt |kwc\-|kyo(c|k)|le(no|xi)|lg( g|\/(k|l|u)|50|54|\-[a-w])|libw|lynx|m1\-w|m3ga|m50\/|ma(te|ui|xo)|mc(01|21|ca)|m\-cr|me(rc|ri)|mi(o8|oa|ts)|mmef|mo(01|02|bi|de|do|t(\-| |o|v)|zz)|mt(50|p1|v )|mwbp|mywa|n10[0-2]|n20[2-3]|n30(0|2)|n50(0|2|5)|n7(0(0|1)|10)|ne((c|m)\-|on|tf|wf|wg|wt)|nok(6|i)|nzph|o2im|op(ti|wv)|oran|owg1|p800|pan(a|d|t)|pdxg|pg(13|\-([1-8]|c))|phil|pire|pl(ay|uc)|pn\-2|po(ck|rt|se)|prox|psio|pt\-g|qa\-a|qc(07|12|21|32|60|\-[2-7]|i\-)|qtek|r380|r600|raks|rim9|ro(ve|zo)|s55\/|sa(ge|ma|mm|ms|ny|va)|sc(01|h\-|oo|p\-)|sdk\/|se(c(\-|0|1)|47|mc|nd|ri)|sgh\-|shar|sie(\-|m)|sk\-0|sl(45|id)|sm(al|ar|b3|it|t5)|so(ft|ny)|sp(01|h\-|v\-|v )|sy(01|mb)|t2(18|50)|t6(00|10|18)|ta(gt|lk)|tcl\-|tdg\-|tel(i|m)|tim\-|t\-mo|to(pl|sh)|ts(70|m\-|m3|m5)|tx\-9|up(\.b|g1|si)|utst|v400|v750|veri|vi(rg|te)|vk(40|5[0-3]|\-v)|vm40|voda|vulc|vx(52|53|60|61|70|80|81|83|85|98)|w3c(\-| )|webc|whit|wi(g |nc|nw)|wmlb|wonu|x700|yas\-|your|zeto|zte\-/i.test(navigator.userAgent.substr(0,4))) { //eslint-disable-line
      deviceIsMobile = true;
    }
    if (deviceIsMobile) {
      return false;
    }

    if (this.timer) {
      clearTimeout(this.timer);
    }
    this.timer = setTimeout(this.toggleSidebar.bind(this), 1000);
  }

  stopTimer() {
    if (this.timer) {
      clearTimeout(this.timer);
      this.timer = undefined;
    }
  }

  render() {
    const FA = require('react-fontawesome')
    const { menuVisible } = this.state;

    const saved = localStorage.getItem("savedConnections");
    let connections;
    if (saved) {
      connections = Object.keys(JSON.parse(localStorage.getItem("savedConnections"))).map(function(key) {
      return <option key={Math.random()} value={key}>{key}</option>;
      });
    }

    if (getCookie('Host') === null || getCookie('Nick') === null || getCookie('Token') === null || document.cookie.indexOf('SDWAuth') === -1) {
      return ( 
        <div className="wrapper fadeInDown fill-cont" style={{backgroundImage : 'url(' + background + ')'}}>
          <div id="formContent" className="fadeIn first">
            <form>
              <br />
              <h1 style={{color:"white"}}>shad0w<br/><span style={{fontSize:"20px"}}>v{packageJson.version}</span></h1>
              <img src={logo} style={{width:"25%"}} alt="jian yang" />
              <br /><br />
              <p style={{color:"white"}}><strong>Enter a host, nickname and token to login.</strong></p>
              <span className="custom-dropdown">
               <select defaultValue="" value={this.state.connOption} id="connection" onChange={this.change.bind(this)}>
                <option disabled value="">Saved Connections</option>
                  { connections }
                </select>
              </span><br />
              <input onChange={function(e){this.handleChangeHost(e)}.bind(this)} value={this.state.hostValue} type="text" id="Host" className="fadeIn third" name="Host" placeholder="Host"/>
              <input onChange={function(e){this.handleChangeNick(e)}.bind(this)} value={this.state.nickValue} type="text" id="Nick" className="fadeIn second" name="Nick" placeholder="Nick"/>
              <input onChange={function(e){this.handleChangeToken(e)}.bind(this)} value={this.state.tokenValue} type="text" id="Token" className="fadeIn third" name="Token" placeholder="Token"/>
              <br /><br /><button type="button" className="fadeIn fourth btn btn-light" onClick={function(){this.setAllCookies()}.bind(this)}>ENTER</button> <button className="fadeIn fourth btn btn-light" onClick={function(){this.clearAllConnections()}.bind(this)}>CLEAR ALL</button><br /><br />
              <button className="fadeIn fourth btn btn-light" onClick={function(){this.clearConnection()}.bind(this)}>DELETE SELECTED CONNECTION</button><br /><br />
            </form>
          </div>
        </div>
      );
    }
 
    if (!this.state.isLoaded) {
      return <div>Loading...</div>;
    }

    return (
      <div onClick={(menuVisible) ? this.showSidebar.bind(this):() => { return false }}>
        <div id="wrapper">
          <ul id="menuContainer" onMouseEnter={this.stopTimer.bind(this)} onMouseLeave={this.restartTimer.bind(this)} className={"scrollable "+( menuVisible ? 'visible' : '' )}>
            <li><img src={logo} style={{width:"30%"}} alt="jian yang" /><span style={{fontSize:"20px"}}>&nbsp;<strong>shad0w</strong></span></li>
            <li onClick={function(){this.eraseAllCookies()}.bind(this)}>Logout</li>
            <li onClick={function(){window.location.reload()}}>Reload</li>
          </ul>
          <div className="drawerScroll">
            <img style={{ position:"relative", width:"25px"}} src={logo} alt="jian yang" /><div className="pad" />
            <div tlte="expand" onClick={this.showSidebar.bind(this)} className={"drawerBtn "+( menuVisible ? 'visibleBtn' : '' )}><FA name="ellipsis-h" /></div><div className="pad" />
            <div title="logout" onClick={function(){this.eraseAllCookies()}.bind(this)} className={"drawerBtn "+( menuVisible ? 'visibleBtn' : '' )}><FA name="sign-out-alt" /></div><div className="pad" />
            <div title="refresh" onClick={function(){window.location.reload()}} className={"drawerBtn "+( menuVisible ? 'visibleBtn' : '' )}><FA name="sync" /></div><div className="pad" />
          </div>
          <div id="layoutContainer"></div>
        </div>
        <GoldenLayoutComponent //config from simple react example: https://golden-layout.com/examples/#qZXEyv
            config={{
              settings: {
                selectionEnabled: true
              },
              content: [{
                type: "column",
                content:[{
                  type:"react-component",
                  title: "Agents",
                  component: "agents-component",
                  isClosable: false,
                  activeItemIndex: 1,
                  props: {  }
                }]
              }]
            }}
            registerComponents={myLayout => {
              myLayout.registerComponent("agents-component", AgentsPanel);
              myLayout.registerComponent("agents-menu-component", AgentsMenuPanel);
            }}
        />
      </div>
    );
  }

}