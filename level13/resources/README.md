# Level 13: Bypassing UID Check in a Binary

This document explains the vulnerability in the `level13` binary and the steps taken to exploit it.

## 1. Initial Assessment

Upon accessing level13, we found a binary executable file:

```bash
level13@SnowCrash:~$ ls -l
total 8
-rwsr-sr-x 1 flag13 level13 7303 Aug 30  2015 level13
```

Running the binary showed it was looking for a specific user ID:

```bash
level13@SnowCrash:~$ ./level13
UID 2013 started us but we we expect 4242
```

To understand how this binary works, we decompiled it using Ghidra (a powerful reverse engineering tool) to obtain its source code structure. This allowed us to see the internal logic and identify ways to bypass its protection.

## 2. Program Explanation

Based on the decompiled code, we learned that the target is a C program that decrypts a token, but only if run by a specific user.

Key parts of the program:

- It uses `getuid()` to check the current user's ID:

  ```c
  current_uid = getuid();
  if (current_uid != 4242) {
      printf("UID %d started us but we we expect %d\n", current_uid, 0x1092);
      exit(1);
  }
  ```

- If the UID check passes, it calls `ft_des()` to decrypt the token:

  ```c
  decrypted_token = ft_des("boe]!ai0FB@.:|L6l@A?>qJ}I");
  printf("your token is %s\n", decrypted_token);
  ```

- The `ft_des()` function contains a custom decryption algorithm that processes the encrypted input character by character using position-based manipulations.

## 3. Vulnerability Analysis

The primary restriction is the **User ID check**:

- The program only decrypts and shows the token if run by a user with UID 4242
- Our level13 user likely has a different UID

The decryption function `ft_des()` is self-contained within the binary, so if we can bypass the UID check, we can get the token.

## 4. Exploitation Method

There are multiple ways to bypass the UID check:

### Method 1: Decompile and Modify the Binary (Used in Solution)

After decompiling the binary with Ghidra, we were able to:

**Steps:**

1. **Extract the full decompiled C code**:

   - With the decompiled code from Ghidra, we created a C file (`level13.c`) containing the complete program logic
   - This included both the main function with the UID check and the custom `ft_des()` function

2. **Create a modified version** by commenting out the UID check:

   ```c
   // current_uid = getuid();
   // if (current_uid != 4242) {
   //     printf("UID %d started us but we we expect %d\n", current_uid, 0x1092);
   //     exit(1);
   // }
   ```

3. **Compile the modified code**:

   ```bash
   gcc -o modified_level13 level13.c
   ```

4. **Execute the modified binary**:
   ```bash
   ./modified_level13
   ```

### Method 2: Using GDB to Bypass the Check

Alternatively, we used GDB (GNU Debugger) to manipulate the program during execution:

```bash
level13@SnowCrash:~$ gdb -q ./level13
```

The `-q` flag starts GDB in quiet mode, suppressing the introductory and copyright messages for a cleaner output.

**Steps:**

1. **Set a breakpoint at the start of the main function:**

   ```
   (gdb) break main
   Breakpoint 1 at 0x804858f
   ```

2. **Start the program:**

   ```
   (gdb) run
   ```

3. **Find where the getuid() call occurs:**
   When we run to the breakpoint, we need to identify where the `getuid()` function is called. Typically, we'd use `disas main` to view the assembly and identify the instruction.

4. **Modify the return value of getuid():**
   In x86 architecture, function return values are stored in the `eax` register. After `getuid()` executes but before the comparison happens, we set:

   ```
   (gdb) set $eax = 4242
   ```

   This changes the return value of `getuid()` from our actual UID (2013) to the expected value (4242).

5. **Continue execution:**
   ```
   (gdb) continue
   ```
   With the modified UID value in place, the program continues past the check and decrypts the token:
   ```
   your token is 2A31L79asukciNyi8uppkEuSx
   ```

This method doesn't require modifying the binary itself, instead changing the program's behavior at runtime by manipulating a CPU register.

## 5. Solution and Token

Running the binary with the UID check bypassed produced the token:

```
your token is 2A31L79asukciNyi8uppkEuSx
```

This token can be used to log in as level14 user.
