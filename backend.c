#include "main.h"
#include "passes.h"
/**
 * Prints the contents of the code and data sections to a .ob file.
 * 
 * @param nameFile The base name of the file to write the output to (without the extension).
 * @param program  A pointer to the translation_unit structure containing the program's code and data.
 */
void print_file_ob(char * nameFile , struct translation_unit *program){
    char * nameFile_ob = (char *)malloc(strlen(nameFile) + 4);
    FILE * file_ob ;
    int i;
    MALLOC_CORECT(nameFile_ob);
    strcpy(nameFile_ob , nameFile);
    strcat(nameFile_ob , ".ob");
    file_ob = fopen(nameFile_ob , "w");
    if(file_ob){
        /* Print the instruction count (IC) and data count (DC) to the .ob file*/
        fprintf(file_ob , "\t%d\t%d\n",program->IC , program->DC);
        /*Print each instruction in the code section*/
        for(i = 0 ; i < program->IC ; i++){
            fprintf(file_ob , "%04d\t%d%d%d%d%d\n",i+100 ,(program->code_content[i] >> 12)& 0x7,
             (program->code_content[i] >> 9)& 0x7,(program->code_content[i] >> 6)& 0x7,
             (program->code_content[i] >> 3)& 0x7,(program->code_content[i]) & 0x7);
        }
        /*Print each data item in the data section*/
        for(i = 0; i < program->DC ; i++){
            fprintf(file_ob ,"%04d\t%d%d%d%d%d\n",i +100 + program->IC,(program->data_content[i] >> 12)& 0x7,
            (program->data_content[i] >> 9)& 0x7,(program->data_content[i] >> 6)& 0x7,
            (program->data_content[i] >> 3)& 0x7,(program->data_content[i] )& 0x7);
        }
        fclose(file_ob);
    }else{
        fprintf(stderr , "Failed to open the file\n");
    }
    free(nameFile_ob);
}
/**
 * Prints the entries of the program to a .ent file.
 * 
 * @param nameFile The base name of the file to write the output to (without the extension).
 * @param program  A pointer to the translation_unit structure containing the program's entries.
 */
void print_file_ent(char * nameFile , struct translation_unit *program){
    char * nameFile_ent = (char *)malloc(strlen(nameFile) + 5);
    FILE * file_ent ;
    int i;
    MALLOC_CORECT(nameFile_ent);
    strcpy(nameFile_ent , nameFile);
    strcat(nameFile_ent , ".ent");
    file_ent = fopen(nameFile_ent, "w");
    if(file_ent){
        for(i = 0; i < program->count_ent ; i++){
            fprintf(file_ent , "%s\t%d\n",program->entries[i]->name_symbol ,program->entries[i]->address_sym);
        }
        fclose(file_ent);
    }else{
        fprintf(stderr , "Failed to open the file\n");
    }
    free(nameFile_ent);
}
/**
 * Prints the external symbols and their addresses to a .ext file.
 * 
 * @param nameFile The base name of the file to write the output to (without the extension).
 * @param program  A pointer to the translation_unit structure containing the program's external symbols.
 */
void print_file_ext(char * nameFile , struct translation_unit *program){
    char *nameFile_ext = (char*)malloc(strlen(nameFile) + 5);
    FILE *file_ext;
    int i, j;
    MALLOC_CORECT(nameFile_ext);
    strcpy(nameFile_ext , nameFile);
    strcat(nameFile_ext , ".ext");
    file_ext = fopen(nameFile_ext, "w");
    if(file_ext){
        for(i = 0; i < program->count_ext; i++){
            for(j = 0; j< program->ext_table[i].len_addresses; j++){
                fprintf(file_ext, "%s\t%04d\n", program->ext_table[i].nameExt_sym, program->ext_table[i].addressesExt_sym[j]);
            }
        }
        fclose(file_ext);
    }
    free(nameFile_ext);
}