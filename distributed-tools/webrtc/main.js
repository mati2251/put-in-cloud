// Host 1
const lc = new RTCPeerConnection();
const dc = lc.createDataChannel("chat");
dc.onmessage = (event) => {
  console.log("Received message: " + event.data);
}

dc.onopen = () => {
  console.log("Data channel is open");
}

lc.createOffer().then((offer) => {
  return lc.setLocalDescription(offer);
}).then(() => {
  console.log("Local description set");
})

console.log(lc.localDescription); // This will be remote description for Host 2. Copy this to Host 2 and paste to the variable offer 

const answer = // Paste the offer from Host 2

lc.setRemoteDescription(answer).then(() => {
  console.log("Remote description set");
})

dc.send("Hello from the data channel!");

// Host 2

const rc = new RTCPeerConnection();

rc.ondatachannel = (event) => {
  rc.dc = event.channel;
  rc.dc.onmessage = (event) => {
    console.log("Received message: " + event.data);
  }
  rc.dc.onopen = () => {
    console.log("Data channel is open");
  }
}

const offer = // Paste the local description from Host 1

rc.setRemoteDescription(offer).then(() => {
  console.log("Remote description set");
})

rc.createAnswer().then((answer) => {
  return rc.setLocalDescription(answer);
}).then(() => {
  console.log("Local description set");
});

console.log(rc.localDescription); // This will be remote description for Host 1. Copy this to Host 1 and paste to the variable answer

rc.dc.send("Hello from the data channel!");
