#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX 10000 

// Struct for thread arguments
typedef struct {
    int left; //starting index of subarray
    int right; //ending index of subarray
    int *arr; //pointer to the array itself
} ThreadArgs;

// Merge function (same as your original)
void merge(int arr[], int left, int mid, int right) {
    int i, j, k;
    int n1 = mid - left + 1;
    int n2 = right - mid;

    int *L = malloc(n1 * sizeof(int)); //Dynamically allocate temporary arrays L and R to store both halves
    int *R = malloc(n2 * sizeof(int));

    //Copies data from the main array into the temporary subarrays.
    for (i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];

    i = 0;
    j = 0;
    k = left;

    //Merges elements back into arr in sorted order
    while (i < n1 && j < n2) {
        if (L[i] <= R[j])
            arr[k++] = L[i++];
        else
            arr[k++] = R[j++];
    }

    //If one of the subarrays has leftover elements, copy them all into arr
    while (i < n1)
        arr[k++] = L[i++];

    while (j < n2)
        arr[k++] = R[j++];

    free(L);
    free(R);
}

void* threaded_merge_sort(void *arg) { //must be void because function pthread_create expects fuction that is void* (*)(void*) only
    ThreadArgs *args = (ThreadArgs*) arg; // so we do casting here
    int left = args->left;
    int right = args->right;
    int *arr = args->arr;


    if (left >= right) {
        // base case: single element or empty
        return NULL;
    }


    int mid = left + (right - left) / 2;


    // Prepare arguments for left and right halves
    ThreadArgs *leftArgs = malloc(sizeof(ThreadArgs));
    ThreadArgs *rightArgs = malloc(sizeof(ThreadArgs));


    leftArgs->left = left;
    leftArgs->right = mid;
    leftArgs->arr = arr;


    rightArgs->left = mid + 1;
    rightArgs->right = right;
    rightArgs->arr = arr;


    pthread_t t1, t2;


    // Create two threads to sort each half
    if (pthread_create(&t1, NULL, threaded_merge_sort, leftArgs) != 0) {
        perror("Failed to create thread t1");
        free(leftArgs);
        free(rightArgs);
        return NULL;
    }


    if (pthread_create(&t2, NULL, threaded_merge_sort, rightArgs) != 0) {
        perror("Failed to create thread t2");
        free(leftArgs);
        free(rightArgs);
        return NULL;
    }

    //Waits for both threads to finish sorting their parts before merging them
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);


    // Merge the sorted halves
    merge(arr, left, mid, right);


    // free argument structs
    free(leftArgs);
    free(rightArgs);


    return NULL;

}

int main() {
    FILE* file = fopen("input", "r");
    if (!file) {
        printf("Error opening file.\n");
        return 1;
    }

    int n;
    if (fscanf(file, "%d", &n) != 1) {
    printf("Invalid input: missing size.\n");
    fclose(file);
    return 1;
    }

    if (n > MAX) {
    printf("Maximum supported size is %d\n", MAX);
    fclose(file);
    return 1;
    }


    int *arr = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        if (fscanf(file, "%d", &arr[i]) != 1) {
            printf("Invalid input: missing array elements.\n");
            free(arr);
            fclose(file);
            return 1;
        }
    }
    fclose(file);

 
    ThreadArgs inputArgs;
    inputArgs.left = 0;
    inputArgs.right = n - 1;
    inputArgs.arr = arr;
    threaded_merge_sort(&inputArgs);


    printf("Sorted array:\n");
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    free(arr);
    return 0;
}
