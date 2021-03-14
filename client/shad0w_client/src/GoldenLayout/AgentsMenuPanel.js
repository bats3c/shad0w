import React from "react";
import { getCookie } from "../App";

export class AgentsMenuPanel extends React.Component {
  constructor(props) {
    super(props);
    this.state = {results: '', agentuuid: this.props.agentuuid, value: '', history: [], historyval: 0, historylatest: '', scroll: true, displayDrop: false };
    this.messagesEnd = React.createRef();
    this.messageInput = React.createRef();
    this.bg = React.createRef();
  }

  render() {
    const { agentuuid, results } = this.state;
    const { returnBeaconData } = this.props;
    const beaconData = returnBeaconData();
    let currentBeaconData;
    try {
      currentBeaconData = beaconData[agentuuid];
    }
    catch {
      currentBeaconData = [];
    }

    return (
      <div className="wrap agent-terminal"
          ref={ this.bg }
          onClick={ e=>this.onBgClick(e) }
          onDoubleClick={ e=>this.onBgClickDouble(e)}
          onContextMenu={ e=>this.handleClick(e) }>
        <div className="output">
          <div>[+] Agent {agentuuid} {(currentBeaconData) ? "Connected." : "Disconnected."}</div>
          <div>Welcome to Shad0w C2.</div><br />
          <div>Enter a command.</div>
          <div>Click the window to scroll to the bottom.  To select text, right click to disable this feature and right click again to re-enable.</div><br />
          <div className="pre">{ results }</div>
          <div style={{ display:"flex" }}>
          <div style={{ whiteSpace:"nowrap" }}>{(this.state.scroll) && "*"} Shad0w {"> "} &nbsp;</div>
            <input ref={this.messageInput} maxLength="512" onChange={function(e){this.handleChange(e)}.bind(this)} value={this.state.value} className="input clear" onKeyDown={function(e){this.keyPress(e)}.bind(this)}/>
          </div>
          <div style={{height:"0"}} ref={this.messagesEnd} />
         </div>
      </div>
    );
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
      results+= "Shad0w > " + result + "\n";
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
    const { agentuuid, latest_result } = this.state;

    const bytes = result.length;

    let { results } = this.state;
    if (localStorage.getItem(agentuuid)) {
      results = localStorage.getItem(agentuuid);
    }

    //const results_latest = localStorage.getItem(agentuuid+"_latest");
    //The below continues to print the last thing in the agent queue when the agent exits, only show if not stale.
    //alert(checkLatest);
    if (result === "LOAD") {
      results+= "";
    } else {
      results+= "Results >>\n" + result + "\n[+] Host responded with [" + bytes + "] bytes.\n";
    }
    if (latest_result !== result) {
      const updateState = () => {
        this.setState({
          results,
          latest_result : result,
          dedup : true
        },() => { 
          localStorage.setItem(agentuuid, results);
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

      if(this.state.value === ""){
        this.print_result_newline(' ');
        return false;
      }
      else if(this.state.value === "clear" || this.state.value.split(" ")[0] === "clear") {
        this.setState({ value: '', results: '', latest_result: ''});
        localStorage.removeItem(agentuuid);
        localStorage.removeItem(agentuuid+"_latest");
      }
      else {
        this.print_result_newline(`${this.state.value}\n[+] Tasking agent to run "${this.state.value}"`);
        this.sendAgentData();
      } 
    }
  }

  sendAgentData(inject) {
    let { history, historyval, value, historylatest, agentuuid } = this.state;
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

    const encodedString = Buffer.from(value).toString('base64');
    fetch(getCookie("Host")+"/cmd", {
      method: 'POST',
      headers: {
        'Accept': 'application/json',
        'Content-Type': 'application/json'
      },
      body: JSON.stringify({"beacon_id": agentuuid, "cmd": encodedString}),
      credentials: 'include'
    })
      .then((response) => response.json())
      .then((json) => {
        console.log(document.cookie);
        if (json["success"] === true) {
          console.log("Success");
        }
      }).catch((err) => {
        this.print_result("Invalid Command.\n");
        console.log(err);
    });
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
    const { returnBeaconData } = this.props;

    const beaconData = returnBeaconData();
    
    if (beaconData !== undefined) {
      if (agentuuid in beaconData) {
        this.print_result("LOAD");
      }
    }

    setInterval(x => this.forceUpdate(), 1000);
  }

  componentDidUpdate() {
    const { agentuuid } = this.state;
    const { returnBeaconData } = this.props;

    const beaconData = returnBeaconData();

    if (beaconData[agentuuid].length > 0) {
        const agentResults = beaconData[agentuuid];
        this.print_result(agentResults.join("\n"));
    }
  }
}