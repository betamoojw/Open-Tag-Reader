// otrData.h
#ifndef OTRDATA_H
#define OTRDATA_H


#include <vector>
#include <LittleFS.h>
#include "otrTime.h"



class TAGS {
    public:
        // bucket file in follwing format
        // "PIC","RFID","NLISID","Visual_ID","IssueDate","ManufactureDate","Colour"
        // "NK509384","940 110030374791","NK509384ASU00001","CHAD 001","17/11/2023","17/11/2023","BLACK"

        struct Tags {
            String PIC;             // property identification code
            String RFID;            // unique - from scan
            String NLISID;
            String Visual_ID;
            String IssueDate;
            String ManufactureDate;
            String Colour;
            String Status;          // Unused, Active, or Inactive
        };
        String bucketFilePath = "/test_bucket.csv";
        String tagsFilePath = "/tags.csv";
        String csvBucket;
        String csvTags;
        uint16_t rowsBucket;
        uint16_t rowsTags;
        uint16_t rowsAdded;
        int csvBucketIndex = 0;
        int numTags = 0;
        int numActiveTags = 0;
        int numUnusedTags = 0;
        void readBucketFile();
        void readTagsFile();
        void readTagsActive();
        bool isTagActive(String RFID);
        Tags getActiveTagDetails(String RFID);
        bool isTagKnown(String RFID);
        Tags getTagDetails(String RFID);
        //char feedRowParser();
        //bool rowParserFinished();
        int countActiveTags();
        int countAllTagsAvailable(); // both active and Unused tags
        //CSV_Parser bucketParser;
        File bucket;
        File tags;
        Tags* allTags;
        Tags* activeTags;

    private:
        char *tagID;
        
        bool isNew;
        bool checkNew();
        void addTagFromBucket();
        // NLIS requirement to report replaced tags https://www.nlis.com.au/NLISDocuments/Prepare%20a%20file%20for%20upload.pdf
        // Old RFID, New RFID, Date (dd/mm/yyyy)

        // NLIS PIC Reconcilliation https://www.nlis.com.au/NLISDocuments/Prepare%20a%20file%20for%20upload.pdf
        // PIC, RFID - All tags on the PIC (including unused)

        // NLIS Tag STatus https://www.nlis.com.au/NLISDocuments/Prepare%20a%20file%20for%20upload.pdf
        // https://www.nlis.com.au/NLISDocuments/PIC%20and%20device%20status%20codes%20(Aug%2014)%20External.pdf
        // RFID. Date (dd/mm/yyyy), Program (NLS), Status (Code), Action (ON/OFF), Comment
        //
};

class ANIMALS {
    public:
        struct Animals {
            String breed;
            String type;    //can't remember why I added this.  Possibly to differentiate between stud andcommercial
            String name;
            String rfid;    //unique
            String tagged;
            bool gender;    //fixed M/F TRUE == "F"
            String mother;  //future feature
            String father;
            uint8_t multi_birth; //was the animal born as a single twin, etc
            String location;
            String group;
            String status;  //Alive,Dead,Sold
            String whpSafeDate;
            String comment;
        };

        struct Deaths {
            // https://www.nlis.com.au/NLISDocuments/Deceased%20livestock%20on%20PIC%20(Mar%2014)%20V2.pdf
            String rfid;
            String date;
            String cause;
        };

        struct Dogs {
            String rfid;
            String name;  
        };

        String animalFilePath = "/animals.csv";
        String animalArchiveFilePath = "/archive/aminals_archive.csv";
        String animalFilePathTemp = "/backup/animals.tmp";
        String dogsFilePath = "/dogs.csv";
        struct Options {
            String name;
            int numOptions;
            String* options;
            String optionsDropdown;

            Options(String defaultName = "breeds",
                    String defaultDropdown = "None") {
                name = defaultName;
                numOptions = 0;
                options = nullptr;         
                optionsDropdown = defaultDropdown;
            }
        };
        String breedsFilePath = "/breeds.csv";
        String typesFilePath = "/types.csv";
        String groupsFilePath = "/groups.csv";
        String* breedsOptions;
        String* typesOptions;
        String* groupsOptions;
        String breedsOptionsDropdown = "Merino\nCrossbred\nDorper";
        String typesOptionsDropdown = "Stud\nCommercial";
        String groupsOptionsDropdown = "Ewes\nLambs\nRams";

        ANIMALS();
        ~ANIMALS() {
            delete [] animal;
        }

        enum Species {
            Sheep,
            Cattle
        };
        
        void setSpecies(Species s);
        Species getSpecies();
        String speciesToString();
        String animalGroup();
        String readOptions(String filePath);
        void readBreeds();
        void readTypes();
        void readGroups();
        void readFile();
        void readAliveOnly();
        void addNew(Animals);   
        void modify(Animals);
        void archive(Animals);
        void remove(Animals);
        void create();
        void renewFile();
        bool find(String& rfid, Animals& result);
        int countAllAlive();
        int countByGroup();
        
        File animalFile;
        bool animalsFilechanged = false;
        uint16_t totalAnimals;



    
    private:
        AnimalsFile* animal;
        AnimalsFile* aliveOnly;
        int numAnimals; 
        Species currentSpecies;
};

class RECORDS {
    public:
        struct Records {
            String index;
            String session;
            String rfid; //record created when scanned
            String timeStamp;
            String location;
            String status;
            String type;
            String group;
            String weight;
            String trait;  //delinineated list of traits new values eg. Udder/Dry;Feet/Bad,
            String treat; //Treat records given an index, multiple treatments delineated eg 123;124;125
            String comment;
        };

        struct Sessions {
            String rfid;
            String timeStamp;
            String location;
            String status;
            String type;
            String group;
            String weight;
            String trait;
            String treat;
            String comment;
        };

        String recordsFilePath = "/records.csv";
        String session;
        String lastSessions[5];
        String lastSessionFilePath = "/sessions/last_sessions.txt";
        String sessionFilePath = "sessions/yymmdd_1.csv";
        String recordsHeader = "Index,Session,RFID,Timestamp,Location,Status,Group,Weight,Trait,Treat,Comment/n";
        void readFile();
        int count(const String& filePath);
        void create();
        void addNew(Records);
        void createSession();
        void deleteSession();
        String readLastSessions();
        Records* find(String& rfid, int& num);
        int numRecordsInSession;
        File recordsFile;
        bool recordsFilechanged = false;    
        uint32_t totalRecords;
        
        bool recordsCounted = false;

    private:
        Records* record;
        int numRecords;
        
    

};

class TREATMENTS {
    public:
        String treatmentsFilePath = "/treatments.csv";
        String productsFilePath = "/products.csv";
        int numProducts;
        int numHumans;
        struct Treatments    {  // Meet LPA Requirement #3 - Livestock Treatment Record
            uint32_t index;
            String date;                // treatment date
            String location;            // treatment location
            uint32_t productIndex;      // index of product in Chemical Inventory
            String product;             // product name
            String batch;               // Batch number
            bool expiryChecked;         // Is date < expiry date (not sure about DOM's)
            int dose;                   // dose quantity
            String doseUnit;            // mL/L, ml/kg, ml/head
            String appliedBy;           // Name of person
            String appliedByContact;    // contact number of person treating
            uint8_t whp;                // witholding period (days)
            uint8_t esi;                // export slaughter interval (days)
            String dateSafe;            // date safe for slaughter (calculated)
            String adverseReactions;    // list of adverse reactions
            bool brokenNeedle;          // broken needle still in animal
            bool equipmentCleaned;      // equipment cleaned/calibrated?
            String dateEquipCleaned;    // date equipment cleaned/calibrated
            String equipCleanedBy;      // Name of person
            String cleanedByContact;    // contact number of person cleaning/calibrating
        };
        struct Products    {    // Meet LPA Requirement #3 - Chemical Inventory
            uint32_t index;
            String received;                    // date received
            String name;                        // Product name
            int quantity;                       // 
            String quantityUnit;                // L, mL, g, kg, Unit
            String expiryOrDateOfManufacture;   // date
            String isExpiryOrDOM;               // Expiry or DOM
            String batchNum;                    // Batch number
            uint8_t whp_days;                   // Witholding period (days)
            uint8_t esi_days;                   // Export slaughter interval (days)
            String updatedBy;                   // Name of person
            bool status;                        // Active or Inactive
        };
        struct Humans    {
            String name;
            String initials;
            String contact;     //contact number (LPA requirement)
        };
        void loadProducts();
        void addProduct(String productRow);
        void editProduct();
        void create();
        void loadHumans();
    private:
    int totalProducts;
    int numActiveProducts;
    Products* product;
    Treatments* treatment;
    Humans* human;
};




struct TraitOptions {
    String optionName;
};
struct Traits {
    String traitName;
    std::vector<TraitOptions> options;
};


struct Transfers {  // Transfer file to meet NLIS requirements
    // Livestock moved off/onto PIC https://www.nlis.com.au/NLISDocuments/Prepare%20a%20file%20for%20upload.pdf
    // RFID,            Source PIC, Destination PIC, NVD/Waybill, Date (dd/mm/yyyy)
    // 999 000011110001, QIZZ0000,   QFZZ4444,        40473164,   13/07/2021
    String rfid;
    String date;
    String location;
    String pic;
    String destination;
    String destinationPic;
    String nvd;             // NVD/Waybill
    float price;
    String comment;
};

    // NLIS Mob based movements. https://www.nlis.com.au/NLISDocuments/Prepare%20a%20file%20for%20upload.pdf 
    // Species, Date moved (dd/mm/yyyy), Source PIC, No. of head, Destination PIC, NVD / Waybill, Other PICs (separated by space-not compulsory), Vendor bred (Y/N), Time on vendor PIC (code below), Comment
    // Code for time on vendor PIC:
    // A - less than 2 months
    // B - 2 to 6 months
    // C - 6 to 12 months
    // D - more than 12 months


struct Locations {
    String name;
    String PIC;
};

enum Status {
    ALIVE,
    DEAD,
    SOLD
};

enum TagStatus {
    Unused,
    Active,
    Inactive
};



struct tags {
    String pic;
    String rfid;
    String nlisid;
    String visual_id;
    String issueDate;
    String manufactureDate;
    String colour;
    Status status;
};

void readLocations();
void readTraitsFromCSV();
void printTraits();

void readAnimals();
void createAnimal();
void modifyAnimal();

void changeStatus();
void changeGroup();

void changeLocation();

#endif