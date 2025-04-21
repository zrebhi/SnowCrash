# Level 14: Reverse Engineering the getflag Binary

This document explains the approach for level 14, the final level of the SnowCrash challenge.

## 1. Initial Assessment

Upon accessing level14, we found no special files or hints in the home directory:

```bash
level14@SnowCrash:~$ ls -la
total 12
dr-x------ 1 level14 level14  100 Mar  5  2016 .
d--x--x--x 1 root    users    340 Aug 30  2015 ..
-r-x------ 1 level14 level14  220 Apr  3  2012 .bash_logout
-r-x------ 1 level14 level14 3518 Aug 30  2015 .bashrc
-r-x------ 1 level14 level14  675 Apr  3  2012 .profile
```

With no obvious attack vector and being the final level, we concluded that we needed to directly examine the `getflag` binary itself.

## 2. Reverse Engineering the getflag Binary

We used Ghidra, a decompiler, to analyze the `getflag` binary and understand its internal workings:

```bash
level14@SnowCrash:~$ which getflag
/bin/getflag
```

After decompilation, we discovered that:

1. The binary contained **anti-debugging measures** using `ptrace` to prevent analysis
2. It performed various **anti-tampering checks** for LD_PRELOAD and library injection
3. It used `getuid()` to determine which user was running the program
4. Based on the user ID, it would decrypt and display the appropriate flag
5. All tokens for all levels were **hardcoded as encrypted strings** within the binary

## 3. Understanding the Structure

The key section of the decompiled code showed how flags were determined:

```c
__uid_t user_id = getuid();

// Determine which flag to print based on user ID
switch (user_id) {
    case 0: // root
        fwrite("You are root are you that dumb ?\n", 1, 33, stdout); // Updated size
        break;
    // ... cases for other levels ...
    case 3014: // level14/flag14 (Updated case)
        fputs(ft_des("g <t61:|4_|!@IF.-62FH&G~DCK/Ekrvvdwz?v|"), stdout);
        break;
    default:
        fwrite("\nNope there is no token here for you sorry. Try again :)",
               1, 56, stdout); // Updated size
        break;
}
```

We identified that the flag for level14 was encoded as `"g <t61:|4_|!@IF.-62FH&G~DCK/Ekrvvdwz?v|"` and was decrypted using a function called `ft_des()`.

## 4. Exploitation Method

Rather than trying to bypass the UID check using GDB (which was complicated due to anti-debugging measures), we took a simpler approach:

1. **Extract the decryption function**: We pulled out the `ft_des()` function from the decompiled code
2. **Extract the encrypted flag string**: We identified the encrypted flag for level14
3. **Create a standalone program**: We developed a small C program that used the extracted function to decrypt the level14 flag directly

Here's the standalone decryption program we created:

```c
char *ft_des(char *encrypted_string)
{
  // Extracted from the decompiled getflag binary
  // Check decrypt_flag.c for the full implementation
  // It's not very important, we just need to re-use it to decrypt the flag
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
```

## 5. Execution and Results

We compiled and ran our program to obtain the flag:

```bash
level14@SnowCrash:~$ gcc -o decrypt_flag14 decrypt_flag14.c
level14@SnowCrash:~$ ./decrypt_flag14
Flag14 token: 7QiHafiNa3HVozsaXkawuYrTstxbpABHD8CPnHJ
```

To verify this was correct, we used the token to log in as flag14:

```bash
level14@SnowCrash:~$ su flag14
Password: 7QiHafiNa3HVozsaXkawuYrTstxbpABHD8CPnHJ
Congratulation. Type getflag to get the key and send it to me the owner of this livecd :)
flag14@SnowCrash:~$ getflag
Check flag.Here is your token : 7QiHafiNa3HVozsaXkawuYrTstxbpABHD8CPnHJ
```

## 6. Key Insights

This level demonstrated several important security concepts:

1. **Security through obscurity is not effective**: Even with anti-debugging measures, the binary could be analyzed and its functionality extracted
2. **Embedded secrets are vulnerable**: Hardcoding encrypted secrets directly in binaries is risky as the decryption algorithm can be extracted
3. **Defense in depth**: The program employed multiple protection mechanisms, but bypassing just one (by extracting the decryption logic) was sufficient to defeat the entire security model
4. **Alternative approaches**: Sometimes the simplest attack vector (extracting and reusing code) is more effective than complex approaches (manipulating program execution with debuggers)

By analyzing the binary's functionality and extracting just what we needed, we were able to obtain the final token and complete the SnowCrash challenge!
