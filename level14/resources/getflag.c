#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>

// Function prototypes
char* ft_des(char* encrypted_string);
int syscall_open(void);
unsigned int syscall_gets(char* buffer, int size);
int isLib(char* str, void* pattern);
char* afterSubstr(char* str, void* substr);

int main(void)
{
    // Stack canary and local variables
    bool is_lib_detected = false;
    char buffer[256];
    FILE* output_stream = NULL;
    
    // Anti-debugging check using ptrace
    if (ptrace(PTRACE_TRACEME, 0, 1, 0) < 0) {
        puts("You should not reverse this");
        return 1;
    }
    
    // Anti-LD_PRELOAD checks
    if (getenv("LD_PRELOAD") != NULL) {
        fwrite("Injection Linked lib detected exit..\n", 1, 37, stderr);
        return 1;
    }
    
    // Check for preload configuration
    int preload_fd = open("/etc/ld.so.preload", 0);
    if (preload_fd >= 1) {
        fwrite("Injection Linked lib detected exit..\n", 1, 37, stderr);
        return 1;
    }
    
    // Check memory maps for suspicious libraries
    int maps_fd = syscall_open();
    if (maps_fd == -1) {
        fwrite("/proc/self/maps is unaccessible, probably a LD_PRELOAD attempt exit..\n", 
               1, 70, stderr);
        return 1;
    }
    
    // Read and check memory maps
    while (1) {
        unsigned int bytes_read = syscall_gets((int)buffer, 256);
        if (bytes_read == 0) break;
        
        // Check for suspicious libraries
        if (isLib(buffer, "libc") == 0) {
            if (is_lib_detected) {
                // Check for trusted library
                if (isLib(buffer, "libft") != 0) {
                    // Print flag based on user ID
                    fwrite("Check flag.Here is your token : ", 1, 32, stdout);
                    
                    __uid_t user_id = getuid();
                    
                    // Determine which flag to print based on user ID
                    switch (user_id) {
                        case 0: // root
                            fwrite("You are root are you that dumb ?\n", 1, 33, stdout);
                            break;
                        case 3000: // level00
                            fputs(ft_des("I`fA>_88eEd:=`85h0D8HE>,D"), stdout);
                            break;
                        case 3001: // level01
                            fputs(ft_des("7`4Ci4=^d=J,?>i;6,7d416,7"), stdout);
                            break;
                        case 3002: // level02
                            fputs(ft_des("<>B16\\AD<C6,G_<1>^7ci>l4B"), stdout);
                            break;
                        case 3003: // level03
                            fputs(ft_des("B8b:6,3fj7:,;bh>D@>8i:6@D"), stdout);
                            break;
                        case 3004: // level04
                            fputs(ft_des("?4d@:,C>8C60G>8:h:Gb4?l,A"), stdout);
                            break;
                        case 3005: // level05
                            fputs(ft_des("G8H.6,=4k5J0<cd/D@>>B:>:4"), stdout);
                            break;
                        case 3006: // level06
                            fputs(ft_des("H8B8h_20B4J43><8>\\ED<;j@3"), stdout);
                            break;
                        case 3007: // level07
                            fputs(ft_des("78H:J4<4<9i_I4k0J^5>B1j`9"), stdout);
                            break;
                        case 3008: // level08
                            fputs(ft_des("bci`mC{)jxkn<\"uD~6%g7FK`7"), stdout);
                            break;
                        case 3009: // level09
                            fputs(ft_des("Dc6m~;}f8Cj#xFkel;#&ycfbK"), stdout);
                            break;
                        case 3010: // level10
                            fputs(ft_des("74H9D^3ed7k05445J0E4e;Da4"), stdout);
                            break;
                        case 3011: // level11
                            fputs(ft_des("70hCi,E44Df[A4B/J@3f<=:`D"), stdout);
                            break;
                        case 3012: // level12
                            fputs(ft_des("8_Dw\"4#?+3i]q&;p6 gtw88EC"), stdout);
                            break;
                        case 3013: // level13
                            fputs(ft_des("boe]!ai0FB@.:|L6l@A?>qJ}I"), stdout);
                            break;
                        case 3014: // level14/flag14
                            fputs(ft_des("g <t61:|4_|!@IF.-62FH&G~DCK/Ekrvvdwz?v|"), stdout);
                            break;
                        default:
                            fwrite("\nNope there is no token here for you sorry. Try again :)", 
                                   1, 56, stdout);
                            break;
                    }
                    
                    fputc('\n', stdout);
                    break;
                }
                
                // Check for malicious library
                char* lib_path = afterSubstr(buffer, "lib");
                if (lib_path == NULL) {
                    fwrite("LD_PRELOAD detected through memory maps exit ..\n", 
                           1, 48, stderr);
                    break;
                }
            } else {
                is_lib_detected = true;
            }
        }
    }
    
    return 0;
}