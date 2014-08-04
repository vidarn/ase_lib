/*
   The MIT License (MIT)

   Copyright (c) 2014 Vidar Nelson

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/
#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <wchar.h>
#include <string.h>
#include <assert.h>

// ------------------ Public symbols --------------------

typedef enum{
    COLORTYPE_RGB,
    COLORTYPE_CMYK,
    COLORTYPE_LAB,
    COLORTYPE_GRAY,
} COLORTYPE;

typedef struct{
    wchar_t *name;
    COLORTYPE type;
    float col[4];
} COLOR;

typedef struct{
    wchar_t *name;
    uint16_t numColors;
    COLOR *colors;
} GROUP;

typedef struct{
    uint16_t numGroups;
    GROUP *groups;
} ASE_FILE;

ASE_FILE readAseFile(FILE *f);
ASE_FILE openAndReadAseFile(const char *filename);
void freeAseFile(ASE_FILE *file);


//  ---------------- Implementation -----------------------

static uint16_t be16_to_cpu(const uint8_t *buf)
{
    return ((uint16_t)buf[1]) | (((uint16_t)buf[0]) << 8);
}

static uint32_t be32_to_cpu(const uint8_t *buf)
{
    return  (((uint32_t)buf[0]) << 8*3) | (((uint32_t)buf[1]) << 8*2) | (((uint32_t)buf[2]) << 8) | ((uint32_t)buf[3]);
}

static void read_uint16(uint16_t *dest, uint16_t num, FILE *f)
{
    assert(!feof(f));
    uint8_t tmp[2];
    for(uint16_t i=0;i<num;i++){
        fread(tmp,sizeof(uint16_t),1,f);
        dest[i] = be16_to_cpu(tmp);
    }
}

static void read_uint32(uint32_t *dest, uint32_t num, FILE *f)
{
    assert(!feof(f));
    uint8_t tmp[4];
    for(uint32_t i=0;i<num;i++){
        fread(tmp,sizeof(uint32_t),1,f);
        dest[i] = be32_to_cpu(tmp);
    }
}

typedef enum{
    BLOCKTYPE_GROUP_START,
    BLOCKTYPE_GROUP_END,
    BLOCKTYPE_COLOR,
} BLOCKTYPE;


static BLOCKTYPE readBlock(float color[4],COLORTYPE *colorType, wchar_t **name,FILE *f)
{
    char model[5];
    uint16_t blockType = 0;
    uint32_t blockLength = 0;
    uint16_t nameLength = 0;
    read_uint16(&blockType,1,f);
    switch(blockType){
        case 0xc001:
            blockType = BLOCKTYPE_GROUP_START;
            break;
        case 0xc002:
            blockType = BLOCKTYPE_GROUP_END;
            break;
        case 0x0001:
            blockType = BLOCKTYPE_COLOR;
            break;
    }
    read_uint32(&blockLength, 1, f);
    if(blockLength > 0){
        read_uint16(&nameLength, 1, f);
        if(nameLength > 0){
            *name = (wchar_t *)malloc(sizeof(wchar_t) * nameLength);
            for(uint16_t i=0;i<nameLength;i++){
                uint16_t tmp;
                read_uint16(&tmp,1,f);
                (*name)[i] = (wchar_t)tmp;
            }
        }
        if(blockType == BLOCKTYPE_COLOR){
            uint32_t tmp[4];
            uint16_t type, numVars;
            model[4] = 0;
            assert(!feof(f));
            fread(model,sizeof(char),4,f);
            if(strcmp(model,"RGB ") == 0){
                *colorType = COLORTYPE_RGB;
                numVars = 3;
            } else if(strcmp(model,"LAB ") == 0){
                *colorType = COLORTYPE_LAB;
                numVars = 3;
            } else if(strcmp(model,"CMYK") == 0){
                *colorType = COLORTYPE_CMYK;
                numVars = 4;
            } else if(strcmp(model,"GRAY") == 0){
                *colorType = COLORTYPE_GRAY;
                numVars = 1;
            }
            else {
                // Error!
            }
            read_uint32(tmp,numVars,f);
            for(uint8_t i = 0; i<4;i++){
                if(i < numVars){
                    color[i] = *(float *)&tmp[i];
                } else{
                    color[i] = -1.f;
                }
            }
            read_uint16(&type,1,f);
        }
    }
    return (BLOCKTYPE)blockType;
}

ASE_FILE openAndReadAseFile(const char *filename)
{
    FILE *f = fopen(filename,"r");
    ASE_FILE file;
    file = readAseFile(f);
    fclose(f);
    return file;
}

ASE_FILE readAseFile(FILE *f)
{
    float c[4];
    COLORTYPE colorType;
    COLOR *col;
    wchar_t *name;
    char sig[5];
    uint16_t version[2];
    uint32_t numBlocks;
    ASE_FILE file;
    file.numGroups = 0;
    file.groups = NULL;
    GROUP *currentGroup = NULL;
    assert(!feof(f));
    fread(sig,sizeof(char),4,f);
    sig[4] = 0;
    read_uint16(version,2,f);
    read_uint32(&numBlocks, 1, f);
    for(uint16_t i=0;i<numBlocks;i++){
        BLOCKTYPE type = readBlock(c,&colorType,&name,f);
        switch(type){
            case BLOCKTYPE_GROUP_START:
                file.numGroups++;
                file.groups = (GROUP *)realloc(file.groups,file.numGroups*sizeof(GROUP));
                currentGroup = file.groups + file.numGroups - 1;
                currentGroup->name = name;
                currentGroup->numColors = 0;
                currentGroup->colors = NULL;
                break;
            case BLOCKTYPE_COLOR:
                currentGroup->numColors++;
                currentGroup->colors = (COLOR *)realloc(currentGroup->colors,currentGroup->numColors*sizeof(COLOR));
                col = currentGroup->colors + currentGroup->numColors - 1;
                col->name = name;
                col->type = colorType;
                for(uint16_t ii=0;ii<4;ii++){
                    col->col[ii] = c[ii];
                }
                break;
            case BLOCKTYPE_GROUP_END:
                break;
        }
    }
    return file;
}

void freeAseFile(ASE_FILE *file)
{
    for(uint16_t i=0;i<file->numGroups;i++){
        GROUP *group = file->groups + i;
        for(uint16_t ii=0;ii<group->numColors;ii++){
            COLOR *color = group->colors + ii;
            free(color->name);
        }
        free(group->colors);
        free(group->name);
    }
    free(file->groups);
    file->numGroups = 0;
}

