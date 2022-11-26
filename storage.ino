

void parse_storage_command() {

  

  if ( storage_command.substring(0, 3) == "CAT" ) {
    root = SD.open("/");
    print_file_catalog(root, false);
    root.close();
  } else if ( storage_command.substring(0, 4) == "LIST" ) {
    file_to_load = storage_command.substring(5);
    list_file(file_to_load);
  } else if ( storage_command.substring(0, 4) == "LOAD" ) {
    file_to_load = storage_command.substring(5);
    is_loading_file = true;
  } else if ( storage_command.substring(0, 4) == "SAVE" ) {
    file_to_save = storage_command.substring(5);
    is_saving_file = true;
    expecting_storage_data = true;
    set_status_led();
    SD.remove(file_to_save);
    current_file = SD.open(file_to_save, FILE_WRITE);
  } else if ( storage_command.substring(0, 6) == "APPEND" ) {
    file_to_save = storage_command.substring(7);
    is_saving_file = true;
    expecting_storage_data = true;
    set_status_led();
    current_file = SD.open(file_to_save, FILE_WRITE);
  } else if ( storage_command.substring(0, 6) == "DELETE" ) {
    set_status_led();
    String file_to_delete = storage_command.substring(7);
    if (SD.remove(file_to_delete)) {
      Serial.println("['" + file_to_delete + "' DELETED]");
    } else {
      Serial.println("[DELETE FAILED FOR '" + file_to_delete + "' ]");
    }
    clear_status_led();
  /*} else if ( storage_command.substring(0, 4) == "COPY" ) {
    if (storage_command.length() > 7) { // Min command length for a copy command would be 8 chars, e.g. "COPY A B"
      copy_file(storage_command, false);
    } else {
      Serial.println("[COMMAND ERROR]");
    }
  } else if ( storage_command.substring(0, 4) == "MOVE" ) {
    if (storage_command.length() > 7) { // Min command length for a move command would be 8 chars, e.g. "MOVE A B"
      copy_file(storage_command, true);  // Move is just a copy but we delete the original
    } else {
      Serial.println("[COMMAND ERROR]");
    }*/
  } else {
    Serial.println("[COMMAND ERROR]");
  }

  

}



/*void copy_file(String storage_command, bool delete_original) {
  String source_filename = "";
  String destination_filename = "";
  int destination_filename_index = 0;

  Serial.println(storage_command);

  // First pull out the source filename
  for (int i = 5; i < storage_command.length(); i++) {
    String command_char = storage_command.substring(i, i+1);

    if (command_char == " ") {
      destination_filename_index = i + 1;
      break;
    } else {
      source_filename = source_filename + command_char;
    }
  }

  // Now we know where to find the destination filename
  for (int i = destination_filename_index; i < storage_command.length(); i++) {
    String command_char = storage_command.substring(i, i+1);
    destination_filename = destination_filename + command_char;
  }

  if (destination_filename_index == 0) {
    Serial.println("[NO DESTINATION FILENAME");
    return;
  }

  Serial.println("[" + source_filename + "]");
  Serial.println("[" + destination_filename + "]");

  if (SD.exists(source_filename)) {
    Serial.println("[SOURCE FILE FOUND]");
    File source_file = SD.open(source_filename, FILE_READ);
    File destination_file = SD.open(destination_filename, FILE_WRITE);
    //byte data;
    Serial.println("[FILES OPEN - PROCEEDING WITH COPY]");
    Serial.println(source_filename);
    Serial.println(destination_filename);
 
    while (source_file.available()) {
      byte byte_from_file = source_file.read();
      Serial.println((char)byte_from_file);
      destination_file.write(byte_from_file);
    }
    source_file.close();
    destination_file.close();
    Serial.println("[COPY COMPLETE]");
  } else {
    Serial.println("[SOURCE FILE NOT FOUND]");
  }
}
*/


/*void send_file_to_apple(String file_name) {
  set_status_led();

  File data_file = SD.open(file_name);
  
  if (data_file) {
    while (data_file.available()) {
      byte byte_from_file = data_file.read();

      if (byte_from_file > -1) {
        if (byte_from_file == 10) {
          byte_from_file = 13;
        }

        // Set bit 7 for the statis LED
        byte_from_file = byte_from_file | 128;
        
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
    Serial.println("[ERROR: '" + file_name + "' NOT FOUND]");
  }

  clear_status_led();
}*/


void print_file_catalog(File dir, bool send_to_printer) {
  set_status_led();

  if (send_to_printer) {
    printer.println("");
    printer.println("");
    printer.println(" NAME:          SIZE:");
    printer.println(" -----          -----");
    printer.println("");
  } else {
    Serial.println("");
    Serial.println("");
    Serial.println(" NAME:          SIZE:");
    Serial.println(" -----          -----");
    Serial.println("");
  }
  
  int file_count = 0;
  
  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }

    // Check for filenames to skip listing
    if (
      (String(entry.name()).substring(0, 1) == "_") ||
      (String(entry.name()).substring(0, 1) == ".") ||
      (String(entry.name()).substring(0, 5) == "TRASH") ||
      (String(entry.name()).substring(0, 4) == "TEMP") ||
      (String(entry.name()).substring(0, 6) == "FSEVEN")
    ) {
      entry.close();
      continue;      
    }

    if (send_to_printer) {
      printer.print(" " + String(entry.name()) + " ");
    } else {
      Serial.print(" " + String(entry.name()) + " ");
    }
    
    if (!entry.isDirectory()) {
      // files have sizes, directories do not

      int tabAmount = 14 - String(entry.name()).length();

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

  clear_status_led();
}


void send_file_to_printer(String file_name) {
  set_status_led();
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
  } else {
    Serial.println("[FILE '" + file_name + "' NOT FOUND");
  }

  printer.println("");

  data_file.close();
  clear_status_led();
}


void list_file(String file_name) {
  set_status_led();
  File data_file = SD.open(file_name);
    
  if (data_file) {
    while (data_file.available()) {
      byte byte_from_file = data_file.read();
      Serial.print((char)byte_from_file);

      if (byte_from_file == 13) { // Send LF if we find a CR
        Serial.print((char)10);
      }
    }
    data_file.close();
    Serial.println("");
  } else {
    Serial.println("[FILE '" + file_name + "' NOT FOUND");
  }

  data_file.close();
  clear_status_led();
}



void set_status_led() {
  digitalWrite(SR_LATCH_PIN, LOW);
  shiftOut(SR_DATA_PIN, SR_CLOCK_PIN, MSBFIRST, 128);
  digitalWrite(SR_LATCH_PIN, HIGH);
}

void clear_status_led() {
  digitalWrite(SR_LATCH_PIN, LOW);
  shiftOut(SR_DATA_PIN, SR_CLOCK_PIN, MSBFIRST, 0);
  digitalWrite(SR_LATCH_PIN, HIGH);
}
