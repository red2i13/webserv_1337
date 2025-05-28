#include <miniserv.hpp>


void write_error_message(int errnum) {
    const char *error_messages[] = {
        "Success",                               // 0
        "Operation not permitted",               // EPERM (1)
        "No such file or directory",            // ENOENT (2)
        "No such process",                       // ESRCH (3)
        "Interrupted system call",               // EINTR (4)
        "I/O error",                             // EIO (5)
        "No such device or address",             // ENXIO (6)
        "Argument list too long",                // E2BIG (7)
        "Exec format error",                      // ENOEXEC (8)
        "Bad file number",                       // EBADF (9)
        "No child processes",                   // ECHILD (10)
        "Try again",                             // EAGAIN (11)
        "Out of memory",                         // ENOMEM (12)
        "Permission denied"                      // EACCES (13)
    };
    
    if (errnum >= 0 && errnum <= 13) {
        printf("Error %d: %s\n", errnum, error_messages[errnum]);
    } else {
        printf("Error %d: Unknown error\n", errnum);
    }
}
