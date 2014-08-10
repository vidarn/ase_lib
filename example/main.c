#include <stdio.h>
#include "ase_loader.h"
#include "ase_writer.h"
int main(int argc, char **argv)
{
    ASE_FILE ase;
    ASE_ERRORTYPE error;
    uint16_t i, ii, a, numFiles = 1;
    const char *filenames[] = {"data/example.ase"};
    for(a=0;a<numFiles;a++){
        wprintf(L"Loading file %s\n", filenames[a]);
        error = ase_openAndReadAseFile(&ase, filenames[a]);
        wprintf(L"ASE file version %d.%d\n", ase.version[0],ase.version[1]);
        if(!error){
            for(i=0;i<ase.numGroups;i++){
                ASE_GROUP *group = ase.groups + i;
                wprintf(L"  Group \"%ls\"\n", group->name);
                for(ii=0;ii<group->numColors;ii++){
                    ASE_COLOR *color = group->colors + ii;
                    uint16_t iii = 0;
                    wchar_t *typeName;
                    switch(color->type){
                        case ASE_COLORTYPE_RGB:  typeName = L"RGB";  break;
                        case ASE_COLORTYPE_CMYK: typeName = L"CMYK"; break;
                        case ASE_COLORTYPE_LAB:  typeName = L"LAB";  break;
                        case ASE_COLORTYPE_GRAY: typeName = L"Gray"; break;
                    }
                    wprintf(L"    Color [%ls] \"%ls\"\n      ",typeName,color->name);
                    while(iii < 4 && color->col[iii] > 0){
                        wprintf(L"%f ", color->col[iii]);
                        iii++;
                    }
                    wprintf(L"\n");
                }
            }
            wprintf(L"Writing file %s\n", "data/out.ase");
            if(ase_openAndWriteAseFile(&ase,"data/out.ase")){
                wprintf(L"  Error occured: %ls\n", ase_getErrorString(error));
            }
            ase_freeAseFile(&ase);
        }
        else{
            wprintf(L"  Error occured: %ls\n", ase_getErrorString(error));
        }
    }
    return 0;
}
