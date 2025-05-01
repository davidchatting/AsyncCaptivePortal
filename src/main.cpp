#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include "AsyncCaptivePortal.h"

auto server = AsyncWebServer(80);
auto portal = AsyncCaptivePortal("Basic Portal");

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
