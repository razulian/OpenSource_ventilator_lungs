//BUTTONS------------------------------------------------------------------------
//PINS---------------------------------------------------------------------------
/* Array Position  - Function
         0           Mute
         1           Breath Hold
         2           Start/Stop
        7/3          Trig Up/Down
        8/4          BPM Up/Down
        9/5          V Up/Down
       10/6          P Up/Down
         11          Mode
*/
//Define the number of buttons
#define numButtons 12
//Define the pins that correspond to the buttons following the list above
uint8_t buttonPins[numButtons] = {13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2};

//VARIABLES AND STRUCTS---------------------------------------------------------------
//define state variables for read operations
boolean previousState[numButtons];
boolean currentState[numButtons];
long numButtonsReadTime[numButtons];

//Software debounce delay (in milliseconds from 0 to 255)
uint8_t buttonDebounceDelay = 10;

//Struct definition for processing of user input
typedef struct {
  uint8_t mode;
  uint8_t start;
  int8_t pressure;
  int8_t volume;
  int8_t trigger;
  int8_t bpm;
  uint8_t hold;
  uint8_t alarm;
} ButtonState;

ButtonState buttonState; //Current button state
ButtonState lastButtonState; //Last button state -> only used to know if a button is kept pressed or not

//INIT---------------------------------------------------------------------------
//Configure pin modes for each button
void Buttons_Init() {
  for (int i = 0; i < numButtons; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP); //All buttons are connected to ground and a corresponding input pin. No extra components required.
  }
  for (int i = 0; i < numButtons; i++) {
    previousState[i] = digitalRead(buttonPins[i]);
  }
}

//UPDATE-------------------------------------------------------------------------------
//Read the state of all buttons
void Buttons_Update() {
  for (int i = 0; i < numButtons; i++) {

    //Only read after the debounce timer has passed
    if (millis() - numButtonsReadTime[i] > buttonDebounceDelay) {
      currentState[i] = !digitalRead(buttonPins[i]); //Invert the states to make it more logical, as pins are pulled up when they are not pressed.

      if (previousState[i] != currentState[i]) {
        previousState[i] = currentState[i];
        numButtonsReadTime[i] = millis();
      }
    }
  }

  //Set button data according to pin states
  for (int i = 0; i < numButtons; i++) {
    switch (i) {
      case 0: //Alarm
        buttonState.alarm = currentState[i];
        break;
      case 1: //Breath Hold
        buttonState.hold = currentState[i];
        break;
      case 2: //Start/Stop
        buttonState.start = currentState[i];
        break;
      case 11: //Mode
        buttonState.mode = currentState[i];
        break;
    }
  }

  //The following simplifies the processing of a user pressing down or up.
  //pin 7/3   Trig Up/Down
  if (currentState[7] == currentState[3]) buttonState.trigger = 0;
  else if (currentState[7]) buttonState.trigger = 1;
  else if (currentState[3]) buttonState.trigger = -1;
  else buttonState.trigger = 0;


  //pin 8/4   BPM Up/Down
  if (currentState[8] == currentState[4]) buttonState.bpm = 0;
  else if (currentState[8]) buttonState.bpm = 1;
  else if (currentState[4]) buttonState.bpm = -1;
  else buttonState.bpm = 0;


  //pin 9/5          V Up/Down
  if (currentState[9] == currentState[5]) buttonState.volume = 0;
  else if (currentState[9]) buttonState.volume = 1;
  else if (currentState[5]) buttonState.volume = -1;
  else buttonState.volume = 0;

  //pin 10/6          P Up/Down
  if (currentState[10] == currentState[6]) buttonState.pressure = 0;
  else if (currentState[10]) buttonState.pressure = 1;
  else if (currentState[6]) buttonState.pressure = -1;
  else buttonState.pressure = 0;

  //Process the states
  Buttons_Process_Data();
}

//PROCESS BUTTON DATA-----------------------------------------------------------------------
/* Additional coding required -> if a button is kept pressed the modified data value should take bigger steps instead of a single step.

    This function takes the state of all buttons into account and modifies the global data accordingly.
*/

void Buttons_Process_Data() {
  //Pressure Data
  if (lastButtonState.pressure != buttonState.pressure) {
    if (buttonState.pressure == 1) setpointData.pressure++;
    else if (buttonState.pressure == -1)setpointData.pressure--;
    lastButtonState.pressure = buttonState.pressure;
  }

  //Trigger Data
  if (lastButtonState.trigger != buttonState.trigger) {
    if (buttonState.trigger == 1) setpointData.trigger++;
    else if (buttonState.trigger == -1)setpointData.trigger--;
    lastButtonState.trigger = buttonState.trigger;
  }

  //Air Volume data
  if (lastButtonState.volume != buttonState.volume) {
    if (buttonState.volume == 1) setpointData.volume++;
    else if (buttonState.volume == -1)setpointData.volume--;
    lastButtonState.volume = buttonState.volume;
  }

  //Breaths Per Minute data
  if (lastButtonState.bpm != buttonState.bpm) {
    if (buttonState.bpm == 1) setpointData.bpm++;
    else if (buttonState.bpm == -1)setpointData.bpm--;
    lastButtonState.bpm = buttonState.bpm;
  }

  //Mode Data
  if (lastButtonState.mode != buttonState.mode) {
    if (buttonState.mode == 1) setpointData.mode++; //Set ventilator mode to 0, 1 or 2 (= Mode A, Mode B, or Mode C)
    if (setpointData.mode > 2) setpointData.mode = 0;
    lastButtonState.mode = buttonState.mode;
  }

  //Breath Hold Data
  if (lastButtonState.hold != buttonState.hold) {
    setpointData.hold = !setpointData.hold; //Toggle hold mode on or off
    lastButtonState.hold = buttonState.hold;
  }

  //Alarm Data
  if (lastButtonState.alarm != buttonState.alarm) {
    setpointData.alarm = !setpointData.alarm; //Toggle the alarm on or off
    lastButtonState.alarm = buttonState.alarm;
  }

  //Start/Stop Data
  if (lastButtonState.start != buttonState.start) {
    setpointData.start = buttonState.start; //Listen to switch
    lastButtonState.start = buttonState.start;
  }
}
