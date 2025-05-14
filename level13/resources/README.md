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

Alternatively, we can use GDB (GNU Debugger) to manipulate the program's execution flow and memory at runtime.

**Steps:**

1.  **Start GDB with the binary:**
    The `-q` flag starts GDB in quiet mode.
    ```bash
    level13@SnowCrash:~$ gdb -q ./level13
    Reading symbols from /home/user/level13/level13...(no debugging symbols found)...done.
    ```

2.  **Disassemble the `main` function:**
    This helps us understand the program's logic and find the UID check.
    ```gdb
    (gdb) disas main
    Dump of assembler code for function main:
       0x0804858c <+0>:     push   %ebp
       0x0804858d <+1>:     mov    %esp,%ebp
       0x0804858f <+3>:     and    $0xfffffff0,%esp
       0x08048592 <+6>:     sub    $0x10,%esp
       0x08048595 <+9>:     call   0x8048380 <getuid@plt>
       0x0804859a <+14>:    cmp    $0x1092,%eax
       0x0804859f <+19>:    je     0x080485cb <main+63>
       0x080485a1 <+21>:    call   0x8048380 <getuid@plt>
       0x080485a6 <+26>:    mov    $0x80486c8,%edx
       0x080485ab <+31>:    movl   $0x1092,0x8(%esp)
       0x080485b3 <+39>:    mov    %eax,0x4(%esp)
       0x080485b7 <+43>:    mov    %edx,(%esp)
       0x080485ba <+46>:    call   0x8048360 <printf@plt>
       0x080485bf <+51>:    movl   $0x1,(%esp)
       0x080485c6 <+58>:    call   0x80483a0 <exit@plt>
       0x080485cb <+63>:    movl   $0x80486ef,(%esp)
       0x080485d2 <+70>:    call   0x8048474 <ft_des>
       0x080485d7 <+75>:    mov    $0x8048709,%edx
       0x080485dc <+80>:    mov    %eax,0x4(%esp)
       0x080485e0 <+84>:    mov    %edx,(%esp)
       0x080485e3 <+87>:    call   0x8048360 <printf@plt>
       0x080485e8 <+92>:    leave
       0x080485e9 <+93>:    ret
    End of assembler dump.
    ```
    From the disassembly, we see the `getuid()` level13/resources/README.md:30 call at `0x08048595` and the comparison at `0x0804859a`:
    ```assembly
       0x08048595 <+9>:     call   0x8048380 <getuid@plt>
       0x0804859a <+14>:    cmp    $0x1092,%eax
    ```
    The result of `getuid()`level13/resources/README.md:30 is stored in the `%eax` register, which is then compared with `0x1092` (decimal 4242).

3.  **Set a breakpoint right after the `getuid()` call and before the comparison:**
    We choose the address `0x0804859a`, which is the `cmp` instruction. This allows us to modify `%eax` before it's used in the comparison.
    ```gdb
    (gdb) break *0x0804859a
    Breakpoint 1 at 0x0804859a
    ```

4.  **Run the program:**
    ```gdb
    (gdb) run main
    Starting program: /home/user/level13/level13 main

    Breakpoint 1, 0x0804859a in main ()
    ```
    The program stops at our breakpoint.

5.  **Modify the value in the `%eax` register:**
    In x86 architecture, function return values are typically stored in the `%eax` register. We set `%eax` to the expected UID, `0x1092` (4242).
    ```gdb
    (gdb) set $eax=0x1092
    ```

6.  **Continue execution:**
    ```gdb
    (gdb) continue
    Continuing.
    your token is 2A31L79asukciNyi8uppkEuSx
    ```
    With the modified UID value in `%eax`, the comparison `cmp $0x1092,%eax` will succeed, and the program proceeds to decrypt and print the token.

This method doesn't require modifying the binary itself, instead changing the program's behavior at runtime by manipulating a CPU register.

## 5. Solution and Token

Running the binary with the UID check bypassed produced the token:

```
your token is 2A31L79asukciNyi8uppkEuSx
```

This token can be used to log in as level14 user.
