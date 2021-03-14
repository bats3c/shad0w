import React from "react";
import ReactDOM from "react-dom";
import "./goldenLayout-dependencies";
import GoldenLayout from "golden-layout";
import $ from "jquery";

export class GoldenLayoutComponent extends React.Component {
  state = {};
  containerRef = React.createRef();

  render() {
    let panels = Array.from(this.state.renderPanels || []);
    return (
      <div ref={this.containerRef} {...this.props.htmlAttrs}>
        {panels.map((panel, index) => {
          return ReactDOM.createPortal(
            panel._getReactComponent(),
            panel._container.getElement()[0]
          );
        })}
      </div>
    );
  }

  componentRender(reactComponentHandler) {
    this.setState(state => {
      let newRenderPanels = new Set(state.renderPanels);
      newRenderPanels.add(reactComponentHandler);
      return { renderPanels: newRenderPanels };
    });
  }
  componentDestroy(reactComponentHandler) {
    this.setState(state => {
      let newRenderPanels = new Set(state.renderPanels);
      newRenderPanels.delete(reactComponentHandler);
      return { renderPanels: newRenderPanels };
    });
  }

  goldenLayoutInstance = undefined;

  componentDidMount() {
    this.goldenLayoutInstance = new GoldenLayout(
      this.props.config || {},
      $('#layoutContainer')
    );
    //The below globalizes the layout for additional use.
    global.myMainLayout = this.goldenLayoutInstance;
    if (this.props.registerComponents instanceof Function)
      this.props.registerComponents(this.goldenLayoutInstance);
    this.goldenLayoutInstance.reactContainer = this;
    this.goldenLayoutInstance.init();

    /*var deviceIsMobile = false; //At the beginning we set this flag as false. If we can detect the device is a mobile device in the next line, then we set it as true.
    if(/(android|bb\d+|meego).+mobile|avantgo|bada\/|blackberry|blazer|compal|elaine|fennec|hiptop|iemobile|ip(hone|od)|ipad|iris|kindle|Android|Silk|lge |maemo|midp|mmp|netfront|opera m(ob|in)i|palm( os)?|phone|p(ixi|re)\/|plucker|pocket|psp|series(4|6)0|symbian|treo|up\.(browser|link)|vodafone|wap|windows (ce|phone)|xda|xiino/i.test(navigator.userAgent) //eslint-disable-line
        || /1207|6310|6590|3gso|4thp|50[1-6]i|770s|802s|a wa|abac|ac(er|oo|s\-)|ai(ko|rn)|al(av|ca|co)|amoi|an(ex|ny|yw)|aptu|ar(ch|go)|as(te|us)|attw|au(di|\-m|r |s )|avan|be(ck|ll|nq)|bi(lb|rd)|bl(ac|az)|br(e|v)w|bumb|bw\-(n|u)|c55\/|capi|ccwa|cdm\-|cell|chtm|cldc|cmd\-|co(mp|nd)|craw|da(it|ll|ng)|dbte|dc\-s|devi|dica|dmob|do(c|p)o|ds(12|\-d)|el(49|ai)|em(l2|ul)|er(ic|k0)|esl8|ez([4-7]0|os|wa|ze)|fetc|fly(\-|_)|g1 u|g560|gene|gf\-5|g\-mo|go(\.w|od)|gr(ad|un)|haie|hcit|hd\-(m|p|t)|hei\-|hi(pt|ta)|hp( i|ip)|hs\-c|ht(c(\-| |_|a|g|p|s|t)|tp)|hu(aw|tc)|i\-(20|go|ma)|i230|iac( |\-|\/)|ibro|idea|ig01|ikom|im1k|inno|ipaq|iris|ja(t|v)a|jbro|jemu|jigs|kddi|keji|kgt( |\/)|klon|kpt |kwc\-|kyo(c|k)|le(no|xi)|lg( g|\/(k|l|u)|50|54|\-[a-w])|libw|lynx|m1\-w|m3ga|m50\/|ma(te|ui|xo)|mc(01|21|ca)|m\-cr|me(rc|ri)|mi(o8|oa|ts)|mmef|mo(01|02|bi|de|do|t(\-| |o|v)|zz)|mt(50|p1|v )|mwbp|mywa|n10[0-2]|n20[2-3]|n30(0|2)|n50(0|2|5)|n7(0(0|1)|10)|ne((c|m)\-|on|tf|wf|wg|wt)|nok(6|i)|nzph|o2im|op(ti|wv)|oran|owg1|p800|pan(a|d|t)|pdxg|pg(13|\-([1-8]|c))|phil|pire|pl(ay|uc)|pn\-2|po(ck|rt|se)|prox|psio|pt\-g|qa\-a|qc(07|12|21|32|60|\-[2-7]|i\-)|qtek|r380|r600|raks|rim9|ro(ve|zo)|s55\/|sa(ge|ma|mm|ms|ny|va)|sc(01|h\-|oo|p\-)|sdk\/|se(c(\-|0|1)|47|mc|nd|ri)|sgh\-|shar|sie(\-|m)|sk\-0|sl(45|id)|sm(al|ar|b3|it|t5)|so(ft|ny)|sp(01|h\-|v\-|v )|sy(01|mb)|t2(18|50)|t6(00|10|18)|ta(gt|lk)|tcl\-|tdg\-|tel(i|m)|tim\-|t\-mo|to(pl|sh)|ts(70|m\-|m3|m5)|tx\-9|up(\.b|g1|si)|utst|v400|v750|veri|vi(rg|te)|vk(40|5[0-3]|\-v)|vm40|voda|vulc|vx(52|53|60|61|70|80|81|83|85|98)|w3c(\-| )|webc|whit|wi(g |nc|nw)|wmlb|wonu|x700|yas\-|your|zeto|zte\-/i.test(navigator.userAgent.substr(0,4))) { //eslint-disable-line
      deviceIsMobile = true;
    }*/

    var myLayout = this.goldenLayoutInstance;
    window.addEventListener('resize', function(event){ 
      //myLayout.updateSize($(window).width(), $(window).height());
      myLayout.updateSize('100%', $(window).height());
    });
    /*var addMenuItem = function( title, text, component ) {
      var element = $( '<li>' + text + '</li>' );
      $( '#menuContainer' ).append( element );
  
      var newItemConfig = {
          title: title,
          type: 'react-component',
          component: component,
          props: { },
      };
    
      if(!deviceIsMobile){
        myLayout.createDragSource( element, newItemConfig );
      }

      element.click(function(){
        if( myLayout.selectedItem === null ) {
            //alert( 'No item selected. Please click the black header bar to select which item you want to add new items to or drag to create a new window.' );
            myLayout.root.contentItems[ 0 ].addChild( newItemConfig );
        } else {
            myLayout.selectedItem.addChild( newItemConfig );
        }
      });
    };
    addMenuItem( 'Agents Graph', 'Agents Graph', 'agents-graph-component');
    addMenuItem( 'HTTP Listener', 'HTTP Listener', 'http-listener-component');
    addMenuItem( 'SMB Listener', 'SMB Listener', 'smb-listener-component');
    if(!deviceIsMobile){
      myLayout.createDragSource( $('#helpmenu'), {title:'Agent Help',type:'react-component',component:'help-component'} );
      myLayout.createDragSource( $('#helpmenuside'), {title:'Agent Help',type:'react-component',component:'help-component'} );
      myLayout.createDragSource( $('#graphmenu'), {title:'Agents Graph',type:'react-component',component:'agents-graph-component',props: { connPipe: this.props.connPipe, getCommState: this.props.getCommState }} );
      myLayout.createDragSource( $('#httpmenu'), {title:'HTTP Listener',type:'react-component',component:'http-listener-component',props: { connPipe: this.props.connPipe, getCommState: this.props.getCommState }} );
      myLayout.createDragSource( $('#smbmenu'), {title:'SMB Listener',type:'react-component',component:'smb-listener-component',props: { connPipe: this.props.connPipe, getCommState: this.props.getCommState }} );
    }*/
  }
}


//Patching internal GoldenLayout.__lm.utils.ReactComponentHandler:

const ReactComponentHandler = GoldenLayout["__lm"].utils.ReactComponentHandler;

class ReactComponentHandlerPatched extends ReactComponentHandler {
  _render() {
    var reactContainer = this._container.layoutManager.reactContainer; //Instance of GoldenLayoutComponent class
    if (reactContainer && reactContainer.componentRender)
      reactContainer.componentRender(this);
  }
  _destroy() {
    //ReactDOM.unmountComponentAtNode( this._container.getElement()[ 0 ] );
    this._container.off("open", this._render, this);
    this._container.off("destroy", this._destroy, this);
  }

  _getReactComponent() {
    //the following method is absolute copy of the original, provided to prevent depenency on window.React
    var defaultProps = {
      glEventHub: this._container.layoutManager.eventHub,
      glContainer: this._container
    };
    var props = $.extend(defaultProps, this._container._config.props);
    return React.createElement(this._reactClass, props);
  }
}

GoldenLayout["__lm"].utils.ReactComponentHandler = ReactComponentHandlerPatched;
