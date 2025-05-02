#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include "AsyncCaptivePortal.h"

static AsyncWebServer server(80);
static AsyncCaptivePortal portal("AsyncCaptivePortal");

void setup() {
  Serial.begin(115200);
  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){ request->send(200, "text/plain", "Hello, world!"); });
  server.addHandler(portal.getHandler()).setFilter(ON_AP_FILTER);  // only when requested from AP
  portal.begin();
  server.begin();
}

void loop() {
  portal.loop();
}
