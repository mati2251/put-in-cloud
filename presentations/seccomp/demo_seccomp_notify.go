package main

import (
	"fmt"
	"os"
	"os/exec"
	"unsafe"

	"golang.org/x/sys/unix"

	libseccomp "github.com/seccomp/libseccomp-golang"
)

const seccompIoctlNotifAddfd uintptr = 0xC0182103

type seccompNotifAddfd struct {
	id         uint64
	flags      uint32
	srcfd      uint32
	newfd      uint32
	newfdFlags uint32
}

func injectFd(notifyFd libseccomp.ScmpFd, req *libseccomp.ScmpNotifReq, srcFd int) (int, error) {
	addfd := seccompNotifAddfd{
		id:    req.ID,
		flags: 0,
		srcfd: uint32(srcFd),
	}
	newFd, _, errno := unix.Syscall(
		unix.SYS_IOCTL,
		uintptr(notifyFd),
		seccompIoctlNotifAddfd,
		uintptr(unsafe.Pointer(&addfd)),
	)
	if errno != 0 {
		return 0, errno
	}
	return int(newFd), nil
}

func fatal(format string, args ...any) {
	fmt.Fprintf(os.Stderr, "error: "+format+"\n", args...)
	os.Exit(1)
}

func main() {
	if len(os.Args) < 2 {
		fmt.Fprintf(os.Stderr, "usage: %s <command> [args...]\n", os.Args[0])
		fmt.Fprintf(os.Stderr, "\nIntercepts all open() syscalls and redirects them to catch.txt\n")
		os.Exit(1)
	}

	file, err := os.OpenFile("catch.txt", os.O_RDWR|os.O_CREATE, 0644)
	if err != nil {
		fatal("failed to open catch.txt: %v", err)
	}
	defer file.Close()
	fmt.Printf("[supervisor] opened catch.txt as fd %d\n", file.Fd())

	filter, err := libseccomp.NewFilter(libseccomp.ActAllow)
	if err != nil {
		fatal("failed to create seccomp filter: %v", err)
	}
	openNr, err := libseccomp.GetSyscallFromName("openat")
	if err != nil {
		fatal("failed to resolve syscall 'open': %v", err)
	}
	if err := filter.AddRule(openNr, libseccomp.ActNotify); err != nil {
		fatal("failed to add seccomp rule: %v", err)
	}
	if err := filter.Load(); err != nil {
		fatal("failed to load seccomp filter: %v", err)
	}
	notifyFd, err := filter.GetNotifFd()
	if err != nil {
		fatal("failed to get notify fd: %v", err)
	}
	filter.Release()
	fmt.Printf("[supervisor] seccomp filter loaded, notify fd %d\n", notifyFd)

	cmd := exec.Command(os.Args[1], os.Args[2:]...)
	cmd.Stdin = os.Stdin
	cmd.Stdout = os.Stdout
	cmd.Stderr = os.Stderr
	if err := cmd.Start(); err != nil {
		fatal("failed to start %q: %v", os.Args[1], err)
	}
	fmt.Printf("[supervisor] started child pid %d: %v\n", cmd.Process.Pid, os.Args[1:])

	go func() {
		for {
			req, err := libseccomp.NotifReceive(notifyFd)
			if err != nil {
				fatal("NotifReceive: %v", err)
			}
			fmt.Printf("[supervisor] intercepted open() from pid %d\n", req.Pid)

			newFd, err := injectFd(notifyFd, req, int(file.Fd()))
			if err != nil {
				fatal("injectFd: %v", err)
			}
			fmt.Printf("[supervisor] injected catch.txt as fd %d into child\n", newFd)

			if err := libseccomp.NotifRespond(notifyFd, &libseccomp.ScmpNotifResp{
				ID:    req.ID,
				Val:   uint64(newFd),
				Error: 0,
				Flags: 0,
			}); err != nil {
				fatal("NotifRespond: %v", err)
			}
		}
	}()

	cmd.Wait()
}
