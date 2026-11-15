#ifndef WEB_MANAGER_H
#define WEB_MANAGER_H

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

class WebManager 
{
  private:
      AsyncWebServer server;
      AsyncWebSocket ws;
      const char* ssid;
      const char* password;
      unsigned long lastTelemetry = 0;

      // Callback pour exécuter les commandes sur le Graoully principal
      void (*commandCallback)(const String& target, const String& action);

      static void handleWebSocketMessage(void *arg, uint8_t *data, size_t len, void* objPtr) 
      {
          WebManager* manager = (WebManager*)objPtr;
          AwsFrameInfo *info = (AwsFrameInfo*)arg;
          
          if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) 
          {
              data[len] = 0;
              String message = (char*)data;
              message.trim();

              Serial.println("[WEB -> ESP32] Reçu : " + message);

              int separatorIndex = message.indexOf(':');
              if (separatorIndex != -1) 
              {
                  String target = message.substring(0, separatorIndex);
                  String action = message.substring(separatorIndex + 1);

                  // Si la cible est le Mega (ex: "mega:mouvement") ou le Graoully local
                  if (target == "mega") 
                  {
                      // Transmission à l'Arduino Mega esclave via Serial2
                      Serial2.println(message);
                      Serial.println("[ESP32 -> MEGA] Transmis : " + message);
                  } 
                  else if (manager->commandCallback) 
                  {
                      // Exécution locale sur l'ESP32
                      manager->commandCallback(target, action);
                  }
              }
          }
      }

      static void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len, void* objPtr) 
      {
          if (type == WS_EVT_CONNECT) 
          {
              Serial.printf("Client web connecté #%u\n", client->id());
          } 
          else if (type == WS_EVT_DISCONNECT) 
          {
              Serial.printf("Client web déconnecté #%u\n", client->id());
          } 
          else if (type == WS_EVT_DATA) 
          {
              handleWebSocketMessage(arg, data, len, objPtr);
          }
      }

  public:
      WebManager(const char* s, const char* p) : server(80), ws("/ws"), ssid(s), password(p), commandCallback(nullptr) {}

      void setCommandCallback(void (*cb)(const String&, const String&)) 
      {
          commandCallback = cb;
      }

      void begin() 
      {
          // Démarrage du Point d'accès Wi-Fi
          WiFi.softAP("Graoully-Studio", "12345678");
          
          Serial.println();
          Serial.println(F("Point d'accès Wi-Fi créé !"));
          Serial.print(F("Adresse IP de l'ESP32 : "));
          Serial.println(WiFi.softAPIP());

          // --- ROUTE POUR HÉBERGER LA PAGE WEB ---
          server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
              request->send(200, "text/html", R"rawliteral(
                  <!DOCTYPE html>
                  <html lang="fr">
                  <head>
                      <meta charset="UTF-8">
                      <title>Cockpit du Graoully</title>
                      <style>
                          body { font-family: Arial, sans-serif; text-align: center; margin-top: 50px; background: #121212; color: #fff; }
                          .card { background: #1e1e1e; padding: 20px; border-radius: 10px; display: inline-block; box-shadow: 0 4px 10px rgba(0,0,0,0.5); }
                          button { background: #ff4757; color: white; border: none; padding: 10px 20px; margin: 10px; font-size: 16px; border-radius: 5px; cursor: pointer; }
                          button:hover { background: #ff6b81; }
                          pre { background: #2d3436; padding: 15px; border-radius: 5px; text-align: left; max-width: 400px; margin: 20px auto; color: #00cec9; }
                      </style>
                  </head>
                  <body>
                      <div class="card">
                          <h1>🐲 Cockpit Virtuel du Graoully</h1>
                          <p id="status" style="color: orange;">Connexion au WebSocket...</p>
                          <div>
                              <button onclick="sendCommand('graoully:home')">Home Graoully</button>
                              <button onclick="sendCommand('pan:0.8')">Bouger Pan (0.8)</button>
                              <button onclick="sendCommand('pan:0.2')">Bouger Pan (0.2)</button>
                          </div>
                          <h3>Télémétrie en direct :</h3>
                          <pre id="telemetry">En attente de données...</pre>
                      </div>
                      <script>
                          const ws = new WebSocket('ws://192.168.4.1/ws');
                          ws.onopen = () => { document.getElementById('status').innerText = 'Connecté au Graoully !'; document.getElementById('status').style.color = '#2ed573'; };
                          ws.onclose = () => { document.getElementById('status').innerText = 'Déconnecté !'; document.getElementById('status').style.color = '#ff4757'; };
                          ws.onmessage = (event) => { document.getElementById('telemetry').innerText = JSON.stringify(JSON.parse(event.data), null, 2); };
                          function sendCommand(cmd) { if (ws.readyState === WebSocket.OPEN) ws.send(cmd); }
                      </script>
                  </body>
                  </html>
              )rawliteral");
          });

          // Configuration des événements WebSocket
          ws.onEvent([this](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
              this->onEvent(server, client, type, arg, data, len, this);
          });

          server.addHandler(&ws);
          server.begin();
      }

      void update(Dragon& graoully) 
      {
          ws.cleanupClients();

          // Envoi de la télémétrie toutes les 100ms aux clients web connectés
          if (millis() - lastTelemetry > 100) 
          {
              lastTelemetry = millis();
              
              String json = "{";
              for (int i = 0; i < Dragon::kNumMotors; i++) 
              {
                  json += "\"" + String(graoully.allMotors[i]->getName()) + "\":";
                  json += String(graoully.allMotors[i]->getPositionRatio(), 2);
                  if (i < Dragon::kNumMotors - 1) json += ",";
              }
              json += "}";
              ws.textAll(json);
          }
      }
};

#endif