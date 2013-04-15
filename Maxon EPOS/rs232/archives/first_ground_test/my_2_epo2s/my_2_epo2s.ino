/*
  Last edited - 10 Apr 2013 - 23:10 AJK
*/
#include <my_epos2.h>

int t;
EPOS2 e;
EPOS2 e2;

void setup() {
  Serial.begin(115200);
  Serial.println("Begun");
  e.init(&Serial1, 7);
  e2.init(&Serial2, 3, CLOCKWISE_ROTATION);
}

void loop() {
  int32_t t;
  Serial.println("going to 30k ...");
  e.position(50000, 2100);
  e2.position(50000, 2100);
  e.enable();
  e2.enable();
  uint16_t w = 0x0F;
  //e.velocity();
  //e2.velocity();
//  Serial.println(do_together(get_frame(WRITE_OPCODE, 2, 0x6040, 0x00, 0x000F, 0)));
    Serial.println(do_together(get_frame(WRITE_OPCODE, 2, 0x6040, 0x00, 0x007F, 0)));
    
  delay(5000);
  e.velocity_stop();
  e2.velocity_stop();
  Serial.println("done");
  while(1);
}

int do_together(epos2_frame f) {
  epos2_frame f1 = f,
              f2 = f;
  int counter_t = 0;
  char c, c2;
  f1.data[1] = (e.nodeId<<8) + (f1.data[1] & 0x00ff);
  f2.data[1] = (e2.nodeId<<8) + (f2.data[1] & 0x00ff);

  e2.set_crc(&f2);
  e.set_crc(&f1);
  do {
    do {
      do {
        if(counter_t++ > 5)
          return 3;
        do {  
          if(counter_t++ > 5)
            return 3;
          e.serial_flush();
          e.serial_->write(f1.opcode);
        } while(e.wait_for_serial());
        e2.serial_flush();
        e2.serial_->write(f2.opcode);
      } while(e2.wait_for_serial());
      
      c = e.serial_->read();
      c2 = e2.serial_->read();
      Serial.print(c2);
      Serial.print(" -- ");
      Serial.print(c); 
      if( c == 'O' && c2 == 'O') {
        e.serial_->write(f1.len);
        e2.serial_->write(f2.len);
        int i = 0;
        for( i = 0; i <= f1.len; i++ ) {
          e.serial_->write(f1.data[i]);
          e.serial_->write(f1.data[i]>>8);
          e2.serial_->write(f2.data[i]);
          e2.serial_->write(f2.data[i]>>8);
        }
        e2.serial_->write(f2.crc);
        e.serial_->write(f1.crc);
        e.serial_flush(); // flush it so after sending don't get garbage values
        e2.serial_flush(); // flush it so after sending don't get garbage values
        e2.serial_->write(f2.crc>>8);
      }
    } while(e2.wait_for_serial());
    e.serial_->write(f1.crc>>8);
  } while(e.wait_for_serial());
  if(c == 'O' && c2 == 'O') {
    c = e.serial_->read();
    c2 = e2.serial_->read();
    if( c2 == 'O' && c == 'O' ) {
      return 0;
    } else if( c2 == 'F' || c == 'F' ) {
  Serial.print(c2);
  Serial.print(" -- ");
  Serial.print(c);  
      return 2;
      
    } else {
      return -2;
    }   
    return 0;
  } else if( c == 'F' || c2 == 'F' ) {
    return 1;
  } else {
    return -1;
  }
}
