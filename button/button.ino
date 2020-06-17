const int buttonPin = 2;     // the number of the pushbutton pin
const int ledPin =  LED_BUILTIN;  // 13;      // the number of the LED pin

// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status

void setup() {
  Serial.begin(9600);
  // Serial.println("Setup starting");
  // initialize the LED pin as an output:
  pinMode(LED_BUILTIN, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
  Serial.println("Setup done");
}

void loop() {
  // read the state of the pushbutton value:
  // Serial.println("about to read");
  buttonState = digitalRead(buttonPin);
  Serial.print("read button: ");
  Serial.println(buttonState);
  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    // turn LED on:
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("button pressed");
  } else {
    // turn LED off:
    Serial.println("button not pressed");
    digitalWrite(LED_BUILTIN, LOW);
  }
  delay(100);
}
