#ifndef serialtools_h
#define serialtools_h
uint8_t* buff = new uint8_t[128];
void writePacketStream(uint8_t* data, size_t len){
  //get number of packets needed to send data plus one if it doesn't fit evenly
  int packets = len/16 + (len%16 == 0 ? 0 : 1);
  //make sure we don't leave data in there
  memset(buff, 0, 128);
  memcpy(buff, data, len);
  for(int i = 0; i < packets; i++){
    //copy 16 byte chunks and write
    Serial.write((buff+i*16), 16);
    Serial.flush();
//    //wait for confirm bit
//    while(Serial.available() < 0);
//    //read single confirm bit
//    Serial.read();
  }
}
void readPacketStream(size_t len){
  int packets = len/16 + (len%16 == 0 ? 0 : 1);
  for(int i = 0; i < packets; i++){
    //read 16 byte chunk
    Serial.readBytes((buff+i*16), 16);
    //write confirm bit
    Serial.write(1);
  }
}
void sendExpect(expect ex){
 expectSerialized exs = ex;
 Serial.write(exs.bytes, sizeof(expect));
 Serial.flush();
}
void readExpect(boolean& flag, expectSerialized &ex){
  if(!flag){
    if(Serial.available() >= sizeof(expect)){
      Serial.readBytes(ex.bytes, sizeof(expect));
    }else{
      return ;
    }
    flag = true;
  }
}
#endif
