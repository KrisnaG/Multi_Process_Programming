# Parallel Search

### Overview
    Simple program that demonstrates the use of the openSSL library functions to brute-force search for an AES encryption key given a partial key.<br>
    The brute-force search is conducted with parallel processes arranged in a "ring-of-processes" topology.

### How it works
    The parent process spawns n given number of processes connected in a ring-of-processes topology. <br>
    Each child process is allocated a subsection of the search returns its results through the ring to the parent. <br>
    The child process returns immediately after a match was found, otherwise, it will wait for neighbouring processes to pass the result along. <br>

### Parameters:
     1. Number of processes
     2. Partial key to use for the search
     3. Cipher text file
     4. Plain text file

### Returns: 
    0 on Success

### Library requirements:
     parallel_search_utils.h
     parallel_search_utils.c

### Build (two methods):
     1. gcc -Wall -pedantic parallel_search_keyspace.c 
            parallel_search_util.c -lcrypto -o parallel_search_keyspace
     2. make (if makefile is included)

### Run:
     parallel_search_keyspace <num. procs.> <partial key> <cipher file> <plain file>
     e.g. parallel_search_keyspace 3 B1AF2507B69F11CCB3AE2C3592039 assignment_cipher.txt plain.txt