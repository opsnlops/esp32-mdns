// This isn't gonna work on anything but an ESP32
#if !defined(ESP32)
#error This code is intended to run only on the ESP32 board
#endif

#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>

#include "secrets.h"
#include "mdns/magicbroker.h"

void setup()
{

    // Open serial communications and wait for port to open:
    Serial.begin(19200);
    while (!Serial)
        ;

    // Nice long delay to let the terminal app start
    delay(2500);
    log_i("Helllllo! I'm up and running on on %s!", ARDUINO_VARIANT);

    WiFi.begin(WIFI_NETWORK, WIFI_PASSWORD);

    /*wait until ESP32 connect to WiFi*/
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        log_d(" (not yet)");
    }
    log_i("WiFi connected to %s", WIFI_NETWORK);

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