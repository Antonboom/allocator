## Задача
При помощи функции syscall реализовать аллокатор памяти. Должны быть имплементированы функции malloc, calloc, realloc, free.
- В качестве основных вызовов можно использовать, например, brk или анонимный mmap
- Учтите, что надо следить за тем, какая из выделенной ядром памяти свободна, а какая - занята и не запрашивать новую, если можно обойтись старой. При использовании mmap не стоит выделять страницу памяти если нужно аллоцировать несколько байт - делите полученные страницы между запросами.
- syscall объявлен в unistd.h
- Полученный результат надо собрать как shared библиотеку (gcc -fPIC -shared) и заменить libc имплементацию аллокатора при помощи переменной окружения LD_PRELOAD (export LD_PRELOAD=filename)

```
clang -O0 -g -W -Wall -Wextra -shared -fPIC allocator.c -o allocator.so
export LD_PRELOAD=/path/to/allocator.so
```
