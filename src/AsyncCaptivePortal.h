#pragma once

#include <DNSServer.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <ESPAsyncWebServer.h>

#include "Debug.h"
#include "AsyncCaptivePortalHandler.h"

class AsyncCaptivePortal
{
private:
  String ssid;
  String passphrase;
  bool popup;

  DNSServer dnsServer;
  AsyncCaptivePortalHandler handler;

  wifi_sta_list_t staList;
  int numConnections = 0;
  static int maxConnections;

  static std::function<void(bool)> onClientConnected;

public:
  AsyncCaptivePortal(String ssid = "", String passphrase = "", bool popup = true) : ssid(ssid), passphrase(passphrase), popup(popup) {}
  virtual ~AsyncCaptivePortal() {}

  /*
    Start the captive portal by turning on WiFi in access-point mode
    and configuring the DNS server to always resolve to the local IP address
  */
  void begin()
  {
    Debug::log("Starting captive portal...");

    WiFi.mode(WIFI_AP);

    if(WiFi.softAP(ssid, passphrase, 1, 0, maxConnections)) {
        String selfIp = WiFi.softAPIP().toString();
        Debug::log("WiFi: " + selfIp);

        if(popup){
          dnsServer.start(53, "*", WiFi.softAPIP());
          Debug::log("DNS: " + selfIp + ":53");
        }

        handler.setHostname(selfIp);
        Debug::log("HTTP: " + selfIp + ":80");
    }
    else Debug::log("Soft AP creation failed.");
  }

  /* Tick the portal, this needs to be called regularly to ensure DNS requests are handled */
  void loop()
  {
    if (esp_wifi_ap_get_sta_list(&staList) == ESP_OK) {
      if(staList.num != numConnections) {
        if (onClientConnected) onClientConnected(staList.num > numConnections);
        numConnections = staList.num;
      }
    }
    if(popup) dnsServer.processNextRequest();
  }

  /* Stop the portal by disconnecting the WiFi and stopping the DNS Server*/
  void end()
  {
    if(popup) dnsServer.stop();
    WiFi.disconnect(true);
  }

  /* Retrieve a RequestHandler thats pre-configured to do HTTP captive Portal logic */
  AsyncWebHandler* getHandler() { return &handler; }

  void setOnClientConnected(std::function<void(bool)> callback)
  {
    onClientConnected = callback;
  }
};

std::function<void(bool)> AsyncCaptivePortal::onClientConnected = nullptr;
int AsyncCaptivePortal::maxConnections = 4;