#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_NUM_IDS 100
#define MAX_ID_LEN 50

/*
    DFA -- 10 states
    0 - initial
    1 - id
    2 - string literal
    3 - char
    4 - preprocessor directive
    5 - open comment
    6 - comment body
    7 - close comment
    8 - string escape
    9 - char escape
*/

int string_in1(char* elt, char set[][MAX_ID_LEN], int len);
int char_in(char elt, char set[], int len);
int string_in(char* elt, char* set[], int len);
int next_state(int state, char input);


char* keywords[] = {"auto", "double", "int", "struct",
"break", "else", "long", "switch",
"case",	"enum", "register", "typedef",
"char",	"extern", "return", "union",
"continue",	"for", "signed", "void",
"do", "if", "static", "while",
"default", "goto", "sizeof", "volatile",
"const", "float", "short", "unsigned"};
int num_keywords = 32;


char op[] = {'*', '/', '%', '+', '-', '!', '~', '&', '|', '^', '=', '<', '>'};
int op_len = 13;

char dlm[] = {'\\', ',', '.', ';', '(', ')', '{', '}', '[', ']'};
int dlm_len = 10;


int main (int argc, char* argv[]) {

    if (argc != 2) {
        fprintf(stderr, "Error: usage %s <flename.c>\n", argv[0]);
        exit(1);
    }
    FILE* fptr = fopen(argv[1], "r");


    char ids[MAX_NUM_IDS][MAX_ID_LEN] = {{'\0'}};

    // int i;
    // for (i = 0; i < MAX_NUM_IDS; i++) {
    //     printf("len of %d = %d\n", i, (int)strlen(ids[i]));
    // }

    int idCount = 0;
    int state = 0;

    char input;
    while (fscanf(fptr, "%c", &input) != EOF) {

        // printf("\nChar read: %c Current state: %d, Next state: %d\n", input, state, next_state(state, input));

        if (state != 1) { //not id
            state = next_state(state, input);
            if (state == 1) {
                int len = strlen(ids[idCount]);
                ids[idCount][len] = input;
                ids[idCount][len+1] = '\0';
            }
        }
        else {
            state = next_state(state, input);
            if (state == 1) {
                int len = strlen(ids[idCount]);
                if (len >= MAX_ID_LEN) {
                    fprintf(stderr, "MAX_ID_LEN exceeded!\n");
                    exit(3);
                }
                ids[idCount][len] = input;
                ids[idCount][len+1] = '\0';
            }
            else {
                idCount++;
                if (idCount >= MAX_NUM_IDS) {
                    fprintf(stderr, "MAX_NUM_IDS exceeded!\n");
                    exit(4);
                }
            }
        }
    }

    int numUniqueIds = 0;
    char uniqueIds[MAX_NUM_IDS][MAX_ID_LEN] = {{'\0'}};
    int i;
    for (i = 0; i < idCount; i++) {

        // printf("\n\ni = %d, numUniqueIds = %d, ids[%d] = %s, uniqueIdsPtr = %p\n", i, numUniqueIds, i, ids[i], uniqueIds);
        // printf("uniqueIds = {");
        // int j;
        // for (j = 0; j < numUniqueIds; j++) {
        //     printf(" %s ", uniqueIds[j]);
        // } printf("}\n");

        if ( !(string_in(ids[i], keywords, num_keywords) || string_in1(ids[i], uniqueIds, numUniqueIds) ) ) {
            // printf("Copying ids[%d] = '%s' to uniqueIds[%d]\n", i, ids[i], numUniqueIds);
            strcpy(uniqueIds[numUniqueIds], ids[i]);
            numUniqueIds++;

        }
    }
    // printf("Identifiers/Keywords found:\n");
    // for (i = 0; i < idCount; i++) {
    //     printf("\t<%s>\n", ids[i]);
    // }

    printf("\n\nUnique Identifiers found:\n");
    for (i = 0; i < numUniqueIds; i++) {
        printf("\t<id, %s>\n", uniqueIds[i]);
    }
    return 0;
}

int next_state(int state, char input) {

    int ans = -1;
    switch(state) {
        case 0: //init
            if(input == '/')
                ans = 5; //comment enter
            else if(char_in(input, op, op_len) || isspace(input) || char_in(input, dlm, dlm_len) || isdigit(input))
                ans = 0; //stay
            else if(isalpha(input) || input == '_')
                ans = 1; //start ID
            else if(input == '\"')
                ans = 2; //str
            else if(input == '\'')
                ans = 3; //char
            else if(input == '#')
                ans = 4; //preprocessor
            break;

        case 1: // id
            if (isalnum(input) || input == '_')
                ans = 1; //stay
            else if(char_in(input, op, op_len) || isspace(input) || char_in(input, dlm, dlm_len))
                ans = 0; //initial
            break;

        case 2: //str
            if (input == '\"')
                ans = 0; // initial
            else if (input == '\\')
                ans = 8; // str escape
            else
                ans = 2; //stay
            break;

        case 3: //char
            if (input == '\'')
                ans = 0; // initial
            else if (input == '\\')
                ans = 9; // char escape
            else
                ans = 3; //stay
            break;

        case 4: //preprocessor
            if (input == '\n')
                ans = 0; //initial
            else
                ans = 4; //stay
            break;

        case 5: //comment enter
            if (input == '*')
                ans = 6; //comment body
            else if(char_in(input, op, op_len) || isspace(input) || char_in(input, dlm, dlm_len) || isdigit(input))
                ans = 0; //initial
            else if(isalpha(input) || input == '_')
                ans = 1; //start ID
            else if(input == '\"')
                ans = 2; //str
            else if(input == '\'')
                ans = 3; //char
            break;

        case 6: //comment body
            if (input == '*')
                ans = 7; //comment close
            else
                ans = 6;
            break;

        case 7: //comment close
            if (input == '/')
                ans = 0; //initial
            else
                ans = 6; //comment body
            break;

        case 8: // string escape
            ans = 2;
            break;

        case 9:
            ans = 3;
            break;

        default:
            fprintf(stderr, "Invalid case, state = %d, input = %c!\n", state, input);
            exit(2);
            break;
    }
    if(ans == -1)
        printf("Returning -1, state = %d, input = %c!\n", state, input);
    return ans;
}


int string_in(char* elt, char* set[], int len) {
    int ans = 0;
    int i;

    for (i = 0; i < len; i++) {
        if (strcmp(elt, set[i]) == 0) {
            ans = 1;
            break;
        }
    }
    return ans;
}

int string_in1(char* elt, char set[][MAX_ID_LEN], int len) {
    int ans = 0;
    int i;

    for (i = 0; i < len; i++) {
        if (strcmp(elt, set[i]) == 0) {
            ans = 1;
            break;
        }
    }
    return ans;
}



int char_in(char elt, char set[], int len) {
    int ans = 0;
    int i;
    for (i = 0; i < len; i++) {
        if (elt == set[i]) {
            ans = 1;
            break;
        }
    }
    return ans;
}
