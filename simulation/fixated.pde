import processing.serial.*; // imports library for serial communication
import java.awt.event.KeyEvent; // imports library for reading the data from the serial port
import java.io.IOException;

Serial myPort; // defines serial port object
final float cm2pix = 37.7952755906/4;
// define variables
String noObject; // object detected
String data=""; // data to be processed

// int indexHash;
int indexR1 = 0;
int indexL1 = 0;
int indexR2 = 0;
int indexL2 = 0;
int indexR3 = 0;
int indexL3 = 0;

String angle_s    = "";
String distanceR1 = "";
String distanceR2 = "";
String distanceR3 = "";
String distanceL1 = "";
String distanceL2 = "";
String distanceL3 = "";

int angle   ; 
int iDistanceR1; 
int iDistanceR2; 
int iDistanceR3; 
int iDistanceL1; 
int iDistanceL2; 
int iDistanceL3; 

float pixsDistance_R1;
float pixsDistance_R2;
float pixsDistance_R3;
float pixsDistance_L1;
float pixsDistance_L2;
float pixsDistance_L3;

int frameWidth = int(50*cm2pix); // in cm
int frameHeight = int(100*cm2pix); // in c.m

float sonicHeight = 4.5*cm2pix; // in c.m
float sonicWidth = 2*cm2pix; // in cm
boolean clockWiseRotateFlag;

float centery;
float centerx;

int beam_threshold;

float beam_center_Rx1;
float beam_center_Ry1;
float beam_center_Rx2;
float beam_center_Ry2;
float beam_center_Rx3;
float beam_center_Ry3;

float beam_center_Lx1;
float beam_center_Ly1;
float beam_center_Lx2;
float beam_center_Ly2;
float beam_center_Lx3;
float beam_center_Ly3;

float sonic_R1_ox;
float sonic_R1_oy;
float sonic_R2_ox;
float sonic_R2_oy;
float sonic_R3_ox;
float sonic_R3_oy;

float sonic_L1_ox;
float sonic_L1_oy;
float sonic_L2_ox;
float sonic_L2_oy;
float sonic_L3_ox;
float sonic_L3_oy;

int start_angle;
int end_angle;

void setup() {  

  // adjust to screen resolution
  print(frameWidth);
  print(frameHeight);

  // size(600, 900);
  size(472, 875);
  // size(500, 875);

  // intialize servo centers also beam centers
  sonic_R1_ox = frameWidth;
  sonic_R1_oy = (frameHeight*3/4)+int(0.5*cm2pix);
  sonic_R2_ox = frameWidth-int(2*cm2pix);
  sonic_R2_oy = (frameHeight/2)-int(3*cm2pix);
  sonic_R3_ox = frameWidth-int(4*cm2pix);
  sonic_R3_oy = (frameHeight/4)+int(1.5*cm2pix);

  sonic_L1_ox = 0;
  sonic_L1_oy = (frameHeight*3/4)+int(6*cm2pix);
  sonic_L2_ox = int(2*cm2pix);
  sonic_L2_oy = (frameHeight/2)+int(1*cm2pix);
  sonic_L3_ox = int(4*cm2pix);
  sonic_L3_oy = (frameHeight/4)+int(1.5*cm2pix);

  fill(0,0,255); // blue color
  rect(sonic_R1_ox-(sonicWidth), sonic_R1_oy-(sonicHeight/2), sonicWidth, sonicHeight, 4, 4, 4, 4); //sonic R1
  fill(0,0,255); // blue color
  rect(sonic_R2_ox-(sonicWidth), sonic_R2_oy-(sonicHeight/2), sonicWidth, sonicHeight, 4, 4, 4, 4);   //sonic R2
  fill(0,0,255); // blue color
  rect(sonic_R3_ox-(sonicWidth), sonic_R3_oy-(sonicHeight/2), sonicWidth, sonicHeight, 4, 4, 4, 4);   // sonic R3
  
  fill(0,0,255); // blue color 
  rect(sonic_L1_ox, sonic_L1_oy-(sonicHeight/2), sonicWidth, sonicHeight, 4, 4, 4, 4);             //sonic L1
  fill(0,0,255); // blue color
  rect(sonic_L2_ox, sonic_L2_oy-(sonicHeight/2), sonicWidth, sonicHeight, 4, 4, 4, 4);   //sonic L2
  fill(0,0,255); // blue color
  rect(sonic_L3_ox, sonic_L3_oy-(sonicHeight/2), sonicWidth, sonicHeight, 4, 4, 4, 4);                //sonic L3
  smooth();

  beam_threshold = int(50*cm2pix);
  // intialize beam ends
  beam_center_Rx1 = sonic_R1_ox-beam_threshold;
  beam_center_Ry1 = sonic_R1_oy;
  beam_center_Rx2 = sonic_R2_ox-beam_threshold;
  beam_center_Ry2 = sonic_R2_oy;
  beam_center_Rx3 = sonic_R3_ox-beam_threshold;
  beam_center_Ry3 = sonic_R3_oy;

  beam_center_Lx1 = sonic_L1_ox+beam_threshold;
  beam_center_Ly1 = sonic_L1_oy; 
  beam_center_Lx2 = sonic_L2_ox+beam_threshold; 
  beam_center_Ly2 = sonic_L2_oy; 
  beam_center_Lx3 = sonic_L3_ox+beam_threshold; 
  beam_center_Ly3 = sonic_L3_oy; 

  myPort = new Serial(this, "COM6", 9600); // starts the serial communication
  myPort.bufferUntil('.'); // reads the data from the serial port up to the character '.'. So actually it reads this: angle,distance.
  clockWiseRotateFlag = false;

  start_angle = 60;
  end_angle = 120;

  angle = 90;

  centery = frameHeight/2;
  centerx = frameWidth/2;

  // String data = "L1=10R1=0L2=15R2=20L3=25R3=30.";
}

void draw() {
  
  // simulating motion blur and slow fade of the moving line
  noStroke();
  fill(0, 2); 
  rect(0, 0, width, height); 
  
  fill(100, 0, 30); // green color
  // calls the functions for drawing the radar
  drawLine();
  drawObject();
}

void serialEvent (Serial myPort) { // starts reading data from the Serial Port
  // reads the data from the Serial Port up to the character '.' and puts it into the String variable "data".
  String data = myPort.readStringUntil('.');
  
  // extract the data
  indexL1 = data.indexOf("L1="); // find the character and puts it into the variable "indexd3"
  indexR1 = data.indexOf("R1="); // find the character and puts it into the variable "indexd1"
  indexL2 = data.indexOf("L2="); // find the character and puts it into the variable "indexd3"
  indexR2 = data.indexOf("R2="); // find the character and puts it into the variable "indexd2"
  indexL3 = data.indexOf("L3="); // find the character and puts it into the variable "indexd3"
  indexR3 = data.indexOf("R3="); // find the character and puts it into the variable "indexd3"

  distanceL1 = data.substring(indexL1+3, indexR1);       // read the data from position
  distanceR1 = data.substring(indexR1+3, indexL2);       // read the data from position
  distanceL2 = data.substring(indexL2+3, indexR2);       // read the data from position
  distanceR2 = data.substring(indexR2+3, indexL3);       // read the data from position
  distanceL3 = data.substring(indexL3+3, indexR3);       // read the data from position
  distanceR3 = data.substring(indexR3+3, data.length()); // read the data from position
  
  // converts the String variables into Integer
  iDistanceR1 = int(distanceR1);
  iDistanceR2 = int(distanceR2);
  iDistanceR3 = int(distanceR3);
  iDistanceL1 = int(distanceL1);
  iDistanceL2 = int(distanceL2);
  iDistanceL3 = int(distanceL3);

  println(data);
}

void drawObject() {
  pushMatrix();

  strokeWeight(7);
  stroke(85,170,255); // red color

  float angleR = radians(angle-90);

  // int iDistanceR1 = 10;
  float pixsDistance_R1 = beam_threshold-iDistanceR1*cm2pix; // covers the distance from the sensor from cm to pixels
  float pixsDistance_R1_x = pixsDistance_R1*cos(radians(angleR));
  float pixsDistance_R1_y = pixsDistance_R1*sin(radians(angleR));

  float[]  points_R1_start = rotate(sonic_R1_ox, sonic_R1_oy, pixsDistance_R1_x, pixsDistance_R1_y + beam_center_Ry1, angleR);
  float pixsDistance_R1_x_start = points_R1_start[0];
  float pixsDistance_R1_y_start = points_R1_start[1];

  float[]  points_R1_end = rotate(sonic_R1_ox, sonic_R1_oy, beam_center_Rx1, beam_center_Ry1, angleR);
  float pixsDistance_R1_x_end = points_R1_end[0];
  float pixsDistance_R1_y_end = points_R1_end[1];

  // draws the object according to the angle and the distance
  if(iDistanceR1>0) {
  // limiting the range to 55 cm
    if(iDistanceR1<55*cm2pix) {
      line(pixsDistance_R1_x_start, pixsDistance_R1_y_start, pixsDistance_R1_x_end, pixsDistance_R1_y_end);
    }
  }
  
  strokeWeight(7);
  stroke(0,0,255); // red color

  // int iDistanceR2 = 25;
  float pixsDistance_R2 = beam_threshold-iDistanceR2*cm2pix; // covers the distance from the sensor from cm to pixels
  float pixsDistance_R2_x = pixsDistance_R2*cos(radians(angleR));
  float pixsDistance_R2_y = pixsDistance_R2*sin(radians(angleR));

  float[]  points_R2_start = rotate(sonic_R2_ox, sonic_R2_oy, pixsDistance_R2_x, pixsDistance_R2_y + beam_center_Ry2, angleR);
  float pixsDistance_R2_x_start = points_R2_start[0];
  float pixsDistance_R2_y_start = points_R2_start[1];

  float[]  points_R2_end = rotate(sonic_R2_ox, sonic_R2_oy, beam_center_Rx2, beam_center_Ry2, angleR);
  float pixsDistance_R2_x_end = points_R2_end[0];
  float pixsDistance_R2_y_end = points_R2_end[1];

  // draws the object according to the angle and the distance
  if(iDistanceR2>0) {
  // limiting the range to 55 cm
    if(iDistanceR2<55*cm2pix) {
      line(pixsDistance_R2_x_start, pixsDistance_R2_y_start, pixsDistance_R2_x_end, pixsDistance_R2_y_end);
    }
  }
  
  strokeWeight(7);
  stroke(127,0,255); // red color

  // int iDistanceR3 = 25;
  float pixsDistance_R3 = beam_threshold-iDistanceR3*cm2pix; // covers the distance from the sensor from cm to pixels
  float pixsDistance_R3_x = pixsDistance_R3*cos(radians(angleR));
  float pixsDistance_R3_y = pixsDistance_R3*sin(radians(angleR));

  float[]  points_R3_start = rotate(sonic_R3_ox, sonic_R3_oy, pixsDistance_R3_x, pixsDistance_R3_y + beam_center_Ry3, angleR);
  float pixsDistance_R3_x_start = points_R3_start[0];
  float pixsDistance_R3_y_start = points_R3_start[1];

  float[]  points_R3_end = rotate(sonic_R3_ox, sonic_R3_oy, beam_center_Rx3, beam_center_Ry3, angleR);
  float pixsDistance_R3_x_end = points_R3_end[0];
  float pixsDistance_R3_y_end = points_R3_end[1];

  // draws the object according to the angle and the distance
  if(iDistanceR3>0) {
  // limiting the range to 55 cm
    if(iDistanceR3<55*cm2pix) {
      line(pixsDistance_R3_x_start, pixsDistance_R3_y_start, pixsDistance_R3_x_end, pixsDistance_R3_y_end);
    }
  }
  
  strokeWeight(7);
  stroke(255,141,100); // red color

  float angleL = radians(angle-90);

  // int iDistanceL1= 40;
  float pixsDistance_L1 = iDistanceL1*cm2pix; // covers the distance from the sensor from cm to pixels
  float pixsDistance_L1_x = pixsDistance_L1*cos(radians(angleL));
  float pixsDistance_L1_y = pixsDistance_L1*sin(radians(angleL));

  float[]  points_L1_start = rotate(sonic_L1_ox, sonic_L1_oy, pixsDistance_L1_x, pixsDistance_L1_y + beam_center_Ly1, angleL);
  float pixsDistance_L1_x_start = points_L1_start[0];
  float pixsDistance_L1_y_start = points_L1_start[1];

  float[]  points_L1_end = rotate(sonic_L1_ox, sonic_L1_oy, beam_center_Lx1, beam_center_Ly1, angleL);
  float pixsDistance_L1_x_end = points_L1_end[0];
  float pixsDistance_L1_y_end = points_L1_end[1];

  // draws the object according to the angle and the distance
  if(iDistanceL1>0) {
  // limiting the range to 55 cm
    if(iDistanceL1<55*cm2pix) {
      line(pixsDistance_L1_x_start, pixsDistance_L1_y_start, pixsDistance_L1_x_end, pixsDistance_L1_y_end);
    }
  }
  
  strokeWeight(7);
  stroke(228,114,0); // red color

  // int iDistanceL2 = 20;
  float pixsDistance_L2 = iDistanceL2*cm2pix; // covers the distance from the sensor from cm to pixels
  float pixsDistance_L2_x = pixsDistance_L2*cos(radians(angleL));
  float pixsDistance_L2_y = pixsDistance_L2*sin(radians(angleL));

  float[]  points_L2_start = rotate(sonic_L2_ox, sonic_L2_oy, pixsDistance_L2_x, pixsDistance_L2_y + beam_center_Ly2, angleL);
  float pixsDistance_L2_x_start = points_L2_start[0];
  float pixsDistance_L2_y_start = points_L2_start[1];

  float[]  points_L2_end = rotate(sonic_L2_ox, sonic_L2_oy, beam_center_Lx2, beam_center_Ly2, angleL);
  float pixsDistance_L2_x_end = points_L2_end[0];
  float pixsDistance_L2_y_end = points_L2_end[1];

  // draws the object according to the angle and the distance
  if(iDistanceL2>0) {
  // limiting the range to 55 cm
    if(iDistanceL2<55*cm2pix) {
      line(pixsDistance_L2_x_start, pixsDistance_L2_y_start, pixsDistance_L2_x_end, pixsDistance_L2_y_end);
    }
  }strokeWeight(7);
  stroke(236,231,95); // red color

  // int iDistanceL3 = 20;
  float pixsDistance_L3 = iDistanceL3*cm2pix; // covers the distance from the sensor from cm to pixels
  float pixsDistance_L3_x = pixsDistance_L3*cos(radians(angleL));
  float pixsDistance_L3_y = pixsDistance_L3*sin(radians(angleL));

  float[]  points_L3_start = rotate(sonic_L3_ox, sonic_L3_oy, pixsDistance_L3_x, pixsDistance_L3_y + beam_center_Ly3, angleL);
  float pixsDistance_L3_x_start = points_L3_start[0];
  float pixsDistance_L3_y_start = points_L3_start[1];

  float[]  points_L3_end = rotate(sonic_L3_ox, sonic_L3_oy, beam_center_Lx3, beam_center_Ly3, angleL);
  float pixsDistance_L3_x_end = points_L3_end[0];
  float pixsDistance_L3_y_end = points_L3_end[1];

  // draws the object according to the angle and the distance
  if(iDistanceL3>0) {
  // limiting the range to 55 cm
    if(iDistanceL3<55*cm2pix) {
      line(pixsDistance_L3_x_start, pixsDistance_L3_y_start, pixsDistance_L3_x_end, pixsDistance_L3_y_end);
    }
  }
  
  
  popMatrix();
}

void drawLine() {
  pushMatrix();

  translate(0,0); // moves the starting coordinats to new location

   if (clockWiseRotateFlag) {
     angle -= 1;
   }
   else {
     angle += 1;
   }
  
   if (angle < start_angle) {
     clockWiseRotateFlag = false;
   }
   if (angle > end_angle) {
     clockWiseRotateFlag = true;
   }
   
  //  if ((75 > angle) && (angle > 35)) {
  //   iDistance1 = 10;
  //  }
  //  else{
  //   iDistance1 = 0;
  //  }
  //  if ((80 > angle) && (angle > 45)) {
  //   iDistance2 = 35;
  //  }
  //  else{
  //   iDistance2 = 0;
  //  }
  //  if ((70 > angle) && (angle > 55)) {
  //   iDistance3 = 60;
  //  }
  //  else{
  //   iDistance3 = 0;
  //  }
  //  if ((25 > angle) && (angle > 5)) {
  //   iDistance4 = 50;
  //  }
  //  else{
  //   iDistance4 = 0;
  //  }

  // right side angle
  float angleR = radians(angle-90);

  // coordinates of sonic R1 line
  float[] pointR1 = rotate(sonic_R1_ox, sonic_R1_oy, beam_center_Rx1, beam_center_Ry1, angleR);
  float sonic_R1_px = pointR1[0];
  float sonic_R1_py = pointR1[1];

  strokeWeight(6);
  stroke(100,200,0); // line color
  line(sonic_R1_ox, sonic_R1_oy, sonic_R1_px, sonic_R1_py);           //sonic R1 line
  
  // coordinates of sonic R2 line
  float[] pointR2 = rotate(sonic_R2_ox, sonic_R2_oy, beam_center_Rx2, beam_center_Ry2, angleR);
  float sonic_R2_px = pointR2[0];
  float sonic_R2_py = pointR2[1];

  strokeWeight(6);
  stroke(0,200,0); // line color
  line(sonic_R2_ox, sonic_R2_oy, sonic_R2_px, sonic_R2_py);           //sonic R2 line
  
  // coordinates of sonic R3 line
  float[] pointR3 = rotate(sonic_R3_ox, sonic_R3_oy, beam_center_Rx3, beam_center_Ry3, angleR);
  float sonic_R3_px = pointR3[0];
  float sonic_R3_py = pointR3[1];

  strokeWeight(6);
  stroke(0,200,100); // line color
  line(sonic_R3_ox, sonic_R3_oy, sonic_R3_px, sonic_R3_py);           //sonic R3 line
  
  // left side angle
  float angleL = radians(angle-90);

  // coordinates of sonic L1 line
  float[] pointL1 = rotate(sonic_L1_ox, sonic_L1_oy, beam_center_Lx1, beam_center_Ly1, angleL);
  float sonic_L1_px = pointL1[0];
  float sonic_L1_py = pointL1[1];

  strokeWeight(6);
  stroke(100,250,100); // line color
  line(sonic_L1_ox, sonic_L1_oy, sonic_L1_px, sonic_L1_py);           //sonic L1 line
  
  // coordinates of sonic L2 line
  float[] pointL2 = rotate(sonic_L2_ox, sonic_L2_oy, beam_center_Lx2, beam_center_Ly2, angleL);
  float sonic_L2_px = pointL2[0];
  float sonic_L2_py = pointL2[1];

  strokeWeight(6);
  stroke(0,100,0); // line color
  line(sonic_L2_ox, sonic_L2_oy, sonic_L2_px, sonic_L2_py);           //sonic L2 line
  
  // coordinates of sonic L3 line
  float[] pointL3 = rotate(sonic_L3_ox, sonic_L3_oy, beam_center_Lx3, beam_center_Ly3, angleL);
  float sonic_L3_px = pointL3[0];
  float sonic_L3_py = pointL3[1];

  strokeWeight(6);
  stroke(50,100,50); // line color
  line(sonic_L3_ox, sonic_L3_oy, sonic_L3_px, sonic_L3_py);           //sonic L3 line
  
  popMatrix();
}

// void drawText() { // draws the texts on the screen
//   pushMatrix();
  
//   popMatrix();
// }

// float pyth(float x1, float y1, float x2, float y2) {
//   return sqrt((x2-x1)*(x2-x1) +(y2-y1)*(y2-y1));
// }


/*
 * rotates the point around a origin and returns the new point
 * @param ox x coordinate of the origin
 * @param oy y coordinate of the origin
 * @param angle in degrees (sign determines the direction + is counter-clockwise - is clockwise)
 * @param px x coordinate of point to rotate 
 * @param py y coordinate of point to rotate 
 * 
 */
float[] rotate(float ox, float oy, float px, float py, float theta) {
  float[] points = new float[2];
  points[0] = cos(theta) * (px-ox) - sin(theta) * (py-oy) + ox;
  points[1] = sin(theta) * (px-ox) + cos(theta) * (py-oy) + oy;
  return points; 
}
