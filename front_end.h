#ifndef FRONT_END_H
#define FRONT_END_H

#define MAX_ERRORS 200
#define MAX_NUMBER 80

/*A data structure to store the command type and the received operands*/
struct data_tree{
    char errors[MAX_ERRORS];
    /*The possible sentence types*/
    enum{
        explan_line,
        empty_line,
        dir_line,
        inst_line
    }line_type;
    union{
        struct{
            char label[MAX_LABEL];
            enum{
                line_data,
                line_string,
                line_entry,
                line_extern
            }dir_type;
            union{
                char label[MAX_LABEL];
                char *string;
                int *data;   
            }option_dir;
            int data_count;
        }directive;
        struct{
            char label[MAX_LABEL];
            enum{
                mov,cmp,add,sub,lea,clr,Not,inc,dec,jmp,bne,red,prn,jsr,rts,stop
            }inst_type;
            struct{
                enum{
                   immediate,
                   label,
                   address_reg,
                   value_reg 
                }operands_type;
                union{
                    int immediate;
                    char label[MAX_LABEL];
                    int reg;
                }option_inst;
            }inst_option[2];
        }instruction;
    }line_option;
};

struct data_tree get_data_tree(char * line);

int corect_label(char *word);
int operand_type(char *operand);
void set_line_data(struct data_tree * lineData,int operand, int index , char * word);
void single_operand_correctness(struct data_tree * lineData, char **line, int command);
void two_operands_correctness(struct data_tree * lineData, char ** line, int command);
void set_directive(struct data_tree * lineData, char ** line, int command);
#endif