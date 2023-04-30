// #include "visca.h"
// #include "../main.h"
// #include "../config.h"

// void ViscaHandle(uint8_t *buf, size_t len){
//     uint8_t modified[18];
//     size_t lastelement = 0;

//     if ((buf[0] == 0x01 && buf[1] == 0x00 && buf[2] == 0x00)) { // && buf[3] == 0x09) || (buf[0] == 0x01 && buf[1] == 0x00 && buf[2] == 0x00 && buf[3] == 0x06)) {
//         int j = 0;
//         for (int i = 8; (i < len && i < 18); i++)
//         {
//             modified[j] = buf[i];
//             lastelement = j;
//             j++;
//         }
//     } else {
//         for(int i = 0; (i < len && i < 18); i++)
//         {
//             modified[i] = buf[i];
//             lastelement = i;
//         }
//     }

//   // is this a PTZ?
//   if (modified[1] == 0x01 && modified[2] == 0x06 && modified[3] == 0x01){
//     Serial.println(F("| PTZ CONTROL DETECTED"));
//   }

//   // is this ZOOM?
//   if (modified[1] == 0x01 && modified[2] == 0x04 && modified[3] == 0x07){
//     Serial.println(F("| ZOOM CONTROL DETECTED"));
//   }

//   if (modified[1] == 0x01 && modified[2] == 0x04 && modified[3] == 0x08){
//     Serial.println(F("| FOCUS CONTROL DETECTED"));
//   }
// #ifdef VISCADEBUG
//     viscaSerial.write(modified, lastelement + 1);
//     Serial.println(F("| VISCA IP: Send ACK"));
// #endif
//     UDP.writeTo(modified, lastelement + 1, Var.lastclientip, Var.lastclientport);
// }


// void ViscaStart(void){
// #ifdef VISCADEBUG
//     Serial.println(F("+--------------------+"));
//     Serial.println(F("|    VISCA server    |"));
//     Serial.println(F("+--------------------+"));
//     Serial.print(F("| starting on UDP port "));
//     Serial.println(setting.viscaport);
// #endif
//     UDP.close(); // will close only if needed
//     if (UDP.listen(setting.viscaport))
//     {
//         // udpstate = 1;
//         #ifdef VISCADEBUG
//         Serial.println(F("| Server is Running!"));
//         #endif
//         UDP.onPacket([](AsyncUDPPacket packet) {
//             // debug(packet);
//             Var.lastclientip = packet.remoteIP();
//             Var.lastclientport = packet.remotePort();
//             #ifdef VISCADEBUG
//             Serial.print(F("| Type of UDP datagram: "));
//             Serial.print(packet.isBroadcast() ? "Broadcast" : packet.isMulticast() ? "Multicast"
//                             : "Unicast");
//             Serial.print(F(", Sender: "));
//             Serial.print(lastclientip);
//             Serial.print(F(":"));
//             Serial.print(lastclientport);
//             Serial.print(F(", Receiver: "));
//             Serial.print(packet.localIP());
//             Serial.print(F(":"));
//             Serial.print(packet.localPort());
//             Serial.print(F(", Message length: "));
//             Serial.print(packet.length());
//             Serial.print(F(" Payload (hex):"));
//             debug(packet.data(), packet.length());
//             Serial.println(F(""));
//             #endif
//             ViscaHandle(packet.data(), packet.length());
//         });
//     }
//     else{
//         // udpstate = 0;
//         #ifdef VISCADEBUG
//         Serial.println(F("| Server failed to start"));
//         #endif
//     }
//     #ifdef VISCADEBUG
//     Serial.println(F("+---------------------"));
//     Serial.println(F(""));
//     #endif
// }