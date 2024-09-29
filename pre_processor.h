#ifndef PRE_PROCESSOR_H
#define PRE_PROCESSOR_H

/*Data structure to save the macro*/
typedef struct Macro{
    char *name_macro;
    char *content_macro;
}macro;
/*Data structure of teaching and guidance commands */
enum commands{
    MOV, CMP, ADD, SUB, LEA, CLR, NOT, INC, DEC, JMP, BNE, RED, PRN, JSR, RTS, STOP,
    DATA, STRING, ENTRY, EXTERN
};
/*Data structure for saving the register types*/
enum Rgister{
    r0,r1,r2,r3,r4,r5,r6,r7
};

/*pre_processor*/
char *pre_process(char *nameFile);

/*functionsP*/
char * getword(char ** line);
void skipSpace(char ** line);
int extra_text(char ** line);
int isComma(char ** line);
int is_command(char * word);
int is_rgister(char * word);
int is_macro_exist(FILE *file_am, macro *arr_macro, char *word, int length);
#endif