//Data Structure of settings that are used
typedef struct {
  uint8_t mode = 0; //Mode: 0 = A, 1 = B ,2 = C -> currently not used
  boolean start = 0; //Start or stop the ventilator
  float pressure = 20; //cmH2O -> Maximum pressure
  float volume = 600; //mL
  float trigger = 8;//cmH2O
  float bpm = 12; //12
  boolean hold; //Pause flag
  boolean alarm; //Alarm flag
} Settings;

Settings setpointData; //Settings that are configured with the control panel
Settings currentData; //Settings acquired from sensors in the ventilator

long currentTime = 0;

//-------------------------------------------------------------------------------

void setup() {
  Buttons_Init(); //Initialize buttons (pinmodes)
  Display_Init(); //Initialize display (I2C setup, clear display and set up labels)
}

//-------------------------------------------------------------------------------

void loop() {
  currentTime = millis();

  Buttons_Update(); //Read button states
  Display_Update(); //Update display to show changes in data
}
