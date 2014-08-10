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
        printf("Loading file %s\n", filenames[a]);
        error = ase_openAndReadAseFile(&ase, filenames[a]);
        printf("ASE file version %d.%d\n", ase.version[0],ase.version[1]);
        if(!error){
            for(i=0;i<ase.numGroups;i++){
                ASE_GROUP *group = ase.groups + i;
                printf("  Group \"%s\"\n", group->name);
                for(ii=0;ii<group->numColors;ii++){
                    ASE_COLOR *color = group->colors + ii;
                    uint16_t iii = 0;
                    char *typeName;
                    switch(color->type){
                        case ASE_COLORTYPE_RGB:  typeName = "RGB";  break;
                        case ASE_COLORTYPE_CMYK: typeName = "CMYK"; break;
                        case ASE_COLORTYPE_LAB:  typeName = "LAB";  break;
                        case ASE_COLORTYPE_GRAY: typeName = "Gray"; break;
                    }
                    printf("    Color [%s] \"%s\"\n      ",typeName,color->name);
                    while(iii < 4 && color->col[iii] > 0){
                        printf("%f ", color->col[iii]);
                        iii++;
                    }
                    printf("\n");
                }
            }
            printf("Writing file %s\n", "data/out.ase");
            if(ase_openAndWriteAseFile(&ase,"data/out.ase")){
                printf("  Error occured: %s\n", ase_getErrorString(error));
            }
            ase_freeAseFile(&ase);
            error = ase_openAndReadAseFile(&ase, "data/out.ase");
            if(error){
                printf("  Error occured: %s\n", ase_getErrorString(error));
            }
        }
        else{
            printf("  Error occured: %s\n", ase_getErrorString(error));
        }
    }
    return 0;
}
