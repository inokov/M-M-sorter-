/////INITIALIZE///////////////////////////////////////////////////////////////////////////////////////////////////
  int redPin = 9;
  int greenPin = 10;
  int bluePin = 11;
  int whitePin = 13;
  int limitPin = 8;
  int servoPin = 3;

  int Color1[] = {0,0,0,0};
  int Color2[] = {0,0,0,0};
  int Color3[] = {0,0,0,0};
  int Color4[] = {0,0,0,0};
  int Color5[] = {0,0,0,0};
  int Color6[] = {0,0,0,0};
  
  int TestColor[] = {0,0,0,6};
  int VoidTally = 0;
  boolean SetUp = false;

#include <Stepper.h> 
Stepper myStepper = Stepper(2048,2,6,4,7);

#include <Servo.h>
Servo myServo;


/////SETUP////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  // set LEDs to output and limit switch to input w/ pullup resistor
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(whitePin, OUTPUT);
  pinMode(limitPin, INPUT_PULLUP);

  //Stepper motor, Servo motor, and serial port
  myStepper.setSpeed(15);
  myServo.attach(servoPin);
  myServo.write(80);
  Serial.begin(9600);
}

/////MAIN/////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {

 // startup routine for first 6 m&ms
  if(SetUp == false){
    CollectReferences(Color1,Color2,Color3,Color4,Color5,Color6);
    SetUp = true;
  }

  // collect value for color being tested
  CollectColor(5, TestColor);
  
  // math to determine closest one
  int match = FindMatch(TestColor,Color1,Color2,Color3,Color4,Color5,Color6);

  // sorts based on result
  Sort(match);

  // ends sequence and prints stats if there have been 10 empty rounds in a row
  VoidTally = EndScript(match, VoidTally,TestColor,Color1,Color2,Color3,Color4,Color5,Color6);
  
}

/////FUNCTIONS////////////////////////////////////////////////////////////////////////////////////////////////////

void CollectReferences(int Color1[],int Color2[],int Color3[],int Color4[],int Color5[],int Color6[]){

  //stepper move to start position
  MovetoLimit();
  delay(1000);

  //loads and stores data stepping through colors... trust me
  myStepper.step(1400); //LOAD 1
  MovetoLimit();
  CollectColor(10,Color1);
  
  SetupSort(1); //SORT 1, LOAD 2
  CollectColor(10,Color2);
  
  SetupSort(2); //SORT 2, LOAD 3
  CollectColor(10,Color3);
  
  SetupSort(3); //SORT 3, LOAD 4
  CollectColor(10,Color4);
  
  SetupSort(4); //SORT 4, LOAD 5
  CollectColor(10,Color5);
  
  SetupSort(5); //SORT 5, LOAD 6
  CollectColor(10,Color6);
  
  SetupSort(6); //SORT 6
}


void SetupSort(int ID){
  // sorting function for first 6 m&ms
  int m = 80;
  int h = 43;
  switch(ID){
    case 1: myServo.write(m+h); myStepper.step(1400); myServo.write(m); MovetoLimit();
    break;
    case 2: myStepper.step(1400); MovetoLimit();
    break;
    case 3: myServo.write(m-h); myStepper.step(1400); myServo.write(m); MovetoLimit();
    break;
    case 4: myServo.write(m+h); MovetoLimit(); myServo.write(m);
    break;
    case 5: MovetoLimit();
    break;
    case 6: myServo.write(m-h); MovetoLimit(); myServo.write(m);
    break;
  }
}

void Sort(int ID){
  // Sorting function controls dispensing sequence
  int m = 80;
  int h = 43;
  switch(ID){
    case 0: MovetoLimit();
    break;
    case 1: myServo.write(m+h); myStepper.step(1050); myServo.write(m); MovetoLimit();
    break;
    case 2: myStepper.step(1050); MovetoLimit();
    break;
    case 3: myServo.write(m-h); myStepper.step(1050); myServo.write(m); MovetoLimit();
    break;
    case 4: myServo.write(m+h); MovetoLimit(); myServo.write(m);
    break;
    case 5: MovetoLimit();
    break;
    case 6: myServo.write(m-h); MovetoLimit(); myServo.write(m);
    break;
  }
}


void MovetoLimit(){
  // moves clockwise until limit switch is hit
  // i provides failsafe for jamming
  int i = 0; 
  myStepper.step(-200);
  while(digitalRead(limitPin) == HIGH){
    myStepper.step(-10);
    if(i>200){
      myStepper.step(300);
      i = 0;
    }
    i++;
  }
}


void setLED(int redValue, int greenValue, int blueValue){
  // function writes three PWM values to RGB LED
  analogWrite(redPin, redValue);
  analogWrite(greenPin, greenValue);
  analogWrite(bluePin, blueValue);
}


void CollectColor(int samples, int ColorArray[]){

  //if the chamber is empty, return zeros
  digitalWrite(whitePin, HIGH);
  delay(50);
  int check = analogRead(A0);
  digitalWrite(whitePin, LOW);
  
  if(check>15){
    ColorArray[0] = 0;
    ColorArray[1] = 0;
    ColorArray[2] = 0;
    return;
  }
  delay(300);

  //collects photoresistor value from red, green, then blue light and stores average values in array
  setLED(255,0,0);
  int sum = 0;
  delay(200);
  for(int i=0;i<samples;i++){
    int red = analogRead(A0);
    sum = sum + red;
  }
  ColorArray[0] = sum/samples;

  setLED(0,255,0);
  sum = 0;
  delay(200);
  for(int i=0;i<samples;i++){
    int red = analogRead(A0);
    sum = sum + red;
  }
  ColorArray[1] = sum/samples;

  setLED(0,0,255);
  sum = 0;
  delay(200);
  for(int i=0;i<samples;i++){
    int red = analogRead(A0);
    sum = sum + red;
  }
  ColorArray[2] = sum/samples;

  // turn LED off and update overall m&m counter
  setLED(0,0,0);
  ColorArray[3]++; //THIS UPDATES COLOR ARRAY COUNTERS ONLY IN THE SETUP FUNCTION
                   //DURING THE MAIN LOOP THIS ONLY UPDATES OVERALL COUNTER
  
//  Serial.print(int(ColorArray[0]));
//  Serial.print("   ");
//  Serial.print(int(ColorArray[1]));
//  Serial.print("   ");
//  Serial.println(int(ColorArray[2]));
}


int FindMatch(int TestColor[],int Color1[],int Color2[],int Color3[],int Color4[],int Color5[],int Color6[]){
  // finds array of values closest to TestColor by difference of squares
  // also calls UpdateReference() to update the reference values, once match has been identified
  
  int match = 0;
  
  // if compartment is empty return 0
  if(TestColor[0] == 0){
    return match;
  }
  
  long sums[] = {0,0,0,0,0,0,0};
  for(int i=0;i<3;i++){
    sums[1] = sums[1] + pow(TestColor[i]-Color1[i],2);
    sums[2] = sums[2] + pow(TestColor[i]-Color2[i],2);
    sums[3] = sums[3] + pow(TestColor[i]-Color3[i],2);
    sums[4] = sums[4] + pow(TestColor[i]-Color4[i],2);
    sums[5] = sums[5] + pow(TestColor[i]-Color5[i],2);
    sums[6] = sums[6] + pow(TestColor[i]-Color6[i],2);
  }
  
  long closest = min(sums[1],min(sums[2],min(sums[3],min(sums[4],min(sums[5],sums[6])))));
  
  if(closest==sums[1]){ match = 1;
  UpdateReference(TestColor, Color1);
  }
  else if(closest==sums[2]){ match = 2;
  UpdateReference(TestColor, Color2);
  }
  else if(closest==sums[3]){ match = 3;
  UpdateReference(TestColor, Color3);
  }
  else if(closest==sums[4]){ match = 4;
  UpdateReference(TestColor, Color4);
  }
  else if(closest==sums[5]){ match = 5;
  UpdateReference(TestColor, Color5);
  }
  else { match = 6;
  UpdateReference(TestColor, Color6);
  }
  return match;
}


void UpdateReference(int TestArray[], int ColorArray[]){
  //updates the color reference array with newly associated values 
  for(int i=0;i<3;i++){
    ColorArray[i] = ColorArray[i] + ((TestArray[i]-ColorArray[i])/(ColorArray[3]+1));
  }
  //incriments the color array counter (how many of this color have been sorted)
  ColorArray[3]++;
}


int EndScript(int match,int VoidTally,int TestColor[],int Color1[],int Color2[],int Color3[],int Color4[],int Color5[],int Color6[]){
  if(match == 0){ VoidTally++;
  }
  else{ VoidTally = 0;
  }

  if(VoidTally >= 10){
    Serial.println("Sorting Complete!");
    Serial.print("Total: ");
    Serial.println(TestColor[3]);
    Serial.print("Red: ");
    Serial.println(Color1[3]);
    Serial.print("Orange: ");
    Serial.println(Color2[3]);
    Serial.print("Yellow: ");
    Serial.println(Color3[3]);
    Serial.print("Green: ");
    Serial.println(Color4[3]);
    Serial.print("Blue: ");
    Serial.println(Color5[3]);
    Serial.print("Brown: ");
    Serial.println(Color6[3]);

    // ends program
    delay(1000);
    exit(0);
  }
  return VoidTally;
}
