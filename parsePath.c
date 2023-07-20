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
 * Description: Handler for directory entries. Creates directory entries,
 *              and other helper functions to facilitate modifications of
 *              directory entries.
 *
 * 
 **************************************************************/


#include "parsePath.h"
#include <stdio.h>
#include <stdlib.h>


#define MAX_CHAR 100
#define TEST_MAX_INDEX 20

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
        //check for invalid consecutive /
        if(strlen(pathname) > 1 && pathname[1] == '/'){
        freePath(ppath);
        return NULL;
      }
    }
    strcpy(copy, pathname);
    //first token
    char* token1 = strtok_r(copy, "/", &tokP);
    //check if invalid consecutive /
    if (*tokP == '/'){
        freePath(ppath);
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
          freePath(ppath);
          return NULL;
        }
        //add token to pathArray and increment array element counter
        ppath->pathArray[ppath->pathSize] = malloc(strlen(token2)+1);
        strcpy(ppath->pathArray[ppath->pathSize], token2);
        ppath->pathSize++;
        token2 = strtok_r(NULL, "/", &tokP);
    }
    free(copy);
    return ppath;
}

//helper function to free all memory allocated to parsedPath struct
void freePath(parsedPath* ppath){
    for (int i = 0; i < ppath->pathSize; i++){
        free(ppath->pathArray[i]);
        ppath->pathArray[i] = NULL;
    }
    free(ppath->pathArray);
    free(ppath);
    ppath = NULL;
}