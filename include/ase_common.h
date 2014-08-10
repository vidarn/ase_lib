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

/* ------------------ Public symbols -------------------- */

typedef enum{
    ASE_COLORTYPE_RGB,
    ASE_COLORTYPE_CMYK,
    ASE_COLORTYPE_LAB,
    ASE_COLORTYPE_GRAY
} ASE_COLORTYPE;

typedef enum{
    ASE_ERRORTYPE_SUCCESS,
    ASE_ERRORTYPE_COULD_NOT_OPEN_FILE,
    ASE_ERRORTYPE_INVALID_FILE,
    ASE_ERRORTYPE_INVALID_ASE,
    ASE_ERRORTYPE_UNEXPECTED_EOF
} ASE_ERRORTYPE;

typedef struct{
    uint16_t *name;
    ASE_COLORTYPE type;
    float col[4];
} ASE_COLOR;

typedef struct{
    uint16_t *name;
    uint16_t numColors;
    ASE_COLOR *colors;
} ASE_GROUP;

typedef struct{
    uint16_t numGroups;
    uint16_t version[2];
    ASE_GROUP *groups;
} ASE_FILE;

static void ase_freeAseFile(ASE_FILE *ase);
static const char* ase_getErrorString(ASE_ERRORTYPE error);

/*  ---------------- Implementation ----------------------- */

typedef enum{
    ASE_BLOCKTYPE_GROUP_START,
    ASE_BLOCKTYPE_GROUP_END,
    ASE_BLOCKTYPE_COLOR
} ASE_BLOCKTYPE;


static void ase_freeAseFile(ASE_FILE *ase)
{
    uint16_t i, ii;
    ASE_GROUP *group;
    ASE_COLOR *color;
    for(i=0;i<ase->numGroups;i++){
        group = ase->groups + i;
        for(ii=0;ii<group->numColors;ii++){
            color = group->colors + ii;
            free(color->name);
        }
        free(group->colors);
        free(group->name);
    }
    free(ase->groups);
    ase->numGroups = 0;
}

static const wchar_t* ase_getErrorString(ASE_ERRORTYPE error)
{
    switch(error){
        case ASE_ERRORTYPE_SUCCESS:
            return L"Success";
        case ASE_ERRORTYPE_COULD_NOT_OPEN_FILE:
            return L"Could not open file";
        case ASE_ERRORTYPE_INVALID_FILE:
            return L"Invalid file";
        case ASE_ERRORTYPE_INVALID_ASE:
            return L"Invalid ase";
        case ASE_ERRORTYPE_UNEXPECTED_EOF:
            return L"Unexpected end-of-file";
    }
    return L"Unknown";
}
