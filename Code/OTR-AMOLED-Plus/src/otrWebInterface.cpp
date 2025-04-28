// OPEN TAG READER
// Lilygo T-Display-S3 AMOELD PLUS
// C.Chad 2025

#include "otrWebInterface.h"
#include "LilyGo_AMOLED.h"
#include "ESPAsyncWebServer.h"
#include "AsyncWebSocket.h"

AsyncWebServer server(80);

String testToHtml(void) {
    String html = readLittleFSFileToString("/test.html");
    return html;
}

void sendWebpage(AsyncWebServerRequest *request, const String& htmlContent) {
    request->send(200, "text/html", htmlContent);
}

void setupWebRoutes() {
    String html = testToHtml();
  
    server.on("/", HTTP_GET, [html](AsyncWebServerRequest *request){
      sendWebpage(request, html);
    });


    // Tag Images Stored in /html/tag_images/on SD card
    server.serveStatic("/images/BLACK.png", SD, "/html/tag_images/Black.png");
    server.serveStatic("/images/BLUE.png", SD, "/html/tag_images/Blue.png");
    server.serveStatic("/images/GREEN.png", SD, "/html/tag_images/Green.png");
    server.serveStatic("/images/nlis_black.png", SD, "/html/tag_images/nlis_black.png");
    server.serveStatic("/images/nlis_white.png", SD, "/html/tag_images/nlis_white.png");
    server.serveStatic("/images/ORANGE.png", SD, "/html/tag_images/Orange.png");
    server.serveStatic("/images/PINK.png", SD, "/html/tag_images/Pink.png");
    server.serveStatic("/images/PURPLE.png", SD, "/html/tag_images/Purple.png");
    server.serveStatic("/images/RED.png", SD, "/html/tag_images/Red.png");
    server.serveStatic("/images/WHITE.png", SD, "/html/tag_images/White.png");
    server.serveStatic("/images/YELLOW.png", SD, "/html/tag_images/Yellow.png");
    server.serveStatic("/main.html", LittleFS, "/main.html");
    // Example of a route to handle the button toggle (as discussed previously)
    server.on("/toggle", HTTP_GET, [](AsyncWebServerRequest *request){
      if (request->hasParam("state")) {
        String state = request->getParam("state")->value();
        Serial.print("Received toggle request: ");
        Serial.println(state);
        // Add your logic here to control the tag reader based on the 'state'
  
        // You might want to send updated HTML back to reflect the state
        // For a simple example, let's just send an OK response
        request->send(200, "text/plain", "OK");
      } else {
        request->send(400, "text/plain", "Invalid request");
      }
    });
  }

  void startWebServer(void) {
    setupWebRoutes();
    server.begin();
  }