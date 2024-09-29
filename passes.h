#ifndef PASSES_H
#define PASSES_H

/*A data structure for saving the symbols*/
struct symbol{
    char name_symbol[MAX_LABEL];
    enum{
        extern_sym,
        entry_sym,/*Temporary label*/
        data_sym,
        code_sym,
        entryCode_sym,
        entryData_sym
    }type_sym;
    int address_sym;
};

/*A structure for saving addresses for use in an external label*/
struct external{
    char nameExt_sym[MAX_LABEL];
    int *addressesExt_sym;
    int len_addresses;
};

/*A data structure to perform the translation unit step*/
struct translation_unit{
    int *code_content;
    int IC;
    int *data_content;
    int DC;
    struct symbol *symbol_table;
    int count_sym;
    struct external *ext_table;
    int count_ext;
    struct symbol ** entries;
    int count_ent;
};

int first_pass(struct translation_unit *program , FILE * file_am , char * nameFile_am); 
int second_pass(struct translation_unit * program , FILE * file_am , char * nameFile_am);
struct symbol * lookFor_sym(struct symbol * symbol_table, int count_sym, char *label);
struct external * lookFor_symExt(struct external * ext_table, int count_ext, char *label);
void print_file_ob(char * nameFile , struct translation_unit *program);
void print_file_ent(char * nameFile , struct translation_unit *program);
void print_file_ext(char * nameFile , struct translation_unit *program);
#endif