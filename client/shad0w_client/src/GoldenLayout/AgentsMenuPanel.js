import React from "react";
import FileDrop from "react-file-drop";
import { getCookie } from "../App";

export class AgentsMenuPanel extends React.Component {
  constructor(props) {
    super(props);
    this.state = {results: '', agentuuid: this.props.agentuuid, value: '', history: [], historyval: 0, historylatest: '', latest_result: '', scroll: true, displayDrop: false };
    this.messagesEnd = React.createRef();
    this.messageInput = React.createRef();
    this.bg = React.createRef();
  }

  render() {
    const { agentuuid, results } = this.state;
    const { getCommState } = this.props;
    const commState = getCommState();
    const activeAgent = commState.active_agents[agentuuid];

    return (
      <div className="wrap agent-terminal"
          ref={ this.bg }
          onClick={ e=>this.onBgClick(e) }
          onDoubleClick={ e=>this.onBgClickDouble(e)}
          onContextMenu={ e=>this.handleClick(e) }>
        <FileDrop onDrop={this.handleDrop} className={(this.state.displayDrop) ? "file-drop":"none"}>
          <div style={{position:"fixed",width:"100%",height:"100%"}} onClick={()=>this.setState({displayDrop: false})} tabIndex="0" id="dropBox"> </div>
          <div>Drop a DLL here or select one below.  Click anywhere to cancel.</div>
          <input style={{zIndex:50000}} type="file" onChange={(e) => this.handleSelect(e)}></input>
        </FileDrop>
        <div className="output">
          <div>[+] Agent {activeAgent && activeAgent["uuid"]} {(activeAgent) ? "Connected." : "Disconnected."}</div>
          <div>I am Bender; Please insert girder.</div><br />
          <div>Type 'help' for a list of commands.  Type help [command] to see more details about a command.</div>
          <div>Click the window to scroll to the bottom.  To select text, right click to disable this feature and right click again to re-enable.</div><br />
          <div className="pre">{ results }</div>
          <div style={{ display:"flex" }}>
          <div style={{ whiteSpace:"nowrap" }}>{(this.state.scroll) && "*"}{this.getStub()}&nbsp;</div>
            <input ref={this.messageInput} maxLength="512" onChange={function(e){this.handleChange(e)}.bind(this)} value={this.state.value} className="input clear" onKeyDown={function(e){this.keyPress(e)}.bind(this)}/>
          </div>
          <div style={{height:"0"}} ref={this.messagesEnd} />
         </div>
      </div>
    );
  }
  
  arrayBufferToBase64( buffer ) {
    var bytes = new Uint8Array( buffer );
    var len = bytes.byteLength;
    var array = new Array(len);
    for (var i = 0; i < len; i++) {
      array[i] = String.fromCharCode( bytes[ i ] );
    }
    return window.btoa( array.join('') );
}

  handleSelect = (e) => {
    const that = this;
    const files = e.target.files;
    const reader = new FileReader();
    let bfile;
    reader.onload= function(e) {
      bfile = e.target.result;
      //let base64String = btoa(String.fromCharCode(...new Uint8Array(bfile)));
      let base64String = new Uint8Array(bfile);
      base64String = Buffer.from(base64String).toString('base64');
      that.setState({ displayDrop: false, value: that.state.value.trim()+" "+base64String });
      const lastIndex = that.state.value.lastIndexOf(" ");
      const str = that.state.value.substring(0, lastIndex);
      that.print_result_newline(`${str}\n[+] Tasking agent to run "${str}"`);
      that.sendAgentData(true);
    };
    reader.readAsArrayBuffer(files[0]);
  }

  handleDrop = (files, event) => {
    const that = this;
    const reader = new FileReader();
    let bfile;
    reader.onload= function(e) {
      bfile = e.target.result;
      let base64String = new Uint8Array(bfile);
      base64String = Buffer.from(base64String).toString('base64');
      that.setState({ displayDrop: false, value: that.state.value.trim()+" "+base64String });
      const lastIndex = that.state.value.lastIndexOf(" ");
      const str = that.state.value.substring(0, lastIndex);
      that.print_result_newline(`${str}\n[+] Tasking agent to run "${str}"`);
      that.sendAgentData(true);
    };
    reader.readAsArrayBuffer(files[0]);
  }

  getStub() {
    const { agentuuid } = this.state;
    const { getCommState } = this.props;
    const commState = getCommState();
    const activeAgent = commState.active_agents[agentuuid];

    let dt;
    if (activeAgent) {
      dt = Math.max(((new Date()).getTime() - activeAgent["lastCheckIn"]) / 1000);
      if (dt >= 1) {
        dt = Math.ceil(dt);
      }
      let stale = false;
      let seconds = dt + 's';
      if (Math.floor(dt) > 6+parseInt(activeAgent["sleep"])+(parseInt(activeAgent["sleep"])*(parseInt(activeAgent["jitter"])/100))+1) {
        stale = true;
      }
      if (dt < 0) {
        dt = 0;
      }
      if (activeAgent["sleep"] === "0") {
        dt = 'interactive';
      }
      else {
        if (dt > 86400) {
          dt = Math.floor(dt/86400);
          dt = seconds + '(' + dt + 'd)';
        }
        else if (dt > 3600) {
          dt = Math.floor(dt/3600);
          dt = seconds + '(' + dt + 'h)';
        }
        else if (dt > 60) {
          dt = Math.floor(dt/60);
          dt = seconds + '(' + dt + 'm)';
        }
        else if (dt >= 1 | dt === 0) {
          dt = seconds;
        }
        else {
          if (Math.round(dt) === 1) {
            dt = Math.round(dt) + 's'
          } else {
            dt = Math.round(dt*1000) + 'ms'
          }
        }
        if (stale === true) {
          dt = "[STALE] " + dt;
        }
      }
    }
    let trueSleep = activeAgent && activeAgent["trueSleep"];
    if (trueSleep === undefined) {
      trueSleep = "∞";
    } 
    return `Agent (${(activeAgent) ? activeAgent["platform"]:"N/A"}) (${dt})[${(activeAgent) ? trueSleep:"N/A"}] >`;
  }

  print_result_newline(result) {
    const { agentuuid } = this.state;
    let { results, latest_result } = this.state;
    if (localStorage.getItem(agentuuid)) {
      results = localStorage.getItem(agentuuid);
    }
    if (localStorage.getItem(agentuuid+"_latest")) {
      latest_result = localStorage.getItem(agentuuid+"_latest");
    }
    if (latest_result !== result) {
      results+= this.getStub() + " " + result + "\n";
      const updateState = () => {
         this.setState({
           results
         },() => { this.bottomScroll(); })
       }
       updateState();
       localStorage.setItem(agentuuid, results);
    }
  }

  time = null;
  print_result(result) {
    const { agentuuid } = this.state;
    const { getCommState } = this.props;
    const commState = getCommState();
    const activeAgent = commState.active_agents[agentuuid];
    let dt;
    let stale = false;
    if (activeAgent) {
      dt = Math.max(((new Date()).getTime() - activeAgent["lastCheckIn"]) / 1000);
      if (dt >= 1) {
        dt = Math.ceil(dt);
      }
      if (Math.floor(dt) > parseInt(activeAgent["sleep"])+(parseInt(activeAgent["sleep"])*(parseInt(activeAgent["jitter"])/100))) {
        stale = true;
      }
    }

    let bytes;
    if (result.includes("[X]")) {
      bytes = 'N/A';
    }
    else {
      bytes = result.length;
    }

    let { results, latest_result } = this.state;
    if (localStorage.getItem(agentuuid)) {
      results = localStorage.getItem(agentuuid);
    }
    let checkLatest = results.split("Results >>")[results.split("Results >>").length-1];
    if (checkLatest === undefined) {
      checkLatest = "";
    }
    const results_latest = localStorage.getItem(agentuuid+"_latest");
    //The below continues to print the last thing in the agent queue when the agent exits, only show if not stale.
    if (this.time === null | (Math.ceil(((new Date()).getTime()-this.time)/1000) > (this.props.getCommState().active_agents[this.props.agentuuid]["trueSleep"]) && stale === false) | result === "LOAD" | 
     (activeAgent["sleep"] === undefined && results_latest !== result && result !== latest_result) | (checkLatest.includes(result) && checkLatest.includes("Tasking agent to") && activeAgent["type"] === "smb")) {
      //alert(checkLatest);
      if (this.props.getCommState().active_agents[this.props.agentuuid]["trueSleep"] < 1 && Math.ceil(((new Date()).getTime()-this.time)/1000) < 2) {
        return false;
      }
      this.time = (new Date()).getTime();
      if (result === "LOAD") {
        results+= "";
      } else {
        results+= "Results >>\n" + result + "\n[+] Host responded with [" + bytes + "] bytes.\n";
      }
      const updateState = () => {
        this.setState({
          results,
          latest_result : result,
          dedup : true
        },() => { 
          localStorage.setItem(agentuuid, results);
          localStorage.setItem(agentuuid+"_latest", result);
          this.bottomScroll(); })
      }
      updateState();
    }
  }

  onBgClick(e) {
    if (!this.state.scroll) {
      return;
    }
    if (this.isInViewport() === false) {
      this.bottomScroll();
    } else {
      this.messageInput.current.focus();
    }
  }

  onBgClickDouble(e) {
    if (!this.state.scroll) {
      return;
    }
    if (this.isInViewport() === false) {
      this.bottomScroll();
    } else {
      this.messageInput.current.focus();
    }
  }

  handleClick(e) {
    e.preventDefault();
    let {scroll} = this.state;
    if (e.button === 2) {
      scroll = !scroll;
      this.setState({ scroll });
    }
  }

  handleChange(e) {
    this.setState({ value: e.target.value });
  }

  keyPress(e) {
    if(e.keyCode === 38) {
      let { value, history, historyval, historylatest } = this.state;

      if(historyval === history.length)
        historylatest = value;

      if(historyval > 0)
        historyval--;

      value = history[historyval];

      e.preventDefault();
      this.HAXLOL = true;
      this.setState({ value, historyval, historylatest });

      return false;
    }
    else if(e.keyCode === 40) {
      let { value, history, historyval, historylatest } = this.state;
      
      if(historyval !== history.length)
        historyval++;
      
      if(historyval === history.length+1)
        value = history[historyval-1];
      else if(historyval === history.length)
        value = historylatest
      else
        value = history[historyval];

      e.preventDefault();
      this.HAXLOL = true;
      this.setState({ value, historyval });

      return false;
    }
    else if(e.keyCode === 13){
      this.bottomScroll();
      const { agentuuid } = this.state;
      const { getCommState } = this.props;
      const commState = getCommState();
      let helpMenu = {};

      if (commState.active_agents[agentuuid] !== undefined) {
        switch (commState.active_agents[agentuuid]["platform"]) {
          case "win32":
            if (commState.active_agents[agentuuid]["type"] === "smb") {
              delete commState.win32commands["sleep"];
            }
            helpMenu = commState.win32commands;
            break;
          case "win64":
            if (commState.active_agents[agentuuid]["type"] === "smb") {
              delete commState.win64commands["sleep"];
            }
            helpMenu = commState.win64commands;
            break;
          case "linux":
            helpMenu = commState.linuxcommands;
            break;
          case "darwin":
            helpMenu = commState.darwincommands;
            break;
          default:
            // do nothing
        }
      }
      else {
        this.print_result_newline("Agent no longer exists - please close window.");
        this.setState({ value: '' });
        return;
      }

      helpMenu["clear"] = ["Clear the current agent panel."];
      helpMenu["help"] = ["Show help menu.", "help [command]", "Display the help menu for a command."];
      if(this.state.value === ""){
        this.print_result_newline(' ');
        return false;
      }
      if(this.state.value === "help" || this.state.value.split(" ")[0] === "help") {
        const helpArr = this.state.value.split(" ");
        if (helpArr[1]) {
          const helpCommand = helpArr[1];
          if (helpCommand in helpMenu) {
            if (helpMenu[helpCommand].length === 2) {
              this.print_result_newline(this.state.value+"\nUsage: "+helpMenu[helpCommand][1]+"\nHelp: "+helpMenu[helpCommand][0]+"\n");
            }
            else if (helpMenu[helpCommand].length === 3) {
              this.print_result_newline(this.state.value+"\nUsage: "+helpMenu[helpCommand][1]+"\nHelp: "+helpMenu[helpCommand][0]+"\n"+helpMenu[helpCommand][2]+"\n");
            }
            else {
              this.print_result_newline(this.state.value+"\nUsage: "+helpCommand+"\nHelp: "+helpMenu[helpCommand][0]+"\n")
            }
          }
          else {
            this.print_result_newline(this.state.value+"\nCommand does not exist!");
          }
          this.setState({ value: '' });
        }
        else {
          let help = '';
          const ordered = {};
          Object.keys(helpMenu).sort().forEach(function(key) {
            ordered[key] = helpMenu[key];
          });
          for (const key in ordered) {
            help += `\n"${key}" : ${ordered[key][0]}`;
          }
          this.print_result_newline(this.state.value+"\n[+] Listing ["+Object.keys(helpMenu).length+"] "+commState.active_agents[agentuuid]["platform"]+" commands."+help);
          this.setState({ value: '' });
        }
      }
      else if(this.state.value === "clear" || this.state.value.split(" ")[0] === "clear") {
        this.setState({ value: '', results: '', latest_result: ''});
        localStorage.removeItem(agentuuid);
        localStorage.removeItem(agentuuid+"_latest");
      }
      else {
        if (this.state.value.split(" ")[0] in helpMenu) {
          if (this.state.value.split(" ")[0] === "inject") {
            if (this.state.value.split(" ").length < 3 || this.state.value.split(" ").length > 5) {
              this.print_result_newline(`${this.state.value}\n[-] Wrong number of arguments.  Use 'help inject' for more info.`);
              this.setState({ value: '' });
            }
            else {
              if (this.state.value.split(" ").length === 3 || this.state.value.split(" ")[3] === "" || this.state.value.split(" ")[3] === " ") {
                this.setState({ value: this.state.value.trim() + " none"});
              }
              const updateState = () => {
                 this.setState({
                  displayDrop: true
                 },() => { document.getElementById("dropBox").focus(); })
               }
              updateState();
            }
          }
          else if (this.state.value.split(" ")[0] === "shInject") {
            if (this.state.value.split(" ").length < 3 || this.state.value.split(" ").length > 4) {
              this.print_result_newline(`${this.state.value}\n[-] Wrong number of arguments.  Use 'help shInject' for more info.`);
              this.setState({ value: '' });
            }
            else {
              const updateState = () => {
                 this.setState({
                  displayDrop: true
                 },() => { document.getElementById("dropBox").focus(); })
               }
              updateState();
            }
          }
          else{
            this.print_result_newline(`${this.state.value}\n[+] Tasking agent to run "${this.state.value}"`);
            this.sendAgentData();
          }
        }
        else {
          this.print_result_newline(`${this.state.value}\n[-] Invalid command.  Type 'help' for a list of commands.`);
          this.setState({ value: '' });
        }
      } 
    }
  }

  sendAgentData(inject) {
    let { history, historyval, value, historylatest } = this.state;
    historylatest = "";
    if(history.length > 1024)
      history = history.slice(history.length-1024)
    if (inject === true) {
      history = history.concat(value);
      const lastIndex = value.lastIndexOf(" ");
      const str = value.substring(0, lastIndex);
      history = history.concat(str);
    }
    else {
      history = history.concat(value);
    }
    historyval = history.length;
    this.setState({ history, historyval, value, historylatest });

    //var data = prepSendData({"route":"agents", "data":{"agentUUID":this.state.agentuuid,"data":value}});
    //this.props.connPipe.send(JSON.stringify(data))
    this.setState({ value: '' });
  }


  bottomScroll() {
    this.scrollToBottom();
    const { current } = this.messageInput;
    if (current && this.HAXLOL) {
      this.HAXLOL = false;
      const { value } = this.state;
      if (value != null) {
        current.setSelectionRange(value.length, value.length);
      }
    }
  }

  scrollToBottom = () => {
    this.messagesEnd.current.scrollIntoView({ behavior: 'smooth' });
  }

  isInViewport(offset = 0) {
    if (!this.messageInput.current) return false;
    const top = this.messageInput.current.getBoundingClientRect().top;
    return (top + offset) >= 0 && (top - offset) <= window.innerHeight;
  }

  componentDidMount() {
    this.scrollToBottom();

    const { agentuuid } = this.state;
    const { getCommState } = this.props;

    const commState = getCommState();
    
    if (agentuuid in commState.active_agents) {
      this.print_result("LOAD");
    }

    setInterval(x => this.forceUpdate(), 1000);
  }

  componentDidUpdate() {
    const { agentuuid } = this.state;
    const { getCommState } = this.props;

    const commState = getCommState();


    if (agentuuid in commState.active_agents) {
      if ('results' in commState.active_agents[agentuuid]) {
          const agentResults = commState.active_agents[agentuuid].results;
          this.print_result(agentResults.join("\n"));
      } 
    }
  }
}