#include <WiFi.h>
#include "SPIFFS.h"
#include "ESPAsyncWebServer.h"
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <HTTPClient.h>

#include <EEPROM.h>
#define EEPROM_SIZE 230

WiFiClient wifiClient;
//##########################  configuration and variables  ##################
int status = WL_IDLE_STATUS;
unsigned long lastSend;
PubSubClient client(wifiClient);
String wsid ;
String wpass;
String password ;
String static_ip ;
String myIP ;
String LOCAL_IP ;
String apname ;
String abc;

const char* file = "/config.json";   //Enter your file name
const char* s_data_file = "/ServiceData_jsonfile.txt";
String Service ;
String service_s ;
String host_ip ;
int port ;
int uinterval ;
String u_time;
String c_id ;

int QOS ; // 0
String U_name ;
String s_pass ;
String p_topic;
String Http_requestpath;

AsyncWebServer server(80);


//################################  MAC ADDRESS FUNCTION  #########################################
String getMacAddress() {
  uint8_t baseMac[6];
  // Get MAC address for WiFi station
  esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
  char baseMacChr[18] = {0};
  sprintf(baseMacChr, "%02X:%02X:%02X:%02X:%02X:%02X", baseMac[5], baseMac[4], baseMac[3], baseMac[2], baseMac[1], baseMac[0]);
  return String(baseMacChr);
}
//####################################### testing wifi connection function ############################
bool testWifi(void) {
  int c = 0;
  Serial.println("Waiting for Wifi to connect");
  while ( c < 30 ) {
    if (WiFi.status() == WL_CONNECTED)
    {
      return true;
    }
    delay(500);
    Serial.print(WiFi.status());
    Serial.print(".");
    c++;
  }
  Serial.println("");
  Serial.println("Connect timed out, opening AP");
  LOCAL_IP = "Not Connected";
  WiFi.mode(WIFI_AP);
  return false;
}

//############   Conversion for acceesspoint ip into unsigned int ###################
const int numberOfPieces = 4;
String ipaddress[numberOfPieces];
void ipAdress(String& eap, String& iip1, String& iip2, String& iip3, String& iip4)
{

  int counter = 0;
  int lastIndex = 0;
  for (int i = 0; i < eap.length(); i++) {

    if (eap.substring(i, i + 1) == ".") {
      ipaddress[counter] = eap.substring(lastIndex, i);

      lastIndex = i + 1;
      counter++;
    }
    if (i == eap.length() - 1) {
      ipaddress[counter] = eap.substring(lastIndex);
    }

  }
  iip1 = ipaddress[0];
  iip2 = ipaddress[1];
  iip3 = ipaddress[2];
  iip4 = ipaddress[3];

}

void setup() {

  Serial.begin(115200);
  if (!EEPROM.begin(EEPROM_SIZE))
  {
    Serial.println("failed to initialise EEPROM"); delay(3000);
  }

  //########################  reading config file ########################################

  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  Serial.println("SPIFF successfully mounted");
  File dataFile = SPIFFS.open(file, "r");   //Open File for reading
  Serial.println("Reading Configuration Data from File:");
  if (!dataFile) {
    Serial.println("Count file open failed on read.");
  }
  else {
    for (int i = 0; i < dataFile.size(); i++) //Read upto complete file size
    {
      abc += (char)dataFile.read();
    }
    dataFile.close();
  }

  Serial.print(abc);
  Serial.println("");
  Serial.println("");

  WiFi.mode(WIFI_AP_STA);

  //@@@@@@@@@@@@@@@@@@@@@@@@@@@  EEPROM read FOR SSID-PASSWORD- ACCESS POINT IP @@@@@@@@@@@@@@@@@@@@@@@@@@@@
  String esid ;
  String ip1, ip2, ip3, ip4;

  for (int ssidaddress = 0; EEPROM.read(ssidaddress) != '\0' ; ++ssidaddress)
  {
    esid += char(EEPROM.read(ssidaddress));
  }
  //  Serial.print("Access point SSID:");
  //  Serial.println(esid);

  String epass = "";
  for (int passaddress = 22 ; passaddress < 43 ; ++passaddress)
  {
    epass += char(EEPROM.read(passaddress));
  }
  //  Serial.print("Access point PASSWORD:");
  //  Serial.print(epass);
  //  Serial.println(" ");

  String eap;
  for (int APaddress = 44 ; EEPROM.read(APaddress) != '\0' ; ++APaddress)
  {
    eap += char(EEPROM.read(APaddress));
  }
  //  Serial.print("Access point ADDRESS: ");
  //  Serial.print(eap);
  //  Serial.println(" ");

  //##############################   ACESS POINT begin on given credential #################################

  if ((esid == NULL ) && (epass == NULL)) {

    Serial.println("###  FIRST TIME SSID PASSWORD SET  ### ");
    StaticJsonBuffer<500> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(abc);

    if (!root.success()) {
      Serial.println("parseObject() failed");
      return;
    }
    String    apsid =  root["AP_name"];
    String    appass = root["AP_pass"];

    for (int i = 0; i < 21; ++i) {
      EEPROM.write(i, 0);  //AP ssid
    }
    EEPROM.commit();
    for (int ssidaddress = 0; ssidaddress < apsid.length(); ++ssidaddress)
    {
      EEPROM.write(ssidaddress , apsid[ssidaddress]);
      Serial.print("WRITING  default AP SSID :: ");
      Serial.println(apsid[ssidaddress]);
    }
    EEPROM.commit();

    for (int i = 22; i < 43 ; ++i) {
      EEPROM.write(i, 0);  // AP password
    }
    EEPROM.commit();
    for (int i = 0; i < appass.length(); ++i)
    {
      EEPROM.write(22 + i, appass[i]);
      Serial.print("WRITING  default AP PASSWORD  :: ");
      Serial.println(appass[i]);
    }
    EEPROM.commit();

    WiFi.softAP(apsid.c_str(), appass.c_str()); //Password not used
    apname = apsid;
    delay(800);

  }

  else {
    //    Serial.println("@@@@  GETTING SSID N PASSWORD  @@@@@ ");
    Serial.print("Access point SSID:");
    Serial.println(esid);
    Serial.print("Access point PASSWORD:");
    Serial.print(epass);
    Serial.println(" ");
    WiFi.softAP(esid.c_str(), epass.c_str());
    apname = esid;
  }

  if (eap == NULL )
  {
    Serial.println("");
    Serial.println("FIRST TIME AP ADDRESS SETTING 192.168.4.1");

    StaticJsonBuffer<500> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(abc);

    if (!root.success()) {
      Serial.println("parseObject() failed");
      return;
    }
    String    apip =   root["AP_IP"];
    for (int i = 0; i < apip.length(); ++i)
    {
      EEPROM.write(44 + i, apip[i]);
      Serial.print("access point IP  WRITE:: ");
      Serial.println(apip[i]);
    }
    EEPROM.commit();

    String AP;
    for (int APaddress = 44 ; APaddress < 65 ; ++APaddress)
    {
      AP += char(EEPROM.read(APaddress));
    }
    Serial.print("ACCESSPOINT ADDRESS: ");
    Serial.println(AP);
    Serial.println("");
    Serial.println("");

    ipAdress(AP, ip1, ip2, ip3, ip4);

    IPAddress Ip(ip1.toInt(), ip2.toInt(), ip3.toInt(), ip4.toInt());
    IPAddress NMask(255, 255, 0, 0);

    WiFi.softAPConfig(Ip, Ip, NMask );
    myIP = WiFi.softAPIP().toString();

    Serial.print("#### SERVER STARTED ON THIS: ");
    Serial.print(myIP);
    Serial.println("####");
  }

  else {
    Serial.print("Access point ADDRESS:");
    Serial.println(eap);

    ipAdress(eap, ip1, ip2, ip3, ip4);
    IPAddress Ip(ip1.toInt(), ip2.toInt(), ip3.toInt(), ip4.toInt());
    IPAddress NMask(255, 255, 0, 0);

    WiFi.softAPConfig(Ip, Ip, NMask );
    myIP = WiFi.softAPIP().toString();

    Serial.print("#### SERVER STARTED ON THIS: ");
    Serial.print(myIP);
    Serial.print("####");
  }

  //#########################  HTML+JS+CSS  HANDLING #####################################

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/login.html", "text/html");
  });
  server.on("/main.html", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/main.html", "text/html");
  });
  server.on("/js/bootstrap.min.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/js/bootstrap.min.js", "text/javascript");
  });
  server.on("/js/jquery-1.12.3.min.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/js/jquery-1.12.3.min.js", "text/javascript");
  });
  server.on("/js/pixie-custom.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/js/pixie-custom.js", "text/javascript");
  });
  server.on("/css/bootstrap.min.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/css/bootstrap.min.css", "text/css");
  });
  server.on("/css/pixie-main.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/css/pixie-main.css", "text/css");
  });
  //############################# IMAGES HANDLING  ######################################################

  server.on("/images/ap.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/images/ap.png", "image/png");
  });
  server.on("/images/eye-close.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/images/eye-close.png", "image/png");
  });
  server.on("/images/light.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/images/light.png", "image/png");
  });
  server.on("/images/network.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/images/network.png", "image/png");
  });
  server.on("/images/other.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/images/other.png", "image/png");
  });
  server.on("/images/periperal.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/images/periperal.png", "image/png");
  });
  server.on("/images/reboot.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/images/reboot.png", "image/png");
  });
  server.on("/images/service.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/images/service.png", "image/png");
  });
  server.on("/images/status.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/images/status.png", "image/png");
  });
  server.on("/images/upgrade.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/images/upgrade.png", "image/png");
  });
  server.on("/images/timezone.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/images/timezone.png", "image/png");
  });

  //###################################   ACTIONS FROM WEBPAGE BUTTTONS  ##############################

  server.on("/login", HTTP_GET, [](AsyncWebServerRequest * request) {
    StaticJsonBuffer<500> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(abc);

    if (!root.success()) {
      Serial.println("parseObject() failed");
      return;
    }
    request->send(200, "text/plain", root["Admin_pass"]);
  });

  server.on("/main", HTTP_GET, [](AsyncWebServerRequest * request) {

    String content = "{\"myIP\":\"" + myIP + "\",\"localIP\":\"" + LOCAL_IP + "\",\"s_pass\":\"" + s_pass + "\",\"wsid\":\"" + wsid + "\",\"c_id\":\"" + c_id + "\",\"Service\":\"" + Service + "\",\"host_ip\":\"" + host_ip + "\",\"port\":\"" + port + "\",\"topic\":\"" + p_topic + "\",\"apname\":\"" + apname + "\",\"service\":\"" + service_s + "\",\"MAC\":\"" + getMacAddress() + "\"}";
    Serial.println(content);

    request->send(200, "application/json", content);

  });
  server.on("/scan_wifi", HTTP_GET, [](AsyncWebServerRequest * request) {
    String scan_wifi = request->getParam("scan_wifi")->value();
    if (scan_wifi)
    {
      Serial.println("scan_wifi");

      String json = "[";
      int n = WiFi.scanNetworks();
      if (n == 0)
      {
        Serial.println("no networks found");
      }
      else
      {
        Serial.print(n);
        Serial.println(" networks found");
        for (int i = 0; i < n; ++i)
        {
          // Print SSID and RSSI for each network found
          if (i)
            json += ", ";
          json += " {";
          json += "\"rssi\":" + String(WiFi.RSSI(i));
          json += ",\"ssid\":\"" + WiFi.SSID(i) + "\"";
          json += ",\"bssid\":\"" + WiFi.BSSIDstr(i) + "\"";
          json += ",\"channel\":" + String(WiFi.channel(i));
          json += ",\"secure\":" + String(WiFi.encryptionType(i));
          //                json += ",\"hidden\":"+String(WiFi.isHidden(i)?"true":"false");
          json += "}";
          if (i == (n - 1))
          {
            json += "]";
          }
        }
        delay(10);
        Serial.println(json);
        request->send(200, "application/json", json);
      }
    }
  });

  //################################# AP SSID-PASSWORD-IP RECEIVING FROM WEB PAGE WRITING TO EEPROM  ###############################

  server.on("/applyBtnFunction", HTTP_GET, [] (AsyncWebServerRequest * request) {

    String txtssid = request->getParam("txtssid")->value();
    String  txtpass = request->getParam("txtpass")->value();
    String  txtaplan = request->getParam("txtaplan")->value();

    if (txtssid.length() > 0) {

      for (int i = 0; i < 21; ++i) {
        EEPROM.write(i, 0);
      }
      EEPROM.commit();

      Serial.print("RE-writting: ");
      for (int ssidaddress = 0; ssidaddress < txtssid.length(); ++ssidaddress)
      {
        EEPROM.write(ssidaddress , txtssid[ssidaddress]);
        Serial.print("WRITING AP SSID :: ");
        Serial.println(txtssid[ssidaddress]);
      }
    }
    if ( txtpass.length() > 0) {

      for (int i = 22; i < 43 ; ++i) {
        EEPROM.write(i, 0);
      }
      EEPROM.commit();

      for (int i = 0; i < txtpass.length(); ++i)
      {
        EEPROM.write(22 + i, txtpass[i]);
        Serial.print("AP PASSWORD WRITE:: ");
        Serial.println(txtpass[i]);
      }
    }
    if ( txtaplan.length() > 0) {

      for (int i = 44 ; i < 65 ; ++i) {
        EEPROM.write(i, 0);
      }
      EEPROM.commit();

      for (int i = 0; i < txtaplan.length(); ++i)
      {
        EEPROM.write(44 + i, txtaplan[i]);
        Serial.print("access point IP  WRITE:: ");
        Serial.println(txtaplan[i]);
      }
    }
    EEPROM.commit();
    request->send(200, "text/plain", "ok");
  });

  //#####################################  Receving WIFI credential from WEB Page ############################

  server.on("/connectBtnFunction", HTTP_GET, [] (AsyncWebServerRequest * request) {

    String  wifi_ssid = request->getParam("wifi_ssid")->value();
    String  wifi_pass = request->getParam("wifi_pass")->value();
    String  wifi_MODE  = request->getParam("wifi_MODE")->value();
    Serial.println(wifi_MODE);

    if (wifi_ssid.length() > 0) {
      for (int i = 66; i < 87; ++i) {
        EEPROM.write(i, 0);
      }
      EEPROM.commit();
      Serial.print("RE-writting wifi SSID: ");
      for (int j = 0; j < wifi_ssid.length(); ++j)
      {
        EEPROM.write(66 + j , wifi_ssid[j]);
        Serial.print("WRITING wifi SSID :: ");
        Serial.println(wifi_ssid[j]);
      }
      EEPROM.commit();
    }
    if ( wifi_pass.length() > 0) {

      for (int i = 88; i < 103; ++i) {
        EEPROM.write(i, 0);
      }
      EEPROM.commit();

      for (int i = 0; i < wifi_pass.length(); ++i)
      {
        EEPROM.write(88 + i, wifi_pass[i]);
        Serial.print("PASS WRITE:: ");
        Serial.println(wifi_pass[i]);
      }
      EEPROM.commit();
    }

    //##########################   Writing WIFI settings to EEPROM ###############################################

    if (wifi_MODE == "dhcp") {

      if ( wifi_MODE.length() > 0) {

        for (int i = 116; i < 122; ++i) {
          EEPROM.write(i, 0);
        }
        EEPROM.commit();

        for (int i = 0; i < wifi_MODE.length(); ++i)
        {
          EEPROM.write(116 + i, wifi_MODE[i]);
          Serial.print("wifi mode WRITE:: ");
          Serial.println(wifi_MODE[i]);
        }
        EEPROM.commit();
      }
    }
    if (wifi_MODE == "static") {
      Serial.println(" Mode STATIC selected ");

      String txtipadd  = request->getParam("txtipadd")->value();
      String net_m  = request->getParam("net_m")->value();
      String G_add  = request->getParam("G_add")->value();
      String P_dns  = request->getParam("P_dns")->value();
      String S_dns  = request->getParam("S_dns")->value();

      Serial.println(wifi_ssid);
      Serial.println(wifi_pass);
      Serial.println(txtipadd);
      Serial.println(net_m);
      Serial.println(G_add);
      Serial.println(P_dns);
      Serial.println(S_dns);

      if ( wifi_MODE.length() > 0) {

        for (int i = 116; i < 122; ++i) {
          EEPROM.write(i, 0);
        }
        EEPROM.commit();

        for (int i = 0; i < wifi_MODE.length(); ++i)
        {
          EEPROM.write(116 + i, wifi_MODE[i]);
          Serial.print("wifi mode WRITE:: ");
          Serial.println(wifi_MODE[i]);
        }
        EEPROM.commit();
      }

      if ( txtipadd.length() > 0) {

        for (int i = 123; i < 143; ++i) {
          EEPROM.write(i, 0);
        }
        EEPROM.commit();

        for (int i = 0; i < txtipadd.length(); ++i)
        {
          EEPROM.write(123 + i, txtipadd[i]);
          Serial.print("Static IP writing:: ");
          Serial.println(txtipadd[i]);
        }
        EEPROM.commit();
      }

      if ( net_m.length() > 0) {

        for (int i = 143; i < 160 ; ++i) {
          EEPROM.write(i, 0);
        }
        EEPROM.commit();

        for (int i = 0; i < net_m.length(); ++i)
        {
          EEPROM.write(143 + i, net_m[i]);
          Serial.print(" net mask writing:: ");
          Serial.println(net_m[i]);
        }
        EEPROM.commit();
      }

      if ( G_add.length() > 0) {

        for (int i = 161; i < 180 ; ++i) {
          EEPROM.write(i, 0);
        }
        EEPROM.commit();

        for (int i = 0; i < G_add.length(); ++i)
        {
          EEPROM.write(161 + i, G_add[i]);
          Serial.print("GATEWAY IP writing:: ");
          Serial.println(G_add[i]);
        }
        EEPROM.commit();
      }
      if ( P_dns.length() > 0) {

        for (int i = 181; i < 200 ; ++i) {
          EEPROM.write(i, 0);
        }
        EEPROM.commit();

        for (int i = 0; i < P_dns.length(); ++i)
        {
          EEPROM.write(181 + i, P_dns[i]);
          Serial.print("PRIMARY DNS writing:: ");
          Serial.println(P_dns[i]);
        }
        EEPROM.commit();
      }

      if ( S_dns.length() > 0) {

        for (int i = 201; i < 216 ; ++i) {
          EEPROM.write(i, 0);
        }
        EEPROM.commit();

        for (int i = 0; i < S_dns.length(); ++i)
        {
          EEPROM.write(201 + i, S_dns[i]);
          Serial.print("SECONDARY DNS writing:: ");
          Serial.println(S_dns[i]);
        }
        EEPROM.commit();
      }
    }
    request->send(200, "text/plain", "ok");

  });

  //###############################  RESTARTING DEVICE ON REBOOT BUTTON ####################################

  server.on("/rebootbtnfunction", HTTP_GET, [](AsyncWebServerRequest * request) {

    if (request->getParam("reboot_btn")->value() == "reboot_device") {
      Serial.print("restarting device");
      request->send(200, "text/plain", "ok");
      delay(5000);
      ESP.restart();
    }
  });

  //################################   ADMIN password change function   ######################################

  server.on("/adminpasswordfunction", HTTP_GET, [](AsyncWebServerRequest * request) {
    String confirmpassword = request->getParam("confirmpassword")->value();
    Serial.print(confirmpassword);

  });

  //######################################    RESET to Default  ############################################
  server.on("/resetbtnfunction", HTTP_GET, [](AsyncWebServerRequest * request) {

    if (request->getParam("reset_btn")->value() == "reset_device") {

      for (int i = 66; i < 103; ++i) {
        EEPROM.write(i, 0);  // wsid- wpass eeprom erase
      }
      EEPROM.commit();
      for (int i = 116; i < 220; ++i) {
        EEPROM.write(i, 0);  // wifi mode and parameters are cleared
      }
      EEPROM.commit();

      StaticJsonBuffer<500> jsonBuffer;
      JsonObject& root = jsonBuffer.parseObject(abc);

      if (!root.success()) {
        Serial.println("parseObject() failed");
        return;
      }
      String    apsid = (root["AP_name"]);
      String    appass = root["AP_pass"];
      String    apip =   root["AP_IP"];

      for (int i = 0; i < 21; ++i) {
        EEPROM.write(i, 0);  //AP ssid
      }
      EEPROM.commit();
      for (int ssidaddress = 0; ssidaddress < apsid.length(); ++ssidaddress)
      {
        EEPROM.write(ssidaddress , apsid[ssidaddress]);
        Serial.print("WRITING  default AP SSID :: ");
        Serial.println(apsid[ssidaddress]);
      }

      for (int i = 22; i < 43 ; ++i) {
        EEPROM.write(i, 0);  // AP password
      }
      EEPROM.commit();
      for (int i = 0; i < appass.length(); ++i)
      {
        EEPROM.write(22 + i, appass[i]);
        Serial.print("AP PASSWORD WRITE:: ");
        Serial.println(appass[i]);
      }
      for (int i = 44; i < 65 ; ++i) {
        EEPROM.write(i, 0);  //AP ip
      }
      EEPROM.commit();
      for (int i = 0; i < apip.length(); ++i)
      {
        EEPROM.write(44 + i, apip[i]);
        Serial.print("access point IP  WRITE:: ");
        Serial.println(apip[i]);
      }

      EEPROM.commit();
      Serial.println("EEPROM cleared");
      SPIFFS.remove("/ServiceData_jsonfile.txt");

    }
    request->send(200, "text/plain", "ok");


  });
  //############################### RECEIVING DATA SEND MMETHODS HTTP-MQTT-TCP ##############################
  server.on("/applyServiceFunction", HTTP_GET, [] (AsyncWebServerRequest * request) {

    String parameters = request->getParam("parameters")->value();
    Serial.println(parameters);

    File f = SPIFFS.open(s_data_file, "w");

    if (!f) {
      Serial.println("file open failed");
    }
    else
    {
      Serial.println("File Writing");
      f.print(parameters);
      f.close(); //Close file
      Serial.println("File closed");
    }

    File dataFile = SPIFFS.open(s_data_file, "r");   //Open File for reading
    Serial.println("Reading Data from File function time:");
    //Data from filee
    if (!dataFile) {
      Serial.println("Count file open failed on read.");
    }
    else
    {
      size_t size = dataFile.size();
      std::unique_ptr<char[]> buf(new char[size]);
      dataFile.readBytes(buf.get(), size);

      const size_t capacity = JSON_OBJECT_SIZE(11) + 240;
      DynamicJsonBuffer jsonBuffer(capacity);
      JsonObject& root  = jsonBuffer.parseObject(buf.get());

      Service = root["service"].as<String>();
      host_ip = root["host_ip"].as<String>();
      port = root["port"];
      uinterval = root["uinterval"];
      u_time = root["u_time"].as<String>(); //
      c_id = root["c_id"].as<String>(); // "Abcdefghijl"
      QOS = root["QOS"]; // 0
      U_name = root["U_name"].as<String>();
      p_topic = root["p_topic"].as<String>();
      Http_requestpath = root["Http_requestpath"].as<String>();

      //      Serial.println(Service);
      //      Serial.println(host_ip);
      //      Serial.println(port);
      //      Serial.println(uinterval);
      //      Serial.println(u_time);
      //      Serial.println(c_id);
      //      Serial.println(QOS);
      //      Serial.println(U_name);
      //      Serial.println(s_pass);
      //      Serial.println(p_topic);
      //      Serial.println(Http_requestpath);
    }
    dataFile.close();
    request->send(200, "text/plain", "ok");
  });
  //################################   WiFi settings Read   #####################################

  for (int wsidaddress = 66; EEPROM.read(wsidaddress) != '\0' ; ++wsidaddress)
  {
    wsid += char(EEPROM.read(wsidaddress));
  }

  Serial.println("");
  Serial.println("");
  Serial.print("Wifi SSID: ");
  Serial.println(wsid);

  for (int passaddress = 88 ; passaddress < 103; ++passaddress)
  {
    wpass += char(EEPROM.read(passaddress));
  }
  Serial.print("Wifi PASSword: ");
  Serial.println(wpass);

  String W_mode = "";
  for (int modeaddress = 116 ; modeaddress < 122 ; ++modeaddress)
  {
    W_mode += char(EEPROM.read(modeaddress));
  }
  Serial.print("WI-FI_MODE: ");
  Serial.println(W_mode);

  if (wsid == NULL) {
    wsid = "Not Given";
    LOCAL_IP = "network not set";
  }

  //####################################### MODE CHECKING(DHCP-STATIC) AND WIFI BEGIN #######################################

  if (W_mode == "dhcp") {
    WiFi.begin(wsid.c_str(), wpass.c_str());
    delay(2000);
    if (testWifi()) {
      Serial.print(WiFi.status());
      Serial.println("YOU ARE CONNECTED");
      LOCAL_IP = WiFi.localIP().toString();
      Serial.println(LOCAL_IP);
    }
  }


  if (W_mode == "static") {
    for (int passaddress = 123 ; passaddress < 143; ++passaddress)
    {
      static_ip += char(EEPROM.read(passaddress));
    }
    Serial.print("W-static_ip: ");
    Serial.println(static_ip);

    ipAdress(static_ip, ip1, ip2, ip3, ip4);
    String sb1, sb2, sb3, sb4;
    String sub_net = "";
    for (int passaddress = 143 ; passaddress < 160; ++passaddress)
    {
      sub_net += char(EEPROM.read(passaddress));
    }
    Serial.print("sub_net-: ");
    Serial.println(sub_net);
    delay(1000);

    ipAdress(sub_net, sb1, sb2, sb3, sb4);

    String g1, g2, g3, g4;
    String G_add = "";
    for (int passaddress = 161 ; passaddress < 180; ++passaddress)
    {
      G_add += char(EEPROM.read(passaddress));
    }
    Serial.print("G_add-: ");
    Serial.println(G_add);
    ipAdress(G_add, g1, g2, g3, g4);

    String p1, p2, p3, p4;
    String P_dns = "";
    for (int passaddress = 181 ; passaddress < 200; ++passaddress)
    {
      P_dns += char(EEPROM.read(passaddress));
    }
    Serial.print("Primary_dns-: ");
    Serial.println(P_dns);
    ipAdress(P_dns, p1, p2, p3, p4);

    String s1, s2, s3, s4;
    String S_dns = "";
    for (int passaddress = 201 ; passaddress < 216; ++passaddress)
    {
      S_dns += char(EEPROM.read(passaddress));
    }
    Serial.print("SECONDARY_dns-: ");
    Serial.println(S_dns);
    ipAdress(S_dns, s1, s2, s3, s4);

    IPAddress S_IP(ip1.toInt(), ip2.toInt(), ip3.toInt(), ip4.toInt());
    IPAddress gateway(g1.toInt(), g2.toInt(), g3.toInt(), g4.toInt());
    IPAddress subnet(sb1.toInt(), sb2.toInt(), sb3.toInt(), sb4.toInt());
    IPAddress primaryDNS(p1.toInt(), p2.toInt(), p3.toInt(), p4.toInt()); //optional
    IPAddress secondaryDNS(s1.toInt(), s2.toInt(), s3.toInt(), s4.toInt()); //optional

    if (!WiFi.config(S_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
      Serial.println("STA Failed to configure");
    }

    WiFi.begin(wsid.c_str(), wpass.c_str());
    delay(1000);

    if (testWifi()) {
      Serial.print(WiFi.status());
      Serial.println("YOU ARE CONNECTED");
      LOCAL_IP = WiFi.localIP().toString();
      Serial.println(LOCAL_IP);
    }
  }

  File f = SPIFFS.open(s_data_file, "r");   //Open File for reading
  Serial.println(" ");
  Serial.println(" ");
  Serial.println("Reading Data first in loop from File:");

  if (!f) {
    Serial.println("Count file open failed on read.");
  }
  else
  {
    size_t size = f.size();
    std::unique_ptr<char[]> buf(new char[size]);
    f.readBytes(buf.get(), size);

    const size_t capacity = JSON_OBJECT_SIZE(11) + 240;
    DynamicJsonBuffer jsonBuffer(capacity);
    JsonObject& root  = jsonBuffer.parseObject(buf.get());

    Service = root["service"].as<String>();
    host_ip = root["host_ip"].as<String>();
    port = root["port"];
    uinterval = root["uinterval"];
    u_time = root["u_time"].as<String>(); //
    c_id = root["c_id"].as<String>(); // "Abcdefghijl"
    QOS = root["QOS"]; // 0
    U_name = root["U_name"].as<String>();
    s_pass = root["s_pass"].as<String>();
    p_topic = root["p_topic"].as<String>();
    Http_requestpath = root["Http_requestpath"].as<String>();

    Serial.println(Service);
    Serial.println(host_ip);
    Serial.println(port);
    Serial.println(p_topic);
    Serial.println(uinterval);
    Serial.println(u_time);
    Serial.println(c_id);
    Serial.println(QOS);
    Serial.println(U_name);
    Serial.println(s_pass);
    Serial.println(Http_requestpath);
  }
  f.close();

  if (Service == NULL) {
    service_s = "Not Set";
  }
  else {
    service_s = Service + "(SET)";
  }
  server.begin();

}

void loop() {

  if (WiFi.status() == WL_CONNECTED) {

    if (Service == "MQTT") {

//      Serial.println("MQTT selected");
//      Serial.print("BROKER:");
//      Serial.println(host_ip);
//      Serial.print("PORT:");
//      Serial.println(port);
//      Serial.print("MQTT TOPIC:");
//      Serial.println(p_topic);
//      Serial.print("MQTT USERNAME:");
//      Serial.println(U_name);
//      Serial.print("MQTT PASSWORD:");
//      Serial.println(s_pass);
//      Serial.print("MQTT UPLOAD :");
//      Serial.print(uinterval);
//      Serial.println(u_time);
//      Serial.print("MQTT CLIENTID :");
//      Serial.println(c_id);
//      Serial.print("MQTT QOS level :");
//      Serial.println(QOS);

      client.setServer(host_ip.c_str(), port );
      if ( !client.connected() ) {
        reconnect();
      }

      if ( millis() - lastSend > 1000 ) { // Update and send only after 1 seconds
        String payload = "{";
        payload += "\"temperature2\":"; payload += 0000; payload += ",";
        payload += "\"humidity2\":"; payload += 9999;
        payload += "}";

        char attributes[800];
        payload.toCharArray(attributes, 800 );
        client.publish(p_topic.c_str(), attributes );
        Serial.println( attributes );
        Serial.println("Data sent successfully");
        //        lastSend = millis();
        service_s = "MQTT(CONNECTED)";
      }
      lastSend = millis();
      client.loop();
    }

    if (Service == "HTTP") {

      HTTPClient http;
      Serial.println("HTTP selected");
      Serial.print("HOST:");
      Serial.println(host_ip);
      Serial.print("PORT:");
      Serial.println(port);
      Serial.print("HTTTP UPLOAD :");
      Serial.print(uinterval);
      Serial.println(u_time);
      Serial.print("HTTTP path :");
      Serial.println(Http_requestpath);

      String host = host_ip ;
      host.concat( ":" );
      host.concat( port );
      host.concat( "/" );
      host.concat( Http_requestpath );
      Serial.println(host);

      String payload = "{";
      payload += "\"temperature2\":"; payload += 0000; payload += ",";
      payload += "\"humidity2\":"; payload += 9999;
      payload += "}";

      // host_ip = "http://apps.atozinfoway.in:1805/v0.0.1/punchstatus";
          http.begin(host);  //Specify destination for HTTP request
          http.addHeader("Content-Type", "application/json");             //Specify content-type header
      
          int httpResponseCode = http.POST(payload);   //Send the actual POST request
          String response = http.getString();
      
          Serial.println(httpResponseCode);   //Print return code
          Serial.println(response);
          delay(3000);
          service_s = "HTTP(CONNECTED)";
          http.end();  //Free resources
    }

    if (Service == "TCP") {

      Serial.println("TCP selected");
      Serial.print("TCP HOST:");
      Serial.println(host_ip);
      Serial.print("TCP PORT:");
      Serial.println(port);
      Serial.print("TCP UPLOAD :");
      Serial.print(uinterval);
      Serial.println(u_time);
      Serial.print("TCP CLIENTID :");

      Serial.println(c_id);

      WiFiClient client;
      if (!client.connect(host_ip.c_str(), port)) {
        Serial.println("Connection to host failed");
        delay(1000);
        return;
      }
      Serial.println("Connected to server successful!");
      //data to tcp server
      String payload = "{";
      payload += "\"temperature2\":"; payload += 0000; payload += ",";
      payload += "\"humidity2\":"; payload += 9999;
      payload += "}";

      client.print(payload);
      service_s = "TCP(CONNECTED)";
      delay(4000);
    }
  }
  delay(5000);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    status = WiFi.status();
    if ( status != WL_CONNECTED) {
      WiFi.begin(wsid.c_str(), wpass.c_str());
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      Serial.println("Connected to AP");
    }
    Serial.print("Connecting to ThingsBoard node ...");
    // Attempt to connect (clientId, username, password)
    //,U_name.c_str(), s_pass.c_str()

    const char* cuname = NULL;
    if (U_name != "") {
      cuname  = U_name.c_str();
    }
    const char* cpass = NULL;
    if (s_pass != "") {
      cpass = s_pass.c_str();
    }

    if (client.connect(c_id.c_str(), cuname, cpass)) {
      Serial.println( "[DONE]" );

    } else {
      Serial.print( "[FAILED] [ rc = " );
      Serial.print( client.state() );
      Serial.println( " : retrying in 5 seconds]" );
      // Wait 5 seconds before retrying
      delay( 4000 );
    }
  }
}
