#import "@preview/polylux:0.4.0": *

#set page(paper: "presentation-16-9")
#set text(size: 25pt, font: "Lato")

#slide[
  #set align(horizon)
  #grid(
    columns: (1fr, auto),
  )[
    #set align(horizon)
    = The Evolution of I/O Handling in Linux: io_uring
    Mateusz Karłowski
  ][
    #align(left + horizon)[
      #image("images/tux.webp", height: 8cm)
    ]
  ]
]

#slide[
  == The Beginning: Standard Syscalls

  #v(2cm)
  #set align(center)
  #rect(fill: gray.lighten(95%), stroke: 1pt + gray, inset: 20pt)[
    #set align(left)
    ```c
    #include <unistd.h>

    // syscall number 0 (x86_64)
    ssize_t read(int fd, void *buf, size_t count);

    // syscall number 1 (x86_64)
    ssize_t write(int fd, const void *buf, size_t count);
    ```
  ]
]

#slide[
  == Buffers (for sockets)
  
  #set align(center)
  #set text(size: 18pt)
  
  #block(spacing: 2cm)[
    #rect(width: 80%, height: 2cm, stroke: 2pt + blue, fill: blue.lighten(95%))[
      #v(0.5cm)
      #set align(center + horizon)
      *User Space (Your App)*
      #grid(
          columns: (auto, auto),
          rect(fill: white, stroke: 1pt + black, inset: 8pt)[App read buffer (4KB)],
          rect(fill: white, stroke: 1pt + black, inset: 8pt)[App write buffer (4KB)]
  )
    ]

    #v(0.5cm)
    #text(size: 30pt)[$arrow.t.double.b$]
    #text(size: 15pt)[System Call (Context Switch + Data Copy)]
    #v(0.5cm)


    #rect(width: 80%, height: 4cm, stroke: 2pt + green, fill: green.lighten(95%))[
      #set align(center + horizon)
      #v(1.0cm)
      *Kernel Space (OS)* \
      #grid(
        columns: (auto, auto),
        rect(fill: white, stroke: 1pt + black, inset: 8pt)[RX Buffer (Read)],
        rect(fill: white, stroke: 1pt + black, inset: 8pt)[TX Buffer (Write)]
      )
      #v(0.5cm)
      #text(size: 12pt)[Hardware Interface (NIC / SSD)]
    ]
  ]
]

#slide[
  == Blocking I/O

  #v(1.5cm)
  #set text(size: 22pt)
  #list(
    spacing: 1.2em,
    [Simple and easy to implement],
    [Too simple for advanced, high-performance use],
    [Inefficient way of using CPU (idle time)],
    [Scaling requires one thread per operation],
  )
]

#slide[
  == The Non-blocking Era
  
  #v(1.5cm)
  #set text(size: 22pt)
  #list(
    spacing: 1.2em,
    [Introduced `O_NONBLOCK` flag for file descriptors],
    [Syscalls return immediately with `EAGAIN` if data is not ready],
    [Infinite loop checking each file descriptor one by one],
    [Possibility to use one thread/process to handle multiple I/O operations],
  )
]

#slide[
  == Non-blocking Event Loop
  
  #v(0.5cm)
  #set align(center)
  #rect(fill: gray.lighten(95%), stroke: 1pt + gray, inset: 15pt, width: 100%)[
    #set align(left)
    #text(size: 18pt)[
    ```c
    while (1) {
        for (int i = 0; i < num_fds; i++) {
            // Try to read data
            ssize_t n = read(fds[i], buf, sizeof(buf));
            if (n > 0) process_data(buf, n);

            // Try to write data (if we have something to send)
            if (has_data_to_send[i]) {
                ssize_t w = write(fds[i], out_buf[i], out_size[i]);
                if (w > 0) update_buffer(i, w);
            }
        }
        // spinning
    }
    ```
    ]
  ]
]

#slide[
  == Efficiency Issues of Busy-Waiting
  
  #v(1.5cm)
  #set text(size: 22pt)
  #list(
    spacing: 1.2em,
    [Extreme CPU Waste],
    [Poor Scalability],
    [Power Inefficiency],
    [High Latency],
  )
  #set align(center)
  #rect(fill: blue.lighten(95%), stroke: (left: 5pt + blue), inset: 15pt)[
    #set align(left)
    #text(size: 20pt)[
      *The Solution:* Offload monitoring to the *Kernel*. 
      Instead of constant "asking", the application waits for a signal 
      from the OS that an I/O event is ready to be handled.
    ]
  ]
]

#slide[
  == Reminder - buffers
  
  #set align(center)
  #set text(size: 18pt)
  
  #block(spacing: 2cm)[
    #rect(width: 80%, height: 2cm, stroke: 2pt + blue, fill: blue.lighten(95%))[
      #v(0.5cm)
      #set align(center + horizon)
      *User Space (Your App)*
      #grid(
          columns: (auto, auto),
          rect(fill: white, stroke: 1pt + black, inset: 8pt)[App read buffer (4KB)],
          rect(fill: white, stroke: 1pt + black, inset: 8pt)[App write buffer (4KB)]
  )
    ]

    #v(0.5cm)
    #text(size: 30pt)[$arrow.t.double.b$]
    #text(size: 15pt)[System Call (Context Switch + Data Copy)]
    #v(0.5cm)


    #rect(width: 80%, height: 4cm, stroke: 2pt + green, fill: green.lighten(95%))[
      #set align(center + horizon)
      #v(1.0cm)
      *Kernel Space (OS)* \
      #grid(
        columns: (auto, auto),
        rect(fill: white, stroke: 1pt + black, inset: 8pt)[RX Buffer (Read)],
        rect(fill: white, stroke: 1pt + black, inset: 8pt)[TX Buffer (Write)]
      )
      #v(0.5cm)
      #text(size: 12pt)[Hardware Interface (NIC / SSD)]
    ]
  ]
]
#slide[
  == Multiplexing: select, poll and epoll
  
  #v(1.5cm)
  #set text(size: 22pt)
  #list(
    spacing: 1.2em,
    [Kernel monitors multiple file descriptors at once],
    [Process is suspended (0% CPU) while waiting for events],
    [Hardware interrupts notify the Kernel when data arrives],
    [App wakes up only when there is actual I/O to handle],
  )

  #v(1cm)
  #rect(fill: blue.lighten(95%), stroke: (left: 5pt + blue), inset: 15pt)[
    *Result:* No more busy-waiting. The OS manages the wait time.
  ]
]

#slide[
  #set align(center)
  #image("images/multi.webp", height: auto)
  #place(bottom + right, dx: -5pt, dy: 20pt)[
    #set text(size: 8pt, fill: white.lighten(20%))
    #box(fill: black.lighten(20%), inset: 3pt, radius: 2pt)[
      Source: https://www.scylladb.com/2020/05/05/how-io_uring-and-ebpf-will-revolutionize-programming-in-linux/
    ]
  ]
]

#slide[
  == The Evolution: From select to epoll
  
  #v(0.5cm)
  #set text(size: 17pt)
  
  #table(
    columns: (1fr, 1fr, 2fr),
    inset: 10pt,
    align: horizon,
    stroke: gray.lighten(50%),
    fill: (x, y) => if y == 0 { gray.lighten(90%) },
    [*Mechanism*], [*Status*], [*Key Characteristic*],
    [`select()`], [Legacy (BSD)], [Oldest, limited to 1024 FDs.],
    [`poll()`], [*POSIX Standard*], [Portable across Unix systems, no FD limit.],
    [`epoll()`], [*Linux Specific*], [High performance $O(1)$, non-portable.],
  )

  #set text(size: 17pt)
  #list(
    spacing: 0.8em,
    [*Standardization:* `poll` is part of the POSIX standard, meaning it works on Linux, BSD, macOS, and Solaris.],
    [*Efficiency:* `epoll` is a Linux-only optimization (RB-Tree). It’s much faster, but locks your code to the Linux kernel.],
  )
  #rect(fill: blue.lighten(97%), stroke: (left: 5pt + blue), inset: 12pt)[
    *Key Trade-off:* Choose *POSIX (poll)* for compatibility, or *Linux-specific (epoll)* for massive scale.
  ]
]
#slide[
  == Epoll in Action
  #v(0.5cm)
  #set align(center)
  #rect(fill: gray.lighten(95%), stroke: 1pt + gray, inset: 15pt, width: 100%)[
    #set align(left)
    #text(size: 18pt)[
    ```c
    struct epoll_event ev;
    ev.events = EPOLLIN; 
    ev.data.fd = fd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
    
    for (;;) {
        int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
    
        for (int n = 0; n < nfds; ++n) {
            read(events[n].data.fd, buffer, sizeof(buffer));
        }
    }
    ```
    ]
  ]
]

#slide[
  == The Limitations of multiplexing
  
  #v(1.5cm)
  #set text(size: 22pt)
  #list(
    spacing: 1.2em,
    [*Too many syscalls:* Every io requires a two context switch(one for epoll_wait and one for read/write)],
    [*Data Copying:* Data must be moved from Kernel to User Space],
    [*Synchronous API:* The thread stops to wait for the copy to finish],
    [*Works for sockets and pipes, not for disk I/O*],
  )
]

#slide[
  == The next step: zero-copy I/O with `O_DIRECT`
  #v(1.5cm)
  #set text(size: 22pt)
  #list(
    spacing: 1.2em,
    [Provide flag `O_DIRECT` to bypass kernel buffers and copy data directly between user space and hardware],
    [Reduces latency and CPU overhead by eliminating unnecessary copying],
    [Requires careful management of memory and alignment],
    [Moves complexity from the kernel to the application, making it harder to use correctly],
    [Works only for disk I/O, not for sockets]
  )
]

#slide[
  #set align(center)
  #image("images/o_direct.jpg", height: auto)
  #place(bottom + right, dx: -5pt, dy: 20pt)[
    #set text(size: 8pt, fill: white.lighten(20%))
    #box(fill: black.lighten(20%), inset: 3pt, radius: 2pt)[
      Source: https://flylib.com/books/en/2.275.1.50/1/
    ]
  ]
]

#slide[
  == Zero-copy I/O for sockets: more complex than it sounds
  #v(1.5cm)
  #set text(size: 22pt)
  #list(
    spacing: 1.2em,
    [Sockets don't support `O_DIRECT`],
    [There is some support for zero-copy with `sendfile()`, `splice()`, and others],
    [For receiving, zero-copy is much harder],
  )
]

#slide[
  == Synchronous vs. Asynchronous I/O
  
  #v(0.5cm)
  #set text(size: 16pt)
  
  #table(
    columns: (1fr, 1fr),
    inset: 15pt,
    stroke: gray.lighten(50%),
    fill: (x, y) => if y == 0 { gray.lighten(90%) },
    [*Synchronous*], [*Asynchronous*],
    [*Wait for Readiness*], [*Wait for Completion*],
    [Kernel:"You *can* read the data."], [Kernel: "I have *read* the data"],
    [*CPU is busy* during the actual data transfer (copying).], [*CPU is free* to execute logic while the transfer happens in the background.],
  )

  #v(0.8cm)
  #grid(
    columns: (1fr, 1fr),
    rect(fill: red.lighten(95%), inset: 10pt, stroke: red)[
      *Sync:* You are the courier. You wait for the package and then you carry it upstairs.
    ],
    rect(fill: green.lighten(95%), inset: 10pt, stroke: green)[
      *Async:* You are the customer. The package is delivered directly to your desk.
    ]
  )
]

#slide[
  == Linux-aio
  #v(1.5cm)
  #set text(size: 22pt)
  #list(
    spacing: 1.2em,
    [Introduced in Linux 2.5 (2003) for asynchronous disk I/O],
    [By defult, use `O_DIRECT` to avoid kernel buffering],
    [Didn't gain widespread adoption due to complexity and limitations],
  )
]

#slide[
  == Linux-aio in action
  #set align(center)
  #rect(fill: gray.lighten(95%), stroke: 1pt + gray, inset: 15pt, width: 100%)[
    #set align(left)
    #text(size: 16pt)[
    ```c
    int fd = open("test.txt", O_RDONLY);
    struct aiocb cb;
    char buffer[4096];

    memset(&cb, 0, sizeof(struct aiocb));
    cb.aio_fildes = fd; cb.aio_buf = buffer;
    cb.aio_nbytes = sizeof(buffer); cb.aio_offset = 0;

    aio_read(&cb);
    printf("Doing other work...\n");
    while (aio_error(&cb) == EINPROGRESS) {
    }
    int n = aio_return(&cb);
    printf("Read %d bytes\n", n);
    ```
    ]
  ]
]

#slide[
  == Linux-aio limitations
  #v(1.5cm)
  #set text(size: 22pt)
  #list(
    spacing: 1.2em,
    [Complex API],
    [Linux-aio only works for O_DIRECT files],
    [Complicated with sockets and other non-disk I/O],
    [Often requires use multiplexing],
    [There are many reasons that can lead it to blocking, often in ways that are impossible to predict]
  )
]

#slide[
== The Verdict on Linux AIO
#set text(size: 26pt, style: "italic")

#quote(attribution: [Linus Torvalds], block: true)[
  "So I think this is ridiculously ugly.
  AIO is a horrible ad-hoc design, with the main excuse being
  'other, less gifted people, made that design, and we are implementing it
  for compatibility because database people --- who seldom have any shred
  of taste --- actually use it'."
]
]

#slide[
  #set align(horizon)
  == What is the problem with these solutions?
  
  #v(2cm)
  
  == Each solution covers only some aspect and different I/O types
]

#slide[
    == Bottlenecks of existing solutions
    #v(1.5cm)
    #set text(size: 22pt)
    #list(
      spacing: 1.2em,
      [Too many syscalls (context switches)],
      [Data copying between kernel and user space],
      [Synchronous APIs that block threads],
    )
]

#slide[
  #set align(horizon)
  = The hero we deserve: `io_uring`
]

#slide[
  == Design goals of `io_uring`
  #v(1.5cm)
  #set text(size: 22pt)
  #list(
    spacing: 1.2em,
    [Easy to use, hard to misuse],
    [Extendable to support new I/O types and operations],
    [Feauture-rich API that supports a wide range of use cases],
    [Efficient and scalable, minimizing syscalls and data copying],
  )
]

#slide[
  == What is `io_uring`?
  
  #v(1.5cm)
  #set text(size: 22pt)
  #list(
    spacing: 1.2em,
    [Introduced in Linux 5.1 (2019) by Jens Axboe],
    [High-performance asynchronous I/O API],
    [Separates the control plane from the data plane],
    [For control plane, uses two shared memory rings (SQ and CQ) between user space and kernel],
  )
]

#slide[
  == What async means in `io_uring`?
  
  #v(1.5cm)
  #set text(size: 22pt)
  #list(
    spacing: 1.2em,
    [Tell the kernel what you want to do (submit)],
    [Kernel does the i/o work in the background],
    [You can do other things while waiting],
    [You can get notified when the work is done],
    [You will receive the result in the provided buffer],
  )
]

#slide[
  == Control Plane vs Data Plane
  #v(1.5cm)
  #set text(size: 22pt)
  #list(
    spacing: 1.2em,
    [*Control Plane*: Shared memory rings for submitting requests and receiving completions],
    [*Data Plane*: Buffers provided by user space for the kernel to read/write data directly],
  )
]

#slide[
  #set align(center)
  #image("images/rings.webp", height: auto)
  #place(bottom + right, dx: -5pt, dy: 20pt)[
    #set text(size: 8pt, fill: white.lighten(20%))
    #box(fill: black.lighten(20%), inset: 3pt, radius: 2pt)[
      Source: https://developers.redhat.com/articles/2023/04/12/why-you-should-use-iouring-network-io
    ]
  ]
]

#slide[
  == Syscall api
  #v(1.5cm)
  #set text(size: 22pt)
  #list(
    spacing: 1.2em,
    [io_uring_setup(): Create and initialize an io_uring instance],
    [io_uring_enter(): Submit I/O requests and wait for completions],
    [io_uring_register(): Register buffers, files, or other resources with the kernel],
  )
]

#slide[
  == liburing
  #set align(horizon)
  The author of the io_uring API has created a library that simplifies its use.
]

#slide[
  == liburing - initialization
  #set align(horizon)

  #rect(fill: gray.lighten(95%), stroke: 1pt + gray, inset: 15pt, width: 100%)[
    #set align(left)
    #text(size: 18pt)[
    ```c
    // initalize rings
    struct io_uring ring;
    io_uring_queue_init(entries, &ring, flags);
    ```
    ]
  ]
]

#slide[
  == liburing - submission

  #set align(horizon)

  #rect(fill: gray.lighten(95%), stroke: 1pt + gray, inset: 15pt, width: 100%)[
    #set align(left)
    #text(size: 18pt)[
    ```c
    struct io_uring_sqe *sqe;
    struct iovec iov[1];
    char buffer[4096];
    
    iov[0].iov_base = buffer;
    iov[0].iov_len = sizeof(buffer);
    
    sqe = io_uring_get_sqe(&ring);
    
    io_uring_prep_readv(sqe, client_fd, iov, 1, 0);
    
    io_uring_sqe_set_data(sqe, buffer);
    
    io_uring_submit(&ring);
    ```
    ]
  ]
]

#slide[
  == liburing - completion handling

  #set align(horizon)

  #rect(fill: gray.lighten(95%), stroke: 1pt + gray, inset: 15pt, width: 100%)[
    #set align(left)
    #text(size: 18pt)[
    ```c
    struct io_uring_cqe *cqe;
    
    io_uring_wait_cqe(&ring, &cqe);
    
    if (cqe->res < 0) {
        fprintf(stderr, "Error in async operation: %s\n", strerror(-cqe->res));
    } else {
        printf("Read %d bytes from socket!\n", cqe->res);
        char *data = (char *)io_uring_cqe_get_data(cqe);
        printf("%.*s", (int)cqe->res, data);
    }
    
    io_uring_cqe_seen(&ring, cqe);
    ```
    ]
  ]
]

#slide[
  == liburing in action
  
  #set text(size: 12pt) 
  
  #grid(
    columns: (1fr, 1fr),
    rect(fill: gray.lighten(95%), stroke: 0.5pt + gray, inset: 8pt, width: 100%, height: 85%)[
      #set align(left)
      ```c
      add_accept_request(listen_sock, &addr, &len);
      io_uring_submit(&ring);

      while (1) {
          io_uring_wait_cqe(&ring, &cqe);
          struct req *req = (struct req *)cqe->user_data;

          switch (req->type) {
          case ACCEPT:
              add_accept_request(listen_sock, &addr, &len);
              add_read_request(cqe->res);
              io_uring_submit(&ring);
              break;

          case READ:
              if (cqe->res <= 0) {
                  add_close_request(req);
              } else {
                  add_write_request(req);
              }
              io_uring_submit(&ring);
              break;
      ```
    ],
    
    rect(fill: gray.lighten(95%), stroke: 0.5pt + gray, inset: 8pt, width: 100%, height: 85%)[
      #set align(left)
      ```c
          case WRITE:
              add_read_request(req->socket);
              io_uring_submit(&ring);
              break;

          case CLOSE:
              free_request(req);
              break;

          default:
              fprintf(stderr, "Unknown type %d\n", 
                      req->type);
              break;
          }

          io_uring_cqe_seen(&ring, cqe);
      }
      ```
    ]
  )
]

#slide[
  == `liburing` in action: Where are the Syscalls?
  
  #set text(size: 11pt) 
  #let syscall(it) = text(fill: red, weight: "bold", it)
  
  #grid(
    columns: (1fr, 1fr),
    rect(fill: gray.lighten(95%), stroke: 0.5pt + gray, inset: 8pt, width: 100%, height: 88%)[
      #set align(left)
      ```c
      add_accept_request(listen_sock, &addr, &len);
      ``` #syscall("io_uring_submit(&ring);") // SYSCALL

      ```c
      while (1) {
      ``` #syscall("        io_uring_wait_cqe(&ring, &cqe);") // SYSCALL
      ```c
          struct req *req = (struct req *)cqe->user_data;

          switch (req->type) {
          case ACCEPT:
              add_accept_request(listen_sock, &addr, &len);
              add_read_request(cqe->res);
      ``` #syscall("                io_uring_submit(&ring);") // SYSCALL
      ```c
              break;
          case READ:
              if (cqe->res <= 0) {
                  add_close_request(req);
              } else {
                  add_write_request(req);
              }
      ``` #syscall("                io_uring_submit(&ring);") // SYSCALL
      ```c
              break;
      ```
    ],
    
    rect(fill: gray.lighten(95%), stroke: 0.5pt + gray, inset: 8pt, width: 100%, height: 88%)[
      #set align(left)
      ```c
          case WRITE:
              add_read_request(req->socket);
      ``` #syscall("                io_uring_submit(&ring);") // SYSCALL
      ```c
              break;

          case CLOSE:
              free_request(req);
              break;

          default:
              fprintf(stderr, "Unknown type %d\n", 
                      req->type);
              break;
          }

          io_uring_cqe_seen(&ring, cqe); // No syscall (RAM)
      }
      ```
      
      #v(0.5cm)
      #text(fill: red, size: 14pt, weight: "bold")[#sym.arrow.r Red items = Syscall]
    ]
  )
]

#slide[
  == Using `io_uring_peek_cqe()` to avoid syscalls
  
  #set text(size: 11pt) 
  #let syscall(it) = text(fill: red, weight: "bold", it)
  
  #rect(fill: gray.lighten(95%), stroke: 0.5pt + gray, inset: 10pt, width: 100%)[
    #set align(left)
    ```c
    while (1) {
        int submissions = 0;
    ``` #syscall("        io_uring_wait_cqe(&ring, &cqe);") 
    
    ```c
        while (1) {
            struct request *req = (struct request *) cqe->user_data;
            // ... [process request, add new work to SQ] ...
            if (req->type == ACCEPT) { submissions += 2; } 

            io_uring_cqe_seen(&ring, cqe);

            if (io_uring_sq_space_left(&ring) < MAX) break; 
    ```
    #h(1cm) `  ret = ` #text(fill: blue.darken(20%), weight: "bold", "io_uring_peek_cqe(&ring, &cqe);") // **NO SYSCALL (RAM)**
    ```c
            if (ret == -EAGAIN) break; 
        }

        if (submissions > 0) {
    ```
    #h(1cm) #syscall("    io_uring_submit(&ring);") 
    ```c
        }
    }
    ```
  ]
]

#slide[
  == SQPOLL: Kernel-Side Polling
  
  #set text(size: 18pt)
  #v(1cm)

  #grid(
    columns: (1fr, 1fr),
    
    [
      #set align(center)
      === Standard Mode
      #v(0.5cm)
      #rect(fill: blue.lighten(90%), stroke: 1pt + blue, inset: 10pt, width: 100%)[
        #set align(left)
        #set text(size: 14pt)
        1. App fills the *Submission Queue* (SQ) \
        2. *Syscall* `io_uring_enter()` required \
        3. Kernel wakes up to process requests \
        4. CPU overhead due to context switching
      ]
      #v(0.5cm)
      #text(fill: blue.darken(20%), weight: "bold")[Overhead: System Calls]
    ],

    [
      #set align(center)
      === SQPOLL Mode
      #v(0.5cm)
      #rect(fill: green.lighten(90%), stroke: 2pt + green, inset: 10pt, width: 100%)[
        #set align(left)
        #set text(size: 14pt)
        1. App fills the *Submission Queue* (SQ) \
        2. *Zero Syscalls* in the hot path \
        3. Kernel thread (*kworker*) polls the SQ \
        4. Background processing on a separate core
      ]
      #v(0.5cm)
      #text(fill: green.darken(20%), weight: "bold")[Near-Zero Latency!]
    ]
  )

  #set align(center)
  #box(fill: yellow.lighten(80%), inset: 10pt, radius: 5pt)[
    *Performance Key:* Decoupling data submission from notification. \
    Enabled via the `IORING_SETUP_SQPOLL` flag.
  ]
]

#slide[
  #set align(horizon)

  = How we can reduce syscalls even further? 🤔
  = By letting the kernel do more work!
]

#slide[
  #set align(horizon)

  = io_uring could be combined with ebpf 🤯

]

#slide[
  == The Power Duo: io_uring + eBPF
  
  #set text(size: 17pt)
  
  === io_uring
  #text[The *Data Path* — handles the movement of bytes from the NIC directly to memory with *zero syscalls*.]
  === eBPF
  #text[The *Control Logic* — parses and filters data *inside the kernel* before the application is notified.]
   
  #rect(fill: gray.lighten(95%), stroke: (left: 4pt + blue), inset: 15pt)[
    #set text(size: 17pt)
    *Key Synergy:* \
    1. eBPF calculates the *actual* payload size from headers. \
    2. io_uring finishes the read *exactly* where the message ends. \
    3. User-space wakes up to a *fully parsed, logical message*.
  ]
  #set align(center)
  #text(size: 14pt, fill: blue.darken(30%))[
    *Result:* Massive reduction in User-Kernel context switches.
  ]
]

#slide[
  #set align(horizon)
  = Is `io_uring` the zero-copy solution?
]

#slide[
  == Zero-Copy with `io_uring`
  #v(1.5cm)
  #set text(size: 22pt)
  
  #list(
    spacing: 1.2em,
    [Control plane (SQ/CQ) is zero-copy between user space and kernel via shared memory],
    [Data plane is not zero-copy by default],
    [`O_DIRECT` file I/O is supported],
    [We can use `io_uring_register_buffers()` to improve `O_DIRECT` performance],
    [For sockets, the situation is still more complex],
  )
]

#slide[
  == The Catch: Security & Risks
  
  #v(1cm)
  #set text(size: 22pt)
  
  #list(
    spacing: 1.5em,
    [*Massive Kernel Attack Surface* – complex code is harder to secure.],
    [*Complexity Shift* – while it simplifies user-space code, it adds complexity to the kernel],
    [*Hard to Debug* – standard tools like `strace` don't see what's happening in the ring.],
    [*Compatibility Issues* – older kernels don't support it, and behavior can vary across versions.],
  )
]

#slide[
  == Performance Comparison: io_uring vs. Others
  
  #v(1cm)
  #set text(size: 16pt)
  
  #align(center)[
    #table(
      columns: (2fr, 1.5fr, 1.5fr, 1fr),
      inset: 12pt,
      align: horizon,
      stroke: 0.5pt + gray,
      fill: (x, y) => if y == 0 { gray.lighten(80%) } else if y == 4 { green.lighten(90%) },
      
      [*Backend*], [*IOPS*], [*Context Switches*], [*vs. io_uring*],
      
      [sync], [4,906,000], [105,797], [-2.3%],
      [posix-aio (thread pool)], [1,070,000], [114,791,187], [-78.7%],
      [linux-aio], [4,127,000], [105,052], [-17.9%],
      [*io_uring*], [*5,024,000*], [*106,683*], [---],
    )
  ]

  #v(2cm)
  
  #set align(right)
  #set text(size: 10pt, fill: gray)
  Source: #link("https://www.scylladb.com/2020/05/05/how-io_uring-and-ebpf-will-revolutionize-programming-in-linux/")[_ScyllaDB (2020), "How io_uring and eBPF will Revolutionize Programming in Linux"_]
]

#slide[
  == The Ultimate Performance: Kernel Bypass (DPDK)
  
  #v(1cm)
  #set text(size: 20pt)
  
  #list(
    spacing: 1.2em,
    [Bypass the kernel for network I/O, directly accessing the NIC from user space],
    [Zero-copy, zero-syscalls, and zero-context-switches for packets],
    [Requires special drivers (PMD) and hardware support],
    [Shifts all complexity to user space, making it harder to develop and maintain],
  )

  #set align(center)
  #set text(size: 14pt)

  #v(1cm)
  
  #link("https://blogs.oracle.com/linux/introduction-to-virtio-part-2-vhost")[
    #text(fill: blue.darken(20%))[
      Interesting read: DPDK over virtio
    ]
  ]
]

#slide[
  == Sources
  #v(1cm)
  #show link: set text(size: 22pt, fill: blue)
  - #link("https://developers.redhat.com/articles/2023/04/12/why-you-should-use-iouring-network-io")[_Red Hat (2023), "Why You Should Use io_uring for Network I/O"_],
  - #link("https://www.scylladb.com/2020/05/05/how-io_uring-and-ebpf-will-revolutionize-programming-in-linux/")[_ScyllaDB (2020), "How io_uring and eBPF will Revolutionize Programming in Linux"_],
  - #link("https://blogs.oracle.com/linux/an-introduction-to-the-io-uring-asynchronous-io-framework")[_Oracle (2020), "An Introduction to the io_uring Asynchronous I/O Framework"_],
  - #link("https://kernel.dk/io_uring.pdf")[_Jens Axboe - "Efficient IO with io_uring"_],
  - #link("https://www.man7.org/linux/man-pages/man7/io_uring.7.html")[_man io_uring_],
  - #link("https://lwn.net/Articles/752188/")[_LWN(2018) - "Zero-copy TCP receive"_]
  - #link("https://flylib.com/books/en/2.275.1.50/1/")[_Host Kernel Version_]
]

