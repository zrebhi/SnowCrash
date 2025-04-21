#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Function prototype
char *ft_des(char *encrypted_input);

void main(void)
{
    int current_uid;
    char *decrypted_token;
    
    // current_uid = getuid();
    // if (current_uid != 4242) {
    //     printf("UID %d started us but we we expect %d\n", current_uid, 0x1092);
    //     exit(1);
    // }
    decrypted_token = ft_des("boe]!ai0FB@.:|L6l@A?>qJ}I");
    printf("your token is %s\n", decrypted_token);
    return;
}

char *ft_des(char *encrypted_input)
{
    char current_char;
    char *result;
    unsigned int str_length;
    char *temp_ptr;
    unsigned char flag;
    unsigned int position;
    int key_index;
    int i;
    
    flag = 0;
    result = strdup(encrypted_input);
    key_index = 0;
    position = 0;
    
    do {
        str_length = 0xffffffff;
        temp_ptr = result;
        
        // Find string length
        do {
            if (str_length == 0) break;
            str_length = str_length - 1;
            current_char = *temp_ptr;
            temp_ptr = temp_ptr + (unsigned int)flag * -2 + 1;
        } while (current_char != '\0');
        
        // Check if we're done processing the string
        if (~str_length - 1 <= position) {
            return result;
        }
        
        // Reset key cycle
        if (key_index == 6) {
            key_index = 0;
        }
        
        // Process even positions
        if ((position & 1) == 0) {
            for (i = 0; i < "0123456"[key_index]; i++) {
                result[position] = result[position] - 1;
                if (result[position] == '\x1f') {
                    result[position] = '~';
                }
            }
        }
        // Process odd positions
        else {
            for (i = 0; i < "0123456"[key_index]; i++) {
                result[position] = result[position] + 1;
                if (result[position] == '\x7f') {
                    result[position] = ' ';
                }
            }
        }
        
        position++;
        key_index++;
    } while(1);
}