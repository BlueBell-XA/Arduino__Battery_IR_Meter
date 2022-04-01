//--------------------------------------------------------------------------------------
//                                        DEFINES
//--------------------------------------------------------------------------------------
#define vIn        A0                      // raw battery voltage
#define tacSwitch  3                       // tac switch to activate load
#define relay      5                       // xsistor base pin

//--------------------------------------------------------------------------------------
//                                       VARIABLES
//--------------------------------------------------------------------------------------

float v1 = 0.000;                               // unloaded battery voltage
float v2 = 0.000;                               // loaded battery voltage
float i1 = 0.000;                               // loaded current
float i2 = 0.000;
long val;
float intRes = 0.000;                           // battery internal resistance in Ohms               
float vCal = 1.093;                             // calculated vRef (measure between 5V & Gnd)
float vCalLoad = 1.0905;
float load = 10.0;                            
float r1 = 990.9;
float r2 = 320.1;
float vDivider = (r1+r2)/r2;
bool j = 1;                                     // is switch triggered


void setup()   {           
  Serial.begin(9600);
  analogReference(INTERNAL);
  pinMode(vIn, INPUT);                      // set A0 as input
  pinMode(tacSwitch, INPUT_PULLUP);         // set D5 as input with internal pullup
  pinMode(relay, OUTPUT);                   // set D9 as output
}


void loop() {
  j = digitalRead(tacSwitch);               // is switch triggered? 1 = no 0 = yes
  if(j == 0){
    analogRead(vIn);
    ircalc();
//    relayOn();
  }
}

void ircalc(){
  voltRead(1);
  Serial.print("Battery Voltage: ");
  Serial.print(v1, 2);
  Serial.println("v");
  digitalWrite(relay, HIGH);           // switch in the load
  delay(3000);
  voltRead(2);
  Serial.print("Voltage under load: ");
  Serial.print(v2, 2);
  Serial.println("v");
  i1 = v1/(r1+r2);
  i2 = v2/load;                             // calculate loaded current
  digitalWrite(relay, LOW);            // switch out the load
  float vCalc = v1-v2;
  float iCalc = i2-i1;
  intRes = (vCalc/iCalc)*1000.0;
  Serial.print("Battery Internal Resistance: ");
  Serial.print(intRes);
  Serial.println(" milli Ohms");
}

void voltRead(int stage){
  int voltageSampleCount = 0;
  float voltageSum = 0.000;
  while (voltageSampleCount < 10) {
    voltageSampleCount = voltageSampleCount + 1;
    voltageSum += analogRead(vIn);
    delay(20);
  }
  if (stage == 1) {
    v1 = (voltageSum / voltageSampleCount) * (vCal/1023.0) * vDivider;
    val = (long) (v1*1000L);
    v1 = val/1000.0;
  } else {
    v2 = (voltageSum / voltageSampleCount) * (vCalLoad/1023.0) * vDivider;
    val = (long) (v2*1000L);
    v2 = val/1000.0;
  }
  return;
}

void relayOn() {
  digitalWrite(relay, HIGH);
  delay(10000);
  Serial.println(analogRead(vIn));
  digitalWrite(relay, LOW);
  
}
