/*
 * File: sim.c
 * Braeden Worcester. I wrote this.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>


typedef struct Data {
    int valid;
    int tag;
    clock_t lastAccessed;
} Data;

typedef struct Row {
    Data blocks[16];
} Row;

typedef struct Cache {
    int numberOfBlocks;
    int tagSize;
    int indexSize;
    int offsetSize;
    int numberOfRows;
    int overheadSize;
    double implementationSize;
    double cost;
} Cache;

int ParseArgs(int argc, char *arvg[]);
Cache *BuildCache();
int PopulateCache(Cache *cache);
int CheckCache(Cache *cache, int address, int bytesRead);
int InsertIntoCache(Cache *cache, int tag, int index);

char *fileName;
int cacheSize;
int blockSize;
int assoc;
char *replacePolicy;

Row *rows;

int attempts = 0;
int hits = 0;
int misses = 0;
int compulsory = 0;
int conflict = 0;
int accesses = 0;
int instructions = 0;
int cycles = 0;

int analysisMode = 0; // if set to 1, the program will print results to CSV file for the use in a spread sheet.
char *analyzedInput; // which input will be printed alongside the results: s, b, a, r

int main(int argc, char *arvg[])
{
    if (ParseArgs(argc, arvg) != 0){
        printf("error: one or more inputs missing\n");
        printf("usage: -f [trace file name] -s [cache size in KB] -b [block size in bytes] -a [associativity] -r [replacement policy RR or RND or LRU]\n");
        return -1;
    }

    printf("\nCache Simulator CS 3853 Spring 2021- Group #18\n\n");

    printf("Trace File: %s\n", fileName);

    printf("\n***** Cache Input Parameters *****\n");
    printf("Cache Size: %d KB\n", cacheSize);
    printf("Block Size: %d bytes\n", blockSize);
    printf("Associativity: %d\n", assoc);
    if (strcmp(replacePolicy, "RR") == 0){
        printf("Replacement Policy: Round Robin\n");
    }
    else if (strcmp(replacePolicy, "RND") == 0){
        printf("Replacement Policy: Random\n");
    }
    else if (strcmp(replacePolicy, "LRU") == 0){
        printf("Replacement Policy: Least Recently Used\n");
    }
    

    Cache *cache = BuildCache();

    printf("\n***** Cache Calculated Values *****\n\n");

    printf("Total # Blocks: %d\n", cache->numberOfBlocks);
    printf("Tag Size: %d bits\n", cache->tagSize);
    printf("Index Size: %d bits\n", cache->indexSize);
    printf("Total # Rows: %d\n", cache->numberOfRows);
    printf("Overhead Size: %d bytes\n", cache->overheadSize);
    printf("Implementation Memory Size: %0.1lf KB\n", cache->implementationSize);
    printf("Cost: $%0.2lf\n", cache->cost);

    int code = PopulateCache(cache);
    if (code != 0){
        return -1;
    }

    printf("\n***** Cache Simulation Results *****\n\n");

    printf("Total Cache Accesses: %d\n", accesses);
    printf("Cache Hits: %d\n", hits);
    printf("Cache Misses: %d\n", misses);
    printf("--- Compulsory Misses: %d\n", compulsory);
    printf("--- Conflict Misses: %d\n", conflict);

    printf("\n***** ***** Cache Hit & Miss Rate: ***** *****\n\n");

    printf("Hit Rate: %.4lf%%\n", (double)hits/accesses*100);
    printf("Miss Rate: %.4lf%%\n", (double)misses/accesses*100);
    printf("CPI: %.2lf Cycles/Instruction\n", (double)cycles/instructions);
    printf("Unused Cache Space: %.2lf KB / %.2lf KB = %.2lf%%   Waste: $%.2lf\n",   (((double)cache->numberOfBlocks-compulsory)*blockSize)/1024, 
                                                                                    ((double)cache->numberOfBlocks/1024)*blockSize, 
                                                                                    ((double)cache->numberOfBlocks-compulsory)/(double)cache->numberOfBlocks*100,
                                                                                    (((double)cache->numberOfBlocks-compulsory)*blockSize) / 1024 * 0.09);
    printf("Unused Cache Blocks: %d / %d\n", cache->numberOfBlocks-compulsory, cache->numberOfBlocks);

    if (analysisMode == 1){
        char input[100];
        FILE *sheet;
        if (strcmp(analyzedInput, "s") == 0) {
            if( access("Size Data.csv", F_OK ) != 0 ) {
                sheet = fopen("Size Data.csv", "a");
                fputs("Cache Size (KB),Cache Hit Rate,Cache Miss Rate,CPI,Wasted Space,Waste in Dollars\n", sheet);
            }
            else {
                sheet = fopen("Size Data.csv", "a");
            }
            sprintf(input, "%d,", cache->numberOfBlocks / 1024 * (int)pow(2, cache->offsetSize));
            fputs(input, sheet);
        }
        if (strcmp(analyzedInput, "b") == 0) {
            if( access("Block Size Data.csv", F_OK ) != 0 ) {
                sheet = fopen("Block Size Data.csv", "a");
                fputs("Block Size (Bytes),Cache Hit Rate,Cache Miss Rate,CPI,Wasted Space,Waste in Dollars\n", sheet);
            }
            else {
                sheet = fopen("Block Size Data.csv", "a");
            }
            sprintf(input, "%d,", (int)pow(2, cache->offsetSize));
            fputs(input, sheet);
        }
        if (strcmp(analyzedInput, "a") == 0) {
            if( access("Associativity Data.csv", F_OK ) != 0 ) {
                sheet = fopen("Associativity Data.csv", "a");
                fputs("Associativity,Cache Hit Rate,Cache Miss Rate,CPI,Wasted Space,Waste in Dollars\n", sheet);
            }
            else {
                sheet = fopen("Associativity Data.csv", "a");
            }
            sprintf(input, "%d,", assoc);
            printf("input: %s\n", input);
            fputs(input, sheet);
        }
        if (strcmp(analyzedInput, "r") == 0) {
            if( access("Replacement Data.csv", F_OK ) != 0 ) {
                sheet = fopen("Replacement Data.csv", "a");
                fputs("Replacement Policy,Cache Hit Rate,Cache Miss Rate,CPI,Wasted Space,Waste in Dollars\n", sheet);
            }
            else {
                sheet = fopen("Replacement Data.csv", "a");
            }
            sprintf(input, "%s,", replacePolicy);
            fputs(input, sheet);
        }
        sprintf(input, "%lf%%,", (double)hits/accesses*100);
        fputs(input,sheet);
        sprintf(input, "%lf%%,", (double)misses/accesses*100);
        fputs(input,sheet);
        sprintf(input, "%lf,", (double)cycles/instructions);
        fputs(input,sheet);
        sprintf(input, "%lf%%,", ((double)cache->numberOfBlocks-compulsory)/(double)cache->numberOfBlocks*100);
        fputs(input,sheet);
        sprintf(input, "$%lf,", (((double)cache->numberOfBlocks-compulsory)*blockSize) / 1024 * 0.09);
        fputs(input,sheet);
        sprintf(input, "$%lf", cache->cost);
        fputs(input,sheet);
        fputs("\n", sheet);
        fclose(sheet);
    }

    free(cache);
    free(rows);
    printf("\n");
    return 0;
}

int PopulateCache(Cache *cache){
    FILE *file = fopen(fileName, "r");
    if (file == NULL){
        printf("%s not found\n", fileName);
        return -1;
    }

    int i = 0;
    char buff[1000];
    while (fgets(buff, 1000, file) != NULL){

        int bytesRead;
        int address;
        int dstAddress;
        int srcAddress;
        
        if (sscanf(buff, "EIP (%d): %x", &bytesRead, &address) == 2){
            CheckCache(cache, address, bytesRead);
            cycles += 2;
            instructions++;
            i++;
        }
        else {
            if (sscanf(buff, "dstM: %x --------    srcM: %x", &dstAddress, &srcAddress) != 0 ){
                if (dstAddress != 0){
                    CheckCache(cache, dstAddress, 4);
                    cycles++;
                    i++;
                }
                if (srcAddress != 0){
                    CheckCache(cache, srcAddress, 4);
                    cycles++;
                    i++;
                }
            }
        }
    }

    fclose(file);
    return 0;
}

Cache *BuildCache(){
    int cacheBytes = cacheSize * 1024;
    if (cacheBytes != (int)pow(2, ceil(log(cacheBytes) / log(2)))){
        printf("warning: cache size not a power of 2, rounding up to %d KB\n", (int)pow(2, ceil(log(cacheBytes) / log(2))) / 1024);
        cacheBytes = (int)pow(2, ceil(log(cacheBytes) / log(2)));
    }

    Cache *cache = malloc(sizeof(Cache));

    cache->offsetSize = ceil(log(blockSize) / log(2));
    if (blockSize != (int)pow(2, cache->offsetSize)){
        printf("warning: block size not a power of 2, rounding up to %d bytes\n", (int)pow(2, cache->offsetSize));
        blockSize = (int)pow(2, cache->offsetSize);
    }

    cache->numberOfBlocks = (double)cacheBytes / blockSize;

    cache->numberOfRows = (double)cache->numberOfBlocks / assoc;

    cache->indexSize = ceil(log(cache->numberOfRows) / log(2));
    cache->numberOfRows = (int)pow(2, cache->indexSize);

    cache->tagSize = 32 - cache->indexSize - cache->offsetSize;

    cache->overheadSize = ceil(((cache->tagSize + 1) * cache->numberOfBlocks)/8.0);

    cache->implementationSize = (cache->overheadSize / 1024.0) + (double)cacheSize;

    cache->cost = cache->implementationSize * 0.09;

    rows = malloc(sizeof(Row) * cache->numberOfRows);

    int i; int x;
    for (i = 0; i < cache->numberOfRows; i++){
        for (x = 0; x < assoc; x++){
            rows[i].blocks[x].valid = 0;
        }
    }

    return cache;
}

int ParseArgs(int argc, char *arvg[]){

    int setF = 0;
    int setC = 0;
    int setB = 0;
    int setA = 0;
    int setR = 0;

    int i;
    for (i = 1; i < argc; i++){
        if      (strcmp(arvg[i], "-f") == 0){
            i++;
            fileName = arvg[i];
            setF++;
        }
        else if (strcmp(arvg[i], "-s") == 0){
            i++;
            cacheSize = atoi(arvg[i]);
            setC++;
        }
        else if (strcmp(arvg[i], "-b") == 0){
            i++;
            blockSize = atoi(arvg[i]);
            setB++;
        }
        else if (strcmp(arvg[i], "-a") == 0){
            i++;
            assoc = atoi(arvg[i]);
            setA++;
        }
        else if (strcmp(arvg[i], "-r") == 0){
            i++;
            replacePolicy = arvg[i];
            setR++;
        }
        else if (strcmp(arvg[i], "-x") == 0){
            analysisMode = 1;
            i++;
            analyzedInput = arvg[i];
        }
        else {
            printf("warning: invalid argument '%s'\n", arvg[i]);
        }
    }

    if (setF == 0 || setC == 0 || setB == 0 || setA == 0 || setR == 0){
        return -1;
    }
    else if (strcmp(replacePolicy, "RR") != 0 && strcmp(replacePolicy, "RND") != 0 && strcmp(replacePolicy, "LRU") != 0){
        return -2;
    }
    else {
        return 0;
    }
}

int CheckCache(Cache *cache, int address, int bytesRead){
    int tag; int index; int offset;
    
    tag = address >> (cache->indexSize + cache->offsetSize);

    index = address;
    int i;
    for (i=31; i > cache->offsetSize + cache->indexSize-1; i--){
        index &= ~(1 << i);      
    }
    index = index >> cache->offsetSize;

    offset = address;
    for (i=31; i > cache->offsetSize-1; i--){
        offset &= ~(1 << i);      
    }
   
    int finalIndex = address + bytesRead;
    for (i=31; i > cache->offsetSize + cache->indexSize-1; i--){
        finalIndex &= ~(1 << i);      
    }
    finalIndex = finalIndex >> cache->offsetSize;

    InsertIntoCache(cache, tag, index);

    if (finalIndex != index){
        InsertIntoCache(cache, tag, finalIndex);
    }

    
    return 0;
}

int InsertIntoCache(Cache *cache, int tag, int index){
    int i;
    int found = 0;
    for (i = 0; i < assoc; i++){
        if (rows[index].blocks[i].tag == tag && rows[index].blocks[i].valid == 1){
            hits++;
            if (strcmp(replacePolicy, "LRU") == 0){             // if we gatekeep this for only LRU, then we don't have to worry about making special data for just RR
                rows[index].blocks[i].lastAccessed = clock();
            }
            found = 1;
            cycles++;
            break;
        }
    }
    if (found == 0){
        if ((ceil(blockSize / 4)) > 1){                     // please don't ask me. I don't know either
            cycles += (4 * ceil(blockSize / 4)); 
       }
       else {
           cycles += 4; 
       }
        misses++;
        for (i = 0; i < assoc; i++){
            if (rows[index].blocks[i].valid == 0){
                rows[index].blocks[i].lastAccessed = clock();
                rows[index].blocks[i].valid = 1;
                rows[index].blocks[i].tag = tag;
                found = 1;
                compulsory++;
                break;
            }
        }
        if (found == 0){
            conflict++;
            if (strcmp(replacePolicy, "LRU") == 0 || strcmp(replacePolicy, "RR") == 0){ // LRU and RR can use the same implementation here, since we make sure lastAccessed only updates when replaced if we're not using LRU
                int LRU = 0;
                clock_t LRUTime = rows[index].blocks[0].lastAccessed;
                for (i = 1; i < assoc; i++){
                    if (rows[index].blocks[i].lastAccessed < LRUTime){
                        LRUTime = rows[index].blocks[i].lastAccessed;
                        LRU = i;
                    }
                }
                rows[index].blocks[LRU].lastAccessed = clock();
                rows[index].blocks[LRU].tag = tag;
            }
            if (strcmp(replacePolicy, "RND") == 0){
                srand(time(NULL));
                int random = (rand() % (assoc));
                rows[index].blocks[random].tag = tag;
            }
        }
    }
    accesses++;
    return 0;
}