#include "main.h"
#include "pre_processor.h"


/*Function to override spaces*/
void skipSpace(char ** line){

    while((**line) != '\0' && (**line) != '\n' && isspace(**line))
            (*line) ++;

}

/*A function that receives the word that is in the position of the first line*/
char * getword(char ** line){
   
    char * Word_name = (char *)malloc(sizeof(char));
    char * temp ;
    if(Word_name == NULL){
        printf("\nNO MEMORY FOE ALLOCATION!\n");
        return NULL;
    }
    (*Word_name) = '\0';
    skipSpace(line);
    while(!isspace(**line) && (**line) != '\0' && (**line)!= '\n' && (**line) != ',' ){
        temp = (char *)realloc(Word_name, strlen(Word_name)+sizeof(char)*2);
        if(temp == NULL){
            free(Word_name);
            printf("\nNO MEMORY FOE ALLOCATION!\n");
                        return NULL;
        }
        Word_name = temp;
        *(Word_name+strlen(Word_name)+1)='\0';
        *(Word_name+strlen(Word_name)) = (**line);
        (*line) ++;
       
    }
    return Word_name;
}

/*A function that checks whether there are any characters after the received command*/
int extra_text(char **line){
    while((**line) != '\0' && (**line) != '\n'){
        if(isspace(**line)){
            (*line)++;
        }
        else{
            return 1;
        }
    }
    return 0;
}

/*A function that checks if there is a comma and returns 1 if there is and 0 otherwise*/
int isComma(char **line){
	skipSpace(line);
	if((**line) == ','){
		(*line) ++;
		return 1;
	}
	return 0;
}

/*The function checks if the word is a macro and if so prints to a file*/
int is_macro_exist(FILE * file_am, macro *arr_macro, char *word, int length ){
    int i;
    for(i = 0; i < length; i++){
            if(strcmp(word , arr_macro[i].name_macro) == 0){
                fprintf(file_am,"%s", arr_macro[i].content_macro);
                return 1;
            }
    }
    return 0;
}


/*A function to check the command - is it an instruction or a directive*/
int is_command(char * word){
    if (strcmp(word, "mov") == 0) return MOV;
    if (strcmp(word, "cmp") == 0) return CMP;
    if (strcmp(word, "add") == 0) return ADD;
    if (strcmp(word, "sub") == 0) return SUB;
    if (strcmp(word, "lea") == 0) return LEA;
    if (strcmp(word, "clr") == 0) return CLR;
    if (strcmp(word, "not") == 0) return NOT;
    if (strcmp(word, "inc") == 0) return INC;
    if (strcmp(word, "dec") == 0) return DEC;
    if (strcmp(word, "jmp") == 0) return JMP;
    if (strcmp(word, "bne") == 0) return BNE;
    if (strcmp(word, "red") == 0) return RED;
    if (strcmp(word, "prn") == 0) return PRN;
    if (strcmp(word, "jsr") == 0) return JSR;
    if (strcmp(word, "rts") == 0) return RTS;
    if (strcmp(word, "stop") == 0) return STOP;
    if (strcmp(word, ".data") == 0) return DATA;
    if (strcmp(word, ".string") == 0) return STRING;
    if(strcmp(word, ".entry") == 0) return ENTRY;
    if(strcmp(word, ".extern") == 0) return EXTERN;
    return -1 ;
}

/*A function to check the type of the register*/
int is_rgister(char *word){
    if (strcmp(word, "r0") == 0) return r0;
    if (strcmp(word, "r1") == 0) return r1;
    if (strcmp(word, "r2") == 0) return r2;
    if (strcmp(word, "r3") == 0) return r3;
    if (strcmp(word, "r4") == 0) return r4;
    if (strcmp(word, "r5") == 0) return r5;
    if (strcmp(word, "r6") == 0) return r6;
    if (strcmp(word, "r7") == 0) return r7;
    return -1;
}