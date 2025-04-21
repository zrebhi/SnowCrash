#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/**
 * This function was extracted from the getflag binary
 * Decrypts an encrypted string using a simple algorithm
 * - Even positions: subtract "0123456"[key_index] from character
 * - Odd positions: add "0123456"[key_index] to character
 * - With wrapping at ASCII boundaries
 */
char *ft_des(char *encrypted_string)
{
  // Create a copy of the input string that we'll modify
  char *decrypted = strdup(encrypted_string);
  
  // Variables for string processing
  int key_index = 0;           // Index into the key "0123456"
  unsigned int position = 0;    // Current position in the string
  char *current_ptr;           // Pointer for string traversal
  unsigned int remaining_len;  // Remaining chars to process
  
  // Key used for the encryption - digits specify shift amount
  const char *key = "0123456";
  
  while (1) {
    // Calculate string length from current position
    remaining_len = 4294967295; // Max unsigned int
    current_ptr = decrypted;
    
    while (*current_ptr != '\0' && remaining_len > 0) {
      remaining_len--;
      current_ptr++;
    }
    
    // If we've processed all characters, we're done
    if (~remaining_len - 1 <= position) {
      return decrypted;
    }
    
    // Reset key index after each cycle
    if (key_index == 6) {
      key_index = 0;
    }
    
    // Get the shift amount from the key
    int shift_amount = key[key_index] - '0';  // Convert digit char to number
    
    // Process character at current position
    if (position % 2 == 0) {
      // Even positions: decrease character value
      for (int i = 0; i < shift_amount; i++) {
        decrypted[position]--;
        // Handle underflow: if character becomes ASCII 31, wrap to '~' (126)
        if (decrypted[position] == 31) {
          decrypted[position] = '~';
        }
      }
    } else {
      // Odd positions: increase character value
      for (int i = 0; i < shift_amount; i++) {
        decrypted[position]++;
        // Handle overflow: if character becomes ASCII 127, wrap to ' ' (32)
        if (decrypted[position] == 127) {
          decrypted[position] = ' ';
        }
      }
    }
    
    // Move to next position and key
    position++;
    key_index++;
  }
}

int main(void)
{
  // The encrypted flag14 token extracted from the getflag binary
  char *encrypted = "g <t61:|4_|!@IF.-62FH&G~DCK/Ekrvvdwz?v|";
  
  // Decrypt the token directly
  char *decrypted = ft_des(encrypted);
  
  // Print the result
  printf("Flag14 token: %s\n", decrypted);
  
  // Free the allocated memory
  free(decrypted);
  
  return 0;
}
