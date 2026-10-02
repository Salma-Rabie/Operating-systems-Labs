#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
double elapsed1_global = 0.0;
double elapsed2_global = 0.0;

// Structure to hold matrix data
typedef struct {
    int rows;
    int cols;
    int **data; //pointer-to-pointer representing a 2D array implemented as an array of row pointers
} Matrix;

// Structure for passing data to threads
typedef struct {
    Matrix *mat1;
    Matrix *mat2;
    Matrix *result;
    int row;
    int col; // for per-row threads row is used and col is ignored
} ThreadData;

// Function to allocate matrix
Matrix* createMatrix(int rows, int cols) {
   Matrix *mat = malloc(sizeof(Matrix)); 
   mat->rows = rows; mat->cols = cols; 
   mat->data = malloc(rows * sizeof(int*)); 
   for (int i = 0; i < rows; i++) { 
        mat->data[i] = calloc(cols, sizeof(int)); // initialize with zeros 
    } 
    return mat;
}

// Function to free matrix
void freeMatrix(Matrix *mat) {
    if (!mat) return; //don't free anything if mat already null
    for (int i = 0; i < mat->rows; i++)
        free(mat->data[i]); //free each row array
    free(mat->data);
    free(mat);//free the Matrix struct
}

// Function to read matrices from file
int readMatrices(Matrix **mat1, Matrix **mat2) {
    FILE *f = fopen("input2", "r");
    if (!f) {
        printf("Error opening file.\n");
        return -1;
    }

    int r1, c1, r2, c2;

    // Read first matrix dimensions
    if (fscanf(f, "%d %d", &r1, &c1) != 2){
        fprintf(stderr, "Invalid format for first matrix dimensions\n");
        fclose(f);
        return -1;
    }

    *mat1 = createMatrix(r1, c1);

    int count1 = 0; // counter to verify number of read elements
    for (int i = 0; i < r1; i++) {
        for (int j = 0; j < c1; j++) {
            if (fscanf(f, "%d", &((*mat1)->data[i][j])) != 1) {
                freeMatrix(*mat1);
                fclose(f);
                return -1;
            }
            else{
                 count1++;
            }
           
        }
    }

    if (count1 != r1 * c1) {
        freeMatrix(*mat1);
        fclose(f);
        return -1;
    }
    
    // Read second matrix dimensions      
    if (fscanf(f, "%d %d", &r2, &c2) != 2) {
        fprintf(stderr, "Invalid format for second matrix dimensions\n");
        fclose(f);       
        return -1;
    }

    if (c1 != r2) {
        printf("Error: matrices cannot be multiplied.\n");
        freeMatrix(*mat1);
        fclose(f);
        return -1;
    }

    *mat2 = createMatrix(r2, c2);
    
    int count2 = 0; // counter to verify number of read elements
    for (int i = 0; i < r2; i++) {
        for (int j = 0; j < c2; j++) {
            if (fscanf(f, "%d", &((*mat2)->data[i][j])) != 1) {
                freeMatrix(*mat1);
                freeMatrix(*mat2);
                fclose(f);
                return -1;
            }
            count2++;
        }
    }

    if (count2 != r2 * c2) {
        freeMatrix(*mat1);
        freeMatrix(*mat2);
        fclose(f);
        return -1;
    }

    fclose(f);
    return 0;
}

// Function to print matrix
void printMatrix(Matrix *mat) {
    for (int i = 0; i < mat->rows; i++) {
        for (int j = 0; j < mat->cols; j++) {
            printf("%d", mat->data[i][j]);
            if (j < mat->cols - 1) printf(" "); //print space between numbers except after last column in a row
        }
        printf("\n"); //Print newline after each row
    }
}

// Thread function for per-element multiplication
void* computeElement(void *arg) { // computes only one element of the result
    ThreadData *data = (ThreadData*) arg;
    int sum = 0;
    for (int k = 0; k < data->mat1->cols; k++) {
        sum += data->mat1->data[data->row][k] * data->mat2->data[k][data->col];
    }
    data->result->data[data->row][data->col] = sum; //Each thread writes to a unique result[row][col], so no race on writing
    free(data);
    return NULL;
}

// Thread function for per-row multiplication
void* computeRow(void *arg) { //computes an entire row of the result at a time
    ThreadData *data = (ThreadData*) arg;
    int row = data->row;
    for (int j = 0; j < data->mat2->cols; j++) {
        int sum = 0;
        for (int k = 0; k < data->mat1->cols; k++) {
            sum += data->mat1->data[row][k] * data->mat2->data[k][j];
        }
        data->result->data[row][j] = sum;
    }
    free(data);
    return NULL;
}

// Method 1: One thread per element
void multiplyMatricesPerElement(Matrix *mat1, Matrix *mat2, Matrix **result) { //creates one thread for each element of the result matrix
    *result = createMatrix(mat1->rows, mat2->cols);
    pthread_t threads[mat1->rows * mat2->cols];
    int t = 0;

    struct timeval start, end;
    gettimeofday(&start, NULL); //capture start time just before creating the threads to include creation overhead

    for (int i = 0; i < mat1->rows; i++) {
        for (int j = 0; j < mat2->cols; j++) {
            ThreadData *data = (ThreadData*) malloc(sizeof(ThreadData));
            data->mat1 = mat1;
            data->mat2 = mat2;
            data->result = *result;
            data->row = i;
            data->col = j;
            if (pthread_create(&threads[t++], NULL, computeElement, data) != 0) {
                perror("pthread_create");
                free(data);
                t--; // adjust since this thread wasn't created
            }
        }
    }

    // Join only the successfully created threads
    for (int i = 0; i < t; i++)
        pthread_join(threads[i], NULL);

    gettimeofday(&end, NULL);
    elapsed1_global  = (end.tv_sec - start.tv_sec) * 1000.0 +
                     (end.tv_usec - start.tv_usec) / 1000.0;

}

// Method 2: One thread per row
void multiplyMatricesPerRow(Matrix *mat1, Matrix *mat2, Matrix **result) { ////creates one thread for each row of the result matrix
    *result = createMatrix(mat1->rows, mat2->cols);
    pthread_t threads[mat1->rows];
    int t = 0;
    struct timeval start, end;
    gettimeofday(&start, NULL);

    for (int i = 0; i < mat1->rows; i++) {
        ThreadData *data = (ThreadData*) malloc(sizeof(ThreadData));
        data->mat1 = mat1;
        data->mat2 = mat2;
        data->result = *result;
        data->row = i;

         if (pthread_create(&threads[t++], NULL, computeRow, data) != 0) {
            perror("pthread_create");
            free(data);
            t--; // adjust since this thread wasn't created
            i--;
         }
    }

    // Join only the successfully created threads
    for (int i = 0; i < t; i++)
        pthread_join(threads[i], NULL);

    gettimeofday(&end, NULL);
    elapsed2_global  = (end.tv_sec - start.tv_sec) * 1000.0 +
                     (end.tv_usec - start.tv_usec) / 1000.0;

}

int main() {

    Matrix *mat1, *mat2, *result1, *result2;

    // Read matrices from file
    if (readMatrices(&mat1, &mat2) != 0) {
        fprintf(stderr, "Error reading matrices\n");
        return 1;
    }

    // Method 1: Per element
    multiplyMatricesPerElement(mat1, mat2, &result1);
    printMatrix(result1);
    printf("END1 %.2f ms\n", elapsed1_global);


    // Method 2: Per row
    multiplyMatricesPerRow(mat1, mat2, &result2);
    printMatrix(result2);
    printf("END2 %.2f ms\n", elapsed2_global);


    // Cleanup
    freeMatrix(mat1);
    freeMatrix(mat2);
    freeMatrix(result1);
    freeMatrix(result2);

    return 0;
}
