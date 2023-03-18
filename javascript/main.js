"use strict"

var cv = document.getElementById('canvas');
var ctx = cv.getContext('2d');
var cvWidth = cv.width;
var cvHeight = cv.height;

//mdtmp
console.log(cvWidth);
console.log(cvHeight);

// Simple scene description : an array of colored rects
var everyObject = [
  [160.2, 100, 30, 30, 'yellow']
];

// animation : always running loop.

function animate() {
  // call again next time we can draw
  requestAnimationFrame(animate);
  // clear canvas
  ctx.clearRect(0, 0, cvWidth, cvHeight);
  // draw everything
  everyObject.forEach(function(o) {
    ctx.fillStyle = o[4];
    ctx.fillRect(o[0], o[1], o[2], o[3]);
  });
  // 
  ctx.fillStyle = "white";
  ctx.font = "20px Arial";
  ctx.fillText("Hello World", 10, 40);
}

animate();
test();


// click handler to add random rects
window.addEventListener('click', function() {
  addRandRect();
});

function addRandRect() {
  var randColor = Math.random() > 0.5 ? 'blue' : 'red';
  everyObject.push([Math.random() * cvWidth, Math.random() * cvHeight, 10 + Math.random() * 40, 10 + Math.random() * 40, randColor]);
}

function test() {
//"https://query1.finance.yahoo.com/v8/finance/chart/" & stock & "?metrics=high?&interval=" & interval & "&range=" & range

fetch("yahoo/v8/finance/chart/aapl?metrics=high?&interval=1d&range=5d")
  .then((response) => response.json())
  .then((data) => receiveYahooStockData(data));

}

function receiveYahooStockData(data) {

const arrClose = data.chart.result[0].indicators.quote[0].close


const max = (arr) => {
  let curr_max = -1
  for (let i = 0; i < arr.length; i++) {
    if (arr[i] > curr_max) {
      curr_max = arr[i]
    }
  }
}

console.log("arrClose")
console.log(arrClose)
console.log("max")
console.log(max(arrClose))

}
