#include <string.h>
#include <sys/types.h>
#include <unistd.h>

/*	
	Задача: при помощи функции syscall реализовать аллокатор памяти. 
	Должны быть имплементированы функции malloc, calloc, realloc, free.	
*/


// Обрабатываем блоки памяти с помощью списка
void *memory_list_head = NULL;
void *memory_list_tail = NULL;


// Информация о блоке памяти
struct CMemoryBlockInfo {
	size_t size;
	int is_free;
	struct CMemoryBlockInfo *next;
};


#define Block struct CMemoryBlockInfo
#define MEMORY_BLOCK_INFO_SIZE sizeof(Block)

// #define __NR_brk 45
#define BRK_FAILED -1


// int asm_brk(void *addr) {
// 	int result = -1;

// 	asm volatile (
// 		"syscall"
// 		: "=a" (result)
//         : "0"(__NR_brk), "D"(addr)
//         : "rcx", "r11", "cc"
//     );

//     return result;
// }


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

	int request = brk(new_block + MEMORY_BLOCK_INFO_SIZE + size);
	if (request == BRK_FAILED) {
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


void *malloc(size_t sizemem) {
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
