# 42-ft_ping
Recoding the ping command. An introduction to communication (TCP/IP) between two machines on a network.
The project must be in C and compile with a Makefile...


### How to compile and execute ?
 ```bash
 make
 sudo ./ft_ping [-v] [-?] <host>
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

