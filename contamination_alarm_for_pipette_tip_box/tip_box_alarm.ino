const int resistorPin = A0;
const int buzzerPin = 6;
int readings[] = {0,0,0,0,0,0,0,0,0,0};
int notes[] = {350,400,450,500,550,600,650,700,750,800};
int index = 0;
const int threshold = 50; 
const unsigned long waitTime = 10000; //adjust this variable to change the time delay before alarm (currently 10 seconds) 
unsigned long currentTime;
unsigned long startTime; 
bool openLid = false;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int totalReading = 0;
  readings[index] = analogRead(resistorPin);
  for(int i=0;i<10;i++){
    totalReading = totalReading + readings[i];
  }
  if(totalReading > threshold){
    if(openLid == false){
    startTime = millis();
    openLid = true;
    } else {
      currentTime = millis();
      if(currentTime - startTime > waitTime){
          tone(buzzerPin,notes[index]);
          delay(15);
      }
    }
  }else{
    noTone(buzzerPin);
    openLid = false;
  }
  Serial.println(totalReading);
  index++;
  if(index >9){
    index = 0;
  }
}
