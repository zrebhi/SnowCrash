# Snow Crash - Introduction to Cybersecurity

## Project Overview

Welcome to the Snow Crash project! This project serves as an introduction to the vast world of cybersecurity. As developers, understanding potential software weaknesses and how they can be exploited is crucial for building secure applications.

This project presents a series of challenges, organized into levels (`level00` through `level14`). Each level requires you to discover vulnerabilities and find the password for the corresponding `flagXX` user account to progress. Successfully accessing the `flagXX` account and running the `getflag` command provides the token needed for the next level.

## Getting Started

- To participate in the challenges, you'll need to use the provided Snow Crash virtual machine. You can download the ISO file here: [SnowCrash.iso](https://cdn.intra.42.fr/isos/SnowCrash.iso).
- Connect to the VM via SSH on port 4242 using the credentials provided in the subject for `level00`.

```bash
ssh level00@<VM_IP_ADDRESS> -p 4242
```
## Challenge Themes

This project explores various cybersecurity concepts and techniques across its levels, including:

*   Reverse Engineering Binaries
*   Path Hijacking Exploits
*   Command Injection via Environment Variables
*   Network Traffic Analysis
*   Bypassing Filename Checks
*   PHP Code Injection
*   Time-of-Check to Time-of-Use (TOCTOU) Vulnerabilities
*   Command Injection via Perl CGI Scripts
*   Basic Password Discovery & Cryptography
*   Cron Job Exploitation
*   Reversing Token Transformations
*   Web Command Injection
*   Bypassing UID Checks in Binaries
*   Command Injection via Lua Scripts

## Repository Purpose

This repository serves as a record of the solutions and resources used to complete each level of the Snow Crash challenge, as required by the project guidelines (`en.subject.pdf`). Each `levelXX` directory contains the flag obtained for that level and a `resources` subdirectory detailing the steps taken and any scripts or notes used.

Good luck, and have fun exploring the world of cybersecurity!