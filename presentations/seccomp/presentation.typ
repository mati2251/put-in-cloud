#import "@preview/polylux:0.4.0": *

#set page(paper: "presentation-16-9")
#set text(size: 25pt, font: "Lato")

#slide[
  #set align(horizon)
  #grid(
    columns: (1fr, auto),
  )[
    #set align(horizon)
    = secure computing - seccomp
    Mateusz Karłowski
  ][
    #align(left + horizon)[
      #image("images/padlock.jpg", height: 8cm)
    ]
  ]
]

#slide[
  #set align(horizon + center)
  #text(size: 150pt, weight: "bold", fill: gray.lighten(60%))[2005]
]

#slide[
  == CPUShare

  #v(1cm)
  #set text(size: 22pt)
  #list(spacing: 1.2em,
    [Founded by Andrea Arcangeli in 2005],
    [Marketplace for idle CPU cycles — owners rent out their machines],
    [Clients submit jobs and pay for compute time],
    [Goal: make unused hardware profitable for Linux users],
  )
]

#slide[
  == The Problem

  #v(1cm)
  #set text(size: 22pt)
  #list(spacing: 1.2em,
    [Client code runs directly on the owner's machine],
    [Nothing stops a malicious client from calling *any* syscall],
    [`fork` a hundred processes, `open` private files, `connect` to the network...],
    [Containers and KVM don't exist yet — virtualization is too slow],
    [How do you run *untrusted code* safely with zero overhead?],
  )
]

#slide[
  == The Solution

  #v(0.8cm)
  #set text(size: 22pt)

  For pure computation, a process only ever needs:

  #v(0.5cm)
  #set text(size: 20pt)
  #table(
    columns: (auto, 1fr),
    fill: (x, y) => if y == 0 { gray.lighten(90%) },
    [*Syscall*], [*Why*],
    [`read`], [receive input data],
    [`write`], [return results],
    [`exit`], [terminate cleanly],
    [`sigreturn`], [return from signal handler],
  )

  #v(0.6cm)
  #rect(fill: blue.lighten(95%), stroke: (left: 5pt + blue), inset: 15pt)[
    #set text(size: 22pt)
    Lock the process to these 4 syscalls. Anything else → *instant kill*.
  ]
]

#slide[
  == seccomp strict: first iteration

  #v(0.5cm)
  #set text(size: 22pt)
  Block all syscalls except `read`, `write`, `exit`, and `sigreturn`. This is the *seccomp strict* mode, introduced in 2005.

  #v(0.5cm)
  #rect(fill: gray.lighten(95%), stroke: 1pt + gray, inset: 15pt, width: 100%)[
    #set align(left)
    #text(size: 18pt)[```bash
echo 1 > /proc/<PID>/seccomp
    ```]
  ]
]

#slide[
  #set align(horizon + center)
  #text(size: 150pt, weight: "bold", fill: gray.lighten(60%))[2007]
]

#slide[
  == Interface change: prctl

  #v(0.5cm)
  #set text(size: 22pt)
  The `/proc/<PID>/seccomp` interface was removed — replaced with `prctl`:

  #v(0.5cm)
  #rect(fill: gray.lighten(95%), stroke: 1pt + gray, inset: 15pt, width: 100%)[
    #set align(left)
    #text(size: 18pt)[```c
#include <sys/prctl.h>
#include <linux/seccomp.h>

prctl(PR_SET_SECCOMP, SECCOMP_MODE_STRICT);

    ```]
  ]

  #v(0.5cm)
  #rect(fill: blue.lighten(95%), stroke: (left: 5pt + blue), inset: 15pt)[
    #set text(size: 22pt)
    Key difference: a process now opts *itself* in — no external actor needed
  ]
]

#slide[
  #set align(horizon + center)
  #text(size: 120pt, weight: "bold", fill: blue.lighten(30%))[Demo]
]

#slide[
  #set align(horizon + center)
  #text(size: 150pt, weight: "bold", fill: gray.lighten(60%))[1992]
]

#slide[
  == tcpdump

  #v(1cm)
  #set text(size: 22pt)
  #list(spacing: 1.2em,
    [Network debugging tool — captures packets on a live interface],
    [Developed at Lawrence Berkeley National Laboratory],
    [Lets you filter traffic: "show me only HTTP", "only packets from 10.0.0.1"],
    [Became the standard tool for network analysis and troubleshooting],
  )
]

#slide[
  == The Problem

  #v(0.8cm)
  #set text(size: 22pt)
  #list(spacing: 1.2em,
    [Kernel copies *every single packet* to user space],
    [tcpdump then checks if the packet matches the filter — and discards most of them],
    [On a busy network: millions of useless copies per second],
    [High CPU usage, dropped packets, poor performance],
  )

  #v(0.6cm)
  #rect(fill: blue.lighten(95%), stroke: (left: 5pt + blue), inset: 15pt)[
    #set text(size: 22pt)
    Filtering happens *too late* — after the expensive copy to user space
  ]
]

#slide[
  == The Solution: BPF

  #v(0.8cm)
  #set text(size: 22pt)
  #list(spacing: 1.2em,
    [McCanne & Jacobson (1992 USENIX paper): move the filter *into the kernel*],
    [User supplies a small filter program — kernel runs it on each packet *before* copying],
    [Only matching packets ever reach user space],
    [Huge performance gain — became the foundation of tcpdump and libpcap],
    [Merged into Linux in 1997 (kernel 2.1.75)],
  )
]

#slide[
  #set align(center + horizon)
  #image("images/bpf_diagram.jpg", height: 85%)
  #place(bottom + right,
    rect(fill: black.transparentize(30%), inset: 6pt)[
      #set text(size: 11pt, fill: white)
      #link("https://www.sciencedirect.com/book/monograph/9781931836746/snort-intrusion-detection-2-0#book-info")[Snort Intrusion Detection 2.0]
    ]
  )
]

#slide[
  == BPF assembly language

  #v(0.5cm)
  #set text(size: 21pt)
    #v(0.3cm)
    #list(spacing: 0.9em,
      [Tiny virtual machine running *inside the kernel*],
      [Registers: `A` (accumulator), `X` (index)],
      [Small scratch memory: `M[0..15]`],
      [No loops — program always terminates],
      [Kernel verifies the program before running it],
    )
]

#slide[
  == BPF filter: UDP port 53

  #v(1cm)
  #rect(fill: gray.lighten(95%), stroke: 1pt + gray, inset: 15pt, width: 100%)[
    #set align(left)
    #text(size: 16pt)[```
ldh  [12]                ; load EtherType (2 bytes at offset 12)
jeq  #0x0800, +0, drop   ; must be IPv4
ldb  [23]                ; load IP protocol (1 byte at offset 23)
jeq  #17,    +0, drop    ; must be UDP (protocol = 17)
ldh  [36]                ; load UDP dest port (2 bytes at offset 36)
jeq  #53,    +0, drop    ; must be port 53
ret  #0xFFFFFFFF         ; accept — return entire packet
drop: ret  #0            ; discard
    ```]
  ]
]

#slide[
  #set align(horizon + center)
  #text(size: 120pt, weight: "bold", fill: blue.lighten(30%))[Demo]
]

#slide[
  == BPF filter in C: UDP port 53

  #v(0.3cm)
  #rect(fill: gray.lighten(95%), stroke: 1pt + gray, inset: 15pt, width: 100%)[
    #set align(left)
    #text(size: 15pt)[```c
/* BPF_STMT(opcode, operand)       — unconditional instruction   */
/* BPF_JUMP(opcode, val, jt, jf)  — jt: jump if true, jf: false */

struct sock_filter filter[] = {
    BPF_STMT(BPF_LD  | BPF_H   | BPF_ABS, 12),         /* ldh [12]  EtherType    */
    BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 0x0800, 0, 4), /* jeq IPv4, else drop    */
    BPF_STMT(BPF_LD  | BPF_B   | BPF_ABS, 23),         /* ldb [23]  IP protocol  */
    BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 17,    0, 2),  /* jeq UDP,  else drop    */
    BPF_STMT(BPF_LD  | BPF_H   | BPF_ABS, 36),         /* ldh [36]  UDP dst port */
    BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, 53,    0, 1),  /* jeq 53,   else drop    */
    BPF_STMT(BPF_RET | BPF_K, 0xFFFFFFFF),              /* accept                 */
    BPF_STMT(BPF_RET | BPF_K, 0),                       /* drop                   */
};
    ```]
  ]
]

#slide[
  #set align(horizon + center)
  #text(size: 150pt, weight: "bold", fill: gray.lighten(60%))[2012]
]

#slide[
  == seccomp-bpf

  #v(0.5cm)
  #set text(size: 22pt)
  In 2012, seccomp was extended to allow *custom BPF filters* — not just the strict mode.

  #v(0.5cm)
  #rect(fill: blue.lighten(95%), stroke: (left: 5pt + blue), inset: 15pt)[
    #set text(size: 22pt)
    This allows for much more flexible policies — e.g. "allow `open` only if the filename is `/tmp/data.txt`"
  ]
]

#slide[
  == What can you do with a syscall?

  #v(0.8cm)
  #set text(size: 20pt)
  #list(spacing: 1.2em,
    [`SECCOMP_RET_ALLOW` - pass through — syscall executes normally],
    [`SECCOMP_RET_KILL` - process killed immediately with SIGSYS],
    [`SECCOMP_RET_ERRNO` - syscall returns -1, errno set by filter],
    [`SECCOMP_RET_TRAP` - deliver SIGSYS to process],
    [`SECCOMP_RET_TRACE` - notify attached ptrace tracer],
    [`SECCOMP_RET_LOG` - allow and log to audit log],
  )

]

#slide[
  #set align(center + horizon)
  #image("images/seccomp_arch.webp", height: 85%)
  #place(bottom + right,
    rect(fill: black.transparentize(30%), inset: 6pt)[
      #set text(size: 11pt, fill: white)
      #link("https://www.researchgate.net/figure/The-architecture-of-the-Seccomp-system-20-in-Linux-Application-developers-specify_fig1_302574274")[_Zachary Tatlock - Jitk: A Trustworthy In-Kernel Interpreter Infrastructure_]
    ]
  )
]

#slide[
  == Filter inheritance

  #v(1cm)
  #set text(size: 22pt)
  #list(spacing: 1.2em,
    [seccomp filter is *inherited* by child processes across `fork` and `exec`],
    [Child can only *add* filters on top — never remove or relax them],
    [Filters stack: all filters in the chain are evaluated, most restrictive wins],
    [A sandboxed process cannot escape by spawning a child],
  )
]

#slide[
  == seccomp-bpf filter: block socket

  #v(0.3cm)
  #set text(size: 21pt)
  BPF input is `struct seccomp_data` — not a packet:

  #v(0.2cm)
  #rect(fill: gray.lighten(95%), stroke: 1pt + gray, inset: 15pt, width: 100%)[
    #set align(left)
    #text(size: 15pt)[```c
struct sock_filter filter[] = {
    /* load syscall number */
    BPF_STMT(BPF_LD | BPF_W | BPF_ABS, offsetof(struct seccomp_data, nr)),
    /* if not socket → allow */
    BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_socket, 0, 3),
    /* load first argument (domain) */
    BPF_STMT(BPF_LD | BPF_W | BPF_ABS, offsetof(struct seccomp_data, args[0])),
    /* if domain != AF_INET → allow */
    BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, AF_INET, 0, 1),
    /* socket(AF_INET, ...) → notify user space */
    BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ERRNO(EPERM)),
    BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
};
    ```]
  ]
]

#slide[
  #set align(horizon + center)
  #text(size: 120pt, weight: "bold", fill: blue.lighten(30%))[Demo]
]

#slide[
  #set align(horizon + center)
  #text(size: 150pt, weight: "bold", fill: gray.lighten(60%))[2019]
]

#slide[
  == seccomp notify

  #v(1cm)
  #set text(size: 22pt)
  #list(spacing: 1.2em,
    [`SECCOMP_RET_USER_NOTIF` — Linux 5.0 (2019)],
    [Instead of killing or blocking, *pause the process* and notify user space],
    [A supervisor process receives the syscall details and decides what to do],
    [Can *allow*, *deny*, or *emulate* the syscall — return any value back to the process],
  )

  #v(0.5cm)
  #rect(fill: blue.lighten(95%), stroke: (left: 5pt + blue), inset: 15pt)[
    #set text(size: 22pt)
    First time seccomp can *change* syscall behavior — not just allow or kill
  ]
]

#slide[
  #set align(center + horizon)
  #image("images/seccomp_notify.webp", height: 85%)
  #place(bottom + right,
    rect(fill: black.transparentize(30%), inset: 6pt)[
      #set text(size: 11pt, fill: white)
      #link("https://www.outflank.nl/blog/2025/12/09/seccomp-notify-injection/")[_Kyle Avery - Linux Process Injection via Seccomp Notifier_]
    ]
  )
]

#slide[
  == seccomp notify: example (libseccomp-go)

  #v(0.2cm)
  #rect(fill: gray.lighten(95%), stroke: 1pt + gray, inset: 15pt, width: 100%)[
    #set align(left)
    #text(size: 13pt)[```go
req, err := libseccomp.NotifReceive(notifyFd)
if err != nil {
	panic(err)
}
fmt.Printf("Received notification for syscall %d\n", req.Data.Syscall)
newFd, err := InjectFd(notifyFd, req, int(file.Fd()))
if err != nil {
	panic(err)
}
fmt.Printf("Injected fd %d into the process\n", newFd)
err = libseccomp.NotifRespond(notifyFd, &libseccomp.ScmpNotifResp{
	ID:     req.ID,
	Val:    uint64(newFd),
	Error:  0,
	Flags:  0,
})
if err != nil {
	panic(err)
}
    ```
  ]
  ]
]

#slide[
  #set align(horizon + center)
  #text(size: 120pt, weight: "bold", fill: blue.lighten(30%))[Demo]
]

#slide[
  == seccomp in the action

  #v(0.8cm)
  #set text(size: 21pt)
  #table(
    columns: (auto, 1fr),
    fill: (x, y) => if y == 0 { gray.lighten(90%) },
    [*Project*], [*How*],
    [Chrome/Chromium],  [renderer sandbox since 2012 — one of the first large users],
    [Docker],           [default JSON seccomp profile, blocks ~40 dangerous syscalls],
    [systemd],          [`SystemCallFilter=` in unit files via libseccomp],
    [OpenSSH],          [sandbox for pre-auth privilege separation (since 9.0)],
    [Firefox],          [content process sandbox on Linux],
    [Flatpak/Bubblewrap],[application sandboxing on the desktop],
    [LXD / Podman],     [seccomp notify for syscall emulation in rootless containers],
    [gvisor],           [kernel for containers - seccomp for syscall interception and emulation],
  )
]

#slide[
  == Sources
  #v(1cm)
  #show link: set text(size: 22pt, fill: blue)
  - #link("https://lwn.net/Articles/656307/")[_LWN(2015), "A seccomp overview"_],
  - #link("https://lwn.net/Articles/120647/")[_LWN(2005), "Securely renting out your CPU with Linux"_],
  - #link("https://docs.kernel.org/bpf/standardization/instruction-set.html")[_Linux kernel documentation, "BPF instruction set"_],
  - #link("https://man7.org/linux/man-pages/man2/seccomp_unotify.2.html")[_man seccomp_unotify(2)_],
  - #link("https://man7.org/linux/man-pages/man2/seccomp.2.html")[_man seccomp(2)_],
  - #link("https://libseccomp.readthedocs.io/en/latest/")[_libseccomp documentation_],
]

