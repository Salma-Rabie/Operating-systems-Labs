[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/oVlx2L-m)

This project implements two multithreaded programs in C using the POSIX threads (pthread) library:

    1-Merge Sort (merge_sort) – A multithreaded merge sort implementation that uses two threads at each recursive step.

    2-Matrix Multiplication (matrix_mul) – A multithreaded matrix multiplication program that compares two methods:

        -Per-element threading: One thread computes a single element of the result matrix.

        -Per-row threading: One thread computes an entire row of the result matrix.

Both programs use text files for input and output results to the console. The provided Makefile compiles both executables.

## Compilation Instructions

-To build both programs, open a terminal in the project directory and run:

    make

This command will produce two executables:

    -merge_sort

    -matrix_mul

-To remove compiled binaries, run:

    make clean

## 1. Merge Sort Program

-Executable: merge_sort

-Input File Format (input)

n
a1 a2 a3 ... an

-The first line contains an integer n – the number of elements.

-The second line contains n integers to be sorted.

-Example Input

5
9 1 5 3 7

-Run Command

./merge_sort

-Expected Output

Sorted array:
1 3 5 7 9

## 2. Matrix Multiplication Program

-Executable: matrix_mul

-Input File Format (input2)

r1 c1
[a11 a12 ... a1c1]
...
[ar1c1]
r2 c2
[b11 b12 ... b1c2]
...
[br2c2]

-r1, c1: Rows and columns of the first matrix

-r2, c2: Rows and columns of the second matrix

-The number of columns in the first matrix (c1) must equal the number of rows in the second (r2).

-Example Input

3 3
1 2 3
4 5 6
7 8 9
3 2
1 0
0 1
1 0

-Run Command

./matrix_mul

-Example Output

385 385 385 385 385 385 385 385 385 385
220 220 220 220 220 220 220 220 220 220
715 715 715 715 715 715 715 715 715 715
770 770 770 770 770 770 770 770 770 770
275 275 275 275 275 275 275 275 275 275
165 165 165 165 165 165 165 165 165 165
30 30 30 30 30 30 30 30 30 30
550 550 550 550 550 550 550 550 550 550
1155 1155 1155 1155 1155 1155 1155 1155 1155 1155
1540 1540 1540 1540 1540 1540 1540 1540 1540 1540
END1 32.13 ms
385 385 385 385 385 385 385 385 385 385
220 220 220 220 220 220 220 220 220 220
715 715 715 715 715 715 715 715 715 715
770 770 770 770 770 770 770 770 770 770
275 275 275 275 275 275 275 275 275 275
165 165 165 165 165 165 165 165 165 165
30 30 30 30 30 30 30 30 30 30
550 550 550 550 550 550 550 550 550 550
1155 1155 1155 1155 1155 1155 1155 1155 1155 1155
1540 1540 1540 1540 1540 1540 1540 1540 1540 1540
END2 2.85 ms

The END1 and END2 lines show the execution time for the per-element and per-row methods, respectively.

## Notes and Limitations

The input file must be named input and located in the same directory as the executables, unless modified in the source code.

For large matrices or arrays, performance will depend on system threading capability.

The merge sort implementation creates two threads at each recursive level. This can result in a large number of threads for big inputs.

All dynamically allocated memory is freed after use to prevent memory leaks.

## Cleaning Up

To remove executables and rebuild the project:

make clean
make