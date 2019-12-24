#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

BLECharacteristic *pCharacteristic;
      char  returnString[9];
      bool  deviceConnected = false;
      bool  onceDone        = false;

const int   Received        =     1;
const int   MotXPin         =     2;
const int   dirXPin         =    12;
const int   MotYPin         =    19;
const int   dirYPin         =    27;
const int   readPin         =    23;

      int   delayT          =  2500;
      int   delayOption[4]  = {250, 100, 50, 25};
      float cycle           =     0;
      float factor          =  1000;
      float X_Req           =     0;
      float Y_Req           =     0;
      float X_Prev          =     0;
      float Y_Prev          =     0;
      float X_Meas          =     0;
      float Y_Meas          =     0;

#define     SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define     CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define     CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

int sign(double x)
{
  return (x>0) - (x<0);
}

class MyServerCallbacks: public BLEServerCallbacks
{
  void onConnect(BLEServer* pServer)
  {
    deviceConnected =  true;
  };
  void onDisconnect(BLEServer* pServer)
  {
    deviceConnected = false;
  }
};

class MyCallbacks: public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    std::string rxValue = pCharacteristic->getValue();

    onceDone = true;
    Serial.println(" :: New Receive :: ");
    if (rxValue.length() <= 0)
    {
      Serial.println(" :: ERROR: Empty Receive :: ");
      return;
    }
    for (int i = 0; i < rxValue.length(); i++)
    {
      Serial.print(" :: Receive: "); Serial.print(rxValue[i]); Serial.println(" :: ");
    }
    if ( (rxValue.find("XC") == -1)||(rxValue.find("XF") == -1)||(rxValue.find("YC") == -1)||(rxValue.find("YF") == -1)||(rxValue.find("FR") == -1) )
    {
      Serial.println(" :: ERROR: Faulty Receive :: ");
      digitalWrite(Received, LOW);
      return;
    }
    Serial.println(" :: Good Receive :: ");
    digitalWrite(Received, HIGH);

    X_Prev = X_Meas;
    Y_Prev = Y_Meas;
    
    /*
    digitalWrite(dirXPin, LOW);
    for(cycle = 0; cycle < X_Prev*factor; cycle++)
    {
      digitalWrite(MotXPin, HIGH);
      delayMicroseconds(delayT);
      X_Meas = X_Meas - 1/factor;
      returnString[0] = (((int)(X_Meas*0.1))%10)+48;
      returnString[1] = (((int)(X_Meas*1))%10)+48;
      returnString[2] = (((int)(X_Meas*10))%10)+48;
      returnString[3] = (((int)(X_Meas*100))%10)+48;
      digitalWrite(MotXPin, LOW);
      delayMicroseconds(delayT);
    }
    pCharacteristic -> setValue(returnString);
    pCharacteristic -> notify();
    Serial.println(returnString);
      
    Serial.println(" :: X Reversal Done :: ");
    digitalWrite(dirYPin, LOW);
    for(cycle = 0; cycle < Y_Prev*factor; cycle++)
    {
      digitalWrite(MotYPin, HIGH);
      delayMicroseconds(delayT);
      Y_Meas = Y_Meas - 1/factor;
      returnString[4] = (((int)(Y_Meas*0.1))%10)+48;
      returnString[5] = (((int)(Y_Meas*1))%10)+48;
      returnString[6] = (((int)(Y_Meas*10))%10)+48;
      returnString[7] = (((int)(Y_Meas*100))%10)+48;
      digitalWrite(MotYPin, LOW);
      delayMicroseconds(delayT);
    }
    pCharacteristic -> setValue(returnString);
    pCharacteristic -> notify();
    Serial.println(returnString);
    
    Serial.println(" :: Y Reversal Done :: ");
    */
    delay(500);
        
    X_Req   = (rxValue[2] -48)*10 + (rxValue[3] -48)*1.0 + (rxValue[6] -48)*0.1 + (rxValue[7] -48)*0.01;
    Y_Req   = (rxValue[10]-48)*10 + (rxValue[11]-48)*1.0 + (rxValue[14]-48)*0.1 + (rxValue[15]-48)*0.01;
    delayT  = delayOption[(int)(rxValue[18]-48-1)];
    Serial.print(" X_Req = "); Serial.print(X_Req); Serial.print("; Y_Req = "); Serial.print(Y_Req); Serial.print("; Delay (microsec) = "); Serial.println(delayT);
    factor = 1000 * 8.125;
    delayT = delayT / 2;

    if(X_Req > X_Prev)
    {
      digitalWrite(dirXPin, HIGH);
    }
    else
    {
      digitalWrite(dirXPin, LOW);
    }
    if(Y_Req > Y_Prev)
    {
      digitalWrite(dirYPin, HIGH);
    }
    else
    {
      digitalWrite(dirYPin, LOW);
    }
    
    
    for(cycle = 0; cycle < abs((X_Req-X_Prev)*factor); cycle++)
    {
      digitalWrite(MotXPin, HIGH);
      delayMicroseconds(delayT);
      X_Meas = X_Meas + (1.0/factor)*sign(X_Req-X_Prev);
      returnString[0] = (((int)(X_Meas*0.1))%10)+48;
      returnString[1] = (((int)(X_Meas*1))%10)+48;
      returnString[2] = (((int)(X_Meas*10))%10)+48;
      returnString[3] = (((int)(X_Meas*100))%10)+48;
      digitalWrite(MotXPin, LOW);
      delayMicroseconds(delayT);
    }
    pCharacteristic -> setValue(returnString);
    pCharacteristic -> notify();
    Serial.println(returnString);
    
    Serial.print(" :: X Movement Done for "); Serial.print((X_Req)*factor); Serial.println(" cycles :: ");

    for(cycle = 0; cycle < abs((Y_Req-Y_Prev)*factor); cycle++)
    {
      digitalWrite(MotYPin, HIGH);
      delayMicroseconds(delayT);
      Y_Meas = Y_Meas + (1.0/factor)*sign(Y_Req-Y_Prev);
      returnString[4] = (((int)(Y_Meas*0.1))%10)+48;
      returnString[5] = (((int)(Y_Meas*1))%10)+48;
      returnString[6] = (((int)(Y_Meas*10))%10)+48;
      returnString[7] = (((int)(Y_Meas*100))%10)+48;
      digitalWrite(MotYPin, LOW);
      delayMicroseconds(delayT);
    }
    pCharacteristic -> setValue(returnString);
    pCharacteristic -> notify();
    Serial.println(returnString);
    
    Serial.print(" :: Y Movement Done for "); Serial.print((Y_Req-Y_Prev)*factor); Serial.println(" cycles :: "); Serial.println(""); Serial.println("");
    digitalWrite(dirXPin,  LOW);
    digitalWrite(dirYPin,  LOW);
    digitalWrite(Received, LOW);
  }
};

void setup()
{
  Serial.begin(115200);

  pinMode(Received,   OUTPUT);
  pinMode(MotXPin,    OUTPUT);
  pinMode(dirXPin,    OUTPUT);
  pinMode(MotYPin,    OUTPUT);
  pinMode(dirYPin,    OUTPUT);
  pinMode(13,         OUTPUT);
  pinMode(14,         OUTPUT);
  
  returnString[0] = '0';
  returnString[1] = '0';
  returnString[2] = '0';
  returnString[3] = '0';
  returnString[4] = '0';
  returnString[5] = '0';
  returnString[6] = '0';
  returnString[7] = '0';
  returnString[8] = '\0';
    
  BLEDevice::init("Main Code");
  BLEServer         *pServer = BLEDevice::createServer();
  pServer         ->  setCallbacks(new MyServerCallbacks());
  BLEService        *pService = pServer -> createService(SERVICE_UUID);
  pCharacteristic  = pService -> createCharacteristic(CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY);
  pCharacteristic -> addDescriptor(new BLE2902());
  BLECharacteristic *pCharacteristic = pService -> createCharacteristic(CHARACTERISTIC_UUID_RX, BLECharacteristic::PROPERTY_WRITE);
  pCharacteristic -> setCallbacks(new MyCallbacks());
  pService        -> start();
  pServer         -> getAdvertising() -> start();
}

void loop()
{
  digitalWrite(Received, LOW);
  digitalWrite(13, LOW);
  digitalWrite(14, LOW);
  
  disableCore0WDT();
  disableCore1WDT();
  if(deviceConnected)
  {
    if(onceDone)
    {
      returnString[0] = (((int)(X_Meas*0.1))%10)+48;
      returnString[1] = (((int)(X_Meas*1))%10)+48;
      returnString[2] = (((int)(X_Meas*10))%10)+48;
      returnString[3] = (((int)(X_Meas*100))%10)+48;
      returnString[4] = (((int)(Y_Meas*0.1))%10)+48;
      returnString[5] = (((int)(Y_Meas*1))%10)+48;
      returnString[6] = (((int)(Y_Meas*10))%10)+48;
      returnString[7] = (((int)(Y_Meas*100))%10)+48;
    }
    Serial.print(" :: X_Meas = "); Serial.print(X_Meas); Serial.print(" :: Y_Meas = "); Serial.print(Y_Meas); Serial.print(" :: returnString = "); Serial.print(returnString); Serial.println(" :: ");
    pCharacteristic -> setValue(returnString);
    pCharacteristic -> notify();
  }
  digitalWrite(Received, LOW);
  delay(5000);
}
