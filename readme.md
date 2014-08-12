## ase_lib

A simple MIT-licensed C89-library for reading Adobe Swatch Exchange (.ASE) files.
The library consists of a single header file.

----------------

## Usage

```
#include "ase_loader.h"
#include "ase_writer.h" 
/*Read ASE-file from filename*/
int main(int argc, char **argv){
    unsigned int i,ii;
    ASE_FILE ase;
    ASE_ERRORTYPE error = ase_openAndReadAseFile(&ase,"file.ase");
    if(!error){
         /*Parse ASE-file*/
        for(i=0;i<ase.numGroups;i++){
            ASE_GROUP *group = ase.groups + i;
            /* Name: group->name (char *, utf-8)*/
            for(ii=0;ii<group->numColors;ii++){
                ASE_COLOR *color = group->colors + ii;
                /* Name: color->name (char *, utf-8)
                 * Color type: color->type (enum, one of ASE_COLORTYPE_RGB, ASE_COLORTYPE_CMYK,
                 *        ASE_COLORTYPE_LAB or ASE_COLORTYPE_GRAY)
                 * Color values: color->col (float[4], unused channels are -1.f)*/
            }
         }  
        /*Write ASE-file*/
        ase_openAndWriteAseFile(&ase,"file_out.ase");
         /*Don't forget to free the data structure afterwards*/
        ase_freeAseFile(&ase);
    }
    return 0;
}
```

