#include "otrData.h"
#include "otrFileHandling.h"
#include "otrTime.h"
#include "LilyGo_AMOLED.h"


extern RECORDS records;
extern ANIMALS animals;


void TAGS::resetCurrentTag() {
    currentTag.PIC = "";
    currentTag.RFID = "";
    currentTag.NLISID = "";
    currentTag.Visual_ID = "";
    currentTag.IssueDate = "";
    currentTag.ManufactureDate = "";
    currentTag.Colour = "";
    currentTag.Status = "";
}

void TAGS::readTagsFile() {
    //Open file
    String path = "/" + animals.speciesToString() + tagsFilePath;
    tags = SD.open(path);
    if(!tags) {
        #ifdef OTR_DEBUG
            Serial.println("TAGS::readTagsFile- Failed to open tags file");
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

TAGS::Tags TAGS::getActiveTagDetails(String RFID) {
  for (int i = 0; i < numActiveTags; i++) {
    if (activeTags[i].RFID == RFID) {
      return activeTags[i];
    }
  }
  // Return an empty tag if not found
  return TAGS::Tags();
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


ANIMALS::ANIMALS()  {
    numAnimals = 0;
    totalAnimals = 0;
    currentSpecies = Sheep;
}

void ANIMALS::setSpecies(Species s) {
  currentSpecies = s;
}

ANIMALS::Species ANIMALS::getSpecies() {
  return currentSpecies;
}

String ANIMALS::speciesToString() {
  switch (currentSpecies) {
    case Sheep: return "Sheep";
    case Cattle: return "Cattle";
    default: return "";
  }
}

String ANIMALS::animalGroup() {
  switch (currentSpecies) {
    case Sheep: return "Flock";
    case Cattle: return "Herd";
    default: return "";
  }
}

String ANIMALS::readOptions(String filePath) {
    File optionsFile = SD.open(filePath, "r");
    if (!optionsFile) {
        #ifdef OTR_DEBUG
            Serial.print("ANIMALS::readOptions-Failed to open file: ");
            Serial.println(filePath);
        #endif
        return "";
    }
    String optionsString = "";
    while (optionsFile.available()) {
        String line = optionsFile.readStringUntil('\n');
        optionsString += line + '\n';
    }
    optionsString = optionsString.substring(0, optionsString.length() - 1);
    optionsFile.close();
    if (optionsString.isEmpty()) {
        #ifdef OTR_DEBUG
            Serial.print("ANIMALS::readOptions- file is empty: ");
            Serial.println(filePath);
        #endif
        return "";
    }
    int index = optionsString.indexOf("\r");
    while (index != -1)   {
        optionsString.remove(index, 1);
        index = optionsString.indexOf("\r");
    }
    
    #ifdef OTR_DEBUG
        Serial.print("ANIMALS::readOptions-Options: ");
        Serial.println(optionsString);
    #endif
    return optionsString;
}

void ANIMALS::readBreeds()  {
    String filePath =+ "/" + speciesToString() + "/" + breedsFilePath;
    File file = SD.open(filePath, "r");
    if (!file) {
        #ifdef OTR_DEBUG
            Serial.print("ANIMALS::readBreeds-Failed to open file: ");
            Serial.println(filePath);
        #endif
        ;
    }
    breedsOptionsDropdown = "";
    breedsOptions = NULL;
    int numBreeds = 0;

    while (file.available()) {
        String line = file.readStringUntil('\n');
        line.trim();
        breedsOptionsDropdown += line + '\n';
        breedsOptions[numBreeds] = line;
        numBreeds ++;
    }
    breedsOptionsDropdown = breedsOptionsDropdown.substring(0, breedsOptionsDropdown.length() - 1);
    file.close();
    if (breedsOptionsDropdown.isEmpty()) {
        #ifdef OTR_DEBUG
            Serial.print("ANIMALS::readOptions- file is empty: ");
            Serial.println(filePath);
        #endif
        return;
    }
    #ifdef OTR_DEBUG
        Serial.print("ANIMALS::readOptions-Options: ");
        Serial.println(breedsOptionsDropdown);
    #endif
}

void ANIMALS::readFile() {
    
    animalFilePath = "/" + speciesToString() + "/" + animalGroup() + ".csv";
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
        animal = (Animals*)realloc(animal, (numAnimals + 1) * sizeof(Animals));
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
        animal[numAnimals].dam = columns[6];
        animal[numAnimals].sire = columns[7];
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
    animalFilePath = "/" + speciesToString() + "/" + animalGroup() + ".csv";
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
            aliveOnly = (Animals*)realloc(animal, (numAnimals + 1) * sizeof(Animals));
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
            aliveOnly[numAnimals].dam = columns[6];
            aliveOnly[numAnimals].sire = columns[7];
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
void ANIMALS::addNew(Animals newAnimal) {
    //readAnimalFile() must have already run
    animal = (Animals*)realloc(animal, (numAnimals + 1) * sizeof(Animals));
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
    animalFile.print(newAnimal.dam + ",");
    animalFile.print(newAnimal.sire + ",");
    animalFile.print(newAnimal.multi_birth + ",");
    animalFile.print(newAnimal.location + ",");
    animalFile.print(newAnimal.group + ",");
    animalFile.print(newAnimal.status + ",");
    animalFile.print(newAnimal.whpSafeDate + ",");
    animalFile.println(newAnimal.comment);
    animalFile.close();
}

void ANIMALS::modify(Animals updatedAnimal) {   
    for (int i = 0; i < numAnimals; i++) {
        if (animal[i].rfid == updatedAnimal.rfid) {
            animal[i] = updatedAnimal;
            animalsFilechanged = true;
            return;
        }
    }
}

void ANIMALS::archive(Animals animalToRemove) {  
    Species species = Sheep; 
    animalArchiveFilePath = "/" + speciesToString() + "/archive/" + animalGroup() + "_archive.csv";
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
    animalArchiveFile.print(animalToRemove.dam + ",");
    animalArchiveFile.print(animalToRemove.sire + ",");
    animalArchiveFile.print(animalToRemove.multi_birth + ",");
    animalArchiveFile.print(animalToRemove.location + ","); 
    animalArchiveFile.print(animalToRemove.group + ",");
    animalArchiveFile.print(animalToRemove.status + ",");
    animalArchiveFile.print(animalToRemove.whpSafeDate + ",");
    animalArchiveFile.println(animalToRemove.comment);
    animalArchiveFile.close();
}

void ANIMALS::remove(Animals animalToRemove) {
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

void ANIMALS::resetCurrentAnimal() {
    currentAnimal.breed = "";
    currentAnimal.type = "";
    currentAnimal.name = "";
    currentAnimal.rfid = "";
    currentAnimal.tagged = false;
    currentAnimal.gender = false;
    currentAnimal.dam = "";
    currentAnimal.sire = "";
    currentAnimal.multi_birth = 0;
    currentAnimal.location = "";
    currentAnimal.group = "";
    currentAnimal.status = "";
    currentAnimal.whpSafeDate = "";
    currentAnimal.comment = "";
}

bool ANIMALS::find(String& rfid, Animals& result) {
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
        newFile.print(animal[i].dam + ",");
        newFile.print(animal[i].sire + ",");
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
    
    recordsFilePath = "/" + animals.speciesToString() + "/records.csv";
    recordsFile = SD.open(recordsFilePath, "r");
    if (!recordsFile) {
        Serial.println("Failed to open records file");
        return;
    }
    String header = recordsFile.readStringUntil('\n');
    
    recordsFile.close();
}

int RECORDS::count(const String& filePath) {
    File file = SD.open(filePath, "r");
    if (!file) {
        #ifdef OTR_DEBUG
            Serial.println("RECORDS::count-Failed to open file-Returning zero");
        #endif
        return 0;
    }
    file.seek(0, SeekEnd);
    file.seek(file.position() - 1);
    int recordCount = file.readStringUntil(',').toInt();
    file.close();
    #ifdef OTR_DEBUG
        Serial.print("RECORDS::count-File: "); Serial.println(filePath);
        Serial.print("RECORDS::count-Record count: "); Serial.println(recordCount);
    #endif
    return recordCount;
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
        numRecords = count(recordsFilePath);
        recordsCounted = true;
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
    recordsFile.print(newRecord.type + ",");
    recordsFile.print(newRecord.group + ",");
    recordsFile.print(newRecord.weight + ","); //placeholder for weight
    recordsFile.print(newRecord.trait + ",");
    recordsFile.print(newRecord.treat + ",");
    recordsFile.println(newRecord.comment);
    recordsFile.close();
    if (newRecord.session != "NONE") {
        sessionFile.print(numRecordsInSession);
        sessionFile.print(",") + newRecord.session + ",";
        sessionFile.print(newRecord.rfid + ",");
        sessionFile.print(newRecord.timeStamp + ",");
        sessionFile.print(newRecord.location + ",");
        sessionFile.print(newRecord.status + ",");
        recordsFile.print(newRecord.type + ",");
        sessionFile.print(newRecord.group + ",");
        sessionFile.print(newRecord.weight + ","); //placeholder for weight
        sessionFile.print(newRecord.trait + ",");
        sessionFile.print(newRecord.treat + ",");
        sessionFile.println(newRecord.comment);
        sessionFile.close();
    }
}

void RECORDS::createSession() {
    //session name is current date with sequential number yymmdd_1, yymmdd_2 etc
    //in order to continue a session after power down etc last 5 sessions stored in last_sessions.txt
    if (numRecordsInSession == 0) {
        //delete empty session
        records.deleteSession();
    }
    String dateStr = getSessionDate();
    
    readLastSessions();
    #ifdef OTR_DEBUG
        Serial.print("RECORDS::createSession - " + dateStr);
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
    sessionFilePath = lastSessionFilePath;
    if (!SD.remove(sessionFilePath)) {
        #ifdef OTR_DEBUG
            Serial.println("RECORDS:createSession-Failed to remove last sessions file");
        #endif
        return;
    }
    File file = SD.open(sessionFilePath, "w");
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
    sessionFilePath = "/" + animals.speciesToString() + "/sessions/" + session + ".csv";
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
    numRecordsInSession = 0;
    recordsCounted = true;
    sessionFile.close();
}

void RECORDS::deleteSession() {
    String path = "/" + animals.speciesToString() + "/sessions/" + session + ".csv";
    deleteFileSD(path.c_str());
}
String RECORDS::readLastSessions() {
    
    lastSessionFilePath = "/" + animals.speciesToString() + "/sessions/last_sessions.txt";
    
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
        #ifdef OTR_DEBUG
            Serial.println("RECORDS:readLastSessions-No sessions listed");
        #endif
    }
    #ifdef OTR_DEBUG
        Serial.println("RECORDS:readLastSessions - " + sessionsDropdown);
    #endif
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
        
        productsFilePath = "/" + animals.speciesToString() + "/products.csv";
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
    
    productsFilePath = "/" + animals.speciesToString() + "/products.csv";
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