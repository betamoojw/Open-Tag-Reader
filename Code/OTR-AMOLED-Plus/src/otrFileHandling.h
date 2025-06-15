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
String listDirToString(const char * dirname);
String readLittleFSFileToString(const char *filename);
String readSDFileToString(const char *filename);
String listSDCardContents();
String listDirSD(const char *dirname, uint8_t currentIndex);
String listLittleFSContents();
String listDirLittleFS(const char *dirname, uint8_t currentIndex);
bool moveFileSD(const char* sourcePath, const char* destinationPath);


#endif