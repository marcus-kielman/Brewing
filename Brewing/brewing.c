#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <errno.h>

/*state of each phase and stage*/
enum STATE {
  INPROGRESS = 0,
  COMPLETED = 1,
  TERMINATED = 2
};

/*global variables*/
FILE *activitiesLog, *errorsLog;
struct BrewingProcess bp;
struct BrewingBatch bb;
char input[100];  //User input
int inputFlag;

/*Control functions*/
int clearLog(char *logFile);
int displayInstant(char *charFile);
int getInput();
int parseAndExecute();

/*Functions for prep stage*/
void recipe();
void raw_materials();
int check_weight();

/*functions for mash stage*/
int add_hot_water();
int sparging_time();
int stirring_time();

/*functions for boiling stage*/
int boiling_time();

/*functions for ferm stage*/
double calculate_gravity();
int fermentation_time();

/*functions used in critical sections*/
int quality_check();
int reset_and_cleaning_time();

/*additional functions*/
void return_err();
int error_check(int retValue);
void thread_stage_create(pthread_t thread, void *parameter);

/*struct for properties in each stage*/
struct BrewingProcess{
  /*Ingredients in Batch*/
  float cool_waterBatch;
  float hot_waterBatch;
  float maltsBatch;
  float yeastBatch;
  float hopsBatch;
  /*Information for activitiesLog.txt and errorsLog.txt*/
  const char *bpe_batchid;
  const char *bpe_date_timestamp;
  const char *bpe_stage;
  char *bpe_status_description;
  double bpe_duration;
  int bpe_status_code;

  /*Additional properties in brewing process*/
  double specificGravity;
  float alcohol_content;
};

/*struct for properties needed for batch*/
struct BrewingBatch{
  /*Ingredients for Recipe*/
  float cool_waterRecipe;
  float hot_waterRecipe;
  float maltsRecipe;
  float yeastRecipe;
  float hopsRecipe;
}batch;

//=================================================================================
//Description: clear activitiesLog and errorsLog
//Input: NULL
//Output: cleared activitiesLog.txt and errorsLog.text
//        1 on error, 0 on success
int clearLog(char *logFile){
  FILE *file;
  char c[] = "";
  char contents[255];

  if((file = fopen(logFile,"w+")) == NULL){
      fprintf(stderr,"error opening text_files/leaderLog.txt\n");
      return 1;
  }
  fprintf(file, "\n");
  fclose(file);
  return 0;
}
//=================================================================================
//Description: Instantly displays file contents to terminal
//Input: character pointer containing file name
//Output: displayed file contents
//        1 on error, 0 on success
int displayInstant(char *charFile){
  FILE *file;
  char c[] = "";
  char contents[255];


  if((file = fopen(charFile,"r")) == NULL){
    fprintf(stderr,"error opening %s\n", charFile);
    return 1;
  }
  while (fgets(contents, 255, (FILE*)file) != NULL){
    printf("%s", contents);
  }
  fclose(file);
  return 0;
}
//==============================================================================
//Description: Gets player input controls from terminal during program run
//Input: inputted player controls
//Output: NULL
int getInput(){
  if (inputFlag == 0){
    printf("\n# ");
    return fgets(input, sizeof(input), stdin) != NULL;
  }
  else{
    return fgets(input, sizeof(input), stdin) != NULL;
  }
}
//==============================================================================
//Description: parse player input controls from terminal during program run
//Input: NULL
//Output: Displayed responses to user controls
//        fget function to prompt input from player
int parseAndExecute(){
  pthread_t thread1, thread2, thread3, thread4, thread5;
  /*batch ids for five batches*/
  const char *message1 = "BATCH_1";
  const char *message2 = "BATCH_2";
  const char *message3 = "BATCH_3";
  const char *message4 = "BATCH_4";
  const char *message5 = "BATCH_5";

  char *command = strtok(input, " \n");

  if (command != NULL){
    if (strncmp(command, "q", 1) == 0){
      displayInstant("MXK.txt");
      return 0;
    }
    else if (strncmp(command, "help", 4) == 0){
      displayInstant("help.txt");
    }
    else if (strncmp(command, "clear", 5) == 0){
      clearLog("activitiesLog.txt");
      clearLog("errorsLog.txt");
    }
    else if (strncmp(command, "input", 5) == 0){
      inputFlag = 1;

      printf("Input Recipe Here:\n");
      printf("Cool Water: ");
      getInput();
      bb.cool_waterRecipe = strtof(input, NULL);
      printf("Hot Water: ");
      getInput();
      bb.hot_waterRecipe = strtof(input, NULL);
      printf("Malts: ");
      getInput();
      bb.maltsRecipe = strtof(input, NULL);
      printf("Yeast: ");
      getInput();
      bb.yeastRecipe = strtof(input, NULL);
      printf("Hops: ");
      getInput();
      bb.hopsRecipe = strtof(input, NULL);


      printf("\nInput Inventory Here\n");
      printf("Cool Water: ");
      getInput();
      bp.cool_waterBatch = strtof(input, NULL);
      printf("Hot Water: ");
      getInput();
      bp.hot_waterBatch = strtof(input, NULL);
      printf("Malts: ");
      getInput();
      bp.maltsBatch = strtof(input, NULL);
      printf("Yeast: ");
      getInput();
      bp.yeastBatch = strtof(input, NULL);
      printf("Hops: ");
      getInput();
      bp.hopsBatch = strtof(input, NULL);

      inputFlag = 0;
    }
    else if (strncmp(command, "run", 3) == 0){
      activitiesLog = fopen("activitiesLog.txt", "a");
      errorsLog = fopen("errorsLog.txt", "a");

      /*create batch threads*/
      thread_stage_create(thread1, (void *)message1);
      thread_stage_create(thread2, (void *)message2);
      thread_stage_create(thread3, (void *)message3);
      thread_stage_create(thread4, (void *)message4);
      thread_stage_create(thread5, (void *)message5);

      fclose(activitiesLog);
      fclose(errorsLog);
    }
    else{
      printf("Please input a valid command\n");
    }
  }

  return 1;
}
//==============================================================================
//Description: Function prints to Terminal the recipe for given beer
//Input:
//Output: Printed statements to Terminal
//        Written bpa_status_description to activitiesLog.txt
void recipe(){
  bp.bpe_status_code = INPROGRESS;

  printf("  Recipe for 1/2 Barrel Kegg:\n");
  printf("    Cool Water: %f gallons\n", bb.cool_waterRecipe);
  printf("    Hot Water:  %f gallons\n", bb.hot_waterRecipe);
  printf("    Malts:      %f lbs\n", bb.maltsRecipe);
  printf("    Yeast:      %f g\n", bb.yeastRecipe);
  printf("    Hops:       %f oz.\n\n", bb.hopsRecipe);
  return;
}

//==============================================================================
//Description: Function prints to Terminal raw materials in inventory
//Input: BrewingBatch struct
//Output: Printed statements to Terminal
void raw_materials(){

  printf("  Inventory:\n");
  printf("    Cool Water: %f gallons\n", bp.cool_waterBatch);
  printf("    Hot Water:  %f gallons\n", bp.hot_waterBatch);
  printf("    Malts:      %f lbs\n", bp.maltsBatch);
  printf("    Yeast:      %f g\n", bp.yeastBatch);
  printf("    Hops:       %f oz.\n", bp.hopsBatch);
  return;
}

//==============================================================================
//Description: Function compares raw materials and ingredients in BrewProcess
//             and BrewBatch. Determines whether to enough ingredients for batch.
//             Terminates batch if there isn't enough ingredients.
//Input:
//Output: Printed statements to Terminal
//        Written bpa_status_description to activitiesLog.txt if enough ingredients,
//        or errorsLog.txt if there isn't enough ingredients
int check_weight(){
  float cool_water = bp.cool_waterBatch - bb.cool_waterRecipe;
  float hot_water = bp.hot_waterBatch - bb.hot_waterRecipe;
  float malts = bp.maltsBatch - bb.maltsRecipe;
  float yeast = bp.yeastBatch - bb.yeastRecipe;
  float hops = bp.hopsBatch - bb.hopsRecipe;

  if(cool_water < 0 || hot_water < 0 || malts < 0 || yeast < 0 || hops < 0){
    printf("Warning: There is not enough ingredients for batch\n");

    return 0;
  }
  printf("\nNote: There's enough ingredients for this batch\n");
  bp.cool_waterBatch = cool_water;
  bp.hot_waterBatch = hot_water;
  bp.maltsBatch = malts;
  bp.yeastBatch = yeast;
  bp.hopsBatch = hops;

  printf("  New Inventory:\n");
  printf("    Cool Water: %f gallons\n", bp.cool_waterBatch);
  printf("    Hot Water:  %f gallons\n", bp.hot_waterBatch);
  printf("    Malts:      %f lbs\n", bp.maltsBatch);
  printf("    Yeast:      %f g\n", bp.yeastBatch);
  printf("    Hops:       %f oz.\n", bp.hopsBatch);

  return 1;
}

//==============================================================================
//Description: Function adds hot water to the mashing stage over a period of time
//Input:
//Output: value 1 upon completion to indicate the end of phase
int add_hot_water(){
  int hot_water = (rand() % (5 - 3 + 1)) + 3;  //generates stirring time between 3s-5s
  sleep(hot_water);
  printf("\nNote: Hot Water Added\n");
  return 1;
}

//==============================================================================
//Description: Function sparges to the mashing stage over a period of time
//Input:
//Output: randomized amount of time to complete sparging
int sparging_time(){
  int sparging = (rand() % (5 - 1 + 1)) + 2;  //generates stirring time between 1s-5s
  sleep(sparging);
  return sparging;
}

//==============================================================================
//Description: Function stirs components of the mashing stage over a period of time
//Input:
//Output: randomized amount of time to complete stirring
int stirring_time(){
  int stirring = (rand() % (5 - 1 + 1)) + 2; //generates stirring time between 1s-5s
  sleep(stirring);
  return stirring;
}

//==============================================================================
//Description: Function boils mashed byproduct in boiling stage over a period of time
//Input:
//Output: randomized amount of time to complete boiling
int boiling_time(){
  int boiling = (rand() % (5 - 1 + 1)) + 2; //generates boiling time between 1s-5s
  sleep(boiling);
  return boiling;
}

//==============================================================================
//Description: Function generates randomized initial and terminal gravity to
//             compute specific gravity
//Input:
//Output: computed specific gravity of boiled substance
double calculate_gravity(){
  double initialGravity = (1.35 + (rand() / (float)RAND_MAX) * (1.5 - 1.35));  //generates initial gravity between 1.4-1.5
  double terminalGravity =  (1.3 + (rand() / (float)RAND_MAX) * (1.4 - 1.3)); //generates terminal gravity between 1.3 and 1.4
  double specificGravity = initialGravity - terminalGravity;

  if(specificGravity <= 0 || specificGravity > 0.10){
    return 0;
  }
  return specificGravity;
}

//==============================================================================
//Description: Function waits for fermentation to complete over a period of time
//Input:
//Output: randomized amount of time to complete fermentation
int fermentation_time(){
  int fermTime = (rand() % (25 - 21 + 1)) + 21; //generates fermentation time between 21s and 25s
  sleep(fermTime);
  return fermTime;
}

//==============================================================================
//Description: Function geneates randomized value of the percentage of
//             stage byproduct, and determines success and failure of stage
//Input:
//Output: returns 0 on failure, and 1 on success
int quality_check(){
  float qualityCheck = (95 + (rand() / (float)RAND_MAX) * (100 - 95));    //generate quality check percentage between 95%-100%
  printf("Quality Check  = %f\n", qualityCheck);
  if (qualityCheck <= 95){
    printf("Warning: Quality Check < 95 percent\n");
    return 0;
  }
  return 1;
}

//==============================================================================
//Description: Function generates randomized amount of time to complete
//             reset and cleaning phase
//Input:
//Output: randomized amount of time to complete reset and cleaning in stage
int reset_and_cleaning_time(){
  int resetClean = (rand() % (10 - 5 + 1)) + 5; //generate time between 1-5 secs
  sleep(resetClean);
  return resetClean;
}

//==============================================================================
//Description: Function executes functions for prep stage and records progress
//             in either activitiesLog.txt or errorsLog.txt
//Input:
//Output: recorded progress in either activitiesLog.txt or errorsLog.txt
void *prep(void *ptr){
  char *phaseId = "PREP";
  time_t oldDuration, newDuration;

  oldDuration = time(NULL);             //begin counting duration
  fprintf(activitiesLog, "\nBPA: PREP\n");
  fprintf(activitiesLog, "BPA: STATUS CODE, BPA: STATUS DESC.\n\n");
  recipe();                               //Gathers ingredients for recipe and places in BrewingBatch
  fprintf(activitiesLog, "BPA: IN PROGRESS, BPA: RECIPE\n");

  raw_materials();                        //Gathers raw materials from inventory and places in BrewingProcess
  fprintf(activitiesLog, "BPA: IN PROGRESS, BPA: RAW MATERIALS\n");

  /*Check the weight and amount of inventory to ingredients*/
  if(check_weight() == 0){
    bp.bpe_status_description = "CHECK WEIGHT FAILURE (NOT ENOUGH INGREDIENTS)";
    newDuration = time(NULL);   //stop counting duration
    newDuration = newDuration - oldDuration; //get overall duration
    /*store in brewing process struct*/
    bp.bpe_duration += (double)newDuration;
    bp.bpe_status_code = TERMINATED;
    return_err(phaseId);
    return NULL;
  }
  fprintf(activitiesLog, "BPA: IN PROGRESS, BPA: CHECK WEIGHT\n");
  /*check if quality check succeeded*/
  if (quality_check() == 0){
    printf("Warning: Quality Check Failed\n");
    newDuration = time(NULL);
    newDuration = newDuration - oldDuration;
    bp.bpe_duration += (double)newDuration;
    bp.bpe_status_description = "QUALITY FAILURE";
    bp.bpe_status_code = TERMINATED;
    return_err(phaseId);
    return NULL;
  }
  fprintf(activitiesLog, "BPA: IN PROGRESS, BPA: QUALITY CHECK\n\n");

  newDuration = time(NULL);
  bp.bpe_duration = (double) newDuration - (double) oldDuration;
  fprintf(activitiesLog, "BPA: %fs, BPA:COMPLETE, BPA: Prep Stage Complete\n", bp.bpe_duration);
  printf("***Prep Stage Complete***\n");
}
//==============================================================================
//Description: Function executes functions for mash stage and records progress
//             in either activitiesLog.txt or errorsLog.txt
//Input:
//Output: recorded progress in either activitiesLog.txt or errorsLog.txt
void *mash(void *ptr){
  char *phaseId = "MASH";
  time_t oldDuration, newDuration;

  /*check if previous stage failed*/
  if(bp.bpe_status_code == TERMINATED){
    printf("\nWarning: Brewing Terminated\n");
    return NULL;
  }
  else{
    oldDuration = time(NULL);
    fprintf(activitiesLog, "\nBPA: MASH\n");

    add_hot_water();
    fprintf(activitiesLog, "BPE: IN PROGRESS, BPE: ADD HOT WATER\n");
    int sparging = sparging_time();
    printf("Note: Sparging Time %ds\n", sparging);
    fprintf(activitiesLog, "BPE: IN PROGRESS, BPE: SPARGING TIME\n");
    int stirring = stirring_time();
    printf("Note: Stirring Time %ds\n", stirring);
    fprintf(activitiesLog, "BPE: IN PROGRESS, BPE: STIRRING TIME\n");

    int resetClean = reset_and_cleaning_time();
    printf("Note: Reset And Cleaning Time %ds\n", resetClean);
    fprintf(activitiesLog, "BPE: IN PROGRESS, BPE: RESET AND CLEANING TIME\n");
    if (quality_check() == 0){
      printf("Warning: Quality Check Failed\n");
      newDuration = time(NULL);
      newDuration = newDuration - oldDuration;
      bp.bpe_duration += (double)newDuration;
      bp.bpe_status_description = "QUALITY FAILURE";
      bp.bpe_status_code = TERMINATED;
      return_err(phaseId);
      return NULL;
    }
    fprintf(activitiesLog, "BPA: IN PROGRESS, BPA: QUALITY CHECK\n\n");
    newDuration = time(NULL);
    newDuration = newDuration - oldDuration;
    bp.bpe_duration += (double)newDuration;
    fprintf(activitiesLog, "BPA: %fs, BPA:COMPLETE, BPA: Mash Stage Complete\n", bp.bpe_duration);
    printf("***Mash Stage Complete***\n");
  }
}
//==============================================================================
//Description: Function executes functions for boil stage and records progress
//             in either activitiesLog.txt or errorsLog.txt
//Input:
//Output: recorded progress in either activitiesLog.txt or errorsLog.txt
void *boil(void *ptr){
  char *phaseId = "BOIL";
  time_t oldDuration, newDuration;

  if(bp.bpe_status_code == TERMINATED){
    printf("\nWarning: Brewing Terminated\n");
    return NULL;
  }
  else{
    oldDuration = time(NULL);
    fprintf(activitiesLog, "\nBPA: BOIL\n");
    int boilTime = boiling_time();
    printf("\nNote: Boiling Time %ds\n", boilTime);
    fprintf(activitiesLog, "BPE: IN PROGRESS, BPE: BOILING TIME\n");
    float boiling_temperature = (211 + (rand() / (float)RAND_MAX) * (215 - 211));
    while (boiling_temperature < 212){
      boiling_temperature = (211 + (rand() / (float)RAND_MAX) * (215 - 211));
    }
    printf("Note: Boiling Temperature %fF\n", boiling_temperature);
    fprintf(activitiesLog, "BPE: IN PROGRESS, BPE: BOILING TEMPERATURE %fF\n", boiling_temperature);

    int resetClean = reset_and_cleaning_time();
    printf("Note: Reset And Cleaning Time %ds\n", resetClean);
    fprintf(activitiesLog, "BPE: IN PROGRESS, BPE: RESET AND CLEANING TIME\n");
    if (quality_check() == 0){
      printf("Warning: Quality Check Failed\n");
      newDuration = time(NULL);
      newDuration = newDuration - oldDuration;
      bp.bpe_duration += (double)newDuration;
      bp.bpe_status_description = "QUALITY FAILURE";
      bp.bpe_status_code = TERMINATED;
      return_err(phaseId);
      return NULL;
    }
    fprintf(activitiesLog, "BPA: IN PROGRESS, BPA: QUALITY CHECK\n\n");

    newDuration = time(NULL);
    newDuration = newDuration - oldDuration;
    bp.bpe_duration += (double)newDuration;
    fprintf(activitiesLog, "BPA: %fs, BPA:COMPLETE, BPA: Boil Stage Complete\n", bp.bpe_duration);
    printf("***Boil Stage Complete***\n");
  }
}
//==============================================================================
//Description: Function executes functions for ferm stage and records progress
//             in either activitiesLog.txt or errorsLog.txt
//Input:
//Output: recorded progress in either activitiesLog.txt or errorsLog.txt
void *ferm(void *ptr){
  char *phaseId = "FERM";
  float gravity;
  int yeast_activation, fermTime, temp, resetClean;
  time_t oldDuration, newDuration;

  if(bp.bpe_status_code == TERMINATED){
    printf("\nWarning: Brewing Terminated\n");
    return NULL;
  }
  else{
    oldDuration = time(NULL);
    fprintf(activitiesLog, "\nBPA: FERM\n");
    bp.specificGravity = calculate_gravity();
    printf("\nNote: Gravity %f\n", bp.specificGravity);
    /*check for gravity failure*/
    if(bp.specificGravity <= 0){
      printf("Warning: Gravity Failure\n");
      newDuration = time(NULL);
      newDuration = newDuration - oldDuration;
      bp.bpe_duration += (double)newDuration;
      bp.bpe_status_description = "BATCH HAS GRAVITY FAILURE";
      bp.bpe_status_code = TERMINATED;
      return_err(phaseId);
      return NULL;
    }

    fprintf(activitiesLog, "BPE: IN PROGRESS, BPE: GRAVITY %f\n", gravity);
    yeast_activation = 1;
    printf("Note: Yeast Activated\n");
    fprintf(activitiesLog, "BPE: IN PROGRESS, BPE: YEAST ACTIVATION\n");
    fermTime = fermentation_time();
    printf("Note: Fermentation Time %ds\n", fermTime);
    fprintf(activitiesLog, "BPE: IN PROGRESS, BPE: FERMENTATION TIME %dF\n", fermTime);
    temp = (rand() % (72 - 68 + 1)) + 68;     //generating random number between 68F-72F
    printf("Note: Temperature %d F\n", temp);
    fprintf(activitiesLog, "BPE: IN PROGRESS, BPE: TEMPERATURE %dF\n", temp);

    bp.alcohol_content = bp.specificGravity * 131.25; //calculate alcohol content
    printf("Note: Alcohol Content %f\n", bp.alcohol_content);
    fprintf(activitiesLog, "BPE: IN PROGRESS, BPE: ALCOHOL CONTENT %f\n", bp.alcohol_content);
    resetClean = reset_and_cleaning_time();
    printf("Note: Reset And Cleaning Time %ds\n", resetClean);
    fprintf(activitiesLog, "BPE: IN PROGRESS, BPE: RESET AND CLEANING TIME\n");
    if (quality_check() == 0){
      printf("Warning: Quality Check Failed\n");
      newDuration = time(NULL);
      newDuration = newDuration - oldDuration;
      bp.bpe_duration += (double)newDuration;
      bp.bpe_status_description = "QUALITY FAILURE";
      bp.bpe_status_code = TERMINATED;
      return_err(phaseId);
    }
    fprintf(activitiesLog, "BPA: IN PROGRESS, BPA: QUALITY CHECK\n\n");

    newDuration = time(NULL);
    newDuration = newDuration - oldDuration;
    bp.bpe_duration += (double)newDuration;
    fprintf(activitiesLog, "BPA: %fs, BPA:COMPLETE, BPA: Ferm Stage Complete\n", bp.bpe_duration);
    printf("***Ferm Stage Complete***\n");
  }
}
//==============================================================================
//Description: Function executes functions for kegg stage and records progress
//             in either activitiesLog.txt or errorsLog.txt
//Input:
//Output: recorded progress in either activitiesLog.txt or errorsLog.txt
void *kegg(void *ptr){
  char *phaseId = "KEGG";
  int carbonation;
  float volume;
  time_t oldDuration, newDuration;

  if(bp.bpe_status_code == TERMINATED){
    printf("\nWarning: Brewing Terminated\n");
    return NULL;
  }
  else{
    oldDuration = time(NULL);
    fprintf(activitiesLog, "\nBPA: KEGG\n");
    carbonation = 1;    //carbonation success
    fprintf(activitiesLog, "BPE: IN PROGRESS, BPE: CARBONATION\n");
    printf("\nNote: Carbonation Complete\n");
    volume = bb.cool_waterRecipe + bb.hot_waterRecipe; //calculate volume
    fprintf(activitiesLog, "BPE: IN PROGRESS, BPE: VOLUME %f GALLONS\n", volume);
    printf("Note: Volume %f\n", volume);

    bp.alcohol_content = bp.specificGravity * 131.25;
    printf("Note: Alcohol Content %f\n", bp.alcohol_content);
    fprintf(activitiesLog, "BPE: IN PROGRESS, BPE: ALCOHOL CONTENT %f\n", bp.alcohol_content);
    int resetClean = reset_and_cleaning_time();
    printf("Note: Reset And Cleaning Time %ds\n", resetClean);
    fprintf(activitiesLog, "BPE: IN PROGRESS, BPE: RESET AND CLEANING TIME\n");
    if (quality_check() == 0){
      printf("Warning: Quality Check Failed\n");
      newDuration = time(NULL);
      newDuration = newDuration - oldDuration;
      bp.bpe_duration += (double)newDuration;
      bp.bpe_status_description = "QUALITY FAILURE";
      bp.bpe_status_code = TERMINATED;
      return_err(phaseId);
      return NULL;
    }
    fprintf(activitiesLog, "BPA: IN PROGRESS, BPA: QUALITY CHECK\n\n");
    newDuration = time(NULL);
    newDuration = newDuration - oldDuration;
    bp.bpe_duration += (double)newDuration;
    fprintf(activitiesLog, "BPA: %fs, BPA:COMPLETE, BPA: Kegg Stage Complete\n", bp.bpe_duration);
    printf("***Kegg Stage Complete***\n");
    bp.bpe_status_code = COMPLETED;
  }
}

//==============================================================================
//Description: Function logs any failures in system to errorsLog.txt
//Input: phaseId (i.e. current stage where failure has occurred)
//Output: NULL
void return_err(char *phaseId){
  fprintf(errorsLog, "BPE: %s\n", bp.bpe_batchid);
  fprintf(errorsLog, "BPA: %s", bp.bpe_date_timestamp);
  fprintf(errorsLog, "BPE: %s\n", phaseId);
  fprintf(errorsLog, "BPE DURATION, BPE: STATUS CODE, BPE: STATUS DESC.\n\n");
  fprintf(errorsLog, "BPE: %fs, BPE: TERMINATED, BPE: %s\n\n", bp.bpe_duration, bp.bpe_status_description);
  return;
}

//==============================================================================
//Description: Function checks to make sure pthread_create returns 1 upon success
//Input: retValue containing value returned from pthread_create
//Output: Function exits upon failure
int error_check(int retValue){
  if(retValue){
    fprintf(errorsLog, "Error - pthread_create() return code: %d\n", retValue);
    exit(EXIT_FAILURE);
  }
}

//==============================================================================
//Description: Function creates and waits upon completion of five stages for a given
//             batch, and logs batchid and date/time stamp into activitiesLog.txt
//Input: batch, message
//Output: randomized amount of time to complete stirring
void thread_stage_create(pthread_t batch, void *batchid){
  int ret;

  bp.bpe_batchid = (const char *)batchid;     //store batch id
  printf("\n%s\n", bp.bpe_batchid);
  fprintf(activitiesLog, "\nBPA: %s\n", bp.bpe_batchid); //write in activitiesLog.txt
  fprintf(activitiesLog, "BPA: %s\n", bp.bpe_date_timestamp);

  /*create stages for thread*/
  ret = pthread_create(&batch, NULL, prep, NULL);
  error_check(ret);
  pthread_join(batch, NULL); //prep join

  ret = pthread_create(&batch, NULL, mash, NULL);
  error_check(ret);
  pthread_join(batch, NULL); //mash join

  ret = pthread_create(&batch, NULL, boil, NULL);
  error_check(ret);
  pthread_join(batch, NULL); //boil join

  ret = pthread_create(&batch, NULL, ferm, NULL);
  error_check(ret);
  pthread_join(batch, NULL); //ferm join

  ret = pthread_create(&batch, NULL, kegg, NULL);
  error_check(ret);
  pthread_join(batch, NULL);//kegg join
  printf("pthread_create() for thread returns:%d\n", ret);

  /*store completion status in activitiesLog.txt and print to terminal*/
  if(bp.bpe_status_code == COMPLETED){
    fprintf(activitiesLog, "\nBPA: %fs, BPA:COMPLETE, BPA: Brewing Batch Complete\n", bp.bpe_duration);
    printf("\n***Brewing Batch Complete***\n");
  }
}

int main(){
  srand(time(0));
  /*get date and time*/
  time_t current;

  time(&current);
  bp.bpe_date_timestamp =  ctime(&current);

  printf("For any assistance, type 'help' into the terminal\n");
  while(getInput() && parseAndExecute());
  exit(EXIT_SUCCESS);
}
