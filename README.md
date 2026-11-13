# 42-ft_ping
Recoding the ping command. An introduction to communication (TCP/IP) between two machines on a network. The project must be in C and compile with a Makefile...

The reference implementation is **`ping` from inetutils-2.0** (check with `ping -v`): except for the RTT values and the revere DNS resolution, the output of `ft_ping` must be formatted exactly like it.


---

## Table of contents

1. [Usage](#usage)
2. [Project structure](#project-structure)
3. [How it works](#how-it-works)
4. [Key concepts](#key-concepts)
5. [Bonus: `--DEBUG` mode](#bonus---debug-mode)
6. [Testing](#testing)
7. [Development environment](#development-environment)
8. [References](#references)

 ---

## Usage

### How to compile and execute ?
 ```bash
 make           # build ./ft_ping
 make clean     # remove object files (objs/)
 make fclean    #remove object files and the binary
 make re        #rebuild from scratch

 sudo ./ft_ping [options] <host>
 ```

`sudo` is required: `ft_ping` opens a ***raw socket***, which needs root privilieges. 


### Options 

| Option | Description |
|---|---|
| `-v`, `--verbose` | Verbose output: shows the packet id, and dumps the IP/ICMP headers when an ICMP error is received |
| `-?`, `--help` | Print the help and exit |
| `--usage` | Print a short usage message and exit |
| `--DEBUG` | **Bonus**: draws every sent/received packet (IP + ICMP headers, payload hexdump) on `stderr` | 

`<host>` can be an IPv4 address (`8.8.8.8`), a hostname (`localhost`) or a FQDN (`google.com`).

### Exit codes

| Code | Meaning |
|---|---|
| `0` | At least one reply was received |
| `1` | No reply received, unknown host, or socket error |
| `64` | Usage error (missing host, invalid option) |

## Project structure

```text

.
├── Makefile
├── include/
│   └── ft_ping.h       # includes, constants, t_ping struct, prototypes
└── src/
    ├── main.c          # entry point, SIGINT handler, "PING ..." header line
    ├── parser.c        # command-line options (-v, -?, --help, --usage, --DEBUG)
    ├── resolve.c       # hostname -> IPv4 with getaddrinfo()
    ├── socket.c        # raw socket, checksum, building and sending the Echo Request
    ├── loop.c          # main loop: send every second, wait with select(), read replies, RTT
    ├── icmp_error.c    # ICMP errors (Time Exceeded, Destination Unreachable) and verbose dump
    ├── stats.c         # final statistics (loss, min/avg/max/stddev)
    └── debug.c         # bonus: --DEBUG packet drawings and hexdump
```

## How it works 

```text
parse_args()  ->  resolve_host()  ->  create_socket()  ->  print "PING ..."
                                                                  |
                                     +----------------------------+
                                     v
                 +---------> send_echo()          build ICMP Echo Request + timestamp, sendto()
                 |               |
                 |               v
                 |          wait_reply()          select() until 1 s has passed since the send
                 |               |                  -> recvfrom() -> handle_packet()
                 |               |                       Echo Reply with our id  -> print_reply()
                 |               |                       Time Exceeded / Unreach -> print_error()
                 +---------------+
                     until Ctrl+C (g_stop = 1)
                                     |
                                     v
                               print_stats()
```

---

## Key concepts

### Hostname resolution

Before sending anything, we must turn what the user typed into a binary IPv4 address: the network only understands IP addresses, so names must be translated first.

This is done by `getaddrinfo()`. The `hints` structure narrows the search:
- `AF_INET` keeps only IPv4 results.
- `SOCK_RAW` + `IPPROTO_ICMP` keep one node per address instead of one per socket type.

`getaddrinfo()` allocates a linked list of `struct addrinfo`, one node per address found (a name can map to several addresses).`ft_ping` only uses the first node, so it may show different IPs for the same name with the reference.
The list is then freed with `freeaddrinfo()` and `inet_ntop()` convert the address back to a string for display.

`getaddrinfo()` tries, in order;

1. **Numeric address**: the text is already an IP.
2. **`/etc/hosts`**: a local file of name-to-IP mappings (for example, `localhost` ⇔ `127.0.0.1`).
3. **DNS** (Domain Name System) query: only if the name was not found locally (the DNS servers are listed in `/etc/resolv.conf`).
As the subject requires, **no reverse DNS** is done on replies: they show `64 bytes from <IP>`, never a name.

### ICMP and Echo Request / Echo Reply
 
ICMP (Internet Control Message Protocol, RFC 792) is carried directly inside IP (protocol number 1): there is no TCP, no UDP and no port. `ping` sends an **Echo Request** (type 8) and the target answers with an **Echo Reply** (type 0)

### Raw socket

```c
socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
```
 
- **Sending:** we write the ICMP message ourselves, and the kernel adds the IP header.
- **Receiving:** we get the **whole IP packet**. The ICMP header starts after `ip->ihl * 4` bytes (IHL is the header length in 32-bit words).
- A raw ICMP socket receives **every** ICMP packet arriving on the machine, including the replies of other `ping` processes. This is why each packet carries an **identifier** (the PID), and only replies with our identifier are kept.

### ICMP packet
 
```text
byte:  0      1      2      3      4      5      6      7      8 ........... 23   24 ......... 63
     ┌──────┬──────┬─────────────┬─────────────┬─────────────┬──────────────────┬───────────────┐
     │ type │ code │  checksum   │ identifier  │  sequence   │ timestamp        │ padding       │
     │  8   │  0   │ (computed)  │   p->id     │  0, 1, 2…   │ (struct timeval) │ (zeros)       │
     └──────┴──────┴─────────────┴─────────────┴─────────────┴──────────────────┴───────────────┘
     └────────────── ICMP header: 8 bytes ───────────────────┘└──────── data: 56 bytes ─────────┘
```
 
- **56 data bytes** + 8 bytes of header = **64 bytes** of ICMP, which makes **84 bytes** on the wire with the 20-byte IP header.
- `sequence` starts at **0**, like inetutils.

### Checksum (RFC 1071)
 
The ICMP checksum is the **one's complement of the one's complement sum** of all 16-bit words of the message (header + data):
 
1. Set the `checksum` field to 0.
2. Add all 16-bit words in a 32-bit integer (a trailing odd byte is added on its own).
3. Fold the carries back into the low 16 bits: `sum = (sum & 0xFFFF) + (sum >> 16)`.
4. Return `~sum`.
The receiver runs the same sum over the received message, checksum included, and must get `0xFFFF`.

### Byte order
 
The network is **big-endian**, while x86 is little-endian. Multi-byte fields are converted with `htons()` before sending and `ntohs()` after receiving (identifier, sequence, IP total length...).

### Round-trip time (RTT)
 
The send time (`gettimeofday()`) is copied into the payload. The target sends the payload back unchanged, so on reception:
 
```text
RTT = now - timestamp read from the reply
```
 
There is no need to remember when each packet was sent: the timestamp travels with the packet.

### Timing loop: `select()`
 
One Echo Request is sent per second. After each send, `wait_reply()` calls `select()` on the socket with a timeout equal to the time left in that second. Every reply that arrives in the meantime is read and handled, then the next packet is sent. A lost packet therefore never blocks the program.

### Signals
 
`Ctrl+C` sends `SIGINT`. The handler only sets a flag (`volatile sig_atomic_t g_stop = 1`), because `printf()` is not async-signal-safe. The main loop sees the flag, exits, and the statistics are printed outside the handler.

 
### ICMP errors and verbose mode
 
When a packet cannot be delivered, a router sends back an ICMP error instead of a reply:
 
| Type | Code | Message |
|---|---|---|
| 11 – Time Exceeded | 0 | Time to live exceeded |
| 11 – Time Exceeded | 1 | Frag reassembly time exceeded |
| 3 – Destination Unreachable | 0 / 1 / 2 / 3 | Destination Net / Host / Protocol / Port Unreachable |
| 3 – Destination Unreachable | 4 | Fragmentation needed and DF set |
| 3 – Destination Unreachable | 5 | Source Route Failed |
 
The payload of an ICMP error contains the **original IP header + the first 8 bytes of the original ICMP message**:
 
```text
[ outer IP ][ ICMP error (8 B) ][ original IP header ][ original ICMP header (8 B) ]
                                                        └─ our identifier is here
```
 
`print_error()` reads the identifier inside this embedded header to check that the error is about *our* ping. With `-v`, it also dumps the original IP header (`IP Hdr Dump`) and the ICMP summary, like inetutils. These errors are reported but never stop the program.
 
---

## Bonus: `--DEBUG` mode
 
```bash
sudo ./ft_ping --DEBUG 8.8.8.8
```
 
For every packet sent and received, `--DEBUG` prints on `stderr`:
 
- a **bit ruler** (0 to 31) and the **IP header** drawn like the RFC 791 diagrams, one 32-bit row per line;
- the **ICMP header** drawn the same way;
- a **hexdump** of the 56-byte payload (where the `struct timeval` timestamp can be seen).
Field values are colored by size: cyan for fields of 8 bits or less, blue for 16-bit fields, red for 32-bit fields (addresses).
 
Because the kernel builds the IP header of outgoing packets, the header shown for a **sent** packet is rebuilt by `build_sent_ip()`:
 
- the TTL is read with `getsockopt(IP_TTL)`;
- the source address is found by `connect()`-ing a UDP socket to the target and reading `getsockname()` (no packet is sent);
- the header checksum is computed with the same `checksum()` function.
The IP `id` field stays at 0 in this drawing, because the real value is chosen by the kernel when the packet is sent.
 
---

## Development environment

The project is developed in a Debian VM, acccessible over SSH from a cluster computer.

    ### Packages to install in the VM

| Package | Purpose |
|---|---|
| `build-essential` | all libc headers and `gcc`, `make` |
| `openssh-server` | to connect the VM over SSH |
| `valgrind` | To detect memory leaks and errors |
| `gdb` | Debugger |
| `inetulis-ping` | The reference ping (can be check with `ping -V`) |

### Setting up SSH access

#### 1. start the SSH server (in the VM)

```bash
    sudo systemctl enable --now ssh  #start ssh 
    systemctl status ssh # check if ssh is running
```

#### 2. Forward a port to the VM
        
In the default network mode (NAT), the VM cannot be reached directly.
We forward port **2222** on the host machine to port *22**.

#### 3. Connect from the host machine

```bash
    ssh -p  2222 username@127.0.0.1
```

#### 4. Optionnal : SSH key and shortcut

```bash
    ssh-keygen -t ed25519
    ssh-copy-id -p 2222 username@127.0.0.1
```

Add a shortcut to `~/.ssh/config` on the host machine:
 
```text
    Host ftping
        HostName 127.0.0.1
        Port 2222
        username
```
You can then connect with `ssh ftping`.

### Hostname resolution 

Before sending anything, we need to turns what the user typed into a binary IPv4 address. The network only understands IP addresses so names must be translated first. 

This is done by getaddrinfo(). It takes some flags to precise the search : AF_INET (IPV4), SOCK_RAW and IPPROTO_ICMP (only ICMP message). As the result, getaddrinfo allocates a linked list of struct addrinfo, one node per address found. A name can map to several addresses. ft_ping only uses the first node. As a consequencem ft_ping and a reference ping may show different IPs for the same name.
In the node we can find the address in IPv4 format. 

#### How the address is found

getaddrinfo tries, in order :
    1- Numeric address : if the text is alread an IP. 
    2-/etc/host : a local file with name-to-IP. (we can find localhost <=> 127.0.0.1)
    3-Ask to a DNS (Domaine Name System) : only if the name is not found locally (DNS list can be found in /etc/resolv.conf)


## References
 
- [RFC 792](https://www.rfc-editor.org/rfc/rfc792): Internet Control Message Protocol
- [RFC 791](https://www.rfc-editor.org/rfc/rfc791): Internet Protocol (IPv4 header)
- [RFC 1071](https://www.rfc-editor.org/rfc/rfc1071): Computing the Internet checksum
- [GNU inetutils](https://www.gnu.org/software/inetutils/): reference `ping` implementation
- `man 7 raw`, `man 7 icmp`, `man 3 getaddrinfo`, `man 2 select`