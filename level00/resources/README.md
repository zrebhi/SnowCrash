# Level 00 - Finding the Password

## Initial Approach

When starting the Snow Crash project, we need to find a way to get from `level00` user to `flag00` user. Each level starts with finding the password for the corresponding `flagXX` user.

## Step 1: Searching for Files Owned by flag00

Since we're looking for the `flag00` user's password, a logical step is to search for files owned by the `flag00` user:

```bash
find / -user flag00 2>/dev/null
```

This command searches the entire filesystem for files owned by `flag00` and discards error messages.

## Step 2: Finding Suspicious Files

Step 1 revealed a file that contained what appeared to be encoded text:

```
cdiiddwpgswtgt
```

## Step 3: Decoding with Caesar Cipher

Since the text looked encoded, I tried using a Caesar cipher approach to decode it. A Caesar cipher is a simple substitution cipher where each letter in the plaintext is shifted a certain number of places down or up the alphabet.

I wrote a loop to try all 25 possible shifts of the alphabet:

```python
#!/usr/bin/env python3

# This function shifts each letter in the text by a certain number of positions in the alphabet
def caesar_shift(text, shift_amount):
    result = ""
    
    for character in text:
        if character.isalpha():
            ascii_value = ord(character)
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
```

One of the outputs clearly revealed the password:

```
Shift 11: nottoohardhere
Shift 12: opuuppibteifsf
Shift 13: pqvvqqjcufjgtg
...
```

The shift that produced a readable English phrase was the answer.

## Step 4: Accessing flag00 Account

Using the decoded password, I was able to switch to the `flag00` user:

```bash
su flag00
# Password: nottoohardhere
```

Then I ran the `getflag` command:

```bash
getflag
```
This revealed the token needed to proceed to level01: x24ti5gi3x0ol2eh4esiuxias