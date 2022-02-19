

void parse_storage_command() {
  switch (storage_command.charAt(0)) {
    case 'C':
      // print file Catalog
      root = SD.open("/");
      print_file_catalog(root);
      root.close();
      break;
    case 'B':
      // load the Boot file
      file_to_load = "BOOT.MON";
      is_loading_file = true;
      break;
    case 'L':
      // Load the requested file
      file_to_load = "BOOT.MON";
      is_loading_file = true;
      break;
  }
}



/*void send_to_storage_controller(int video_data) {

  load_mon_file("BOOT.MON");

  
  expecting_storage_command = false;
}*/







void load_mon_file(String file_name) {
  File data_file = SD.open(file_name);
  
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
  } else {
    Serial.println("ERROR: '" + file_name + "' NOT FOUND");
  }
}


void print_file_catalog(File dir) {
  Serial.println("");
  Serial.println("");
  Serial.println(" NAME:                SIZE:");
  Serial.println(" -----                -----");
  Serial.println("");

  
  int file_count = 0;
  
  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    
    Serial.print(" " + String(entry.name()) + " ");
    
    if (!entry.isDirectory()) {
      // files have sizes, directories do not

      int tabAmount = 20 - String(entry.name()).length();

      if (tabAmount > 0)
      for (int i=tabAmount; i>0; i--) {
        Serial.print(" ");
      }
      
      Serial.println(entry.size(), DEC);

      file_count++;
    }
    entry.close();
  }

  Serial.println("");
  Serial.println("");
  Serial.println("TOTAL FILES: " + String(file_count));
  Serial.println("");
}
