import React from "react";

export class MyGoldenPanel extends React.Component {
  state = {
    value: this.props.value || "bla"
  };
  
  setValue = e => {
    this.setState({ value: e.target.value });
  };

  setContainerTitle = () => {
    this.props.glContainer.setTitle(this.state.value);
  };

  render() {
    return (
      <div>
        <p>This is my panel</p>
        <input type="text" value={this.state.value} onChange={this.setValue} />
        <button onClick={this.setContainerTitle}>set title</button>
      </div>
    );
  }
}
