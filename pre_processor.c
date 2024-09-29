#include "main.h"
#include "pre_processor.h"

/**
 * Pre-processes the input file, handling macro definitions and replacements.
 * 
 * @param nameFile The name of the input file to be pre-processed.
 * 
 * @return A pointer to a string containing the name of the pre-processed file (with the ".am" extension), 
 *         or NULL if an error occurs.
 */
char *pre_process(char *nameFile){
    FILE *file_am;
    FILE *file_as;
    char *nameFile_am = (char *)malloc(strlen(nameFile)+ 4);
    char *temp_am = (char *)malloc(strlen(nameFile)+ 4);
    char *nameFile_as = (char *)malloc(strlen(nameFile)+ 4);
    char line[MAX_LINE];/*Setting a place to save the lines of the file*/
    char word[MAX_LINE]; /*variable to get the first word*/
    char *copy_line = (char *)malloc(MAX_LINE); /*Changed to keep the original row*/
    char *address_copy_line = copy_line;/*A variable to hold the address of word*/
    char *tempWord;
    char *temp_content;/*A helper variable to increment an array of the macro's contents*/
    macro *arr_macro = NULL; /*Setting a pointer to a data structure for a macro*/
    macro *temp;
    int length_arr = 0; /*varies along the array*/
    int first_line = 1;/*Variable to check if this is the first line of the macro*/
    int flag_defind = 0; /*Flag to check if this is a macro definition*/
    int flag_error = 0;
    int i;
    if(!copy_line || !nameFile_am || !nameFile_as || !temp_am){
        fprintf(stderr , "No space in memory\n");
        return NULL;
    }
    strcpy(nameFile_am,nameFile);
    strcpy(nameFile_as,nameFile);
    strcat(nameFile_am,".am");
    strcat(nameFile_as,".as");
    strcpy(temp_am , nameFile_am);
    file_am = fopen(nameFile_am,"w"); /*Opening the file for reading*/
    file_as = fopen(nameFile_as,"r"); /*Opening a file for writing*/
    free(nameFile_as);
    if(!file_am || !file_as){
        fprintf(stderr,"Failed to open files\n");
        free(nameFile_am);
        free(temp_am);
        free(copy_line);
        if(file_am) fclose(file_am);
        if(file_as) fclose(file_as);
        return NULL;
	}
    while(fgets(line, MAX_LINE,file_as) != NULL){ /*Receiving a row from a file*/
        copy_line = address_copy_line;
        strcpy(copy_line, line);
        tempWord = getword(&copy_line);
        strcpy(word ,tempWord);
        free(tempWord);
        if(is_macro_exist(file_am, arr_macro, word, length_arr)){/*Checking if the word is a macro*/
            if(extra_text(&copy_line)){
				fprintf(stderr , "ERROR: extra text after the define macro\n");
                if(nameFile_am != NULL)free(nameFile_am);
				nameFile_am = NULL;
                flag_error = 1;
			}
            continue;
        }
        else if(strcmp(word , "macr") == 0){
            flag_defind = 1;
            tempWord = getword(&copy_line);
            strcpy(word ,tempWord);
            free(tempWord);
            /*Checking whether the macro name is not an instruction word or directive*/
            if(strcmp(word , "macr") == 0){
                fprintf(stderr , "ERROR: Macro name can't be macr\n");
                if(nameFile_am != NULL)free(nameFile_am);
                nameFile_am = NULL;
                flag_error = 1;
                continue;
            }
            /*Checking if the macro name already exists*/
            if(is_macro_exist(file_am, arr_macro, word, length_arr)){
                fprintf(stderr , "ERROR: name macro exist\n");
                if(nameFile_am != NULL)free(nameFile_am);
                nameFile_am = NULL;
                flag_error = 1;
                continue;
            }
            /*Validates that the macro name is not a directive or instruction or rgister*/
            if(is_command(word) != -1 || is_rgister(word) != -1){
                fprintf(stderr , "ERROR: Invalid macro name\n");
                if(nameFile_am != NULL)free(nameFile_am);
                nameFile_am = NULL;
                flag_error = 1;
                continue;
            }
            if(extra_text(&copy_line)){
				fprintf(stderr , "ERROR: extra text after the define macro\n");
                if(nameFile_am != NULL)free(nameFile_am);
				nameFile_am = NULL;
                flag_error = 1;
                continue;
			}
            /*Allocates or reallocates memory for the macro array*/
            if(length_arr == 0){
				arr_macro = (macro *)malloc((length_arr+1) * sizeof(macro));
				MALLOC_CORECT(arr_macro);
            }
            else{
				temp = (macro*)realloc(arr_macro, (length_arr + 1)*sizeof(macro));
				REALLOC_CORECT(temp , arr_macro);
				arr_macro = temp;
			}
            arr_macro[length_arr].name_macro = (char *)malloc(strlen(word)+1);
            MALLOC_CORECT(arr_macro[length_arr].name_macro);
			strcpy(arr_macro[length_arr].name_macro , word);
			length_arr++;
            continue;
        }
		else if(strcmp(word , "endmacr") == 0){
				if(extra_text(&copy_line)){
                    fprintf(stderr , "ERROR: extra text after the end macro\n");
                    if(nameFile_am != NULL)free(nameFile_am);
					nameFile_am = NULL;
				}
                flag_defind = 0;
                first_line = 1;
                flag_error = 0;
                continue;
		}
		else{
            /*If currently defining a macro, append the line to the macro's content*/
			if(flag_defind){
                if(flag_error)continue;
				if(first_line){
                    arr_macro[length_arr-1].content_macro = (char *)malloc(strlen(line)+1);
                    MALLOC_CORECT(arr_macro[length_arr-1].content_macro);
                    first_line = 0; 
                    strcpy(arr_macro[length_arr-1].content_macro ,line);
                }
                else{
                    temp_content = (char *)realloc(arr_macro[length_arr-1].content_macro, (strlen(arr_macro[length_arr-1].content_macro)+1+ strlen(line)));
                    REALLOC_CORECT(temp_content, arr_macro[length_arr-1].content_macro);
                    arr_macro[length_arr-1].content_macro = temp_content;
                    strcat(arr_macro[length_arr-1].content_macro ,line);
                }
			}
            else{
                fprintf(file_am, "%s", line);/*Writes the line to the output file if not within a macro definition*/
            }

		}
    }
     /* Check if nameFile_am is NULL, and delete the file if it is */
    free(address_copy_line);
    fclose(file_as);
    fclose(file_am);
    for(i = 0 ; i < length_arr ; i++){
        free(arr_macro[i].content_macro);
        free(arr_macro[i].name_macro);
    }
    free(arr_macro);
    if (nameFile_am == NULL) {
        remove(temp_am);
    }
    free(temp_am);
    return nameFile_am;
}