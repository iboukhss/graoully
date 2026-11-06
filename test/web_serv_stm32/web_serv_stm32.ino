#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "Animatronic_Master";
const char* password = "password123";

// Définition de la broche de la LED
const int LED_PIN = 15;

WebServer server(80);

// Code HTML de la page web (Dark Mode type Dashboard)
void handleRoot() {
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<meta charset='UTF-8'>";
  html += "<title>Animatronic Master Control</title>";
  html += "<style>";
  html += "body { font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; background-color: #121212; color: #e0e0e0; text-align: center; margin: 0; padding: 20px; }";
  html += "h1 { color: #00bcd4; font-size: 24px; margin-bottom: 5px; }";
  html += ".subtitle { color: #888; font-size: 14px; margin-bottom: 30px; }";
  html += ".card { background: #1e1e1e; border-radius: 12px; padding: 20px; margin: 15px auto; max-width: 400px; box-shadow: 0 4px 10px rgba(0,0,0,0.5); }";
  html += ".btn { display: block; width: 100%; padding: 15px; font-size: 16px; font-weight: bold; color: white; border: none; border-radius: 8px; cursor: pointer; margin: 10px 0; text-decoration: none; transition: 0.2s; }";
  html += ".btn-green { background-color: #2e7d32; }";
  html += ".btn-green:active { background-color: #1b5e20; }";
  html += ".btn-blue { background-color: #1565c0; }";
  html += ".btn-blue:active { background-color: #0d47a1; }";
  html += ".btn-red { background-color: #c62828; }";
  html += ".btn-red:active { background-color: #b71c1c; }";
  html += "#status { margin-top: 15px; font-size: 14px; color: #ffeb3b; min-height: 20px; }";
  html += "</style>";
  html += "<script>";
  html += "function sendAction(cmd) {";
  html += "  document.getElementById('status').innerText = 'Envoi de la commande : ' + cmd + '...';";
  html += "  fetch('/action?cmd=' + cmd)";
  html += "    .then(response => response.text())";
  html += "    .then(data => { document.getElementById('status').innerText = 'Succès : ' + data; })";
  html += "    .catch(error => { document.getElementById('status').innerText = 'Erreur de communication'; });";
  html += "}";
  html += "</script>";
  html += "</head><body>";
  
  html += "<h1>🤖 Animatronic Master</h1>";
  html += "<div class='subtitle'>Interface de Contrôle Sans Fil (ESP32-S3)</div>";
  
  // Bloc Séquences
  html += "<div class='card'>";
  html += "<h3>Séquences Globales</h3>";
  html += "<button class='btn btn-green' onclick=\"sendAction('seq1')\">▶ Lancer Séquence 1 (Vérin + Son)</button>";
  html += "<button class='btn btn-blue' onclick=\"sendAction('seq2')\">▶ Lancer Séquence 2 (Regard + LED)</button>";
  html += "</div>";

  // Bloc Sécurité / Urgence
  html += "<div class='card'>";
  html += "<h3>Contrôle Manuel / Sécurité</h3>";
  html += "<button class='btn btn-red' onclick=\"sendAction('stop')\">🛑 Arrêt d'Urgence (Reset)</button>";
  html += "</div>";

  html += "<div id='status'>Prêt.</div>";
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

// Gestion des actions reçues depuis le téléphone
void handleAction() {
  if (server.hasArg("cmd")) {
    String command = server.arg("cmd");
    
    Serial.print("Action reçue depuis le GSM : ");
    Serial.println(command);
    
    if (command == "seq1") {
      // Allume la LED sur la GPIO 15
      digitalWrite(LED_PIN, HIGH);
      delay(1000); // Laisse allumé 1 seconde
      digitalWrite(LED_PIN, LOW);
      
      server.send(200, "text/plain", "Séquence 1 exécutée (LED testée) !");
    } 
    else if (command == "seq2") {
      // Fait clignoter la LED deux fois
      for(int i = 0; i < 2; i++) {
        digitalWrite(LED_PIN, HIGH);
        delay(200);
        digitalWrite(LED_PIN, LOW);
        delay(200);
      }
      server.send(200, "text/plain", "Séquence 2 exécutée (LED clignotée) !");
    } 
    else if (command == "stop") {
      // Éteint la LED de force
      digitalWrite(LED_PIN, LOW);
      server.send(200, "text/plain", "Arrêt d'urgence appliqué !");
    } 
    else {
      server.send(200, "text/plain", "Commande inconnue");
    }
  } else {
    server.send(400, "text/plain", "Argument manquant");
  }
}

void setup() 
{
  Serial.begin(115200);
  delay(2000);

  Serial.println("\n--- DEMARRAGE DU SERVEUR WEB AVEC LED ---");

  // Configuration de la broche de la LED en sortie
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); // S'assure qu'elle est éteinte au démarrage

  // Démarrage du Point d'Accès Wi-Fi
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  Serial.print("Adresse IP : ");
  Serial.println(WiFi.softAPIP());

  // Configuration des routes du serveur
  server.on("/", handleRoot);
  server.on("/action", handleAction);
  
  server.begin();
  Serial.println("Serveur HTTP prêt !");
}

void loop() 
{
  server.handleClient();
}
