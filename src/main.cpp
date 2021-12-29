// This isn't gonna work on anything but an ESP32
#if !defined(ESP32)
#error This code is intended to run only on the ESP32 board
#endif

#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>

#include "secrets.h"

IPAddress DEFAULT_IP_ADDRESS = IPAddress(0, 0, 0, 0);
uint16_t DEFAULT_MQTT_PORT = 1883;

String BROKER_ROLE = "magic";





class MagicBroker
{
  public:

    MagicBroker()
    {
      ipAddress = DEFAULT_IP_ADDRESS;
      port = DEFAULT_MQTT_PORT;
    }

    MagicBroker(IPAddress _ip, uint16_t _port)
    {
      ipAddress = _ip;
      port = _port;
    }

    int operator ==(MagicBroker _that)
		{
			if(_that.port == port && _that.ipAddress == ipAddress)
			  return 1;
			else
			  return 0;
		}

    IPAddress ipAddress;
    uint16_t port;
};

MagicBroker findMagicBroker();

const MagicBroker UNKNOWN_BROKER = MagicBroker();

void setup()
{

  // Open serial communications and wait for port to open:
  Serial.begin(19200);
  while (!Serial)
    ;
 
  // Nice long delay to let minicom start
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
    while(1)
    {
        delay(1000);
    }
  }
  log_i("MDNS set up");



// Register a fake server
MDNS.addService("creatures", "tcp", 666);
log_d("created a fake service");


MagicBroker broker = findMagicBroker();

if(broker == UNKNOWN_BROKER)
{
  log_e("unable to find the magic broker");
}
else
{
  log_i("found the magic broker! %s", broker.ipAddress.toString().c_str());
}


}

MagicBroker findMagicBroker()
{

  log_i("looking for the magic broker");

  int n = MDNS.queryService("mqtt", "tcp");
  if (n == 0)
  {
      log_w("couldn't find the magic broker in mDNS");
      return UNKNOWN_BROKER;
  }
  else
  {
      log_d("%d services(s) found", n);
      for (int i = 0; i < n; ++i)
      {
        String hostname = MDNS.hostname(i).c_str();
        String role = MDNS.txt(i, "role").c_str();
        IPAddress ipAddress = MDNS.IP(i);
        int port = MDNS.port(i);

          // Print details for each service found
          log_d("  %d: %s (%s:%d) role: %s", 
                  (i + 1), 
                  hostname, 
                  ipAddress.toString().c_str(), 
                  port, 
                  role
              );

        if(role == BROKER_ROLE)
        {
          log_i("ding ding ding ding!");
          return MagicBroker(ipAddress, port);
        }
      }
  }
  log_w("unable to find the magic broker (role '%s'", BROKER_ROLE.c_str());

  return UNKNOWN_BROKER;

}

void loop()
{
// put your main code here, to run repeatedly:
}