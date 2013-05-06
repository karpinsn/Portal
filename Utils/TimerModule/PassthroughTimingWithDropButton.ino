// Constants won't change. They're used here to set pin numbers:
const int projectorPin = 12;      // the number of the pushbutton pin
const int cameraPin =  7;         // the number of the LED pin
const int dropFrameButtonPin = 2; // The pin for ISR 0 that the button is attached to

int projectorState = 0;
volatile int dropFrames = 0;

void DropFrameISR()
{
    dropFrames++;
}

void setup() {
  // Projector camera timing pins
  pinMode(cameraPin, OUTPUT);   // The camera pin is an output      
  pinMode(projectorPin, INPUT); // We are reading the trigger from the projector

  // Save the current state so we dont have to read it in the main loop
  projectorState = digitalRead(projectorPin);

  // Setup the Drop Frame Interrupt
  attachInterrupt(0, DropFrameISR, FALLING); // Attach the ISR
}

void loop()
{
  if(projectorState != digitalRead(projectorPin))
  {
    // Different state
    projectorState = digitalRead(projectorPin);
    
    if(projectorState == LOW || 0 == dropFrames)
    {
        // Dont need to drop any frames so output the signal
        digitalWrite(cameraPin, projectorState);
    }
    else if(projectorState == HIGH)
    {
        // Rising edge, reduce the drop frame count
        dropFrames--;
    }
  }
}