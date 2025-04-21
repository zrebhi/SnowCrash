# Level09 Solution - Reversing Token Transformation

## Analysis

1. We have a binary `level09` that's running with flag09's permissions
2. There's a `token` file in the home directory with binary content
3. When we run `./level09 token`, it outputs `tpmhr`

## Investigation Process

By comparing the token file content with the program output:

- Token content (with binary data): `b'f4kmm6p|=\x82\x7fp\x82n\x83\x82DB\x83Du{\x7f\x8c\x89'`
- Program output when given token: `tpmhr`

We notice that the program doesn't output the entire token content. This suggests it's transforming the input in some way.

Testing with simple inputs confirmed the transformation pattern:

```
./level09 aaaaa → abcde
./level09 11111 → 12345
```

This reveals that the program adds the character's position index to its ASCII value:

- 'a' (position 0) → 'a' (no change)
- 'a' (position 1) → 'b' (incremented by 1)
- 'a' (position 2) → 'c' (incremented by 2)
  And so on...

## Solution

To get the original flag, we need to reverse this transformation by subtracting each character's position from its ASCII value:

```python
def decode(encoded_data):
    decoded_string = ""

    for position, char_value in enumerate(encoded_data):
        decoded_character = chr(char_value - position)
        decoded_string += decoded_character

    return decoded_string
```

Applying this to the token content gives us the full decoded token: `f3iji1ju5yuevaus41q1afiuq`

## Steps to Solve

1. Create and run the Python script to decode the token: `python decoder.py`
2. Use the decoded password (`f3iji1ju5yuevaus41q1afiuq`) to log in as flag09: `su flag09`
3. Run `getflag` to get the token for the next level: `s5cAJpM8ev6XHw998pRWG728z`
