#include "main.h"
#include "front_end.h"
#include "pre_processor.h"

/**
 * Parses a line of assembly code and returns a data structure representing the line type and content.
 *
 * @param line A pointer to the line of assembly code to be parsed.
 * @return A `struct data_tree` containing the parsed line's data, including its type and any errors encountered.
 *
 * This function analyzes a given line of assembly code, determines its type (e.g., instruction, directive, comment),
 * and extracts relevant information, such as labels, commands, and operands. It handles labels, checks for syntax
 * correctness, and identifies any errors in the line.
 */
struct data_tree get_data_tree(char * line){
    struct data_tree lineData = {0};
    char* str;
    char word[MAX_LINE];/*Setting a variable to save the word from the command line*/
    char label[MAX_LABEL];/*Setting a variable to save the label from the command line*/
    int check_command = 0;/*A variable to store a value of a function that checks which command is received*/
    int flag_label = 0;/*Variable to check if there is a symbol in the line*/
    skipSpace(&line);
    memset(lineData.errors, 0 ,sizeof(lineData.errors));
    memset(lineData.line_option.directive.label, 0 ,MAX_LABEL);
    memset(lineData.line_option.instruction.label, 0, MAX_LABEL);
    if(*line == ';'){/*Checking whether the line is a comment line*/
        lineData.line_type = explan_line;
        return lineData;
    }
    if(*line == '\n' || *line == '\0'){/*Checking whether the line is empty*/
        lineData.line_type = empty_line;
        return lineData; 
    }
    str = getword(&line);
    strcpy(word, str);
    free(str);
    /*Checks for the correctness of the label*/
    if(*(word + strlen(word)-1) == ':'){
        *(word + strlen(word) - 1) = '\0';
        strcpy(label , word);
        if(!corect_label(label)){
            strcpy(lineData.errors, "label is not corect\n");
            return lineData;
        }
        flag_label = 1;
        str = getword(&line);
        strcpy(word, str);
        free(str);
    }
    if(isComma(&line)){/*Checking whether there is an unnecessary comma*/
        strcpy(lineData.errors, "Unnecessary comma\n");
        return lineData;
    }
    check_command = is_command(word);
    if(check_command >= 0 && check_command <= 15){/*Checking whether it is an instruction command*/
        lineData.line_type = inst_line;
        if(flag_label) 
            strcpy(lineData.line_option.instruction.label ,label);
        lineData.line_option.instruction.inst_type = check_command;
        if(check_command < 5){/*Case of commands having 2 operands*/
            two_operands_correctness(&lineData, &line, check_command);
        }
        if(check_command > 4 && check_command < 14){/*A case of commands having one operand*/
            single_operand_correctness(&lineData, &line, check_command);
        }
        if(check_command > 13){/*A case of commands without operands*/
            if(extra_text(&line))
                strcpy(lineData.errors, "extra text after the command\n");
            return lineData;
        }
    }
    else if(check_command > 15 && check_command <= 19){/*Checking if this is a prompt command*/
        lineData.line_type = dir_line;
        if(flag_label) 
            strcpy(lineData.line_option.directive.label , label);
        lineData.line_option.directive.dir_type = check_command - 16; 
        set_directive(&lineData, &line, check_command - 16);
    } 
    else{/*In case a line with invalid input was received*/
        strcpy(lineData.errors, "Invalid command\n");
         return lineData;
    }
    
    return lineData;
}

/*A function to check whether the label is correct*/
int corect_label(char *word){
    if(strlen(word) > MAX_LABEL) return 0;
    if(is_command(word) != -1 ||is_rgister(word) != -1) return 0;
    if(!isalpha(*word)){
        word++;
        return 0;
    }
    while (*word) {
        if (!isalpha(*word) && !isdigit(*word)) {
            return 0;
        }
        word++;
    }
    return 1;
}
/*A function to check the type of the sent operand*/
int operand_type(char *operand){
    if(*operand == '#'){
        operand++;
        if(*operand == '-' || *operand == '+') operand++;
        while(*operand){
            if(!isdigit(*operand)) return -1;
            operand++;
        }
        return immediate;
    }
    else if(corect_label(operand)) return label;
    else if(*operand == '*'){
        if(is_rgister(++operand) != -1) return address_reg;
    }
    else if(is_rgister(operand) != -1) return value_reg;
    return -1;
}

/*The function checks which value was received and accordingly inserts values   into the tree*/
void set_line_data(struct data_tree * lineData,int operand, int index, char * word){
    switch(operand){
        case immediate:
            (*lineData).line_option.instruction.inst_option[index].option_inst.immediate = atoi(++word);
            break;
        case label:
            strcpy((*lineData).line_option.instruction.inst_option[index].option_inst.label, word);
            break;
        case address_reg:
            (*lineData).line_option.instruction.inst_option[index].option_inst.reg = is_rgister(++word);
            break;
        case value_reg:
            (*lineData).line_option.instruction.inst_option[index].option_inst.reg = is_rgister(word);
            break;
    }
}
/*A function to check the correctness of commands with a single operand*/
void single_operand_correctness(struct data_tree * lineData, char ** line, int command){
    char* str;
    char word[100];
    int operand;
    str = getword(line);
    strcpy(word, str);
    free(str);
    operand = operand_type(word);
    if(operand == -1){
            strcpy((*lineData).errors, "Invalid operand\n");
            return;
    }
    if(command < 9 || command == 11){/*Commands with operands of type 1,2,3*/
        if(operand == 0){
            strcpy((*lineData).errors, "Invalid operand for command\n");
            return;
        }
    }
    if((command > 8 && command < 11) || command == 13 ){/*Commands with operands of type 1,2*/
        if(operand != 1 && operand !=2){
            strcpy((*lineData).errors, "Invalid operand for command\n");
            return;
        }
    }
    if(extra_text(line)){
        strcpy((*lineData).errors, "extra text after the operand\n");
        return;
    }
    (*lineData).line_option.instruction.inst_option[0].operands_type = operand;
    set_line_data(lineData, operand , 0 , word);/*Sending to the function to place the values ​​in the tree*/
}

/*A function to check the correctness of commands with two operands*/
void two_operands_correctness(struct data_tree * lineData, char ** line, int command){
    char *word = getword(line);
    int operand = operand_type(word);
    if(operand == -1){
        strcpy((*lineData).errors, "Invalid operand\n");
        free(word);
        return;
    }
    if(command == 4){
        if(operand != 1){
            strcpy((*lineData).errors, "Invalid operand for command\n");
            free(word);
            return;
        }
    }
    if(!isComma(line)){/*Checking whether a comma is missing*/
        strcpy((*lineData).errors, "missing a comma\n");
        free(word);
        return;
    }
    if(isComma(line)){/*Checking whether there is an unnecessary comma*/
        strcpy((*lineData).errors, "Unnecessary comma\n");
        free(word);
        return;
    }
    (*lineData).line_option.instruction.inst_option[0].operands_type = operand;
    set_line_data(lineData, operand, 0 , word);/*Sending to the function to initialize the values ​​in the data structure of the first variable*/
    free(word);
    word = getword(line);/*Getting the next word to test the second variable*/
    operand = operand_type(word);
    if(operand == -1){
        strcpy((*lineData).errors, "Invalid operand\n");
        free(word);
        return;
    }
    if(command != 1){
        if(operand == 0){
            strcpy((*lineData).errors, "Invalid operand for command\n");
            free(word);
            return;
        }
    }
    if(extra_text(line)){
        strcpy((*lineData).errors, "extra text after the operand\n");
        free(word);
        return;
    }
    (*lineData).line_option.instruction.inst_option[1].operands_type = operand;
    set_line_data(lineData, operand , 1 , word);/*A call to the function to edit the second variable in the data structure*/
    free(word);
}
/*A function for checking the correctness of directive commands and editing them in the data structure*/
void set_directive(struct data_tree * lineData, char ** line, int command){
    char* str;
    char word[11];
    char *ptr; 
    int *temp;
    int len = 0;
    int num;
    int j ;
    int flag_first = 0;
    switch(command){
        case line_data:
            while(**line != '\0' && **line != '\n'){
                j = 0;
                str = getword(line);
                strcpy(word,str); 
                free(str);
                num = atoi(word);
                if(*word == '+' || *word == '-') j++;
                for (; j < (int)strlen(word); j++) {
                    if (!isdigit(word[j])) {
                        strcpy((*lineData).errors,"Not a number");
                        free((*lineData).line_option.directive.option_dir.data);
                        return;
                    }
                }
                if(flag_first == 0){
                    (*lineData).line_option.directive.option_dir.data = (int *)malloc(sizeof(int));
                    MALLOC_CORECT((*lineData).line_option.directive.option_dir.data);
                    *((*lineData).line_option.directive.option_dir.data)= num;
                    len++;
                    flag_first = 1;
                }
                else if(flag_first){
                    temp = (int *)realloc((*lineData).line_option.directive.option_dir.data, (len + 1)*sizeof(int));
                    REALLOC_CORECT(temp, (*lineData).line_option.directive.option_dir.data);
                    (*lineData).line_option.directive.option_dir.data = temp;
                    *((*lineData).line_option.directive.option_dir.data + len)= num;
                    len++;
                }
                if(!isComma(line)){/*Checking whether a comma is missing*/
                    if(extra_text(line)){
                        strcpy((*lineData).errors, "missing a comma\n");
                        free((*lineData).line_option.directive.option_dir.data);
                    }
                    (*lineData).line_option.directive.data_count = len;
                    return;
                }
                if(isComma(line)){/*Checking whether there is an unnecessary comma*/
                    strcpy((*lineData).errors, "Unnecessary comma\n");
                    free((*lineData).line_option.directive.option_dir.data);
                    return;
                }
                if(!extra_text(line)){
                    strcpy((*lineData).errors, "Unnecessary comma\n");
                    free((*lineData).line_option.directive.option_dir.data);
                    return;
                }
            }
            strcpy((*lineData).errors, "missing data\n");
            break;
        case line_string:
            skipSpace(line);
            if(**line == '\0' || **line == '\n'){
                strcpy((*lineData).errors, "missing string\n");
                return;
            }
            if(**line != '"'){
                strcpy((*lineData).errors, "Missing quotes\n");
                return;
            }
            (*line)++;
            ptr = *line;
            while(**line != '\0' && **line != '\n'){
                if(**line == '"'){
                    (*line)++;
                    if(extra_text(line)){
                        strcpy((*lineData).errors, "extra text after the string\n");
                        return;
                    }
                    (*lineData).line_option.directive.option_dir.string = (char *)malloc(sizeof(char)*(len + 1));
                    MALLOC_CORECT((*lineData).line_option.directive.option_dir.string);
                    strncpy((*lineData).line_option.directive.option_dir.string, ptr , len);
                    (*lineData).line_option.directive.option_dir.string[len] = '\0';
                    return;
                }
                len++;
                (*line)++;
            }
            strcpy((*lineData).errors, "Missing quotes\n");
            break;
        default:/*The case of 'entry' and 'extern' commands have the same checks*/
            str = getword(line);
            strcpy(word, str);
            free(str);
            if(corect_label(word)){
                if(extra_text(line)){
                    strcpy((*lineData).errors, "extra text after the label\n");
                    return;
                }
                strcpy((*lineData).line_option.directive.option_dir.label, word);
            }
            else if(*word == '\0'|| *word == '\n'){
                strcpy((*lineData).errors, "missing label\n");
            }
            else{
                strcpy((*lineData).errors, "Invalid label\n");
            }
            break;
    }
}