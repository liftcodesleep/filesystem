/**************************************************************
 * Class:  CSC-415-01 Summer 2023
 * Names: Hyok In Kwon
 * Student IDs: 922373878
 * GitHub Name: hkwon4
 * Group Name: Coffee On The Rocks
 * Project: Basic File System
 *
 * File: parsePath.c
 *
 * Description: helper functions to parse path and keep track of 
 *              loaded directory entries. Used for most functions
 *              within mfs.h
 * 
 **************************************************************/


#include "parsePath.h"
#include <stdio.h>
#include <stdlib.h>


#define MAX_CHAR 100
#define TEST_MAX_INDEX 20

//direntry pointers to be loaded in memory
direntry * rootD;
//lastPathD's parent directory
direntry * parentD;
//the directry that was loaded in the completion of
//validatePath()
direntry * lastPathD;
//actual current directory entry (to be determined by setcwd() function)
direntry * currentD;
//check if the root,parent,current directories have been loaded
int init = 0;

//an init function to allocate memory and load the
//root directory and make the parent directory and
//current directory point to root
//Function will only run the first time a path is validated
void init_loadedDir(){
  if (init == 0){
    rootD = getRoot();
    parentD = malloc(BLOCK_SIZE * 12);
    parentD = rootD;
    lastPathD = malloc(BLOCK_SIZE * 12);
    lastPathD = rootD;
    init = 1;
  }
}

//returns based on successful parsing
//returns pointer to parsedPath struct or NULL if it fails
//if error occurs, all allocated memory of parsedPath is freed
parsedPath* parsePath(const char* pathname){
    //check if pathname is an emptry string
    //return NULL
    if (strlen(pathname) == 0){
      return NULL;
    }
    parsedPath* ppath = malloc(sizeof(parsedPath));
    //init ppath member variables
    ppath->pathArray = malloc(sizeof(char) * MAX_CHAR * TEST_MAX_INDEX);
    ppath->pathSize = 0;
    ppath->absPath = 0;
    ppath->relPath = 0;
    ppath->parPath = 0;
    //save pointer for strtok_r
    char * tokP;
    //copy of pathname to be passed in strtok_r
    char * copy = malloc(strlen(pathname));
    //check if absolute path
    if (pathname[0] == '/'){
        ppath->absPath = 1;
        if(strlen(pathname) == 1){
          ppath->pathArray[0] = " ";
          return ppath;
        }
        //check for invalid consecutive /
        if(strlen(pathname) > 1 && pathname[1] == '/'){
        if (ppath != NULL){
          freePath(ppath);
          ppath = NULL;
        }
        return NULL;
      }
    }
    strcpy(copy, pathname);
    //first token
    char* token1 = strtok_r(copy, "/", &tokP);
    //check if invalid consecutive /
    if (*tokP == '/'){
        if (ppath != NULL){
          freePath(ppath);
          ppath = NULL;
        }
        return NULL;
    }
    if (strcmp(token1, ".") == 0){ //check if its relative path
        ppath->relPath = 1; 
    } else if (strcmp(token1, "..") == 0){ //check if its relative to parent
        ppath->parPath = 1;
    } else { //first value is a path name relative to current directory 
      ppath->pathArray[ppath->pathSize] = malloc(strlen(token1)+1);
        strcpy(ppath->pathArray[ppath->pathSize], token1);
        ppath->pathSize++;
        if(ppath->absPath == 0){
          ppath->relPath = 1; 
        } 
    }
    //token2 will be used to tokenize remaining path levels
    char * token2 = strtok_r(NULL, "/", &tokP); //get second token
        //add tokenized path sections to pathArray
    while (token2 != NULL && ppath->pathSize < TEST_MAX_INDEX){
        //check for invalid consecutive iterations of /
        if (*tokP == '/'){
          if (ppath != NULL){
          freePath(ppath);
          ppath = NULL;
          }
          return NULL;
        }
        //add token to pathArray and increment array element counter
        ppath->pathArray[ppath->pathSize] = malloc(strlen(token2)+1);
        strcpy(ppath->pathArray[ppath->pathSize], token2);
        ppath->pathSize++;
        token2 = strtok_r(NULL, "/", &tokP);
    }
    if (copy != NULL){
      free(copy);
      copy = NULL;
    }
    return ppath;
}

//helper function to free all memory allocated to parsedPath struct
void freePath(parsedPath* ppath){
  if (ppath->pathSize > 0){
    for (int i = 0; i < ppath->pathSize; i++){
        free(ppath->pathArray[i]);
        ppath->pathArray[i] = NULL;
    }
  }
    free(ppath);
}

//validator for the parsedPath
//loads the root directory into memory
//checks from current directory
// current and parent will be root if it is the first call
int validatePath(parsedPath *ppath){
  init_loadedDir();
  //direntry pointers that will be used to index through
  //directory entries found within the path
  direntry * indexD = malloc(BLOCK_SIZE * 12);
  //saves the index's parent
  direntry * iparentD = malloc(BLOCK_SIZE * 12);;
  //the return value that displays the index of the parent
  //which the path name exists on
  //value is default set to -1 that shows the path is invalid
  int indexOfParent = -1;
  //special condition for just root directory
  if (ppath->absPath == 1 && ppath->pathSize == 0){
//    //printf("inside / path\n");
    indexOfParent = 0;
    parentD = rootD;
    lastPathD = rootD;
    if(ppath != NULL){
      freePath(ppath);
      ppath = NULL;
    }
    return indexOfParent;
  }
  if (ppath->absPath == 1){
    indexD = rootD;
  } else if (ppath->parPath == 1){
    indexD = parentD;
  } else {
    indexD = lastPathD;
  }
  for (int i = 0; i < ppath->pathSize; i++){
    //check if each path exists within the directory
    //starts from 2 as . and .. are the first 2 indexes
    for(int j = 0; j < indexD->entries; j++){
      //if the path exists, change the current directory 
      //that matches the pathname and continue to the 
      //next path value
      //updates the parent directory to the current directory
      //then updates the current directory to the newly found
      //directory entry
      if(strcmp(ppath->pathArray[i],indexD[j].name) == 0){
        //might need to figure out a way to free some of these****************************************************
        //may run into malloc problems
        iparentD = indexD;
        loadDir(indexD, j);
        indexOfParent = j;
        //upon finding the final valid path
        //function will be complete and update the parent
        //and current directories
        //This will prevent the corruption of loaded current
        //and parent directories when the path is invalid
        if(i == ppath->pathSize - 1){
          parentD = iparentD;
          lastPathD = indexD;
          free(indexD);
          free(iparentD);
        }
        break;
      }
    }
    //loop ends if the path does not exist
    if(ppath != NULL){
      freePath(ppath);
      ppath = NULL;
    }
    if (indexD != NULL){
      free(indexD);
      indexD = NULL;
    }
    if (iparentD != NULL){
      free(iparentD);
      iparentD = NULL;
    }
    
    indexOfParent = -1;
    return indexOfParent;
  } //end of first for loop
  //last path has been checked, last path is not valid
  //free all values and return negative
  if(ppath != NULL){
      freePath(ppath);
      ppath = NULL;
  }
  if (indexD != NULL){
    free(indexD);
    indexD = NULL;
  }
  if (iparentD != NULL){
    free(iparentD);
    iparentD = NULL;
  }
  indexOfParent = -1;
  return indexOfParent;
}

//helper function to free loadedDirectories
void freeLoadedDir(void){
  if(rootD != NULL){
    free(rootD);
    rootD = NULL;
  }
  if (parentD != NULL){
    free(parentD);
    parentD = NULL;
  }
  if (currentD != NULL){
    free(currentD);
    currentD = NULL;
  }
  init = 0;
}