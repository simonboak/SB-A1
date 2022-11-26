uint8_t column_count = 0;
bool printer_enabled = false;

/*
 * Retrieves video (character) data from the PIA and sends it to the user over the serial connection.
 */
void process_video_data() {
  // Tell the PIA we are ready to receive data.
  digitalWrite(PIN_RDA, HIGH);
  delay(DELAY_VIDEO);

  while (digitalRead(PIN_DA)) {
    // Data is available.
    video_data = 0;

    // Decode the data bits to a byte.
    for (int i = 0; i < 7; i++) { 
      if (digitalRead(video_data_pins[i])) {
        video_data |= (1 << i);
      }
    }

    if (expecting_storage_data) {
      current_file.write((char)video_data);
    }
    
    send_to_video_terminal(video_data);

    // Done receiving this byte.
    digitalWrite(PIN_RDA, LOW);
    delay(1);
 
    // Tell the PIA we are ready to receive more data.
    digitalWrite(PIN_RDA, HIGH);
    delay(1);
  }
}


void send_to_video_terminal(int video_data) {
  // Handle printer control codes
  if (video_data == 17) {
    printer_enabled = true;
  } else if (video_data == 19) {
    printer_enabled = false;
  } else if (video_data == 13) {      
    // Carrage return.
    Serial.println();
    column_count = 0;

    if (printer_enabled) {
      printer.println();
    }
  } else if (video_data > 31) {
    // Display-compatible character.
    Serial.print((char)video_data);

    // Send carriage return if we're at 40 columns
    column_count++;
    if (column_count > 39) {
      Serial.println();
      column_count = 0;
    }
    
    // Now send it to the printer
    if (printer_enabled) {
      printer.print((char)video_data);
    }
  }
}
