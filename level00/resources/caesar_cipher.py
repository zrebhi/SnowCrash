#!/usr/bin/env python3

# This function shifts each letter in the text by a certain number of positions in the alphabet
def caesar_shift(text, shift_amount):
    result = ""
    
    for character in text:
        if character.isalpha():
            ascii_value = ord(character) # ord returns the ASCII value of the character
            alphabet_position = ascii_value - 97  # 97 is the ASCII value of 'a'
            new_position = (alphabet_position + shift_amount) % 26            
            new_ascii_value = new_position + 97            
            new_character = chr(new_ascii_value)
            result += new_character
        else:
            # If not a letter, keep the character as is
            result += character
    
    return result

encoded_text = "cdiiddwpgswtgt"

# Try every possible shift from 1 to 25
print("Trying all possible Caesar cipher shifts:")
for shift in range(1, 26):
    decoded_text = caesar_shift(encoded_text, shift)
    print(f"Shift {shift}: {decoded_text}")