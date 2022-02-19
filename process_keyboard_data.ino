uint8_t keyboard_data;



/*
 * Processes data received from the serial connection.
 * Puts the received data on to the PIA where the CPU will read it from.
 */
void process_keyboard_data()
{
  if (is_loading_file) {
    File data_file = SD.open(file_to_load);
    
    if (data_file) {
      while (data_file.available()) {
        byte byte_from_file = data_file.read();
  
        if (byte_from_file > -1) {
          if (byte_from_file == 10) {
            byte_from_file = 13;
          }
          
          digitalWrite(SR_LATCH_PIN, LOW);
          shiftOut(SR_DATA_PIN, SR_CLOCK_PIN, MSBFIRST, byte_from_file);
          digitalWrite(SR_LATCH_PIN, HIGH);
    
          // Pulse the STROBE bit so the PIA will process the input.
          digitalWrite(PIN_STROBE, HIGH);
          delay(DELAY_ASCII);
          digitalWrite(PIN_STROBE, LOW);
          
          process_video_data();
  
          delay(5);
        }
      }
      data_file.close();
      is_loading_file = false;
    } else {
      Serial.println("ERROR: FILE '" + file_to_load + "' NOT FOUND");
      is_loading_file = false;
    }
  
   
  } else {
    // Wait for serial data coming in.
    while (Serial.available() > 0)
    {
      keyboard_data = Serial.read();
  
      if (keyboard_data == 203) 
      {
        // Translate uppercase ESC to normal ESC.
        keyboard_data = 27;
      }
  
      if (keyboard_data >= 97 && keyboard_data <= 122)
      {
        // Translate lowercase characters to uppercase as the Apple 1 only supports uppercase.
        keyboard_data -= 32;
      }
  
      // Transform line feed character into carriage return, e.g. when pasting a file via minicom
      if (keyboard_data == 10)
      {
        keyboard_data = 13;
      }

      /*if ( keyboard_data == 88 ) { // X
        is_loading_file = true;
        process_video_data();
        continue;
      }*/
  
      if (keyboard_data < 96)   
      {
        // Shift out any Apple 1 compatible character to the PIA.
        digitalWrite(SR_LATCH_PIN, LOW);
        shiftOut(SR_DATA_PIN, SR_CLOCK_PIN, MSBFIRST, keyboard_data);
        digitalWrite(SR_LATCH_PIN, HIGH);
  
        // Pulse the STROBE bit so the PIA will process the input.
        digitalWrite(PIN_STROBE, HIGH);
        delay(1);
        digitalWrite(PIN_STROBE, LOW);
      }
  
      process_video_data();
    }
  }

  
    
}
