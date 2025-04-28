// otrFileHandling.h
#ifndef OTRFILEHANDLING_H
#define OTRFILEHANDLING_H



#include "FS.h"
#include "LittleFS.h"
#include <Arduino.h>
#include "time.h"
#include "Wire.h"
#include "ui/ui.h"
//#include "otrDisplay.h"


void initFileSystem(void);
void SD_init(void);
void copyFileFromLittleFStoSD(const char* sourcePath, const char* destinationDirectory, 
                                const char* destinationFilename);
void moveFileFromLittleFStoSD(const char* sourcePath, const char* destinationDirectory, 
                                const char* destinationFilename);
void printFileContents(String filePath);
void getStorageInfo(void);
void createExampleFile(void);
void appendNewLine(const char* filePath, char** bucketRow);
String create_new_list(void);
void listDir(const char * dirname);
String readLittleFSFileToString(const char *filename);
String readSDFileToString(const char *filename);
void listDir(fs::FS &fs, const char *dirname, uint8_t levels);
void createDir(fs::FS &fs, const char *path);
void removeDir(fs::FS &fs, const char *path);
void readFile(fs::FS &fs, const char *path);
void writeFile(fs::FS &fs, const char *path, const char *message);
void appendFile(fs::FS &fs, const char *path, const char *message);
void renameFile(fs::FS &fs, const char *path1, const char *path2);
void deleteFile(fs::FS &fs, const char *path);
void testFileIO(fs::FS &fs, const char *path);
bool moveFileSD(const char* sourcePath, const char* destinationPath);


#endif