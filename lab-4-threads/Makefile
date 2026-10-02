all: merge_sort		matrix_mul

merge_sort: Merge_sort.c
	gcc -pthread -O2 -Wall -o merge_sort Merge_sort.c


matrix_mul: Matrix_Multiplication.c
	gcc -pthread -O2 -Wall -o matrix_mul Matrix_Multiplication.c


clean:
	rm -f merge_sort matrix_mul