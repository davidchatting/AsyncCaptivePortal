#pragma once

#include <DNSServer.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

#include "Debug.h"
#include "AsyncCaptivePortalHandler.h"

class AsyncCaptivePortal
{
private:
  String ssid;
  String passphrase;

  DNSServer dnsServer;
  AsyncCaptivePortalHandler handler;

public:
  AsyncCaptivePortal(String ssid = "", String passphrase = "") : ssid(ssid), passphrase(passphrase) {}
  virtual ~AsyncCaptivePortal() {}

  /*
    Start the captive portal by turning on WiFi in access-point mode
    and configuring the DNS server to always resolve to the local IP address
  */
  void begin()
  {
    Debug::log("Starting captive portal...");

    WiFi.mode(WIFI_AP);
    if(WiFi.softAP(ssid, passphrase, 1, 0, 8)) {
        String selfIp = WiFi.softAPIP().toString();
        Debug::log("WiFi: " + selfIp);

        dnsServer.start(53, "*", WiFi.softAPIP());
        Debug::log("DNS: " + selfIp + ":53");

        handler.setHostname(selfIp);
        Debug::log("HTTP: " + selfIp + ":80");
    }
    else Debug::log("Soft AP creation failed.");
  }

  /* Tick the portal, this needs to be called regularly to ensure DNS requests are handled */
  void loop()
  {
    dnsServer.processNextRequest();
  }

  /* Stop the portal by disconnecting the WiFi and stopping the DNS Server*/
  void end()
  {
    dnsServer.stop();
    WiFi.disconnect(true);
  }

  /* Retrieve a RequestHandler thats pre-configured to do HTTP captive Portal logic */
  AsyncWebHandler* getHandler() { return &handler; }
};