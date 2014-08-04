## ASE_Loader

A simple MIT-licensed C89-library for reading Adobe Swatch Exchange (.ASE) files.
The library consists of a single header file.

----------------

## Usage

```
//Read ASE-file from filename
ASE_FILE file;
ASE_ERRORTYPE error = ase_openAndReadAseFile(&file,"file.ase");

//Read ASE-file from file stream
ASE_FILE ase;
FILE *f = fopen("file.ase","r");
ASE_ERRORTYPE error = ase_readAseFile(&ase,f)
fclose(f);

//Parse ASE-file
for(i=0;i<file.numGroups;i++){
    ASE_GROUP *group = file.groups + i;
    // Name: group->name (wchar_t *)
    for(ii=0;ii<group->numColors;ii++){
        ASE_COLOR *color = group->colors + ii;
        // Name: color->name (wchar_t *)
        // Color type: color->type (enum, one of ASE_COLORTYPE_RGB, ASE_COLORTYPE_CMYK,
        //        ASE_COLORTYPE_LAB or ASE_COLORTYPE_GRAY)
        // Color values: color->col (float[4], unused channels are -1.f)
    }
}

//Don't forget to free the data structure afterwards
ase_freeAseFile(&ase);
```

