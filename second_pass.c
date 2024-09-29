#include "main.h"
#include "passes.h"
#include "front_end.h"
/**
 * performs the second pass in the assembly translation process.
 * It processes each line from the assembly source file and generates the machine code
 * by encoding instructions, handling directives, and managing external symbols.
 *
 *  @param program: A pointer to the translation unit that contains the code and symbol tables.
 *  @param file_am: The assembly source file to be processed.
 *  @param nameFile_am: The name of the assembly source file, used for error messages.
 *
 *  @return An integer flag_error indicating if any errors were encountered during the process (0 for success, 1 for errors).
 */
int second_pass(struct translation_unit * program , FILE * file_am , char * nameFile_am){
    char line[LINE];
    int flag_error = 0 , count_line = 0 , max_loop = 0 , i ;
    int *temp_code;
    struct data_tree lineData = {0};
    struct symbol *find_sym;
    struct external *ext_sym;
    struct external *temp_ext_sym;
    while(fgets(line, LINE, file_am)){ /*Process each line from the assembly file*/
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
        max_loop = 0;
        /*A call to a function that defines the content of the row in the data structure*/
        lineData = get_data_tree(line);
        /*Handle directive lines .data, .string*/
        if(lineData.line_type == dir_line){ 
            if(lineData.line_option.directive.dir_type == line_string){
                free(lineData.line_option.directive.option_dir.string);
            }
            else if(lineData.line_option.directive.dir_type == line_data){
                free(lineData.line_option.directive.option_dir.data);
            }
            continue;
        }
        /*Handle instruction lines*/
        if(lineData.line_type == inst_line){ 
            if(program->IC == 0){
                /*Allocate memory for the first instruction code*/
                program->code_content = (int *)malloc(sizeof(int));
                program->code_content[0] = 0;
                MALLOC_CORECT(program->code_content);
            }else{ /*Reallocate memory to store additional instruction codes*/
                temp_code = (int *)realloc(program->code_content,(program->IC + 1)*sizeof(int));
                REALLOC_CORECT(temp_code,program->code_content);
                program->code_content = temp_code;
                program->code_content[program->IC] = 0;
            }
            /*Insert the opcode bits into the instruction code*/
            program->code_content[program->IC] = lineData.line_option.instruction.inst_type << 11;
            /*Add bits for the addressing method types of the operands*/
            if(lineData.line_option.instruction.inst_type < 5 ){
                program->code_content[program->IC] |= 1 << (7 + lineData.line_option.instruction.inst_option[0].operands_type);
                program->code_content[program->IC] |= 1 <<( 3 + lineData.line_option.instruction.inst_option[1].operands_type) ;
            }
            if(lineData.line_option.instruction.inst_type < 14 && lineData.line_option.instruction.inst_type > 4)
                program->code_content[program->IC] |= 1 << ( 3 + lineData.line_option.instruction.inst_option[0].operands_type) ;
            program->code_content[program->IC] |= 1 << 2;/*Adding the bits to the ARE coding*/
            program->IC++;
            /*Handle operands that are registers*/
            if(lineData.line_option.instruction.inst_option[0].operands_type >= address_reg && lineData.line_option.instruction.inst_option[1].operands_type >= address_reg){
                temp_code = (int*)realloc(program->code_content,(program->IC + 1)*sizeof(int));
                REALLOC_CORECT(temp_code,program->code_content);
                program->code_content = temp_code;
                program->code_content[program->IC] = lineData.line_option.instruction.inst_option[0].option_inst.reg << 6;
                program->code_content[program->IC] |= lineData.line_option.instruction.inst_option[1].option_inst.reg << 3;
                program->code_content[program->IC] |= 1 << 2;/*Adding the bits to the ARE coding*/
                program->IC++;
            }else{
                /*Determine how many operand handling loops are needed*/
                if(lineData.line_option.instruction.inst_type < 5) max_loop = 2;
                else if(lineData.line_option.instruction.inst_type < 14) max_loop = 1;
                /*Process each operand*/
                for(i = 0; i < max_loop ; i++){
                    temp_code = (int*)realloc(program->code_content,(program->IC + 1)*sizeof(int));
                    REALLOC_CORECT(temp_code,program->code_content);
                    program->code_content = temp_code;
                    program->code_content[program->IC] = 0;
                    /*Handle immediate addressing mode*/
                    if(lineData.line_option.instruction.inst_option[i].operands_type == immediate){
                        program->code_content[program->IC] = lineData.line_option.instruction.inst_option[i].option_inst.immediate << 3;
                        program->code_content[program->IC] |= 1 << 2;/*Adding the bits to the ARE coding*/
                    /*Handle label addressing mode*/
                    }else if(lineData.line_option.instruction.inst_option[i].operands_type == label){
                        find_sym = lookFor_sym(program->symbol_table , program->count_sym , lineData.line_option.instruction.inst_option[i].option_inst.label);
                        if(find_sym){
                            program->code_content[program->IC] = find_sym->address_sym << 3;
                            if(find_sym->type_sym == extern_sym){
                                program->code_content[program->IC] |= 1;/*Adding the bits to the ARE coding*/
                                /*Manage external symbols and their addresses*/
                                ext_sym = lookFor_symExt(program->ext_table, program->count_ext, find_sym->name_symbol);
                                if(ext_sym){
                                    temp_code = (int*)realloc(ext_sym->addressesExt_sym,(ext_sym->len_addresses + 1)*sizeof(int));
                                    REALLOC_CORECT(temp_code,ext_sym->addressesExt_sym);
                                    ext_sym->addressesExt_sym = temp_code;
                                    ext_sym->addressesExt_sym[ext_sym->len_addresses] = program->IC + 100;
                                    ext_sym->len_addresses++ ;
                                }else{
                                    if(program->count_ext == 0){
                                        program->ext_table =  (struct external*)malloc(sizeof(struct external));
                                        MALLOC_CORECT(program->ext_table);
                                    }else{
                                        temp_ext_sym = (struct external*)realloc(program->ext_table, (program->count_ext + 1)*sizeof(struct external));
                                        REALLOC_CORECT(temp_ext_sym ,program->ext_table);
                                        program->ext_table = temp_ext_sym;
                                    }
                                    strcpy(program->ext_table[program->count_ext].nameExt_sym , find_sym->name_symbol);
                                    program->ext_table[program->count_ext].addressesExt_sym = (int*)malloc(sizeof(int));
                                    MALLOC_CORECT(program->ext_table[program->count_ext].addressesExt_sym);
                                    program->ext_table[program->count_ext].len_addresses = 0;
                                    program->ext_table[program->count_ext].addressesExt_sym[0] = program->IC + 100;
                                    program->ext_table[program->count_ext].len_addresses++;
                                    program->count_ext++;
                                }
                            }else
                                program->code_content[program->IC] |= 1 << 1;/*Adding the bits to the ARE coding*/
                        }else{
                            fprintf(stderr ,"ERROR: in line: %d in file: %s - Using an undefined label\n", count_line, nameFile_am);
                            flag_error = 1;
                        }
                    }else if(lineData.line_option.instruction.inst_option[i].operands_type >= address_reg){
                        if(max_loop == 2){
                            program->code_content[program->IC] = lineData.line_option.instruction.inst_option[i].option_inst.reg << (6 - (3 * i));
                        }else{
                            program->code_content[program->IC] = lineData.line_option.instruction.inst_option[i].option_inst.reg << 3;
                        }
                        program->code_content[program->IC] |= 1 << 2;/*Adding the bits to the ARE coding*/
                    }
                    program->IC++;
                }
            }
        }
    }
    return flag_error;
}

/**
 * Searches for an external symbol in the external symbols table by its label.
 *
 * @param ext_table: A pointer to an array of struct external, which represents the table of external symbols.
 * @param count_ext: An integer representing the number of external symbols in the table.
 * @param label: A string representing the label of the external symbol to search for.
 *
 * @returns: A pointer to the struct external corresponding to the label if found; otherwise, returns NULL.
 */
struct external * lookFor_symExt(struct external *ext_table, int count_ext, char *label){
    int i;
    for(i = 0; i < count_ext; i++){
        if(strcmp(ext_table[i].nameExt_sym, label) == 0){
            return &ext_table[i];
        }
    }
    return NULL;
}