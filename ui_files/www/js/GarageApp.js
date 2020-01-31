function loadScript(url)
{
    var head = document.getElementsByTagName('head')[0];
    var script = document.createElement('script');
    script.type = 'text/javascript';
    script.src = url;
    head.appendChild(script);
}

var myParticleAccessToken = "98807f8bc207e1dd4e9667ee2521c93a8ea3f73a"
var myDeviceId =            "4c004a000351353530373132"
var topic =                 "GarageThing";
var topic2 =                "autoCloseThing";
var topic3 =                "soToggle";
var state = 9;
var b;
var autoClose;
var lightTimer;
var autoToggle;
var maxLight;
var lightStatus;
var oldlightStatus;
var newJSON;
//var Particle = require('particle-api-js');
var particle = new Particle();

//loadScript('Garage.js');
loadScript('resources/particle/particle.min.js');
console.log("loaded");

function init() {
  document.getElementById("login").hidden = false;
  document.getElementById("basic-controls").hidden = true;
  document.getElementById("advanced-features").hidden = true;
  document.getElementById("create-account").hidden = true;
  document.getElementById("status-bar-open").hidden = true;
  document.getElementById("status-bar-closed").hidden = true;
  document.getElementById("status-bar-loading").hidden = false;
  document.getElementById("status-bar-fault").hidden = true;
  document.getElementById("door-open/close").hidden = true;
  //document.getElementById("door-close").hidden = false;
  document.getElementById("light-bar-on").hidden = true;
  document.getElementById("light-bar-off").hidden = true;
  document.getElementById("light-on").hidden = true;
  document.getElementById("light-off").hidden = true;
  document.getElementById("ACcontrols").hidden = true;
  //closeSliderChange();
  //closeSliderChange(event)

}

function adv() {
  document.getElementById("login").hidden = true;
  document.getElementById("basic-controls").hidden = true;
  document.getElementById("advanced-features").hidden = false;
  document.getElementById("create-account").hidden = true;
}

function signOut() {
  document.getElementById("login").hidden = false;
  document.getElementById("basic-controls").hidden = true;
  document.getElementById("advanced-features").hidden = true;
  document.getElementById("create-account").hidden = true;
}

function home() {
  document.getElementById("login").hidden = true;
  document.getElementById("basic-controls").hidden = false;
  document.getElementById("advanced-features").hidden = true;
  document.getElementById("create-account").hidden = true;
}

function go2create() {
  document.getElementById("login").hidden = true;
  document.getElementById("basic-controls").hidden = true;
  document.getElementById("advanced-features").hidden = true;
  document.getElementById("create-account").hidden = false;
}

function create() {
  document.getElementById("login").hidden = false;
  document.getElementById("basic-controls").hidden = true;
  document.getElementById("advanced-features").hidden = true;
  document.getElementById("create-account").hidden = true;
}

function on() {
  oldlightStatus = true;
  callLightStatus();
  light();
}

function off() {
  oldlightStatus = false;
  callLightStatus();
  light();
}

function light() {
  if (lightStatus==1) {
    document.getElementById("light-bar-on").hidden = false;
    document.getElementById("light-bar-off").hidden = true;
    document.getElementById("light-on").hidden = true;
    document.getElementById("light-off").hidden = false;
  }
  else if (lightStatus==0) {
    document.getElementById("light-bar-on").hidden = true;
    document.getElementById("light-bar-off").hidden = false;
    document.getElementById("light-on").hidden = false;
    document.getElementById("light-off").hidden = true;
  }
  //if you want to light to be in the UI, comment out this bottom and uncomment the part above
  // if (garage.lightStatus==true) {
  //   document.getElementById("light-bar-on").hidden = true;
  //   document.getElementById("light-bar-off").hidden = true;
  //   document.getElementById("light-on").hidden = true;
  //   document.getElementById("light-off").hidden = true;
  // }
  // else if (garage.lightStatus==false) {
  //   document.getElementById("light-bar-on").hidden = true;
  //   document.getElementById("light-bar-off").hidden = true;
  //   document.getElementById("light-on").hidden = true;
  //   document.getElementById("light-off").hidden = true;
  // }
}

function open() {
  callButton();
  //garage.openingDoor();
}

function close() {
  callButton();
  //garage.closingDoor();
}

function door() {
  //hide the light stuff because it is not being used in this assignment
  light();

  if (state=="1") {//(garage.doorStatus=="opening") {
    document.getElementById("status-bar-open").hidden = true;
    document.getElementById("status-bar-closed").hidden = true;
    document.getElementById("status-bar-stopped").hidden = true;
    document.getElementById("status-bar-loading").hidden = true;
    document.getElementById("status-bar-fault").hidden = true;
    document.getElementById("door-open/close").hidden = false;
    document.getElementById("status-bar-opening").hidden = false;
    document.getElementById("status-bar-closing").hidden = true;
    //unhide the advanced features button
    document.getElementById("adv").hidden = false;
  }
  else if (state=="2") {//(garage.doorStatus=="open") {
    document.getElementById("status-bar-open").hidden = false;
    document.getElementById("status-bar-closed").hidden = true;
    document.getElementById("status-bar-stopped").hidden = true;
    document.getElementById("status-bar-loading").hidden = true;
    document.getElementById("status-bar-fault").hidden = true;
    document.getElementById("door-open/close").hidden = false;
    document.getElementById("status-bar-opening").hidden = true;
    document.getElementById("status-bar-closing").hidden = true;
    //unhide the advanced features button
    document.getElementById("adv").hidden = false;
  }
  else if (state=="3") {//(garage.doorStatus=="closing") {
    document.getElementById("status-bar-open").hidden = true;
    document.getElementById("status-bar-closed").hidden = true;
    document.getElementById("status-bar-stopped").hidden = true;
    document.getElementById("status-bar-loading").hidden = true;
    document.getElementById("status-bar-fault").hidden = true;
    document.getElementById("door-open/close").hidden = false;
    document.getElementById("status-bar-opening").hidden = true;
    document.getElementById("status-bar-closing").hidden = false;
    //unhide the advanced features button
    document.getElementById("adv").hidden = false;
  }
  else if (state=="0") {//(garage.doorStatus=="closed") {
    document.getElementById("status-bar-open").hidden = true;
    document.getElementById("status-bar-closed").hidden = false;
    document.getElementById("status-bar-stopped").hidden = true;
    document.getElementById("status-bar-loading").hidden = true;
    document.getElementById("status-bar-fault").hidden = true;
    document.getElementById("door-open/close").hidden = false;
    document.getElementById("status-bar-opening").hidden = true;
    document.getElementById("status-bar-closing").hidden = true;
    //unhide the advanced features button
    document.getElementById("adv").hidden = false;
  }

  else if (state=="4") {//(garage.doorStatus=="stopped") {
    document.getElementById("status-bar-open").hidden = true;
    document.getElementById("status-bar-closed").hidden = true;
    document.getElementById("status-bar-stopped").hidden = false;
    document.getElementById("status-bar-loading").hidden = true;
    document.getElementById("status-bar-fault").hidden = true;
    document.getElementById("door-open/close").hidden = false;
    document.getElementById("status-bar-opening").hidden = true;
    document.getElementById("status-bar-closing").hidden = true;
    //unhide the advanced features button
    document.getElementById("adv").hidden = false;
  }
  else if (state=="9"||state=="6") {//(garage.doorStatus=="loading") {
    document.getElementById("status-bar-open").hidden = true;
    document.getElementById("status-bar-closed").hidden = true;
    document.getElementById("status-bar-stopped").hidden = true;
    document.getElementById("status-bar-loading").hidden = false;
    document.getElementById("status-bar-fault").hidden = true;
    document.getElementById("door-open/close").hidden = true;
    document.getElementById("status-bar-opening").hidden = true;
    document.getElementById("status-bar-closing").hidden = true;

    //advanced features button hidden during 'waiting' period
    document.getElementById("adv").hidden = true;

    //hide the light stuff while waiting
    document.getElementById("light-bar-on").hidden = true;
    document.getElementById("light-bar-off").hidden = true;
    document.getElementById("light-on").hidden = true;
    document.getElementById("light-off").hidden = true;
    document.getElementById("ACcontrols").hidden = true;
  }
  else if (state=="5") { //fault
    document.getElementById("status-bar-open").hidden = true;
    document.getElementById("status-bar-closed").hidden = true;
    document.getElementById("status-bar-stopped").hidden = true;
    document.getElementById("status-bar-loading").hidden = true;
    document.getElementById("status-bar-fault").hidden = false;
    document.getElementById("door-open/close").hidden = false;
    document.getElementById("status-bar-opening").hidden = true;
    document.getElementById("status-bar-closing").hidden = true;
    //unhide the advanced features button
    document.getElementById("adv").hidden = false;
  }
}

function changeElement(id, value) {
  var el = document.getElementById(id);
  var st = "brightness(" + value + "%)";
  el.style.filter = st;
}

function brightSliderChange(event) {
  document.getElementById("brightLabel").innerHTML = brightSlider.value;
  maxLight = brightSlider.value;
  b = 30 + (brightSlider.value)*0.7;
  changeElement("light-bar-on", b);
  console.log("brightness")
  callmaxLight();
}

function lightSliderChange(event) {
  document.getElementById("lightLabel").innerHTML = lightSlider.value;

  lightTimer = (lightSlider.value);
  callLightTime();

}

function closeSliderChange(event) {
  if(autoToggle==1){
    document.getElementById("closeLabel").innerHTML = closeSlider.value;
    autoClose = closeSlider.value;
    callaTime();

    console.log("closeTime")
    console.log(garage.closeTime)
  }
}

function enableAC(event) {
  if (document.getElementById('enableAC').checked) {
    console.log("checked registered");
    autoToggle = 1;
    closeSlider.value = autoClose;
    closeLabel.innerHTML = autoClose;
    callaToggle();
    //callaTime();
    document.getElementById("ACcontrols").hidden = false;


  }
  else {
    document.getElementById("ACcontrols").hidden = true;
    console.log("unchecked registered");
    closeSlider.value = autoClose;
    autoToggle = 0;
    callaToggle();
    //callaTime();
  }
}

//LISTENERS
document.addEventListener("DOMContentLoaded", function(event) {


  brightLabel = document.getElementById("brightLabel")
  brightSlider = document.getElementById("brightSlider")
  lightLabel = document.getElementById("lightLabel")
  lightSlider = document.getElementById("lightSlider")
  closeLabel = document.getElementById("closeLabel")
  closeSlider = document.getElementById("closeSlider")

  // login-show = document.getElementById("login")
  // basic-controls-show = document.getElementById("basic-controls")
  // advanced-features-show = document.getElementById("advanced-features")
  // create-account-show = document.getElementById("create-account")


  document.getElementById('adv').addEventListener("click", adv)
  document.getElementById('sign-out').addEventListener("click", signOut)
  document.getElementById('return-home').addEventListener("click", home)
  document.getElementById('login-button').addEventListener("click", home)
  document.getElementById('login-button').addEventListener("click", initUI)
  document.getElementById('go2create').addEventListener("click", go2create)
  document.getElementById('create').addEventListener("click", create)
  document.getElementById('light-on').addEventListener("click", on)
  document.getElementById('light-off').addEventListener("click", off)
  document.getElementById('door-open/close').addEventListener("click", open)
  //document.getElementById('door-open/close').addEventListener("click", close)
  document.getElementById('brightSlider').addEventListener("change", brightSliderChange)
  document.getElementById('lightSlider').addEventListener("change", lightSliderChange)
  document.getElementById('closeSlider').addEventListener("change", closeSliderChange)
  document.getElementById('enableAC').addEventListener('change', enableAC);

  //startup
  brightSlider.value = 100
  lightSlider.value = 5
  //closeSlider.value;
  init(event)
  brightLabel.innerHTML = brightSlider.value
  lightLabel.innerHTML = lightSlider.value
  closeLabel.innerHTML = closeSlider.value

  light()
  door()
  brightSliderChange()
  lightSliderChange()
  closeSliderChange()
  console.log("started")
  initUI();


});

var particle = new Particle();
particle.getEventStream({ deviceId: myDeviceId, name: topic, auth: myParticleAccessToken }).then(function(stream) {
  stream.on('event', function(data) {
    console.log("Event: ", data);
    newJSON = JSON.parse(data.data);
    state = newJSON.doorState;
    autoToggle = newJSON.autoOn;
    autoClose=newJSON.autoClose/1000;
    lightTimer = newJSON.lightTimer/1000;
    maxLight = newJSON.maxLight;
    lightStatus = newJSON.lightStatus;
    console.log("lightStatus: ",lightStatus);
    if (lightStatus==1) {
      document.getElementById("light-bar-on").hidden = false;
      document.getElementById("light-bar-off").hidden = true;
      document.getElementById("light-on").hidden = true;
      document.getElementById("light-off").hidden = false;
    }
    else if (lightStatus==0) {
      document.getElementById("light-bar-on").hidden = true;
      document.getElementById("light-bar-off").hidden = false;
      document.getElementById("light-on").hidden = false;
      document.getElementById("light-off").hidden = true;
    }

    if (autoToggle==1) {
      document.getElementById('enableAC').checked = true;
      //enableAC();

    }
    else if (autoToggle==0) {
      document.getElementById('enableAC').checked = false;
      //enableAC();
    }

    console.log("State: ", state);
    door();
    light();
  });
});


//functions
  function initUI(){
    var fnPr = particle.callFunction({ deviceId: myDeviceId, name: 'INITUI', argument: 'arg', auth: myParticleAccessToken });

      fnPr.then(
        function(data) {
          console.log('init called succesfully:', data);
        }, function(err) {
          console.log('An error occurred:', err);
        });
  }
  function callButton(){
    var fnPr = particle.callFunction({ deviceId: myDeviceId, name: 'button', argument: 'arg', auth: myParticleAccessToken });

      fnPr.then(
        function(data) {
          console.log('callButton called succesfully:', data);
        }, function(err) {
          console.log('An error occurred:', err);
        });
  }
  function callaTime(){
    var fnPr = particle.callFunction({ deviceId: myDeviceId, name: 'autoTime', argument: autoClose, auth: myParticleAccessToken });

      fnPr.then(
        function(data) {
          console.log('callaTime called succesfully:', data);
        }, function(err) {
          console.log('An error occurred:', err);
        });
  }
  function callLightTime(){
    var fnPr = particle.callFunction({ deviceId: myDeviceId, name: 'lightTimer', argument: lightTimer, auth: myParticleAccessToken });

      fnPr.then(
        function(data) {
          console.log('callLightTime called succesfully:', data);
        }, function(err) {
          console.log('An error occurred:', err);
        });
  }
  function callaToggle(){
    console.log("CALLATOGGLE REACHED: ");
    console.log(autoToggle);
    if(autoToggle==0){
      var fnPr = particle.callFunction({ deviceId: myDeviceId, name: 'autoToggle', argument: "0", auth: myParticleAccessToken });

        fnPr.then(
          function(data) {
            console.log('autoToggle called succesfully:', data);
          }, function(err) {
            console.log('An error occurred:', err);
          });
    }
    else if(autoToggle==1){
      var fnPr = particle.callFunction({ deviceId: myDeviceId, name: 'autoToggle', argument: "1", auth: myParticleAccessToken });

        fnPr.then(
          function(data) {
            console.log('autoToggle called succesfully:', data);
          }, function(err) {
            console.log('An error occurred:', err);
          });

    }

  }

  function callmaxLight(){
    var fnPr = particle.callFunction({ deviceId: myDeviceId, name: 'maxLight', argument: maxLight, auth: myParticleAccessToken });

      fnPr.then(
        function(data) {
          console.log('callmaxLight called succesfully:', data);
        }, function(err) {
          console.log('An error occurred:', err);
        });
  }

  function callLightStatus(){
    console.log("CallLightBr REACHED: ");
    if(oldlightStatus==false){
      var fnPr = particle.callFunction({ deviceId: myDeviceId, name: 'lightStatus', argument: "0", auth: myParticleAccessToken });

        fnPr.then(
          function(data) {
            console.log('lightStatus called succesfully:', data);
          }, function(err) {
            console.log('An error occurred:', err);
          });
    }
    else if(oldlightStatus==true){
      var fnPr = particle.callFunction({ deviceId: myDeviceId, name: 'lightStatus', argument: "1", auth: myParticleAccessToken });

        fnPr.then(
          function(data) {
            console.log('lightStatus called succesfully:', data);
          }, function(err) {
            console.log('An error occurred:', err);
          });

    }

  }
