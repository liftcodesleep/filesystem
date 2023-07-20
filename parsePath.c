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
#include <stdlib.h>



//parsed path container
//OBSOLETE WILL DELETE LATER****************************
parsedPath* ppath;
#define MAX_CHAR 8
//indicator if ppath has been initialized
int p_init = 0;

//initializer for ppath array member
void init_parsedPath(){
    if (p_init == 1 ){
        return;
    }
    for (int i = 0; i < TEST_MAX_INDEX ; i++){
        ppath->pathArray[i] = NULL;
    }
    ppath->pathSize = 0;
    ppath->absPath = 0;
    ppath->relPath = 0;
    ppath->parPath = 0;
    p_init = 1;

}

//returns based on successful parsing
//returns pointer to parsedPath struct or NULL if it fails
//future plan to validate paths using 
parsedPath* parsePath(const char* pathname){
    //allocate space
    parsedPath* ppath = (parsedPath*)malloc(sizeof(char*) * TEST_MAX_INDEX * MAX_CHAR + sizeof(int) *4 + 1);
    //init ppath member variables
    ppath->pathSize = 0;
    ppath->absPath = 0;
    ppath->relPath = 0;
    ppath->parPath = 0;
    //save pointer for strtok_r     
    char * tokP;
    //copy of pathname to be passed in strtok_r
    char * copy = malloc(strlen(pathname));
    //check if relative path or absolute
    if (pathname[0] == '/'){
        ppath->absPath = 1;
    }
    strcpy(copy, pathname);
    //first token
    char* token1 = strtok_r(copy, "/", &tokP);
    if (strcmp(token1, "\0") == 0){ //Invalid path
        printf("INVALID PATH\n");
        return NULL;
    }
    else if (strcmp(token1, ".") == 0){ //check if its relative path
        ppath->relPath = 1; 
    } else if (strcmp(token1, "..") == 0){ //check if its relative to parent
        ppath->parPath = 1;
    } else {
        //allocate space for each entry
        ppath->pathArray[ppath->pathSize] = malloc(strlen(token1)+1);
        strcpy(ppath->pathArray[ppath->pathSize], token1);
        ppath->pathSize++;
    }
    //token2 will be used to tokenize remaining path levels
    char * token2 = strtok_r(NULL, "/", &tokP); //get second token
    if(token2 == NULL){ //END of path
        //use findIndex to find the index of directory entry
    } else {
        //add tokenized path sections to pathArray
        while (token2 != NULL && ppath->pathSize < TEST_MAX_INDEX){
            //allocate space for each entry
            ppath->pathArray[ppath->pathSize] = malloc(strlen(token2)+1);
            strcpy(ppath->pathArray[ppath->pathSize], token2);
            ppath->pathSize++;
            token2 = strtok_r(NULL, "/", &tokP);
        }
    }
    return ppath;
}

//helper function to free memory of parsedPath
void freePath(parsedPath* ppath){
    for (int i = 0; i < ppath->pathSize; i++){
        free(ppath->pathArray[i]);
    }
    free(ppath);
}