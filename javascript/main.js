"use strict"

//--------------------------------------------------
// Const Configuration
//--------------------------------------------------



//--------------------------------------------------
// Data Model
//--------------------------------------------------

class CvContext {
 constructor(ctx, width, height) {
  this.ctx = ctx
  this.width = width
  this.height = height
 }
}

//--------------------------------------------------
// Data Memory
//--------------------------------------------------

let context
// Simple scene description : an array of colored rects
let everyObject = [
  [160.2, 100, 30, 30, 'yellow']
]

//--------------------------------------------------
// Functions
//--------------------------------------------------

// Entry point
main()

//--------------------------------------------------
function trace(msg)
{
 console.log(msg)
}
//TODO add width() and height() function to convert to relative coordinate.
//FIXME add aspect ratio
function width(widthPourcentage)
{
  return context.width * widthPourcentage
}

function height(heightPourcentage)
{
  return context.height * heightPourcentage
}

//--------------------------------------------------
function drawCandleNoArguments()
{
 // 3 case => getting higher, getting lower, equal
 // -need minimum candle height so we can see it.
 
 class CandleData
 {
  constructor(low, high, open, close)
  {
   this.low = low
   this.high = high
   this.open = open
   this.close = close
  }
 }
 
 const ECandleType =
 {
   UP : "UP",
   DOWN : "DOWN",
   EQUAL : "EQUAL"
 }
 let testCandleData = new CandleData(20, 30, 21, 29)
 let candleType = ECandleType.EQUAL
 let color = "white"
 let openCloseDifference = testCandleData.open - testCandleData.close

 if(testCandleData.open > testCandleData.close)
 {
   candleType = ECandleType.DOWN
   color = "red"
 }

 if(testCandleData.open < testCandleData.close)
 {
   candleType = ECandleType.UP
   color = "green"
   openCloseDifference = testCandleData.close - testCandleData.open
 }


 const origin = {x:.1,y:.4}
 const candleWidth = 0.02
 const candleMinimumHeight = 0.1
 const candleStickWidth = 0.005 

 const lowHighDifference = testCandleData.high - testCandleData.low
 const stickHeight = lowHighDifference / testCandleData.high

 const candleHeight = openCloseDifference / testCandleData.high

  
// Draw high-low
  context.ctx.fillStyle = "white"
  //context.ctx.fillRect(
  //  width(origin.x), height(origin.y), width(0.1), height(0.1));
  context.ctx.fillRect(
    width(origin.x), height(origin.y), width(candleStickWidth), height(stickHeight));

// Draw up-down
  context.ctx.fillStyle = color
  context.ctx.fillRect(
    width(origin.x), height(origin.y), width(candleWidth), height(candleHeight));

}

//--------------------------------------------------
function main() {
const cv = document.getElementById('canvas')
context = new CvContext(cv.getContext("2d"), cv.width, cv.height)

// main loop
animate()
test()

}

// animation : always running loop.
function animate() {
  // call again next time we can draw
  requestAnimationFrame(animate);
  // clear canvas
  context.ctx.clearRect(0, 0, context.width, context.height);
  // draw everything
  everyObject.forEach(function(o) {
    context.ctx.fillStyle = o[4];
    context.ctx.fillRect(o[0], o[1], o[2], o[3]);
  });
  // 
  context.ctx.fillStyle = "white";
  context.ctx.font = "20px Arial";
  context.ctx.fillText("Hello World", 10, 40);

  //drawCandleNoArguments();

}

//--------------------------------------------------
// click handler to add random rects
window.addEventListener('click', function() {
  addRandRect();
});

function addRandRect() {
  var randColor = Math.random() > 0.5 ? 'blue' : 'red';
  everyObject.push([Math.random() * context.width, Math.random() * context.height, 10 + Math.random() * 40, 10 + Math.random() * 40, randColor]);
}
//--------------------------------------------------

function test() {


//"https://query1.finance.yahoo.com/v8/finance/chart/" & stock & "?metrics=high?&interval=" & interval & "&range=" & range
fetch("yahoo/v8/finance/chart/amzn?metrics=high?&interval=1h&range=5d")
  .then((response) => response.json())
  .then((data) => receiveYahooStockData(data));


}

function receiveYahooStockData(data) {

const max = (arr, begin, end) => {
  let curr_max = -1
  let itr = begin
  for (; itr < end; itr++) {
    if (arr[itr] > curr_max) {
      curr_max = arr[itr]
    }
  }
  return curr_max
}
const min = (arr, begin, end) => {
  let curr_min = 9999999
  let itr = begin
  for (; itr < end; itr++) {
    if (arr[itr] < curr_min) {
      curr_min = arr[itr]
    }
  }
  return curr_min
}
const abs = (i) => {
  return i < 0 ? -i : i;
}

//const open = data.chart.result[0].indicators.quote[0].open
//const close = data.chart.result[0].indicators.quote[0].close
const high = data.chart.result[0].indicators.quote[0].high
const low = data.chart.result[0].indicators.quote[0].low

// Start from end.
// Try 2-3 scenario. (Skip 1, 2 or 3 last data)
// Then count 10 to 50
// Check if last candles are higher or lower.

/*
let high_max_ratio = 0;
for(let i = 1; i < 4; i++)
{
 for(let j = high.length - i - 10; j > 0 && j > (high.length - 50); j--)
 {
  const prev_max = max(high, high.length - j, high.length - i) 
  const next_max = max(high, high.length - i, high.length)
  const diff_prc = (next_max - prev_max) / prev_max

  if(abs(high_max_ratio) < abs(diff_prc)) {
   high_max_ratio = diff_prc
  }
  
 }
}

let high_min_ratio = 0;
for(let i = 1; i < 4; i++)
{
 for(let j = high.length - i - 10; j > 0 && j > (high.length - 50); j--)
 {
  const prev_min = min(high, high.length - j, high.length - i) 
  const next_min = min(high, high.length - i, high.length)
  const diff_prc = (next_min - prev_min) / prev_min

  if(abs(high_min_ratio) < abs(diff_prc)) {
   high_min_ratio = diff_prc
  }
  
 }
}

trace(high_max_ratio)
trace(high_min_ratio)
*/

const open = data.chart.result[0].indicators.quote[0].open
const close = data.chart.result[0].indicators.quote[0].close

let diff_arr = []
for(let i = 1; i < open.length; i++)
{
 diff_arr.push(close[i] - open[i])
}


let diff_max = -10000;
for(let i = 1; i < 4; i++)
{
 for(let j = 9 + i; j <= diff_arr.length; j++)
 {
  const prev_max = max(diff_arr, diff_arr.length - j, diff_arr.length - i) 
  const next_max = max(diff_arr, diff_arr.length - i, diff_arr.length)
  const diff_prc = (next_max - prev_max) / prev_max

  if(diff_max < diff_prc)
  {
   diff_max = diff_prc
  }
/*
trace("length")
trace(diff_arr.length)
trace("i")
trace(i)
trace("j")
trace(j)
trace("first")
trace(diff_arr.length - j)
trace("mid")
trace(diff_arr.length - i)
trace("end")
trace(diff_arr.length)
trace("prev_max")
trace(prev_max)
trace("next_max")
trace(next_max)
trace("diff_prc")
trace(diff_prc)
*/
 }
}

trace(diff_max)






}

