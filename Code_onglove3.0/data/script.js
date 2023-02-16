var textonread;
const ws = new WebSocket("ws://192.168.4.1:81"); //"ws://192.168.4.1:81");connect to another espon websocket too. (the one where il be sending he messages)
var t = 0;
//var url = "ws://192.168.4.1:81";
//var url = "ws://192.168.4.1:81";

ws.onopen = function () {
  alert("Connection opened");
  speak("hello!");
  
};

ws.onclose = function () {
  alert("Connection closed");
};

ws.onmessage = async function (event) {
  var text = JSON.parse(event.data)
  var text = text['letter']
  if (text === "NAN") {
    var box = document.getElementById("super");
    await speak(box.innerText).then(box.replaceChildren())
  }
  else{
    textonread == text;
    var box = document.getElementById("super");
    box.append(text);
    console.log("test");
    console.log(text)
  }
};


function settings() {
  if (t == 1) {
    document.getElementById('settings').style.display = 'flex';
    t = 0;
  }
  else if (t == 0) {
    document.getElementById('settings').style.display = 'none';
    t = 1
  }
}

function sleep(milliseconds) {
  const date = Date.now();
  let currentDate = null;
  do {
    currentDate = Date.now();
  } while (currentDate - date < milliseconds);
}

async function speak(text) {
  var msg = new SpeechSynthesisUtterance();
  var voices = speechSynthesis.getVoices();
  msg.voice = voices[1];
  msg.voiceURI = 'native';
  msg.volume = 1;
  msg.rate = 1;
  msg.pitch = 2;
  msg.text = text;
  msg.lang = 'en-US';
  
  await speechSynthesis.speak(msg);
  console.log(text);
}
