#include "otrFileHandling.h"
#include "LilyGo_AMOLED.h"


extern LilyGo_Class amoled;
void initFileSystem(void) {
    bool littleFSMounted = LittleFS.begin();
    #ifdef OTR_DEBUG
       
        if (!littleFSMounted) {
            
            Serial.println("An Error has occurred while mounting LittleFS");
        } else {
            Serial.println("LittleFS Mounted");
            uint64_t littlefsSize = LittleFS.totalBytes() / 1024; //KB
            uint64_t littlefsUsed = LittleFS.usedBytes() / 1024; //KB
            Serial.printf(" LittleFS total: %d kb\r\n", littlefsSize);
            Serial.printf(" LittleFS used: %d kb\r\n", littlefsUsed);
        
        };
        
    #endif
  
}

void SD_init(void)  {
    //Note that the SD Card will already be mounted amoled.begin
    bool mounted = amoled.installSD();
    #ifdef OTR_DEBUG
        lv_label_set_recolor(ui_Main_Label_RFID, true);
        lv_label_set_text_fmt(ui_Main_Label_RFID, "SD Card \n%s", 
            mounted ? "#00ff00 Mounted" : "#ff0000 Not Mounted");
        if (!mounted) {
            Serial.println("SD Card Mount Failed");            
        } else {
            Serial.println("SD Card Mounted");
            uint32_t size = SD.cardSize() / (1024 * 1024);
            uint32_t totalBytes = SD.totalBytes() / (1024 * 1024);
            uint32_t usedBytes = SD.usedBytes() / (1024 * 1024);
            Serial.printf("Size: %d MB, Total: %d MB, Used: %d MB\n", 
            size, totalBytes, usedBytes);
        }
        
    #endif

}

void copyFileFromLittleFStoSD(const char* sourcePath, const char* destinationDirectory, const char* destinationFilename) {
    String destinationPath = String(destinationDirectory) + "/" + String(destinationFilename);

    File sourceFile = LittleFS.open(sourcePath, "r");
    if (!sourceFile) {
        Serial.println("Failed to open source file.");
        return;
    }

    if (!SD.exists(destinationDirectory)) {
        SD.mkdir(destinationDirectory);
    }

    File destinationFile = SD.open(destinationPath.c_str(), "w");
    if (!destinationFile) {
        Serial.println("Failed to create destination file.");
        sourceFile.close();
        return;
    }

    while (sourceFile.available()) {
        destinationFile.write(sourceFile.read());
    }

    sourceFile.close();
    destinationFile.close();
}

void moveFileFromLittleFStoSD(const char* sourcePath, const char* destinationDirectory, const char* destinationFilename) {
    String destinationPath = String(destinationDirectory) + "/" + String(destinationFilename);

    File sourceFile = LittleFS.open(sourcePath, "r");
    if (!sourceFile) {
        Serial.println("Failed to open source file.");
        return;
    }

    if (!SD.exists(destinationDirectory)) {
        SD.mkdir(destinationDirectory);
    }

    File destinationFile = SD.open(destinationPath.c_str(), "w");
    if (!destinationFile) {
        Serial.println("Failed to create destination file.");
        sourceFile.close();
        return;
    }

    while (sourceFile.available()) {
        destinationFile.write(sourceFile.read());
    }

    sourceFile.close();
    destinationFile.close();

    // Remove the source file after moving
    LittleFS.remove(sourcePath);
}

void printFileContents(String filePath) {
    File file = LittleFS.open(filePath, "r");
    if (!file) {
        Serial.println("Failed to open file.");
        return;
    }

    int lineNumber = 1;
    Serial.print(String(lineNumber) + ": ");
    while (file.available()) {
        char c = file.read();
        Serial.print(c);
        if (c == '\n') {
            lineNumber++;
            Serial.print(String(lineNumber) + ": ");
        }
    }
    Serial.println();

    file.close();
}

void getStorageInfo() {
    uint32_t size = SD.cardSize() / (1024 * 1024);
    uint32_t totalBytes = SD.totalBytes() / (1024 * 1024);
    uint32_t usedBytes = SD.usedBytes() / (1024 * 1024);
    Serial.printf(" SD card Size: %d MB, Total: %d MB, Used: %d MB\n", size, totalBytes, usedBytes);
    uint64_t psramSize = ESP.getPsramSize() / 1024; //KB
    uint64_t flashSize = ESP.getFlashChipSize() / 1024; //KB
    uint64_t littlefsSize = LittleFS.totalBytes() / 1024; //KB
    uint64_t littlefsUsed = LittleFS.usedBytes() / 1024; //KB
    Serial.printf(" psram size: %d kb\r\n", psramSize);
    Serial.printf(" FLASH size: %d kb\r\n", flashSize);
    Serial.printf(" LittleFS total: %d kb\r\n", littlefsSize);
    Serial.printf(" LittleFS used: %d kb\r\n", littlefsUsed);
}

void createExampleFile() {
  File file = SD.open("/example.txt", FILE_WRITE);
  if (file) {
    file.println("This is an example file.");
    file.println("You can add any content you want here.");
    file.close();
    Serial.println("Example file created.");
  } else {
    Serial.println("Failed to create example file.");
  }
}


String readLittleFSFileToString(const char *filename) {
  File file = LittleFS.open(filename, "r");
  if (file) {
    String contents;
    while (file.available()) {
      contents += file.readStringUntil('\n');
    }
    file.close();
    return contents;
  } else {
    return "";
  }
}

String readSDFileToString(const char *filename) {
  File file = SD.open(filename, "r");
  Serial.println("readSDFileToString: " + String(filename));
  if (file) {
    Serial.println("File opened");
    String html;
    while (file.available()) {
      html += file.readStringUntil('\n');
          }
    file.close();
    Serial.println(html);
    return html;
  } else {
    Serial.println("Failed to open file");
    return "";
  }
}

String listSDCardContents() {
  String treeListing = "SD card Contents:\n";
  treeListing += listDirSD("/", 0);
  return treeListing;
}
String listDirSD(const char *dirname, uint8_t currentIndent)
{
    String contentList = "";
    File root = SD.open(dirname);
    if (!root) {
        contentList += "Failed to open directory: ";
        contentList += dirname;
        contentList += "\n";
        return contentList;
    }
    if (!root.isDirectory()) {
        contentList += dirname;
        contentList += " is not a directory.\n";
        root.close();
        return contentList;
    }

    File file = root.openNextFile();
    while (file) {
        for (int i = 0; i < currentIndent; i++) {
            contentList += "  ";
        }
        contentList += file.name(); 
        // Get and format last write time
        time_t lastWriteTime = file.getLastWrite();
        struct tm *tm_info = localtime(&lastWriteTime);
        char timeStr[20];
        // Format: YYYY-MM-DD HH:MM:SS
        strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", tm_info);

        if (file.isDirectory()) {
            contentList += "/ (Modified: "; 
            contentList += timeStr;
            contentList += ")\n";
            // Recursively call for subdirectory, increasing indentation level
            contentList += listDirSD(file.path(), currentIndent + 1);
        } else {
            contentList += " (Size: ";
            contentList += file.size();
            contentList += " bytes, Modified: ";
            contentList += timeStr;
            contentList += ")\n";
        }
        file.close(); 
        file = root.openNextFile(); 
    }
    return contentList; 
}

String listLittleFSContents() {
  String treeListing = "LittleFS File Structure:\n";
  treeListing += listDirLittleFS("/", 0);
  return treeListing;
}
String listDirLittleFS(const char *dirname, uint8_t currentIndent)
{
    String contentList = "";
    File root = LittleFS.open(dirname);
    if (!root) {
        contentList += "Failed to open directory: ";
        contentList += dirname;
        contentList += "\n";
        return contentList;
    }
    if (!root.isDirectory()) {
        contentList += dirname;
        contentList += " is not a directory.\n";
        root.close();
        return contentList;
    }

    File file = root.openNextFile();
    while (file) {
        for (int i = 0; i < currentIndent; i++) {
            contentList += "  ";
        }
        contentList += file.name(); 
        // Get and format last write time
        time_t lastWriteTime = file.getLastWrite();
        struct tm *tm_info = localtime(&lastWriteTime);
        char timeStr[20];
        // Format: YYYY-MM-DD HH:MM:SS
        strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", tm_info);

        if (file.isDirectory()) {
            contentList += "/ (Modified: "; 
            contentList += timeStr;
            contentList += ")\n";
            // Recursively call for subdirectory, increasing indentation level
            contentList += listDirLittleFS(file.path(), currentIndent + 1);
        } else {
            contentList += " (Size: ";
            contentList += file.size();
            contentList += " bytes, Modified: ";
            contentList += timeStr;
            contentList += ")\n";
        }
        file.close(); 
        file = root.openNextFile(); 
    }
    return contentList; 
}

bool moveFileSD(const char* sourcePath, const char* destinationPath) {
  // 1. Check if SD card is mounted
  if (!SD.begin()) {
    Serial.println("SD card not mounted");
    return false; // Return false to indicate failure
  }

  // 2. Check if the source file exists
  if (!SD.exists(sourcePath)) {
    Serial.print("Source file does not exist: ");
    Serial.println(sourcePath);
    return false; // Return false if the source file does not exist
  }

  // 3. Extract destination directory and filename
    String destDir = "";
    String destFilename = "";
    String temp = destinationPath;
    int lastSlash = temp.lastIndexOf('/');
    if (lastSlash > 0) {
        destDir = temp.substring(0, lastSlash);
        destFilename = temp.substring(lastSlash + 1);
    } else {
        destFilename = destinationPath;
    }

  // 4. Create the destination directory if it doesn't exist
  if (destDir.length() > 0 && !SD.exists(destDir.c_str())) {
      if (!SD.mkdir(destDir.c_str())) {
          Serial.print("Failed to create destination directory: ");
          Serial.println(destDir);
          return false; // Return false if directory creation fails
      }
  }
  // 5. Check if the destination file already exists
  if (SD.exists(destinationPath)) {
    SD.remove(destinationPath);  //remove the destination file.
  }

  // 6. Rename the source file to the destination path (move operation)
  if (SD.rename(sourcePath, destinationPath)) {
    Serial.print("File moved successfully from ");
    Serial.print(sourcePath);
    Serial.print(" to ");
    Serial.println(destinationPath);
    return true; // Return true to indicate success
  } else {
    Serial.print("Failed to move file from ");
    Serial.print(sourcePath);
    Serial.print(" to ");
    Serial.println(destinationPath);
    return false; // Return false if the rename (move) operation failed
  }
}

String fileSystemStatistics() {
        uint32_t size = SD.cardSize() / (1024 * 1024);
    uint32_t totalBytes = SD.totalBytes() / (1024 * 1024);
    uint32_t usedBytes = SD.usedBytes() / (1024 * 1024);
    Serial.printf(" SD card Size: %d MB, Total: %d MB, Used: %d MB\n", size, totalBytes, usedBytes);
    uint64_t psramSize = ESP.getPsramSize() / 1024; //KB
    uint64_t flashSize = ESP.getFlashChipSize() / 1024; //KB
    uint64_t littlefsSize = LittleFS.totalBytes() / 1024; //KB
    uint64_t littlefsUsed = LittleFS.usedBytes() / 1024; //KB
    Serial.printf(" psram size: %d kb\r\n", psramSize);
    Serial.printf(" FLASH size: %d kb\r\n", flashSize);
    Serial.printf(" LittleFS total: %d kb\r\n", littlefsSize);
    Serial.printf(" LittleFS used: %d kb\r\n", littlefsUsed);
    String stats = "SD Card:\n";
    stats += " Card Size: " + String(SD.cardSize() / (1024 * 1024)) + " Mb\n";
    stats += " Total Space: " + String(SD.totalBytes() / (1024 * 1024)) + " Mb\n";
    stats += " Used Space: " + String(SD.usedBytes()/(1024 * 1024)) + " Mb\n";
    stats += " Free Space: " + String((SD.totalBytes() - SD.usedBytes()) / (1024 * 1024)) + " Mb\n";
    stats += "Internal Storage:\n";
    stats += " Total Space: " + String(LittleFS.totalBytes() / (1024)) + " kb\n";
    stats += " Used Space: " + String(LittleFS.usedBytes() / (1024)) + " kb\n";
    stats += "PSRAM: " + String(ESP.getPsramSize() / 1024) + " kb\n";
    stats += "Flash: " + String(ESP.getFlashChipSize() / 1024) + " kb\n";
    Serial.printf(" psram size: %d kb\r\n", psramSize);
    Serial.printf(" FLASH size: %d kb\r\n", flashSize);
    Serial.printf(" LittleFS total: %d kb\r\n", littlefsSize);
    Serial.printf(" LittleFS used: %d kb\r\n", littlefsUsed);
    return stats;
}