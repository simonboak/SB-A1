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

          // Set bit 7 for the status LED
          byte_from_file = byte_from_file | 128;
          
          digitalWrite(SR_LATCH_PIN, LOW);
          shiftOut(SR_DATA_PIN, SR_CLOCK_PIN, MSBFIRST, byte_from_file);
          digitalWrite(SR_LATCH_PIN, HIGH);
    
          // Pulse the STROBE bit so the PIA will process the input.
          digitalWrite(PIN_STROBE, HIGH);
          delay(DELAY_ASCII);
          digitalWrite(PIN_STROBE, LOW);
          
          process_video_data();
  
          if (
            (byte_from_file == 141) || // CR with bit 7 set
            (byte_from_file == 138) // LF with bit 7 set
          ) {
            delay(400); // Bigger delays for CR or LF chars allow the Apple to parse the line (needed most for BASIC)
          } else {
            delay(10); // Tiny delay to allow for catchup
          }
        }
      }
      data_file.close();
      is_loading_file = false;
      clear_status_led();
    } else {
      Serial.println("ERROR: FILE '" + file_to_load + "' NOT FOUND");
      is_loading_file = false;
    }
  } else {
    // Wait for serial data coming in.
    while (Serial.available() > 0)
    {
      keyboard_data = Serial.read();

      if (keyboard_data == 1) { // Ctrl + A = 1
        
        
        if (is_saving_file) {
          // Stop saving to the file
          expecting_storage_data = false;
          is_saving_file = false;
          current_file.close();
          Serial.println("");
          Serial.println("[CLOSED '" + file_to_save + "']");
          clear_status_led();
        } else {
          // Open storage command
          Serial.print("[? ");
          storage_command = "";
          receiving_storage_command = true;
        }
      } else if (receiving_storage_command) {
        if (keyboard_data == 13) {
          // CR ends command
          Serial.println("]");
          //Serial.print("[CMD DEBUG: ");
          //Serial.print(storage_command);
          //Serial.println("]");
          receiving_storage_command = false;
          parse_storage_command();
        } else {
          Serial.print((char)keyboard_data);
          storage_command += String((char)keyboard_data);
        }
      } else {
        // Otherwise carry on sending to the Apple
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
          // Set the status LED during file read and write modes
          if ( (is_loading_file) || (is_saving_file)) {
            keyboard_data = keyboard_data | B10000000;
          }

          // Shift out any Apple 1 compatible character to the PIA.
          digitalWrite(SR_LATCH_PIN, LOW);
          shiftOut(SR_DATA_PIN, SR_CLOCK_PIN, MSBFIRST, keyboard_data);
          digitalWrite(SR_LATCH_PIN, HIGH);
    
          // Pulse the STROBE bit so the PIA will process the input.
          digitalWrite(PIN_STROBE, HIGH);
          delay(1);
          digitalWrite(PIN_STROBE, LOW);
        }
      }
      
      process_video_data();
    }
  }

  
    
}
