# Level 01 - Finding the Password

## Initial Discovery

When examining the system for potential security vulnerabilities, I checked the `/etc/passwd` file, which contains user account information:

```bash
level01@SnowCrash:~$ cat /etc/passwd
```

## The Discovery

While reviewing the file contents, I found something unusual in the entry for `flag01`:

```
flag01:42hDRfypTqqnw:3001:3001::/home/flag/flag01:/bin/bash
```

Unlike most other users that show an `x` in the password field (indicating that the password is stored in `/etc/shadow`), the `flag01` user has what appears to be an actual password hash directly in the file: `42hDRfypTqqnw`.

## Understanding the Vulnerability

This is a significant security vulnerability because:

1. The `/etc/passwd` file is readable by all users
2. Modern systems typically store password hashes in `/etc/shadow` which has restricted permissions
3. Finding an actual hash in `/etc/passwd` means it can be extracted and potentially cracked

## Cracking the Password

To crack this password hash, I used John the Ripper, a popular password cracking tool:

1. First, I extracted the relevant line from `/etc/passwd`:

   ```bash
   echo "flag01:42hDRfypTqqnw:3001:3001::/home/flag/flag01:/bin/bash" > flag01.txt
   ```

2. Then, I used John the Ripper to crack the password:

   ```bash
   john flag01.txt
   ```

3. After some time, John the Ripper found the password: `abcdefg`

## Accessing the flag01 Account

With the cracked password, I was able to switch to the `flag01` user:

```bash
su flag01
# Password: abcdefg
```

## Getting the Flag

Finally, I ran the `getflag` command to retrieve the token for the next level:

```bash
getflag
# Output: Check flag.Here is your token : f2av5il02puano7naaf6adaaf
```

## Conclusion

This level demonstrates the importance of proper password storage mechanisms. Storing password hashes in `/etc/passwd` is a significant security vulnerability since this file is readable by all users on the system. Modern systems use `/etc/shadow` with restricted permissions to mitigate this risk.
