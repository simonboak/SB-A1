

void parse_storage_command() {
  Serial.println("");
  Serial.println("---");
  Serial.println(storage_command);
  Serial.println("---");
        
  switch (storage_command.charAt(0)) {
    case 'C':
      // list file Catalog
      root = SD.open("/");
      print_file_catalog(root, false);
      root.close();
      break;
    case 'Q':
      // list file Catalog
      root = SD.open("/");
      print_file_catalog(root, true);
      root.close();
      break;
    case 'V':
      file_to_load = storage_command;
      file_to_load.remove(0,1);
      send_file_to_video(file_to_load);
      break;
    case 'P':
      file_to_load = storage_command;
      file_to_load.remove(0,1);
      send_file_to_printer(file_to_load);
      break;
    case 'B':
      // load the Boot file
      file_to_load = "BOOT.MON";
      is_loading_file = true;
      break;
    case 'L':
      // Load the requested file
      file_to_load = storage_command;
      file_to_load.remove(0,1); // trim the L from command string to get filename
      is_loading_file = true;
      break;
    case 'W':
      // Save the upcoming data to this file
      file_to_save = storage_command;
      file_to_save.remove(0,1); // trim the W from command string to get filename
      is_saving_file = true;
      expecting_storage_data = true;
      current_file = SD.open(file_to_save, FILE_WRITE);
      break;
    case 'R':
      // Remove the file
      storage_command.remove(0,1); // trim the R from command string to get filename
      SD.remove(storage_command);
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


void print_file_catalog(File dir, bool send_to_printer) {
  if (send_to_printer) {
    printer.println("");
    printer.println("");
    printer.println(" NAME:                SIZE:");
    printer.println(" -----                -----");
    printer.println("");
  } else {
    Serial.println("");
    Serial.println("");
    Serial.println(" NAME:                SIZE:");
    Serial.println(" -----                -----");
    Serial.println("");
  }
  
  int file_count = 0;
  
  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }

    if (send_to_printer) {
      printer.print(" " + String(entry.name()) + " ");
    } else {
      Serial.print(" " + String(entry.name()) + " ");
    }
    
    if (!entry.isDirectory()) {
      // files have sizes, directories do not

      int tabAmount = 20 - String(entry.name()).length();

      if (tabAmount > 0)
      for (int i=tabAmount; i>0; i--) {
        if (send_to_printer) {
          printer.print(" "); 
        } else {
          Serial.print(" ");
        }
      }

      if (send_to_printer) {
        printer.println(entry.size(), DEC);
      } else {
        Serial.println(entry.size(), DEC);
      }
      

      file_count++;
    }
    entry.close();
  }


  if (send_to_printer) {
    printer.println("");
    printer.println("");
    printer.println("TOTAL FILES: " + String(file_count));
    printer.println("");
  } else {
    Serial.println("");
    Serial.println("");
    Serial.println("TOTAL FILES: " + String(file_count));
    Serial.println("");
  }
}

void send_file_to_video(String file_name) {
  File data_file = SD.open(file_name);
    
  if (data_file) {
    Serial.println("");
    Serial.println(file_name + ":");
    Serial.println("");
    while (data_file.available()) {
      byte byte_from_file = data_file.read();
      Serial.print((char)byte_from_file);
    }
    data_file.close();
  }

  Serial.println("");
}

void send_file_to_printer(String file_name) {
  File data_file = SD.open(file_name);
    
  if (data_file) {
    printer.println("");
    printer.println(file_name + ":");
    printer.println("");
    while (data_file.available()) {
      byte byte_from_file = data_file.read();
      printer.print((char)byte_from_file);
    }
    data_file.close();
  }

  printer.println("");
}
