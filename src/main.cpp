// This isn't gonna work on anything but an ESP32
#if !defined(ESP32)
#error This code is intended to run only on the ESP32 board
#endif

#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>

#include "secrets.h"
#include "network/connection.h"
#include "mdns/magicbroker.h"

const int LED_PIN = LED_BUILTIN;

void setup()
{

    pinMode (LED_PIN, OUTPUT);

    // Open serial communications and wait for port to open:
    Serial.begin(19200);
    while (!Serial)
        ;

    // Nice long delay to let the terminal app start
    delay(2500);
    log_i("Helllllo! I'm up and running on on %s!", ARDUINO_VARIANT);
    digitalWrite(LED_PIN, LOW);
    

    NetworkConnection network = NetworkConnection();
    network.connectToWiFi();


    if (!MDNS.begin("esp32-mdns"))
    {
        log_e("Error setting up MDNS responder!");
        while (1)
        {
            delay(1000);
        }
    }
    log_i("MDNS set up");

    // Register a fake server
    MDNS.addService("creatures", "tcp", 666);
    MDNS.addServiceTxt("creatures", "tcp", "board", ARDUINO_VARIANT);
    log_d("created a fake service");
    
    MagicBroker broker = MagicBroker();
    if (broker.find())
    {
        log_i("found the magic broker! %s", broker.ipAddress.toString().c_str());
    }
    else
    {
        log_e("unable to find the magic broker (role is suppose to be '%s')", broker.getMagicRole());
    }
}

void loop()
{
    // put your main code here, to run repeatedly:
}