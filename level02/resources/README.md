# Level 02 - Network Traffic Analysis

## Understanding the Challenge

In level02, we find a `.pcap` file in the home directory. This presents an opportunity to analyze network traffic to find credentials.

```bash
level02@SnowCrash:~$ ls -l
----r--r-- 1 flag02  level02 8302 Aug 30  2015 level02.pcap
```

## What is a PCAP File?

A PCAP (Packet Capture) file contains network traffic data captured by tools like tcpdump or Wireshark. These files record complete network conversations, including:

- The source and destination IP addresses
- Protocol information
- Packet contents (potentially containing sensitive data)
- Timestamps of each packet

## Installing Wireshark

To properly analyze the PCAP file, we first needed to transfer it to our local machine and use a specialized tool:

```bash
# Transfer the file to our local machine
scp -P 4242 level02@[VM_IP]:/home/user/level02/level02.pcap .

# Install Wireshark on local Linux machine
sudo apt install wireshark
```

## Following the TCP Stream

After opening the PCAP file in Wireshark, we followed these steps to analyze the data:

1. Right-clicked on a packet
2. Selected "Follow > TCP Stream"
3. Observed a login session where someone attempted to log in

## Extracting the Password

Looking at the TCP stream, we could see that after a "Password:" prompt, characters were typed one by one. By examining each packet carefully, we observed:

```
07:23:34.363418: 'f'
07:23:35.253053: 't'
07:23:35.873401: '_'
07:23:36.343811: 'w'
07:23:36.573585: 'a'
07:23:36.803330: 'n'
07:23:36.943261: 'd'
07:23:37.283708: 'r'
07:23:38.864101: '.' (backspace)
07:23:39.233935: '.' (backspace)
07:23:39.604364: '.' (backspace)
07:23:40.374542: 'N'
07:23:40.574439: 'D'
07:23:42.264451: 'R'
07:23:43.574954: 'e'
07:23:44.014684: 'L'
07:23:44.635281: '0'
07:23:44.805020: 'L'
```

## Understanding the Backspaces

The three consecutive dots ('.') in the capture represent backspace characters. The user typed "ft_wandr", then pressed backspace three times to delete "ndr", and then typed "NDReL0L".

Therefore, the correct password is: `ft_waNDReL0L`

## Getting the Flag

With this password, we could access the flag02 account:

```bash
su flag02
# Password: ft_waNDReL0L
getflag
Output: Check flag.Here is your token : kooda2puivaav1idi4f57q8iq
```

## Conclusion

This level demonstrates the importance of secure communication channels. Sending passwords over unencrypted connections allows anyone with access to the network traffic to capture credentials. Always use encrypted protocols like SSH, HTTPS, or VPNs for sensitive information.

Similar code found with 1 license type