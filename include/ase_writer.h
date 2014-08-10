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
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.  */

#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <wchar.h>
#include <string.h>
#include <assert.h>

/* ------------------ Public symbols -------------------- */

static ASE_ERRORTYPE ase_writeAseFile(ASE_FILE *ase, FILE *f);
static ASE_ERRORTYPE ase_openAndWriteAseFile(ASE_FILE *ase, const char *filename);

/*  ---------------- Implementation ----------------------- */

static void ase_cpu_to_be16(uint8_t *buf, uint16_t val)
{
    buf[0] = (val & 0xFF00) >> 8;
    buf[1] = (val & 0x00FF);
}

static void ase_cpu_to_be32(uint8_t *buf, uint32_t val)
{
    buf[0] = (val & 0xFF0000) >> 16;
    buf[1] = (val & 0x00FF00) >> 8;
    buf[2] = (val & 0x0000FF);
}

static ASE_ERRORTYPE ase_write_uint16(uint16_t *val, uint16_t num, FILE *f)
{
    uint8_t tmp[2];
    uint16_t i;
    if(feof(f)) return ASE_ERRORTYPE_UNEXPECTED_EOF;
    for(i=0;i<num;i++){
        ase_cpu_to_be16(tmp,val[i]);
        fwrite(tmp,sizeof(uint16_t),1,f);
    }
    return ASE_ERRORTYPE_SUCCESS;
}

static ASE_ERRORTYPE ase_write_uint32(uint32_t *val, uint16_t num, FILE *f)
{
    uint8_t tmp[3];
    uint16_t i;
    if(feof(f)) return ASE_ERRORTYPE_UNEXPECTED_EOF;
    for(i=0;i<num;i++){
        ase_cpu_to_be32(tmp,val[i]);
        fwrite(tmp,sizeof(uint32_t),1,f);
    }
    return ASE_ERRORTYPE_SUCCESS;
}

static ASE_ERRORTYPE ase_openAndWriteAseFile(ASE_FILE *ase, const char *filename)
{
    ASE_ERRORTYPE error;
    FILE *f = fopen(filename,"w");
    if(f){
        error = ase_writeAseFile(ase,f);
        fclose(f);
    }
    else{
        error = ASE_ERRORTYPE_COULD_NOT_OPEN_FILE;
    }
    return error;
}

static ASE_ERRORTYPE ase_write_block(ASE_BLOCKTYPE blockType, ASE_COLOR *color, const wchar_t *name, FILE *f)
{
    uint16_t tmpBlockType;
    uint32_t blockLen;
    switch(blockType){
        case ASE_BLOCKTYPE_GROUP_START:
            tmpBlockType =  0xc001;
            break;
        case ASE_BLOCKTYPE_GROUP_END:
            tmpBlockType =  0xc002;
            break;
        case ASE_BLOCKTYPE_COLOR:
            tmpBlockType =  0x0001;
            break;
        default:
            return ASE_ERRORTYPE_INVALID_ASE;
    }
    ase_write_uint16(&tmpBlockType,1,f);
#pragma message("TODO: write block length correctly")
    blockLen = 20;
    ase_write_uint32(&blockLen,1,f);
    if(name != NULL){
        uint16_t nameLen = wcslen(name);
        ase_write_uint16(&nameLen,1,f);
        ase_write_uint16((uint16_t*)name,nameLen,f);
    }
    else{
        tmpBlockType = 0;
        ase_write_uint16(&tmpBlockType,1,f);
    }
    if(blockType == ASE_BLOCKTYPE_COLOR){
        char *model;
        uint16_t numVars, i;
        switch(color->type){
            case ASE_COLORTYPE_RGB:
                model = "RGB "; numVars = 3;
                break;
            case ASE_COLORTYPE_CMYK:
                model = "CMYK"; numVars = 4;
                break;
            case ASE_COLORTYPE_LAB:
                model = "LAB "; numVars = 3;
                break;
            case ASE_COLORTYPE_GRAY:
                model = "GRAY"; numVars = 1;
                break;
            default:
                return ASE_ERRORTYPE_INVALID_ASE;
        }
        fwrite(model,sizeof(char),4,f);
        for(i=0;i<numVars;i++){
            ase_write_uint32((uint32_t *)(color->col + i),1,f);
        }
        i=2;
        ase_write_uint16(&i,1,f);
    }
    return ASE_ERRORTYPE_SUCCESS;
}

static ASE_ERRORTYPE ase_writeAseFile(ASE_FILE *ase, FILE *f)
{
    uint16_t i;
    uint32_t numBlocks = 0;
    fwrite("ASEF",sizeof(char),4,f);
    ase_write_uint16(ase->version,2,f);
    for(i=0;i<ase->numGroups;i++){
        numBlocks += ase->groups[i].numColors + 2;
    }
    wprintf(L"numBlocks: %d\n",numBlocks);
    ase_write_uint32(&numBlocks,1,f);
    for(i=0;i<ase->numGroups;i++){
        uint16_t ii;
        ASE_GROUP *group = ase->groups + i;
        ase_write_block(ASE_BLOCKTYPE_GROUP_START,NULL,group->name,f);
        for(ii=0;ii<group->numColors;ii++){
            ase_write_block(ASE_BLOCKTYPE_COLOR,group->colors+ii,group->colors[ii].name,f);
        }
        ase_write_block(ASE_BLOCKTYPE_GROUP_END,NULL,NULL,f);
    }
    return ASE_ERRORTYPE_SUCCESS;
}

