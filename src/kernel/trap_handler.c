#include "trap-codes.h"


uint32_t trap_handler(uint32_t code, void* params)
{
	kdebug("trap_handler: %d, %d", code,params);
	
	switch (code) {
/*
	case TRAP_FNCTL_CREAT:
	case TRAP_FNCTL_FNCTL:
	case TRAP_FNCTL_OPEN:
	case TRAP_FNCTL_OPENAT:
	case TRAP_FNCTL_FADVISE:
	case TRAP_FNCTL_FALLOCATE:

	case TRAP_POLL_POLL:

	case TRAP_UNISTD_EXIT: {
		int fd = *(int*)(params+0);
		return process_exit(fd);
	}
	case TRAP_UNISTD_FORK: {
		int fd = *(int*)(params+0);
		return process_fork(fd);
	}
*/

	case TRAP_UNISTD_CLOSE: {
		int fd = *(int*)(params+0);
		return fd_close(fd);
	}
	case TRAP_UNISTD_DUP: {
		int fd = *(int*)(params+0);
		return fd_dup(fd);
	}
	case TRAP_UNISTD_READ: {
		int fd = *(int*)(params+8);
		void* buffer = *(int*)(params+4);
		uint32 nbytes = *(int*)(params+0);
		return fd_read(fd, buffer, size);
	}
	case TRAP_UNISTD_WRITE: {
		int fd = *(int*)(params+8);
		void* buffer = *(int*)(params+4);
		uint32 nbytes = *(int*)(params+0);
		return fd_write(fd, buffer, nbytes);
	}

	default:
		kpanic("unknown function code %d",code);
		break;
	}


	return_value = 0x1234;
	return return_value;
}
