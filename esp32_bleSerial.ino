#include <esp_now.h>
#include <WiFi.h>
HardwareSerial mySerial(1);
#define TX2 17
#define RX2 16
void clear();
uint8_t broadcastAddress[] = {0x40, 0x91, 0x51, 0xBE, 0xF6, 0xD8};
unsigned long lastMillis = 0;
typedef struct struct_message {
  char a[32];
} struct_message;
struct_message myData;

esp_now_peer_info_t peerInfo;

bool clr_ngo = false;
char buf[5];
int32_t mqtt;
int i = 0;
void clear();
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
 

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
mySerial.begin(9600, SERIAL_8N1, TX2, RX2);
  WiFi.mode(WIFI_STA);
   if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
   // publish a message roughly every second.
 
  char ch;
  Serial.println("Program start.");
  while(true){
    if(mySerial.available()>0){
      mySerial.read(&ch,1);
      switch(ch){
        case 'A':
        clr_ngo = true;
        break;

        case 'B':
        clr_ngo = false;
        clear();        
        break;

        default:
        if(clr_ngo == true){
        buf[i] = ch;
        //Serial.println(buf[i]);
        i++;
        }
        if(i>5){
          int sp = atoi(buf);
          if(millis() - lastMillis > 200){
            lastMillis = millis();
            strcpy(myData.a, buf);
            esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
            Serial.println(sp);
          }
          i = 0;
        }
          
  }

      }
    }
  }

  void clear(){
    while(i!=0){
      buf[i]=0;      
      i--;
    }
  }
  
 
