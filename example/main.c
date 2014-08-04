#include <stdio.h>
#include "ase_loader.h"
int main(int argc, char **argv)
{
    ASE_FILE ase;
    ERRORTYPE error;
    uint16_t i, ii, a, numFiles = 1;
    const char *filenames[] = {"data/example.ase"};
    for(a=0;a<numFiles;a++){
        wprintf(L"Loading file %s\n", filenames[a]);
        error = openAndReadAseFile(&ase, filenames[a]);
        if(!error){
            for(i=0;i<ase.numGroups;i++){
                GROUP *group = ase.groups + i;
                wprintf(L"  Group \"%ls\"\n", group->name);
                for(ii=0;ii<group->numColors;ii++){
                    COLOR *color = group->colors + ii;
                    uint16_t iii = 0;
                    wchar_t *typeName;
                    switch(color->type){
                        case COLORTYPE_RGB:  typeName = L"RGB";  break;
                        case COLORTYPE_CMYK: typeName = L"CMYK"; break;
                        case COLORTYPE_LAB:  typeName = L"LAB";  break;
                        case COLORTYPE_GRAY: typeName = L"Gray"; break;
                    }
                    wprintf(L"    Color [%ls] \"%ls\"\n    ",typeName,color->name);
                    while(color->col[iii] > 0 && iii < 4){
                        wprintf(L"%f ", color->col[iii]);
                        iii++;
                    }
                    wprintf(L"\n");
                }
            }
            freeAseFile(&ase);
        }
        else{
            wprintf(L"  Error occured: %ls\n", getErrorString(error));
        }
    }
    return 0;
}
