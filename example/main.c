#include <stdio.h>
#include "ase_loader.h"
int main(int argc, char **argv)
{
    ASE_FILE file = openAndReadAseFile("data/example.ase");
    for(uint16_t i=0;i<file.numGroups;i++){
        GROUP *group = file.groups + i;
        wprintf(L"Group \"%ls\"\n", group->name);
        for(uint16_t ii=0;ii<group->numColors;ii++){
            COLOR *color = group->colors + ii;
            uint16_t iii = 0;
            wchar_t *typeName;
            switch(color->type){
                case COLORTYPE_RGB:  typeName = L"RGB";  break;
                case COLORTYPE_CMYK: typeName = L"CMYK"; break;
                case COLORTYPE_LAB:  typeName = L"LAB";  break;
                case COLORTYPE_GRAY: typeName = L"Gray"; break;
            }
            wprintf(L"  Color [%ls] \"%ls\"\n    ",typeName,color->name);
            while(color->col[iii] > 0 && iii < 4){
                wprintf(L"%f ", color->col[iii]);
                iii++;
            }
            wprintf(L"\n");
        }
    }
    freeAseFile(&file);
    return 0;
}
