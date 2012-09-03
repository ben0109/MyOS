#include "kernel/fd.h"

typedef struct __fd_list_item_struct {
	int fd;
	file_t* file;
	struct __file_list* next;
} fd_list_item_t;

struct __fd_list_struct {
	mutex_t lock;
	fd_list_item_t* head;
};

void fd_init_list(fd_list_t* list)
{
	mutex_init(&list->lock);
	list->head = 0;
}

extern process_t* current;

file_t* get_file(int fd)
{
	fd_list_item_t *ptr = current->file_descrs->head;
	while (ptr) {
		if (ptr->fd==fd) {
			return ptr->file;
		}
		ptr = ptr->next;
	}
	return 0;
}

// adds the file to the fd list, returning the allocated fd
static int fd_allocate(file_t* file)
{
	mutex_lock(&current->file_descr->lock);

	int fd = 0;
	fd_list_item_t* here = 0;
	fd_list_item_t* next = current->file_descrs->head;

	while (next && next->fd==fd) {
		fd++;
		here = next;
		next = next->next;
	}

	fd_list_item_t* new = (fd_list_item_t*)kmalloc(sizeof(fd_list_item_t));
	new->fd = fd;
	new->file = file;
	new->next = next;

	if (here==0) {
		current->file_descrs = new;
	} else {
		here->next = new;
	}

	mutex_unlock(&current->file_descr->lock);
	return fd;
}

void fd_dup(int fd)
{
	file_t* file = get_file(fd);
	return fd_allocate(file);
}

