#pragma once

// ─── WiFi ────────────────────────────────────────────────────────────────────
#define WIFI_SSID     "MonWiFi"
#define WIFI_PASSWORD "MonMotDePasse"

// ─── Philips Hue Bridge ───────────────────────────────────────────────────────
// IP de votre pont Hue (fixez-la dans votre routeur ou via DHCP reservation)
#define HUE_BRIDGE_IP  "192.168.1.100"

// Clé API : appuyez sur le bouton du pont puis lancez une requête POST
// http://<bridge_ip>/api  { "devicetype": "hue-kids-room#esp32" }
// Récupérez le "username" retourné et collez-le ici :
#define HUE_USERNAME   "votreCleApiIci"

// ─── Lumières de la chambre ───────────────────────────────────────────────────
// IDs des ampoules Hue à contrôler (trouvez-les via GET /api/<user>/lights)
static const int HUE_LIGHT_IDS[]  = { 1, 2 };
static const int HUE_LIGHT_COUNT  = 2;

// ─── Modes prédéfinis ────────────────────────────────────────────────────────
// Night mode : blanc chaud très tamisé
#define HUE_NIGHT_CT   450    // Mireds — plus élevé = plus chaud (153-500)
#define HUE_NIGHT_BRI  40     // 0-254

// Party mode : vitesse de changement de couleur (ms)
#define HUE_PARTY_INTERVAL_MS  1500
