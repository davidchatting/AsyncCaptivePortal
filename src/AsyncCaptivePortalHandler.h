class AsyncCaptivePortalHandler : public AsyncWebHandler {
    private:
      String hostname;
  
    public:
      bool canHandle(__unused AsyncWebServerRequest *request) const override {
        return this->hostname.length() > 0 && request->host() != this->hostname;
      }
  
      // https://datatracker.ietf.org/doc/html/rfc6585#section-6.1
      void handleRequest(AsyncWebServerRequest *request) {
        if (request->host() == "netcts.cdn-apple.com") {
          Serial.println("-> 503 Service Unavailable");
          return request->send(503, "text/plain", "Service Unavailable");
        }
        else {
          Serial.println("-> redirect " + request->host() + request->url() +  " -> " + hostname);
        
          auto location = "http://" + hostname;
          AsyncResponseStream *response = request->beginResponseStream("text/html");
          response->setCode(511);
          response->print(
            "<!DOCTYPE html>"
            "<html>"
            "  <head>"
            "    <meta charset=\"UTF-8\" />"
            "    <title>Network Authentication Required</title>"
            "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" />"
            "    <meta http-equiv=\"refresh\" content=\"0; url=" + location + "\">"
            "  </head>"
            "  <body>"
            "    <p>You need to <a href=\"" + location + "\">log in</a> to access this network.</p>"
            "  </body>"
            "</html>"
          );
          request->send(response);
        }
      }
  
      String getHostname() { return hostname; }
      void setHostname(String value) { hostname = value; }
  };