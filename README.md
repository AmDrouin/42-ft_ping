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

## ICMP Package

octet :  0      1      2      3      4      5      6      7      8 ........... 23   24 ......... 63
       ┌──────┬──────┬─────────────┬─────────────┬─────────────┬──────────────────┬───────────────┐
       │ type │ code │  checksum   │ identifiant │  séquence   │ horodatage       │ remplissage   │
       │  8   │  0   │  (calculé)  │   p->id     │  0, 1, 2…   │ (struct timeval) │               │
       └──────┴──────┴─────────────┴─────────────┴─────────────┴──────────────────┴───────────────┘
       └─────────────── en-tête ICMP : 8 octets ──────────────┘└────────── données : 56 octets ────┘