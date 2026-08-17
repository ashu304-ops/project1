#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>

int _close(int file) { (void)file; return -1; }
int _fstat(int file, struct stat *st) { (void)file; st->st_mode = S_IFCHR; return 0; }
int _isatty(int file) { (void)file; return 1; }
int _lseek(int file, int ptr, int dir) { (void)file; (void)ptr; (void)dir; return 0; }
int _read(int file, char *ptr, int len) { (void)file; (void)ptr; (void)len; return 0; }
int _getpid(void) { return 1; }
int _kill(int pid, int sig) { (void)pid; (void)sig; return -1; }

// ARM Semihosting system call writer (Operation 0x05 = SYS_WRITE)
static int semihosting_write(int file, char *ptr, int len) {
    volatile int block[3];
    block[0] = file;           // 1 for stdout
    block[1] = (int)ptr;       // pointer to string buffer
    block[2] = len;            // length of string

    register int reg0 __asm__("r0") = 0x05; // SYS_WRITE
    register int reg1 __asm__("r1") = (int)block;

    __asm__ volatile (
        "bkpt #0xAB"
        : "=r" (reg0)
        : "r" (reg0), "r" (reg1)
        : "memory"
    );

    // Returns number of bytes NOT written
    return len - reg0;
}

int _write(int file, char *ptr, int len) {
    if (file == 1 || file == 2) { // stdout or stderr
        return semihosting_write(1, ptr, len);
    }
    return -1;
}