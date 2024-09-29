#include "main.h"
#include "passes.h"
#include "front_end.h"

/**
 * Performs the first pass over the source code, processing symbols and data.
 * 
 * @param program A pointer to the translation_unit struct which holds the symbol table and data content.
 * @param file_am A pointer to the FILE structure that represents the input file.
 * @param nameFile_am The name of the input file being processed.
 * 
 * @return An integer flag indicating whether an error was encountered during the first pass.
 *         0 if no errors were found, 1 otherwise.
 */
int first_pass(struct translation_unit *program , FILE * file_am , char * nameFile_am){
    char line[LINE];
    char label[MAX_LABEL];
    int IC = 100 ,DC = 0 , flag_error = 0 , count_line = 0 ,flag_first = 1 , i;
    int *temp_data;
    struct data_tree lineData = {0};
    struct symbol * find_sym;
    struct symbol * temp_sym;
    struct symbol ** temp_entries;
    while(fgets(line , LINE ,file_am)){
        count_line++;
        /*In the case of a received line that is longer than allowed, an error message is received and the next line is received*/
        if(strlen(line) > MAX_LINE){
            fprintf(stderr ,"ERROR: in file: %s - line is too long\n",nameFile_am);
            flag_error = 1;
            if(*(line + strlen(line)) != '\n'){
                fgets(line , LINE ,file_am);
            }
            continue;
        }
        /*A call to a function that defines the content of the row in the data structure*/
        lineData = get_data_tree(line);
        if(lineData.errors[0] != '\0'){
            fprintf(stderr ,"ERROR: in line: %d in file: %s is: %s",count_line,nameFile_am,lineData.errors);
            flag_error = 1;
            continue;
        }
        /*Handle symbol definitions*/
        if((lineData.line_option.instruction.label[0] != '\0' )|| (lineData.line_option.directive.label[0] != '\0' && lineData.line_option.directive.dir_type <= line_string)){
            if(lineData.line_option.instruction.label[0] != '\0')
                strcpy(label,lineData.line_option.instruction.label);
            else
                strcpy(label, lineData.line_option.directive.label);
            find_sym = lookFor_sym(program->symbol_table , program->count_sym , label);
            if(find_sym){
                if(find_sym->type_sym == entry_sym){
                    if(lineData.line_type == dir_line){
                        find_sym->type_sym = entryData_sym;
                        find_sym->address_sym = DC;/*Address update*/
                    }
                    else{
                        find_sym->type_sym = entryCode_sym;
                        find_sym->address_sym = IC;/*Address update*/
                    }
                } 
                else{
                    fprintf(stderr ,"ERROR: in line: %d in file: %s - redefining of symbol: %s\n",count_line,nameFile_am,label);
                    flag_error = 1;
                    continue;
                }
            }
            else{
                /*Add new symbol to the symbol table*/
                if(program->count_sym == 0){
                    program->symbol_table = (struct symbol *)malloc(sizeof(struct symbol));
                    MALLOC_CORECT(program->symbol_table);
                }
                else{
                    temp_sym = (struct symbol *)realloc(program->symbol_table , (program->count_sym + 1)* sizeof(struct symbol));
                    REALLOC_CORECT(temp_sym, program->symbol_table);
                    temp_sym[program->count_sym].address_sym = 0;
                    program->symbol_table = temp_sym;
                }
                strcpy(program->symbol_table[program->count_sym].name_symbol, label);
                if(lineData.line_type == dir_line){
                    program->symbol_table[program->count_sym].type_sym = data_sym;
                    program->symbol_table[program->count_sym].address_sym = DC;/*Address update*/
                }else{
                    program->symbol_table[program->count_sym].type_sym = code_sym;
                    program->symbol_table[program->count_sym].address_sym =  IC;/*Address update*/
                }
                program->count_sym++; 
            }
        }  
        /*Update of the IC according to the command type*/
        if(lineData.line_type == inst_line){
            IC++;
            if(lineData.line_option.instruction.inst_type >= 5 && lineData.line_option.instruction.inst_type <= 13) IC++;
            else if((lineData.line_option.instruction.inst_option[0].operands_type >= address_reg) &&(lineData.line_option.instruction.inst_option[1].operands_type >= address_reg)) IC++;
            else if(lineData.line_option.instruction.inst_type < 5) IC += 2;
        }
        else if((lineData.line_type == dir_line) && (lineData.line_option.directive.dir_type <= line_string)){
            /*Process data directives*/
            if(lineData.line_option.directive.dir_type == line_data){
                if(flag_first){
                    program->data_content = (int *)malloc(lineData.line_option.directive.data_count*sizeof(int));
                    MALLOC_CORECT(program->data_content);
                    flag_first = 0;
                }
                else{
                    temp_data = (int *)realloc(program->data_content , (program->DC + lineData.line_option.directive.data_count)*sizeof(int));
                    REALLOC_CORECT(temp_data,program->data_content);
                    program->data_content = temp_data;
                }
                memcpy(&program->data_content[program->DC], lineData.line_option.directive.option_dir.data ,lineData.line_option.directive.data_count*sizeof(int));
                free(lineData.line_option.directive.option_dir.data);
                lineData.line_option.directive.option_dir.data = NULL;
                DC += lineData.line_option.directive.data_count;
                program->DC = DC;

            }
            /*Process string directives*/
            if(lineData.line_option.directive.dir_type == line_string){
                if(flag_first){
                    program->data_content = (int *)malloc((strlen(lineData.line_option.directive.option_dir.string)+1)*sizeof(int));
                    MALLOC_CORECT(program->data_content);
                    flag_first = 0;
                }
                else{
                    temp_data = (int *)realloc(program->data_content , (program->DC + strlen(lineData.line_option.directive.option_dir.string) +1)*sizeof(int));
                    REALLOC_CORECT(temp_data,program->data_content);
                    program->data_content = temp_data;
                }
                for(i = 0 ; i < (int)strlen(lineData.line_option.directive.option_dir.string) ; i++){
                    program->data_content[DC++] = (int)lineData.line_option.directive.option_dir.string[i];
                }
                program->data_content[DC++] = (int)('\0');
                program->DC = DC;
                free(lineData.line_option.directive.option_dir.string);
                lineData.line_option.directive.option_dir.string = NULL;
            }
        }
        else if((lineData.line_type == dir_line) && (lineData.line_option.directive.dir_type >= line_entry)){
            /*Process entry and external directives*/
            find_sym = lookFor_sym(program->symbol_table , program->count_sym , lineData.line_option.directive.option_dir.label);
            if((lineData.line_option.directive.dir_type == line_entry) && find_sym){
                if(find_sym->type_sym == code_sym) find_sym->type_sym = entryCode_sym;
                else if(find_sym->type_sym == data_sym) find_sym->type_sym = entryData_sym;
                else{
                    fprintf(stderr ,"ERROR: in line: %d in file: %s - redefining of symbol: %s\n",count_line,nameFile_am,lineData.line_option.directive.option_dir.label);
                    flag_error = 1;
                }
            }
            else if(!find_sym){
                if(program->count_sym == 0){
                    program->symbol_table = (struct symbol *)malloc(sizeof(struct symbol));
                    MALLOC_CORECT(program->symbol_table);
                }else{
                    temp_sym = (struct symbol *)realloc(program->symbol_table , (program->count_sym + 1)* sizeof(struct symbol));
                    REALLOC_CORECT(temp_sym, program->symbol_table);
                    program->symbol_table = temp_sym;
                    temp_sym[program->count_sym].address_sym = 0;
                }
                strcpy(program->symbol_table[program->count_sym].name_symbol, lineData.line_option.directive.option_dir.label);
                if(lineData.line_option.directive.dir_type == line_entry) program->symbol_table[program->count_sym].type_sym = entry_sym;
                else program->symbol_table[program->count_sym].type_sym = extern_sym;
                program->count_sym++; 
            }
            else{
                fprintf(stderr ,"ERROR: in line: %d in file: %s  - symbol %s definition that is external \n",count_line,nameFile_am,lineData.line_option.directive.option_dir.label);
                flag_error = 1;
                continue;
            }
        }
    }
    /*Final processing of the symbol table*/
    flag_first = 1;
    program->count_ent = 0;
    for(i = 0; i < program->count_sym ; i++){
        if(program->symbol_table[i].type_sym == entry_sym){
            fprintf(stderr , "ERROR: in file: %s the label %s declared entry but not defined\n" , nameFile_am ,program->symbol_table[i].name_symbol);
            flag_error = 1;
        }
        if(program->symbol_table[i].type_sym == data_sym || program->symbol_table[i].type_sym == entryData_sym)
            program->symbol_table[i].address_sym += IC;
        if(program->symbol_table[i].type_sym >= entryCode_sym){
            if(flag_first){
                program->entries = (struct symbol **)malloc(sizeof(struct symbol));
                MALLOC_CORECT(program->entries);
                flag_first = 0;
            }
            else{
                temp_entries = (struct symbol **)realloc(program->entries ,(program->count_ent + 1)*sizeof(struct symbol));
                REALLOC_CORECT(temp_entries,program->entries);
                program->entries = temp_entries;
            }
            program->entries[program->count_ent] = &program->symbol_table[i];
            program->count_ent++;
        }
    
    }
    return flag_error;
}
/**
 * A function that checks if the label exists in the symbol table.
 * 
 * @param symbol_table A pointer to the symbol table array.
 * @param count_sym The number of symbols in the symbol table.
 * @param label The label to search for in the symbol table.
 * 
 * @return A pointer to the symbol struct if the label is found, NULL otherwise.
 */

struct symbol * lookFor_sym(struct symbol * symbol_table, int count_sym, char *label){
    int i;
    for(i = 0 ; i < count_sym ; i++){
        if(strcmp(symbol_table[i].name_symbol,label) == 0){
            return &symbol_table[i];
        }
    }
    return NULL;
}