int sensor = 7;

int val = 0; 

// the setup routine runs once when you press reset:
void setup() {                 
  Serial.begin(9600);      
}

// the loop routine runs over and over again forever:
void loop() {
  val = analogRead(sensor);    // read the input pin
  Serial.println(val); 
  //delay(50);               // wait for a second
}
