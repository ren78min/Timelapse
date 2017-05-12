
// command format {<STARTpushTime,releaseTime,total,counter|STOP|QUERY>}
// {START200,300,4}

const String NO_COMMAND = "";

// time in millisecond
unsigned long trigerTime;
unsigned long releaseTime;
unsigned long total;
unsigned long counter;
unsigned long start;

int state;

const int S_IDLE = 0;
const int S_TRIGER = 1;
const int S_RELEASE = 2;

void setup() {
  // init serial port
  Serial.begin(9600);
  // init shot control pin
  pinMode(2, OUTPUT);
  // reset params
  resetParams();
}

void loop() {
  String command = readSerial();

  if(command != NO_COMMAND){
    execute(command);
  }

  update();

  delay(5);
}

void resetParams(){
  trigerTime = 0;
  releaseTime = 0;
  total = 0;
  counter = 0;  
  start = 0;
  state = S_IDLE;
  digitalWrite(2, LOW);
}

// read a valid command from serial port
String serialData;
String readSerial(){
  while(Serial.available() > 0){
    char c = Serial.read();
    switch(c){
      // begin of a command
    case '{':
      serialData = "";
      break;
      // end of a command
    case '}':
      return serialData;
      break;
      // content of a command
    default:
      serialData += c;
      break;
    }
  }
  return NO_COMMAND; 
}

void execute(String command){
  if(command.startsWith("START")){
    resetParams();
    // trigerTime
    String tmpStr = command.substring(5);
    int index = tmpStr.indexOf(',');
    trigerTime = tmpStr.substring(0, index).toInt();
    // releaseTime
    tmpStr = tmpStr.substring(index + 1);
    index = tmpStr.indexOf(',');
    releaseTime = tmpStr.substring(0, index).toInt();
    // total
    total = tmpStr.substring(index + 1).toInt();
  }
  else if (command.startsWith("STOP")){
    resetParams();
  }
  else if (command.startsWith("QUERY")){
    // do nothing
  }

  String response = "<";
  Serial.print(response + trigerTime + "," + releaseTime + "," + total + "," + counter + ">");
}

unsigned long toRelease;
unsigned long toTriger;
unsigned long toIdle;

void update(){
  switch(state){
  case S_IDLE:
    if(counter < total){
      // change to triger state
      state = S_TRIGER;
      start = millis();
      digitalWrite(2, HIGH);
      counter ++;
      toTriger = start;
      toRelease = counter * trigerTime + (counter - 1) * releaseTime + start;
      toIdle =  total * trigerTime + (total - 1) * releaseTime + start;
    }
    else{
      delay(95);
    }
    break;

  case S_TRIGER:
    if(millis() >= toIdle){
      resetParams();
    }
    else if(millis() >= toRelease){
      state = S_RELEASE;
      toTriger = counter * (trigerTime + releaseTime) + start;
      digitalWrite(2, LOW);
    }
    break;

  case S_RELEASE:
    if(millis() >= toTriger){
      // change to triger state
      state = S_TRIGER;
      digitalWrite(2, HIGH);
      counter ++;
      toRelease = counter * trigerTime + (counter - 1) * releaseTime + start;
    }
    break;

  }

}
































