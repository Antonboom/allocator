'''
clang -O0 -g -W -Wall -Wextra -shared -fPIC allocator.c -o allocator.so
'''

'''
export LD_PRELOAD=/path/to/allocator.so
'''

