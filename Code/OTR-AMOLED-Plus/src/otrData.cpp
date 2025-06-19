#include "otrData.h"
#include "otrFileHandling.h"
#include "otrTime.h"
#include "LilyGo_AMOLED.h"

Species species = Sheep;
extern RECORDS records;

void TAGS::readTagsFile() {
    //Open file
    tags = SD.open(tagsFilePath);
    if(!tags) {
        #ifdef OTR_DEBUG
            Serial.println("Failed to open tags file");
        #endif
        return;
    }
    
    while (tags.available())    {
        String tagsRow = tags.readStringUntil('\n'); 
        numTags++;
        // Pick out columns
        int commaIndex = 0;
        String columns[7];
        for (int i = 0; i < 7; i++) {
            commaIndex = tagsRow.indexOf(",", commaIndex);
            if (commaIndex == -1) {
                columns[i] = tagsRow.substring(commaIndex + 1);
                break;
            }
            columns[i] = tagsRow.substring(0, commaIndex);
            tagsRow = tagsRow.substring(commaIndex + 1);
        }
        allTags = (Tags*)realloc(allTags, (numTags + 1)  * sizeof(Tags));
        allTags[numTags].PIC = columns[0];
        allTags[numTags].RFID = columns[1];
        allTags[numTags].NLISID = columns[2];
        allTags[numTags].Visual_ID = columns[3];
        allTags[numTags].IssueDate = columns[4];
        allTags[numTags].ManufactureDate = columns[5];
        allTags[numTags].Colour = columns[6];    
        allTags[numTags].Status = columns[7];
    }
    tags.close();
}

void TAGS::readTagsActive() {
    //Open file
    tags = SD.open(tagsFilePath);
    if(!tags) {
        #ifdef OTR_DEBUG
            Serial.println("Failed to open tags file");
        #endif
        return;
    }
    
    while (tags.available())    {
        String tagsRow = tags.readStringUntil('\n'); 
        numTags++;
        // Pick out columns
        int commaIndex = 0;
        String columns[8];
        for (int i = 0; i < 7; i++) {
            commaIndex = tagsRow.indexOf(",", commaIndex);
            if (commaIndex == -1) {
                columns[i] = tagsRow.substring(commaIndex + 1);
                break;
            }
            columns[i] = tagsRow.substring(0, commaIndex);
            tagsRow = tagsRow.substring(commaIndex + 1);
        }
        // check tag status add active tags but count Unused
        if (columns[7] == "Active") {
            numActiveTags++;
            activeTags = (Tags*)realloc(activeTags, (numActiveTags + 1)  * sizeof(Tags));
            activeTags[numActiveTags].PIC = columns[0];
            activeTags[numActiveTags].RFID = columns[1];
            activeTags[numActiveTags].NLISID = columns[2];
            activeTags[numActiveTags].Visual_ID = columns[3];
            activeTags[numActiveTags].IssueDate = columns[4];
            activeTags[numActiveTags].ManufactureDate = columns[5];
            activeTags[numActiveTags].Colour = columns[6];    
            activeTags[numActiveTags].Status = columns[7];
        } else if (columns[7] == "Unused") {
            numUnusedTags++;
        } 
        
    }
    tags.close();
}

void TAGS::readBucketFile() {
    //Open file
    bucket = SD.open(bucketFilePath);
    if(!bucket) {
        #ifdef OTR_DEBUG
            Serial.println("Failed to open bucket file");
        #endif
        return;
    }

    // Open tags.csv to compare with
    tags = SD.open(tagsFilePath);
    if(!tags) {
        #ifdef OTR_DEBUG
            Serial.println("Failed to open tags file");
        #endif
        return;
    }
    
    //Read Header and check in correct format
    csvBucket = bucket.readStringUntil('\n');
    if (csvBucket.isEmpty()) {
        #ifdef OTR_DEBUG
            Serial.println("Empty bucket file");
        #endif
        return;
    }
    if (csvBucket != "PIC,RFID,NLISID,Visual_ID,IssueDate,ManufactureDate,Colour") {
        //check if quotation marks are the issue
        csvBucket.replace("\"","");
        #ifdef OTR_DEBUG
            Serial.println(csvBucket);
        #endif
        if (csvBucket != "PIC,RFID,NLISID,Visual_ID,IssueDate,ManufactureDate,Colour") {
            #ifdef OTR_DEBUG 
                Serial.println("Incorrect header");
            #endif
            return;
        }
        #ifdef OTR_DEBUG
            Serial.println("TAGS:readBucketFile-Correct header");
        #endif
    }
    
    while (bucket.available()) {
        // Read a row
        csvBucket = bucket.readStringUntil('\n');
        rowsBucket++;
        if(checkNew())  {
            addTagFromBucket();
        }
    }
    bucket.close();
    tags.close();
    #ifdef OTR_DEBUG
        Serial.print(rowsBucket);
        Serial.println(" rows in bucket file read");
        Serial.print(rowsAdded);
        Serial.println(" rows added to tags.csv");
    #endif
    if(rowsBucket == rowsAdded) {
        #ifdef OTR_DEBUG
            Serial.println("Tags.csv successfully updated");
        #endif
        SD.remove(bucketFilePath);
    }
}


bool TAGS::checkNew() {
  //tags.csv has an added column for Status
  csvBucket.replace("\n",",UNUSED\n");
  tags.seek(1, SeekEnd);//start from the end of file (most recently added) but before /n
  char c;
  int startPosition;
  while (tags.position() > 0) {
    c = tags.read();
    if (c == '\n') {
        startPosition = tags.position()-1;
        csvTags = tags.readStringUntil('\n');
        if (csvTags == csvBucket) {
            return true;
        }
        tags.seek(startPosition);
    }
    tags.seek(tags.position() - 1);
  }
  return false;
}

void TAGS::addTagFromBucket() {
    tags.print(csvBucket);
    tags.print(",Unused");
    tags.println();
    rowsAdded++;
}

int TAGS::countActiveTags() {
    //Open file
    tags = SD.open(tagsFilePath);
    if(!tags) {
        #ifdef OTR_DEBUG
            Serial.println("Failed to open tags file");
        #endif
        return 0;
    }
    
    while (tags.available())    {
        String tagsRow = tags.readStringUntil('\n'); 
        numTags++;
        // Pick out columns
        int commaIndex = 0;
        String columns[8];
        for (int i = 0; i < 7; i++) {
            commaIndex = tagsRow.indexOf(",", commaIndex);
            if (commaIndex == -1) {
                columns[i] = tagsRow.substring(commaIndex + 1);
                break;
            }
            columns[i] = tagsRow.substring(0, commaIndex);
            tagsRow = tagsRow.substring(commaIndex + 1);
        }
        // check tag status add active tags but count Unused
        if (columns[7] == "Active") {
            numActiveTags++;
            
        } 
    }
    tags.close();
    return numActiveTags;
}

bool TAGS::isTagActive(String RFID) {
    for (int i = 0; i < numActiveTags; i++) {
        if (activeTags[i].RFID == RFID) {
            return true;
        }
    }
    return false;
}

bool TAGS::isTagKnown(String RFID) {
    tags = SD.open(tagsFilePath);
    String line;
    while (tags.available())    {
        line = tags.readStringUntil('\n');
        if (line.indexOf(RFID) != -1) {
            tags.close();
            return true;
        }
    }
    tags.close();
    return false;
}

TAGS::Tags TAGS::getTagDetails(String RFID) {
    Tags tagDetails;
    tags = SD.open(tagsFilePath);
    String line;
    while (tags.available())    {
        line = tags.readStringUntil('\n');
        if (line.indexOf(RFID) != -1) {
            //pick out columns
            int commaIndex = 0;
            String columns[8];
            for (int i = 0; i < 7; i++) {
                commaIndex = line.indexOf(",", commaIndex);
                if (commaIndex == -1) {
                    columns[i] = line.substring(commaIndex + 1);
                    break;
                }
                columns[i] = line.substring(0, commaIndex);
                line = line.substring(commaIndex + 1);
            }
            tagDetails.PIC = columns[0];
            tagDetails.RFID = columns[1];
            tagDetails.NLISID = columns[2];
            tagDetails.Visual_ID = columns[3];
            tagDetails.IssueDate = columns[4];
            tagDetails.ManufactureDate = columns[5];
            tagDetails.Colour = columns[6];
            tagDetails.Status = columns[7];
        }
    }
    tags.close();
    return tagDetails;
}
//INCOMPLETE
void readTraitsFromCSV() {
    // Reads traits from csv
    // Format of file is Traitname followed by options separated by commas
    // User can define their own traits therefore no fixed dimensions
    // uint8_t numTraits = 0;
    // Traits* trait = nullptr;
    // File traitsFile = LittleFS.open(traitsFilePath);

    // if (traitsFile) {
    //     while (traitsFile.available()) {
    //         traitsFile.readStringUntil('\n');
    //         numTraits++;
    //     }
    //     traitsFile.seek(0); // move back to the beginning of the file
    //     trait = new Traits[numTraits-1]; // allocate memory for the array
    //     while (traitsFile.available()) {
    //         String traitRow = traitsFile.readStringUntil('\n');
    //         if (numTraits == 0) {
    //             // Skip header row
    //             continue;
    //         }
    //         // Pick out columns
    //         trait[numTraits-1].traitName = traitRow.substring(0, traitRow.indexOf(","));
    //         String allTraitOptions = traitRow.substring(traitRow.indexOf(",") + 1, traitRow.length());
    //         int optionCommaIndex;
    //         while ((optionCommaIndex = allTraitOptions.indexOf(",")) != -1) {
    //             String option = allTraitOptions.substring(0, optionCommaIndex);
    //             trait[numTraits-1].options.push_back(option);
    //             allTraitOptions = allTraitOptions.substring(optionCommaIndex + 1, allTraitOptions.length());
    //         }
    //         // Add the last option
    //         trait[numTraits-1].options.push_back(allTraitOptions);
    //     }
    // }
 
    // while (traitsFile.available()) {
    //     // Read a row
    //     String traitRow = traitsFile.readStringUntil('\n');
    //     if (numTraits == 0) {
    //         // Skip header row
    //         traitRow = traitsFile.readStringUntil('\n');
    //     }
    //     String traitName = traitRow.substring(0, traitRow.indexOf(","));
    //     Traits newTrait;
    //     newTrait.traitName = traitName;
    //     String allTraitOptions = traitRow.substring(traitRow.indexOf(",") + 1, traitRow.length());
    //     int optionCommaIndex;
    //     while ((optionCommaIndex = allTraitOptions.indexOf(",")) != -1) {
    //         String option = allTraitOptions.substring(0, optionCommaIndex);
    //         TraitOptions newOption;
    //         newOption.optionName = option;
    //         newTrait.options.push_back(newOption);
    //         allTraitOptions = allTraitOptions.substring(optionCommaIndex + 1, allTraitOptions.length());
    //     }
    //     // Add the last option
    //     TraitOptions newOption;
    //     newOption.optionName = allTraitOptions;
    //     newTrait.options.push_back(newOption);
    //     // Add the trait to the traits vector
    //     traits.push_back(newTrait);
    //     numTraits++;
    // }
    // traitsFile.close();
}

//Not working
void printTraits() {
    // for (size_t i = 0; i < traits.size(); i++) {
    //     Serial.print("Trait: ");
    //     Serial.println(traits[i].traitName);
    //     for (size_t j = 0; j < traits[i].options.size(); j++) {
    //         Serial.print("  Option: ");
    //         Serial.println(traits[i].options[j].optionName);
    //     }
    // }
}

ANIMALS::ANIMALS()  {
    numAnimals = 0;
    totalAnimals = 0;
}
    
void ANIMALS::readFile() {
    Species species = Sheep;
    animalFilePath = "/" + speciesStrings[species] + "/" + speciesGroups[species] + ".csv";
    animalFile = SD.open(animalFilePath);
    if (!animalFile) {
        #ifdef OTR_DEBUG
            Serial.print("Failed to open file: ");
            Serial.println(animalFilePath);
        #endif
        return;
    }
    animal = nullptr; // Initialize the class member animal to nullptr
    animalFile.readStringUntil('\n'); // skip header
    String animalRow;
    while (animalFile.available()) {
        // Read a row
        String animalRow = animalFile.readStringUntil('\n');
        totalAnimals++;
        // Pick out columns
        int commaIndex = 0;
        String columns[15];
        for (int i = 0; i < 14; i++) {
        commaIndex = animalRow.indexOf(",", commaIndex);
        if (commaIndex == -1) {
            columns[i] = animalRow.substring(commaIndex + 1);
            break;
        }
        columns[i] = animalRow.substring(0, commaIndex);
        animalRow = animalRow.substring(commaIndex + 1);
        }
        animal = (AnimalsFile*)realloc(animal, (numAnimals + 1) * sizeof(AnimalsFile));
        animal[numAnimals].breed = columns[0];
        animal[numAnimals].type = columns[1];
        animal[numAnimals].name = columns[2];
        animal[numAnimals].rfid = columns[3];
        animal[numAnimals].tagged = columns[4];
        if (columns[5] == "F") {
            animal[numAnimals].gender = true;
        }
        else {
            animal[numAnimals].gender = false;
        }
        animal[numAnimals].mother = columns[6];
        animal[numAnimals].father = columns[7];
        animal[numAnimals].multi_birth = columns[8].toInt();
        animal[numAnimals].location = columns[9];
        animal[numAnimals].group = columns[10];
        animal[numAnimals].status = columns[11];
        animal[numAnimals].whpSafeDate = (columns[12]);
        animal[numAnimals].comment = columns[13];
        numAnimals++; // Move the increment to the end
    }
    animalFile.close();
}

void ANIMALS::readAliveOnly() {
    Species species = Sheep;
    animalFilePath = "/" + speciesStrings[species] + "/" + speciesGroups[species] + ".csv";
    animalFile = SD.open(animalFilePath);
    if (!animalFile) {
        #ifdef OTR_DEBUG
            Serial.print("Failed to open file: ");
            Serial.println(animalFilePath);
        #endif
        return;
    }
    aliveOnly = nullptr; // Initialize the class member animal to nullptr
    animalFile.readStringUntil('\n'); // skip header
    String animalRow;
    while (animalFile.available()) {
        // Read a row
        String animalRow = animalFile.readStringUntil('\n');
        totalAnimals++;
        // Pick out columns
        int commaIndex = 0;
        String columns[15];
        for (int i = 0; i < 14; i++) {
            commaIndex = animalRow.indexOf(",", commaIndex);
            if (commaIndex == -1) {
                columns[i] = animalRow.substring(commaIndex + 1);
                break;
            }
            columns[i] = animalRow.substring(0, commaIndex);
            animalRow = animalRow.substring(commaIndex + 1);
        }
        // Only add records with "Alive" status
        if (columns[11] != "Alive") {
            aliveOnly = (AnimalsFile*)realloc(animal, (numAnimals + 1) * sizeof(AnimalsFile));
            aliveOnly[numAnimals].breed = columns[0];
            aliveOnly[numAnimals].type = columns[1];
            aliveOnly[numAnimals].name = columns[2];
            aliveOnly[numAnimals].rfid = columns[3];
            aliveOnly[numAnimals].tagged = columns[4];
            if (columns[5] == "F") {
                aliveOnly[numAnimals].gender = true;
            }
            else {
                aliveOnly[numAnimals].gender = false;
            }
            aliveOnly[numAnimals].mother = columns[6];
            aliveOnly[numAnimals].father = columns[7];
            aliveOnly[numAnimals].multi_birth = columns[8].toInt();
            aliveOnly[numAnimals].location = columns[9];
            aliveOnly[numAnimals].group = columns[10];
            aliveOnly[numAnimals].status = columns[11];
            aliveOnly[numAnimals].whpSafeDate = (columns[12]);
            aliveOnly[numAnimals].comment = columns[13];
            numAnimals++; // Move the increment to the end            
        }

    }
    animalFile.close();
}
void ANIMALS::addNew(AnimalsFile newAnimal) {
    //readAnimalFile() must have already run
    animal = (AnimalsFile*)realloc(animal, (numAnimals + 1) * sizeof(AnimalsFile));
    animal[numAnimals] = newAnimal;
    numAnimals++;
    //append to file
    File animalFile = SD.open(animalFilePath, FILE_APPEND);
    if (!animalFile) {
        #ifdef OTR_DEBUG
            Serial.println("Failed to open animal file");
        #endif
        return;
    }
    animalFile.print(newAnimal.breed + ",");
    animalFile.print(newAnimal.type + ",");
    animalFile.print(newAnimal.name + ",");
    animalFile.print(newAnimal.rfid + ",");
    animalFile.print(newAnimal.tagged + ",");
    animalFile.print((newAnimal.gender ? "F," : "M,") );
    animalFile.print(newAnimal.mother + ",");
    animalFile.print(newAnimal.father + ",");
    animalFile.print(newAnimal.multi_birth + ",");
    animalFile.print(newAnimal.location + ",");
    animalFile.print(newAnimal.group + ",");
    animalFile.print(newAnimal.status + ",");
    animalFile.print(newAnimal.whpSafeDate + ",");
    animalFile.println(newAnimal.comment);
    animalFile.close();
}

void ANIMALS::modify(AnimalsFile updatedAnimal) {   
    for (int i = 0; i < numAnimals; i++) {
        if (animal[i].rfid == updatedAnimal.rfid) {
            animal[i] = updatedAnimal;
            animalsFilechanged = true;
            return;
        }
    }
}

void ANIMALS::archive(AnimalsFile animalToRemove) {  
    Species species = Sheep; 
    animalArchiveFilePath = "/" + speciesStrings[species] + "/archive/" + speciesGroups[species] + "_archive.csv";
    File animalArchiveFile = SD.open(animalArchiveFilePath, FILE_APPEND);
    if (!animalArchiveFile) {
        #ifdef OTR_DEBUG
            Serial.println("Failed to open animal archive file");
        #endif
        return;
    }
    animalArchiveFile.print(animalToRemove.breed + ",");
    animalArchiveFile.print(animalToRemove.type + ",");
    animalArchiveFile.print(animalToRemove.name + ",");
    animalArchiveFile.print(animalToRemove.rfid + ",");
    animalArchiveFile.print(animalToRemove.tagged + ",");
    animalArchiveFile.print((animalToRemove.gender ? "F," : "M,") );
    animalArchiveFile.print(animalToRemove.mother + ",");
    animalArchiveFile.print(animalToRemove.father + ",");
    animalArchiveFile.print(animalToRemove.multi_birth + ",");
    animalArchiveFile.print(animalToRemove.location + ","); 
    animalArchiveFile.print(animalToRemove.group + ",");
    animalArchiveFile.print(animalToRemove.status + ",");
    animalArchiveFile.print(animalToRemove.whpSafeDate + ",");
    animalArchiveFile.println(animalToRemove.comment);
    animalArchiveFile.close();
}

void ANIMALS::remove(AnimalsFile animalToRemove) {
    for (int i = 0; i < numAnimals; i++) {
        if (animal[i].rfid == animalToRemove.rfid) {
            // Shift all elements after the removed element to the left
            for (int j = i; j < numAnimals - 1; j++) {
                animal[j] = animal[j + 1];
            }
            numAnimals--;
            animalsFilechanged = true;
            return;
        }
    }
}

bool ANIMALS::find(String& rfid, AnimalsFile& result) {
    for (int i = 0; i < numAnimals; i++) {
        if (animal[i].rfid == rfid) {
            #ifdef OTR_DEBUG
                Serial.print("Animal found: Record #"); Serial.println(i);
            #endif
            result = animal[i];
            return true;
        }
    }
    #ifdef OTR_DEBUG
        Serial.println("Animal not found");
    #endif

    return false;
}
//UNFINISHED
void ANIMALS::create() {
    //pull data from ui
}

void ANIMALS::renewFile() {
    //copy header
    File currentFile =SD.open(animalFilePath, "r");
    if (!currentFile) {
        #ifdef OTR_DEBUG
            Serial.println("Failed to open animal file");
        #endif
        return;
    }
    String header ="";
    if (currentFile.available()) {
        header = currentFile.readStringUntil('\n');
    }
    currentFile.close();
    //rename file
    animalFilePathTemp = animalFilePath;
    animalFilePathTemp.replace(".csv", ".tmp");
    if (!SD.rename(animalFilePath, animalFilePathTemp)) {
        #ifdef OTR_DEBUG
            Serial.println("Failed to rename animal file");
        #endif
        return;
    }
    //create new file
    File newFile = SD.open(animalFilePath, FILE_WRITE);
    if (!newFile) {
        #ifdef OTR_DEBUG
            Serial.println("Failed to create new animal file");
        #endif
        return;
    }
    newFile.println(header);
    //copy animals to new file
    for (int i = 0; i < numAnimals; i++) {
        newFile.print(animal[i].breed + ",");
        newFile.print(animal[i].type + ",");
        newFile.print(animal[i].name + ",");
        newFile.print(animal[i].rfid + ",");
        newFile.print(animal[i].tagged + ",");
        newFile.print((animal[i].gender ? "F," : "M,") );
        newFile.print(animal[i].mother + ",");
        newFile.print(animal[i].father + ",");
        newFile.print(animal[i].multi_birth + ",");
        newFile.print(animal[i].location + ",");
        newFile.print(animal[i].group + ",");
        newFile.print(animal[i].status + ",");
        newFile.print(animal[i].whpSafeDate + ",");
        newFile.println(animal[i].comment);
    }
    newFile.close();
}
//UNFINISHED
void RECORDS::readFile() {
    Species species = Sheep;
    recordsFilePath = "/" + speciesStrings[species] + "/records.csv";
    recordsFile = SD.open(recordsFilePath, "r");
    if (!recordsFile) {
        Serial.println("Failed to open records file");
        return;
    }
    String header = recordsFile.readStringUntil('\n');
    
    recordsFile.close();
}

void RECORDS::count() {
    recordsFile = SD.open(recordsFilePath, "r");
    recordsFile.seek(0, SeekEnd);
    recordsFile.seek(recordsFile.position() - 1);
    totalRecords = recordsFile.readStringUntil(',').toInt();
    recordsFile.close();
    recordsCounted = true;
}
//UNFINISHED
void RECORDS::create() {
    //pull data from ui
}

void RECORDS::addNew(Records newRecord) {
    recordsFile = SD.open(recordsFilePath, FILE_APPEND);
    if (!recordsFile) {
        Serial.println("RECORDS:addNew-Failed to open records file");
        return;
    }
    recordsFile.seek(0, SeekEnd); //go to end of file
    if (!recordsCounted) {//count function must be run prior
        count();
    }
    File sessionFile = SD.open(sessionFilePath, "w");
    if (!sessionFile) {
        #ifdef OTR_DEBUG
            Serial.println("RECORDS:addNew-Failed to create session file");
        #endif
        return;
    }
    totalRecords++; 
    numRecordsInSession++;
    recordsFile.print(totalRecords);
    recordsFile.print(",") + newRecord.session + ",";
    recordsFile.print(newRecord.rfid + ",");
    recordsFile.print(newRecord.timeStamp + ",");
    recordsFile.print(newRecord.location + ",");
    recordsFile.print(newRecord.status + ",");
    recordsFile.print(newRecord.group + ",");
    recordsFile.print(newRecord.weight + ","); //placeholder for weight
    recordsFile.print(newRecord.trait + ",");
    recordsFile.print(newRecord.treat + ",");
    recordsFile.println(newRecord.comment);
    recordsFile.close();
    sessionFile.print(numRecordsInSession);
    sessionFile.print(",") + newRecord.session + ",";
    sessionFile.print(newRecord.rfid + ",");
    sessionFile.print(newRecord.timeStamp + ",");
    sessionFile.print(newRecord.location + ",");
    sessionFile.print(newRecord.status + ",");
    sessionFile.print(newRecord.group + ",");
    sessionFile.print(newRecord.weight + ","); //placeholder for weight
    sessionFile.print(newRecord.trait + ",");
    sessionFile.print(newRecord.treat + ",");
    sessionFile.println(newRecord.comment);
    sessionFile.close();
}

void RECORDS::createSession() {
    //session name is current date with sequential number yymmdd_1, yymmdd_2 etc
    //in order to continue a session after power down etc last 5 sessions stored in last_sessions.txt
    String dateStr;
    getSessionDate(dateStr);
    readLastSessions();
    #ifdef OTR_DEBUG
        Serial.println(dateStr);
        Serial.println(lastSessions[4].substring(0, 6));
    #endif
    if(lastSessions[4].substring(0, 6) == dateStr) {
        int num = lastSessions[4].substring(7).toInt();
        session = dateStr + "_" + String(num + 1);
    } else {
        session = dateStr + "_1";
    }
    String newSessions[5];
    for (int i = 1; i < 5; i++) {
        newSessions[i-1] = lastSessions[i];
    }
    newSessions[4] = session;
    if (!SD.remove(lastSessionFilePath)) {
        #ifdef OTR_DEBUG
            Serial.println("RECORDS:createSession-Failed to remove last sessions file");
        #endif
        return;
    }
    File file = SD.open(lastSessionFilePath, "w");
     if (!file) {
        #ifdef OTR_DEBUG
            Serial.println("RECORDS:createSession-Failed to create last sessions file");
        #endif
        return;
    }

    for (int i = 0; i < 5; i++) {
        file.println(newSessions[i]);
    }
  
    file.close();
    sessionFilePath = "/" + speciesStrings[species] + "/sessions/" + session + ".csv";
    File sessionFile = SD.open(sessionFilePath, "w");
    if (!sessionFile) {
        #ifdef OTR_DEBUG
            Serial.println("RECORDS:createSession-Failed to create session file");
        #endif
        return;
    }
    sessionFile.println(recordsHeader);
    recordsFilePath = sessionFilePath;
    numRecords = 0;
    recordsCounted = true;
    sessionFile.close();
}

String RECORDS::readLastSessions() {
    
    lastSessionFilePath = "/" + speciesStrings[species] + "/sessions/last_sessions.txt";
    
    String sessionsDropdown;

    File file = SD.open(lastSessionFilePath, "r");
    if (!file) {
        // Create the file if it doesn't exist
        file = SD.open(lastSessionFilePath, "w");
        if (!file) {
            #ifdef OTR_DEBUG
                Serial.println("RECORDS:readLastSessions-Failed to create last sessions file");
            #endif
            return "error";
        }
        // Initialize the file with empty lines
        for (int i = 0; i < 5; i++) {
            file.println();
        }
        file.close();
        // Reopen the file for reading
        file = SD.open(lastSessionFilePath, "r");
    }

    int i = 0;
    while (file.available() && i < 5) {
        String line = file.readStringUntil('\n');
        lastSessions[i] = line;
        sessionsDropdown = sessionsDropdown + line + '\n';
        i++;
    }
    file.close();
    sessionsDropdown = sessionsDropdown.substring(0, sessionsDropdown.length() - 1);
    if (sessionsDropdown.isEmpty()) {
        sessionsDropdown = "Empty";
    }
    return sessionsDropdown;
    
}
RECORDS::Records* RECORDS::find(String& rfid, int& num) {
    recordsFile = SD.open(recordsFilePath, "r");
    String csvRecord = recordsFile.readStringUntil('\n'); //1st line
    Records* thisRfid;
    num = 0;
    while (recordsFile.available()) {
        csvRecord = recordsFile.readStringUntil('\n');
        int fileIndex = csvRecord.indexOf(rfid);
        if (fileIndex != -1) {
            // Pick out columns
            int commaIndex = 0;
            String columns[10];
            for (int i = 0; i < 10; i++) {
                if (commaIndex == -1) {
                    columns[i] = csvRecord.substring(commaIndex + 1);
                    break;
                }
                else {
                    columns[i] =csvRecord.substring(commaIndex, csvRecord.indexOf(',', commaIndex));
                    commaIndex = csvRecord.indexOf(',', commaIndex + 1);
                }
            }
            thisRfid[num].index = columns[0].toInt();
            thisRfid[num].session = columns[1];
            thisRfid[num].rfid = columns[2];
            thisRfid[num].timeStamp = columns[3];
            thisRfid[num].location = columns[4];
            thisRfid[num].status = columns[5];
            thisRfid[num].group = columns[6];
            thisRfid[num].weight = columns[7];
            thisRfid[num].trait = columns[8];
            thisRfid[num].treat = columns[9];
            thisRfid[num].comment = columns[10];
            num++;
        }
    }
    recordsFile.close();
    return thisRfid;
    
}
//UNFINISHED

// Treatments 

// Chemical Inventory
void TREATMENTS::loadProducts() {
        Species species = Sheep;
        productsFilePath = "/" + speciesStrings[species] + "/" + speciesGroups[species] + ".csv";
        File productsFile = SD.open(productsFilePath);
        if (!productsFile) {
            #ifdef OTR_DEBUG
                Serial.print("TREATMENTS:loadProducts-Failed to open file: ");
                Serial.println(productsFilePath);
            #endif
            return;
        }
        product = nullptr; // Initialize the class member animal to nullptr
        productsFile.readStringUntil('\n'); // skip header
        String productRow;
        while (productsFile.available()) {
            // Read a row
            String productRow = productsFile.readStringUntil('\n');
            totalProducts++;
            // Pick out columns
            int commaIndex = 0;
            String columns[11];
            
            for (int i = 0; i < 1; i++) {
            commaIndex = productRow.indexOf(",", commaIndex);
            if (commaIndex == -1) {
                columns[i] = productRow.substring(commaIndex + 1);
                break;
            }
            columns[i] = productRow.substring(0, commaIndex);
            productRow = productRow.substring(commaIndex + 1);
            }
            product = (Products*)realloc(product, (totalProducts + 1) * sizeof(Products));
            product[totalProducts].index = columns[0].toInt();
            product[totalProducts].received = columns[1];
            product[totalProducts].name = columns[2];
            product[totalProducts].quantity = columns[3].toInt();
            product[totalProducts].quantityUnit = columns[4];
            product[totalProducts].expiryOrDateOfManufacture = columns[5];
            product[totalProducts].isExpiryOrDOM = columns[6];
            product[totalProducts].batchNum = columns[7];
            product[totalProducts].whp_days = columns[8].toInt();
            product[totalProducts].esi_days = columns[9].toInt();
            product[totalProducts].updatedBy = columns[10];
            if (columns[11] == "0") {
                product[totalProducts].status = false;
            } else {
                product[totalProducts].status = true;
                numActiveProducts++;
            }
            product[totalProducts].status = columns[11].toInt();
            totalProducts++; // Move the increment to the end
        }
     
    productsFile.close();
}



void TREATMENTS::addProduct(String productRow) {
    Species species = Sheep;
    productsFilePath = "/" + speciesStrings[species] + "/" + speciesGroups[species] + ".csv";
    File productsFile = SD.open(productsFilePath);
    if (!productsFile) {
        #ifdef OTR_DEBUG
            Serial.print("TREATMENTS:addProduct-Failed to open file: ");
            Serial.println(productsFilePath);
        #endif
        return;
    }
    productsFile.print(productRow);
    productsFile.close();
}

void TREATMENTS::loadHumans() {
    
    File humansFile = SD.open("/humans.csv", "r");
    if (!humansFile) {
        #ifdef OTR_DEBUG
            Serial.println("Failed to open humans file");
        #endif
        return;
    }
    numHumans = 0;
    String header = humansFile.readStringUntil('\n');
    while (humansFile.available()) {
        String humansRow = humansFile.readStringUntil('\n');
        human[numHumans].name = humansRow.substring(0, humansRow.indexOf(','));
        human[numHumans].initials = humansRow.substring(humansRow.indexOf(',') + 1);
        numHumans++;
    }

    humansFile.close();
    #ifdef OTR_DEBUG
        Serial.print("TREATMENS:loadHumans-humans read: ");
        Serial.println(numHumans);
    #endif

}

void readLocations() {
    uint8_t numLocations = 0;
    Locations* location = nullptr;
    File locationFile = SD.open("/locations.csv");
    if (locationFile) {
        while (locationFile.available()) {
            locationFile.readStringUntil('\n');
            numLocations++;
        }
        locationFile.seek(0); // move back to the beginning of the file
        location = new Locations[numLocations-1]; // allocate memory for the array
        for (uint8_t i = 0; i < numLocations; i++) {
            String locationRow = locationFile.readStringUntil('\n');
            if (i == 0) {
                // Skip header row
                continue;
            }
            // Pick out columns
            location[i].name = locationRow.substring(0, locationRow.indexOf(","));
            location[i].PIC = locationRow.substring(locationRow.indexOf(",") + 1, locationRow.length());
        }
        locationFile.close();
    }
    
}