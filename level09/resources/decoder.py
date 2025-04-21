#!/usr/bin/env python3

# This script decodes the token file by reversing the transformation
# applied by the level09 program (subtracting each character's position from its value)

def decode(encoded_data):
    decoded_string = ""
    
    for position, char_value in enumerate(encoded_data):
        decoded_character = chr(char_value - position)
        decoded_string += decoded_character
    
    return decoded_string

# Open the token file in binary mode
with open('token', 'rb') as file:
    # Read the content and remove any whitespace
    token_data = file.read().strip()
    print(f"Read token data: {token_data}")

decoded_token = decode(token_data)

print("Decoded token:", decoded_token)