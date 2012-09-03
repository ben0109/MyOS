// fnctl.h
#define TRAP_FNCTL_CREAT     0x2000 // int creat(const char *, mode_t);
#define TRAP_FNCTL_FNCTL     0x2001 // int fcntl(int, int, ...);
#define TRAP_FNCTL_OPEN      0x2002 // int open(const char *, int, ...);
#define TRAP_FNCTL_OPENAT    0x2003 // int openat(int, const char *, int, ...);
#define TRAP_FNCTL_FADVISE   0x2004 // int posix_fadvise(int, off_t, off_t, int);
#define TRAP_FNCTL_FALLOCATE 0x2005 // int posix_fallocate(int, off_t, off_t); 

// poll.h
#define TRAP_POLL_POLL       0x2010 // int poll(struct pollfd [], nfds_t, int); 

// unistd.h
#define TRAP_UNISTD_CLOSE    0x2020 // int close(int);
#define TRAP_UNISTD_DUP      0x2021 // int dup(int);
#define TRAP_UNISTD_EXIT     0x2022 // void _exit(int);
#define TRAP_UNISTD_FORK     0x2023 // pid_t fork(void);
#define TRAP_UNISTD_READ     0x2024 // ssize_t read(int, void *, size_t);
#define TRAP_UNISTD_WRITE    0x2025 // ssize_t write(int fildes, const void *buf, size_t nbyte);

// pthreads.h

// sched.h
#define TRAP_SCHED_YIELD     0x3010 // void yield();

// sys/ipc.h
#define TRAP_IPC_FTOK        0x4000 // key_t ftok(const char *, int); 

// sys/msg.h
#define TRAP_MSG_MSGCTL      0x4010 // int msgctl(int, int, struct msqid_ds *);
#define TRAP_MSG_MSGGET      0x4011 // int msgget(key_t, int);
#define TRAP_MSG_MSGRCV      0x4012 // ssize_t msgrcv(int, void *, size_t, long, int);
#define TRAP_MSG_MSGSND      0x4013 // int msgsnd(int, const void *, size_t, int); 

// sys/sem.h
#define TRAP_SEM_SEMCTL      0x4020 // int semctl(int, int, int, ...);
#define TRAP_SEM_SEMGET      0x4021 // int semget(key_t, int, int);
#define TRAP_SEM_SEMOP       0x4022 // int semop(int, struct sembuf *, size_t); 

// sys/select.h
#define TRAP_SELECT_PSELECT  0x3020 // int pselect(int, fd_set *restrict, fd_set *restrict, fd_set *restrict, const struct timespec *restrict, const sigset_t *restrict); 

// sys/wait.h
#define TRAP_WAIT_WAITID     0x3030 // int waitid(idtype_t, id_t, siginfo_t *, int);
#define TRAP_WAIT_WAITPID    0x3031 // pid_t waitpid(pid_t, int *, int); 


// sys/mman.h
// sys/shm.h
// sys/socket.h
// sys/stat.h

