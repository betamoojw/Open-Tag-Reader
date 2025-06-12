#include "otrFileHandling.h"
#include "LilyGo_AMOLED.h"


extern LilyGo_Class amoled;
void initFileSystem(void) {
    bool littleFSMounted = LittleFS.begin();
    #ifdef OTR_DEBUG
       
        if (!littleFSMounted) {
            
            Serial.println("An Error has occurred while mounting LittleFS"); delay(1000);
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
        lv_label_set_recolor(ui_Main_Label_Testing, true);
        lv_label_set_text_fmt(ui_Main_Label_Testing, "SD Card \n%s", 
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

void appendNewLine(const char* filePath, char** bucketRow) {
    File file = LittleFS.open(filePath, "a");
    if (!file) {
    Serial.println("Failed to open file for appending.");
    return;
    }
    file.print(bucketRow[0]);
    file.print(",");
    file.print(bucketRow[1]);
    file.print(",");
    file.print(bucketRow[2]);
    file.print(",");
    file.print(bucketRow[3]);
    file.print(",");
    file.print(bucketRow[4]);
    file.print(",\"0\",\"0\",\"\",\"0\",\"0\""); // add placeholders for gender, status, name, group and location
    file.println(); // Write the line with a newline character
    file.close();
}

// String create_new_list() {
//     // //DateTime now = rtc.now();

//     // String baseName = "-scans.csv";
//     // //String currentDate = String(now.year()) + "-" + String(now.month()) + "-" + String(now.day());
//     // String fileName = "/" + currentDate + baseName;
//     // int counter = 1;

//     // while (LittleFS.exists(fileName.c_str())) {
//     //     fileName = "/" + currentDate + baseName.substring(0, baseName.indexOf('.')) + "-" + String(counter) + ".csv";
//     //     counter++;
//     // }

//     // File file = LittleFS.open(fileName.c_str(), "w");
//     // if (file) {
//     //     // Add content to the file if needed
//     //     file.close();
//     // }
    
//     // return fileName;
// }



String readLittleFSFileToString(const char *filename) {
  File file = LittleFS.open(filename, "r");
  if (file) {
    String html;
    while (file.available()) {
      html += file.readStringUntil('\n');
    }
    file.close();
    return html;
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
  treeListing += listDir(SD, "/", 0);
  return treeListing;
}
String listDir(fs::FS &fs, const char *dirname, uint8_t currentIndent)
{
    String contentList = "";
    return contentList;
    File root = fs.open(dirname);
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
    // Add indentation for the current item
    for (int i = 0; i < currentIndent; i++) {
      contentList += "  "; // Two spaces per indentation level
    }

    contentList += file.name(); // Just the name, no leading dash

    // Get and format last write time
    time_t lastWriteTime = file.getLastWrite();
    struct tm *tm_info = localtime(&lastWriteTime);
    char timeStr[20];
    // Format: YYYY-MM-DD HH:MM:SS
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", tm_info);

    if (file.isDirectory()) {
      contentList += "/ (Modified: "; // Indicate a directory with a slash
      contentList += timeStr;
      contentList += ")\n";
      // Recursively call for subdirectory, increasing indentation level
      // Note: The original 'levels' parameter is replaced by 'currentIndent'
      contentList += listDir(fs, file.path(), currentIndent + 1);
    } else {
      contentList += " (Size: ";
      contentList += file.size();
      contentList += " bytes, Modified: ";
      contentList += timeStr;
      contentList += ")\n";
    }
    file.close(); // Crucially, close the file/directory after processing
    file = root.openNextFile(); // Move to the next item
  }
  root.close(); // Close the current directory after listing all its contents
}

void createDir(fs::FS &fs, const char *path)
{
    Serial.printf("Creating Dir: %s\n", path);
    if (fs.mkdir(path)) {
        Serial.println("Dir created");
    } else {
        Serial.println("mkdir failed");
    }
}

void removeDir(fs::FS &fs, const char *path)
{
    Serial.printf("Removing Dir: %s\n", path);
    if (fs.rmdir(path)) {
        Serial.println("Dir removed");
    } else {
        Serial.println("rmdir failed");
    }
}

void readFile(fs::FS &fs, const char *path)
{
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if (!file) {
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.print("Read from file: ");
    while (file.available()) {
        Serial.write(file.read());
    }
    file.close();
}

void writeFile(fs::FS &fs, const char *path, const char *message)
{
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if (!file) {
        Serial.println("Failed to open file for writing");
        return;
    }
    if (file.print(message)) {
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
    file.close();
}

void appendFile(fs::FS &fs, const char *path, const char *message)
{
    Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if (!file) {
        Serial.println("Failed to open file for appending");
        return;
    }
    if (file.print(message)) {
        Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
    file.close();
}

void renameFile(fs::FS &fs, const char *path1, const char *path2)
{
    Serial.printf("Renaming file %s to %s\n", path1, path2);
    if (fs.rename(path1, path2)) {
        Serial.println("File renamed");
    } else {
        Serial.println("Rename failed");
    }
}

void deleteFile(fs::FS &fs, const char *path)
{
    Serial.printf("Deleting file: %s\n", path);
    if (fs.remove(path)) {
        Serial.println("File deleted");
    } else {
        Serial.println("Delete failed");
    }
}

void testFileIO(fs::FS &fs, const char *path)
{
    File file = fs.open(path);
    static uint8_t buf[512];
    size_t len = 0;
    uint32_t start = millis();
    uint32_t end = start;
    if (file) {
        len = file.size();
        size_t flen = len;
        start = millis();
        while (len) {
            size_t toRead = len;
            if (toRead > 512) {
                toRead = 512;
            }
            file.read(buf, toRead);
            len -= toRead;
        }
        end = millis() - start;
        Serial.printf("%u bytes read for %u ms\n", flen, end);
        file.close();
    } else {
        Serial.println("Failed to open file for reading");
    }


    file = fs.open(path, FILE_WRITE);
    if (!file) {
        Serial.println("Failed to open file for writing");
        return;
    }

    size_t i;
    start = millis();
    for (i = 0; i < 2048; i++) {
        file.write(buf, 512);
    }
    end = millis() - start;
    Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
    file.close();
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