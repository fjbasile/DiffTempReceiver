/*
  LED

  This example creates a BLE peripheral with service that contains a
  characteristic to control an LED.

  The circuit:
  - Arduino MKR WiFi 1010, Arduino Uno WiFi Rev2 board, Arduino Nano 33 IoT,
    Arduino Nano 33 BLE, or Arduino Nano 33 BLE Sense board.

  You can use a generic BLE central app, like LightBlue (iOS and Android) or
  nRF Connect (Android), to interact with the services and characteristics
  created in this sketch.

  This example code is in the public domain.
*/

#include <ArduinoBLE.h>

BLEService tempService("19B10000-E8F2-537E-4F6C-D104768A1214"); // BLE Temperature Service

// BLE LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLEByteCharacteristic firstTempCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLEWrite | BLENotify);
BLEByteCharacteristic secondTempCharacteristic("19B10002-E8F2-537E-4F6C-D104768A1214", BLEWrite | BLENotify);


const int ledPin = LED_BUILTIN; // pin to use for the LED
int temp;

void setup() {
  Serial.begin(9600);

  // set LED pin to output mode
  pinMode(ledPin, OUTPUT);

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");

    while (1);
  }

  // set advertised local name and service UUID:
  BLE.setLocalName("Temperature");
  BLE.setAdvertisedService(tempService);

  // add the characteristic to the service
  tempService.addCharacteristic(firstTempCharacteristic);
  tempService.addCharacteristic(secondTempCharacteristic);

  // add service
  BLE.addService(tempService);

  // set the initial value for the characeristic:
  firstTempCharacteristic.writeValue(0);
  secondTempCharacteristic.writeValue(0);

  // start advertising
  BLE.advertise();

  Serial.println("BLE Temperature Peripheral");
}

void loop() 
{
  // listen for BLE peripherals to connect:
  BLEDevice central = BLE.central();

  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());

    // while the central is still connected to peripheral:
    while (central.connected())
    {
      // if the remote device wrote to the characteristic,
      // use the value to control the LED:
      if (firstTempCharacteristic.written())
      {
        Serial.print("High Byte: ");
        Serial.println(firstTempCharacteristic.value());    
      }

      if (secondTempCharacteristic.written())
      {
        Serial.print("Low Byte: ");
        Serial.println(secondTempCharacteristic.value());
        if (firstTempCharacteristic.value() == 1)
        {
            temp = 256 + secondTempCharacteristic.value();
        }
        else
        {
            temp = secondTempCharacteristic.value();
        }
        Serial.print ("Temp: ");
        Serial.println (temp); 
        if (temp > 190)
        {
          digitalWrite(ledPin, HIGH);      
        }
        else
        {
          digitalWrite(ledPin, LOW);
        }
      }
    }

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}
