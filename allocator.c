#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>


void *memory_list_head = NULL;
void *memory_list_tail = NULL;


struct CMemoryBlockInfo {
	size_t size;
	int is_free;
	struct CMemoryBlockInfo *next;
};


#define Block struct CMemoryBlockInfo
#define MEMORY_BLOCK_INFO_SIZE sizeof(Block)
#define SBRK_FAILED ((void *)-1)


Block *get_block_pointer(void *ptr) {
	return (Block *)ptr - 1;
}


Block *find_free_block(size_t size) {
	if (size <= 0) {
		return NULL;
	}

	Block *current = memory_list_head;
	while (current && !(current->is_free && current->size >= size)) {
		current = current->next;
	}

	return current;
}


Block *memory_request(size_t size) {
	if (size <= 0) {
		return NULL;
	}

	Block *new_block = NULL;
	new_block = sbrk(0);

	void *request = sbrk(size + MEMORY_BLOCK_INFO_SIZE);
	if (request == SBRK_FAILED) {
		return NULL;
	}

	if (memory_list_tail) {
		((Block *)memory_list_tail)->next = new_block;
		memory_list_tail = new_block;
	}

	new_block->size = size;
	new_block->next = NULL;
	new_block->is_free = 0;

	return new_block;
}


void* malloc(size_t sizemem) {
	if (sizemem <= 0) {
		return NULL;
	}

	Block *block = NULL;
	if (!memory_list_head) {
		block = memory_request(sizemem);
		if (!block) {
			return NULL;
		}

		memory_list_head = block;
		memory_list_tail = block;
	} else {
		Block *last = memory_list_head;
		block = find_free_block(sizemem);
		if (!block) {
			block = memory_request(sizemem);
			if (!block) {
				return NULL;
			}
		} else {
			block->is_free = 0;
		}
	}

	return (block + 1);
}


void free(void *ptr) {
	if (!ptr) {
		return;	
	}

	Block *block_ptr = get_block_pointer(ptr);
	block_ptr->is_free = 1;
}


void *realloc(void *ptr, size_t size) {
	if (size <= 0) {
		return NULL;
	}

	if (!ptr) {
		return malloc(size);
	}

	Block *block_ptr = get_block_pointer(ptr);
	if (block_ptr->size >= size) {
		return ptr;
	}

	void *new_block_ptr = NULL;
	new_block_ptr = malloc(size);
	if (!new_block_ptr) {
		return NULL;
	}

	memcpy(new_block_ptr, ptr, block_ptr->size);
	free(ptr);
	
	return new_block_ptr;
}


void *calloc(size_t num, size_t size) {
	size_t sizemem = num * size;
	void *new_block_ptr = malloc(sizemem);
	memset(new_block_ptr, 0, sizemem);
	return new_block_ptr;
}


int main(void) {
	return 0;
}