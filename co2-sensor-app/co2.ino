#define AIQ_SENSOR_ANALOG_PIN 13

unsigned long SLEEP_TIME = 5*1000; // Sleep time between reads (in milliseconds)
unsigned long SIGNAL_WAIT = 20*1000000;

void co2_init()
{
  pinMode(AIQ_SENSOR_ANALOG_PIN, INPUT);  
}

void co2_measure()
{
  Serial.print("co2_measure");
  
  noInterrupts();
  //wait time in usec
  unsigned long duration = pulseIn(AIQ_SENSOR_ANALOG_PIN, HIGH, SIGNAL_WAIT);
  //unsigned long duration = Mypulsein(AIQ_SENSOR_ANALOG_PIN, HIGH);
  
  interrupts();
  
  Serial.print(duration); Serial.println(" us ");
  //from datasheet
  //CO2 ppm = 5000 * (Th - 2ms) / (Th + Tl - 4ms)
  //  given Tl + Th = 1004
  //        Tl = 1004 - Th
  //        = 5000 * (Th - 2ms) / (Th + 1004 - Th -4ms)
  //        = 5000 * (Th - 2ms) / 1000 = 5 * (Th - 2ms)
  long co2ppm = 5 * ((duration/1000) - 2);
  Serial.print(co2ppm);Serial.print("ppm");
  if ((co2ppm != lastAIQ)&&(abs(co2ppm-lastAIQ)>=10)) {    
      lastAIQ = ceil(co2ppm);
  }
  
  Serial.println();
}


unsigned long Mypulsein(int pin, int level)
{
  int i = 0;
  unsigned long start, startImp, finishImp;
  start =  millis();
  startImp =  micros();
  finishImp =  micros();
  do {
    if (digitalRead(pin)==level){
      i = 1;
      startImp =  micros();
    }
  } while((i==0)&&((millis()-start)<500));
  i = 0;
  do {
    if (digitalRead(pin)!=level){
      i = 1;
      finishImp =  micros();
    }
  } while((i==0)&&((millis()-start)<1000));
 
  return finishImp - startImp;
}
