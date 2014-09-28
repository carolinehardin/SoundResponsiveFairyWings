
/*This work is licened under GPL 3

/* Pin Definitions */
//these are the pins for the Lilly Pad Simple. 5 will respond to lowest volume level, 11 to highest
int ledPins[] = {5,9,10,11};
int soundSensorPin = A5; //connect the microphone to pin A5. I used https://www.sparkfun.com/products/9964

/*Global Variables */
double nowLoud = 0; //most recent mic reading
double lowLoud = 0; //current lowest mic reading in sample set
double highLoud = 0; //current highest mic reading in sample set
double diffLoud = 0; //highLoud - lowLoud = actual volume level. 

double lowDiffLoud =0; //lowest diff over sound dynamic sample cycles
double highDiffLoud =0; //highest diff over sound dynamic sample cycles
double diffDiffLoud =0; //highDiffLoud - lowDiffLoud = range of volume. Used later to reset the ranges


// starting audio levels. These initialization numbers are arbitrary, and dynamically adjusted at the end of the K loop
int levelOne = 0;
int levelTwo = 1;
int levelThree = 20;
int levelFour = 30;

int lastLevel = 0; // remember which level you are fading from

void setup()
{
  
 /* When booting, test all lights work
 for (int i=0; i<5; i++)
  {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], HIGH);
    delay(1000);
    digitalWrite(ledPins[i], LOW); 
    delay(250);
  }
 */
//the Serial output is very useful for debugging and seeing what the mic is actually picking up
//add your own serial output throughout the code as needed.
Serial.begin(9600);
Serial.println("Start");
 
}

void loop()  {

  for (int k=0; k<100; k++) { //after 100 sample cycles, re-adjust the levels to more closely reflect the ambient noise

    for (int j=0; j<100; j++){   //sample sound 100 times to get solid reading on 'loudness'
      
      nowLoud = analogRead(soundSensorPin); // this picks up amplitude, not volume
      
      if(j==0){ //the first time through,J will be 0, so initialize these to something useful - nowLoud will do
        lowLoud = nowLoud; 
        highLoud = nowLoud;
      }
      
      if (nowLoud < lowLoud) { //if we've found a new low, keep track
        lowLoud = nowLoud; 
      } 
           
      if (nowLoud > highLoud){ //if we've found a new high, keep track
        highLoud = nowLoud; 
      }
      
    } //END TAKE 100 AUDIO LEVEL SAMPLES to find volume
  
  //find 'volume' by subtracting the lowest sampled amplitude from the highest
  diffLoud = highLoud - lowLoud;
 
 
  /* Testing output 
  Serial.print("nowLoud=");
  Serial.println(nowLoud);
 */
 
   //if it falls below the lowest threshold, turn all lights off. This adds more dynamic range.
  if (diffLoud < levelOne) {
      
      fade(lastLevel,0);
      lastLevel= 0;
              
  }  //else, if the volume is at least levelOne threshold, but not as much as level two threshold, light up the lowest lights only
  else if(diffLoud >= levelOne && diffLoud < levelTwo){
     
      fade(lastLevel,1); 
      lastLevel= 1;
 
  }  
  else if(diffLoud >= levelTwo && diffLoud < levelThree){
      
      fade(lastLevel,2);
      lastLevel= 2;
   
  }
  else if(diffLoud >= levelThree && diffLoud < levelFour){
      
      fade(lastLevel,3);
      lastLevel= 3;
 
  } 
  else if(diffLoud >= levelFour){
      
      fade(lastLevel,4);
      lastLevel= 4;
    
  }
  
   // we need to keep track of the diff range - the lowest volume and the highest volume over the sample cycle time.
  // This lets us re-set the levels so the lights are responding to the range of volume actually experienced recently.
  
  if(k==0){ //the first time through, initialize these to something within the actual range
    lowDiffLoud = diffLoud; //we use diffLoud instead of nowLoud b/c we set the levels based on diff
    highDiffLoud = diffLoud;
  }
  if (diffLoud < lowDiffLoud) { //if we've found a new low, keep track
    lowDiffLoud = diffLoud; 
  } 
  if (diffLoud > highDiffLoud){ //if we've found a new high, keep track
    highDiffLoud = diffLoud; 
  }
  
} // end k loop of 1000000 sample cyles

  //now that 1000000 samples have been processed, re-adjust the levels
  diffDiffLoud = highDiffLoud - lowDiffLoud; //the range  size
 
 /*
 //print out the values  
  Serial.print("lowDiffLoud =");
  Serial.println(lowDiffLoud);
  Serial.print("highDiffLoud =");
  Serial.println(highDiffLoud);
 */
 
  levelOne = lowDiffLoud + (.1*diffDiffLoud); //for the lowest 10% of the sound range, light nothing up
  levelTwo = lowDiffLoud + (.35*diffDiffLoud); //level one is 10% to 35% of volume range
  levelThree = lowDiffLoud + (.6*diffDiffLoud); //level two is 35% to 60% of volume range
  levelFour = lowDiffLoud + (.85*diffDiffLoud);   //level four is 85% of range and higher
  
    
} // end loop. start over with the new ranges!

void fade(int lastLevel,int level){ //this function helps soften the effect of changing levels by adding a bit of fade
   
  if(lastLevel == level){ //if they are the same, return. This function is recursiveso, this will be reached.
    return;
  }
   
   int brightness;             //the amount to set the LED to
   int fadeSpeed = 15;          // the lower the number, the faster the speed
   int bright = 255; // Hightest possible brightness. Useful to set this lower when testing to be easier on the eyes
   int fadeAmount = bright/fadeSpeed; //by using a variable for the brightes, you can test at a less blinding level easily 
  
   int fadePin;                //pin being currently faded
   int fadePinAdjustment;      //positive for going up, negative for going down
   int fadeNum = abs(lastLevel - level); //number of pins to fade
   
  //first determine if we are fading up or down
  if (lastLevel < level){  //we are fading up
    brightness = 0;        //the pins being faded start at 0
    fadePin = lastLevel;   //because it's zero indexed, level 1 is at index 0
    fadePinAdjustment = 1; //next pin to fade will be the one above this one
  }  
  else if (lastLevel > level){ //we are fading down
    brightness = bright;       //the pins being faded start at the highest brightness
    fadeAmount = -fadeAmount;  //fading down, remember
    fadePin = lastLevel-1;     //the pins array is zero indexed, so subtract to be working on the correct index
    fadePinAdjustment = -1;    //next pin to fade will be the one below this one
  }  

   
  for (int f=0; f<fadeSpeed; f++){   
     Serial.println(brightness); 
    brightness = brightness + fadeAmount;
    analogWrite(ledPins[fadePin], brightness);   
                               //to slow the fade even more, add a delay here
  }
  //if the fade speed doesn't divide evenly into the bright, then the LEDs won't ever get to 0 brightness.
  //so, at the end of the loop for fading down, just set them to zero
  if (((bright%fadeSpeed) != 0) && (fadeAmount < 0)) { analogWrite(ledPins[fadePin],0); } 
  
  if(fadeNum > 1){ //if we have more than one pin to fade,
      int adjustedLevel = lastLevel+fadePinAdjustment;  //this is the level we'll be fading to
      fade(adjustedLevel, level);  //fade all pins until you reach the goal level
  }

  
}
