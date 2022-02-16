import processing.serial.*; // add the serial library
Serial myPort; // define a serial port object to monitor
 
int time = 115; //x position where graph startts
float yDist1;
float ydist2;
float MeasuredTemp;
float motorspeed;

color c1, c2;

//small piece of gradient code taken from processing linear gradient example: https://processing.org/examples/lineargradient.html
void setGradient(int x, int y, float w, float h, color c1, color c2) {


  for (int i = y; i <= y+h; i++) { //increment from y to y+h
     float inter = map(i, y, y+h, 0, 1); //interpolation value to increment by. we map our integer value i from y->y+h to 0->1 because lerpcolor increments need to be less than one
     color c = lerpColor(c1, c2, inter); //lerpcolor function used to interpolate between 2 colors
    stroke(c);
      line(x, i, x+w, i);
    }
  } 


void setup() {
  size(600, 600); // set the window size
  printArray(Serial.list()); // list all available serial ports
  myPort = new Serial(this, Serial.list()[2], 9600); // define input port
  myPort.clear(); // clear the port of any initial junk

  // Define colors
  c1 = color(0, 0, 255); // fading from solid blue (cold)
  c2 = color(255, 0, 0); //fading to solid red (hot)
}


void draw () {
if (myPort.available () > 0) { // make sure port is open
  String inString = myPort.readStringUntil('\n'); // read input string
if (inString != null) { // ignore null strings
inString = trim(inString); // trim off any whitespace
String[] xyRaw = splitTokens(inString, ","); // extract string read from arduino into an array

// proceed only if correct # of values extracted from the string:

if (xyRaw.length == 2) {
  
int MeasuredTemp = int(xyRaw[0]);
println(MeasuredTemp);
int motorspeed = int(xyRaw[1]);
println(motorspeed);
background(123);


setGradient(49, 19, 22, 162, c1, c2); //using coordinates of rectangle to ensure gradient is placed within the shape

//labeling our progress bar
fill(0,0,0); //black
textAlign(RIGHT);
text("100 F", 38, 25); 
text("0 F", 38, 187);


//create marker for temp rectangle
yDist1 = map(MeasuredTemp, 0, 100, 181, 19); //need to map distance of marker according to measured temperature
stroke(0);
triangle(75, yDist1, 85, yDist1 -5, 85, yDist1 + 5); //left end of triangle marked as ydist, 2 other ends are plus or minus 5 points

//write the temp in F
fill(0,0,0);
text(str(int(MeasuredTemp)) + " F", 115, 37);


if (MeasuredTemp > 80) {
  text("Warning: System In Critical Condition", 450, 100);

  }
if (MeasuredTemp< 65) {
  text("Operable Conditions", 450, 100);
  }

if ((MeasuredTemp>= 65) && (MeasuredTemp <= 80)){
  text("Measures Being Taken To Keep System Stable", 450, 100);

  }


ydist2 = map(motorspeed, 0, 255, 550, 250); //ydist2 is the motor speed at which we need to map the location within the graph we'll be plotting

//draw graph
stroke(0);
fill(255,255,255);
rect(115,250,350,300);
time = time + 20; //increment by which ellipse moves across the x axis
if (time > 415) {
  time = 115; //if x value reaches end of graph, reset its position to the beginning (which is x=115 here instead of x=0)
  fill(255,255,255);
}

fill(0,0,0);
ellipse(time, ydist2, 10, 10); //plotting motor speed as a function of time



//
fill(0,0,0);
text("Duty Cycle: 0", 100, 550);
text("Duty Cycle: 255", 100, 255);
text("50", 113, 495);
text("100", 113, 435);
text("150", 113, 375);
text("200", 113, 315);
textAlign(RIGHT);


line(115,280, 465,280);
line(115,310, 465,310);
line(115,340, 465,340);
line(115,370, 465,370);
line(115,400, 465,400);
line(115,430, 465,430);
line(115,460, 465,460);
line(115,490, 465,490);
line(115,520, 465,520);  


  }
}
  
}
}
