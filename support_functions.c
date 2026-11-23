#include <stdio.h>
#include <stdlib.h>
#include "symnmf.h"

/* Free allocated data and close the file after a reading failure.
 * Input: open file, completed vectors, and current coordinates.
 * Output: NULL */
static vector *failure(FILE *, vector *, cord *);

/* Free all memory allocated for a matrix.
 * Input: matrix and its number of rows.
 * Output: no return value */
void free_mat(double **matrix, int rows)
{
    int i=0;
    if (matrix == NULL) {
        return;
    }
    while (i < rows) {
        free(matrix[i]);
        i++;
    }
    free(matrix);
}

/* Free a linked list of coordinates.
 * Input: pointer to the first coordinate.
 * Output: no return value */
void free_cord(cord *head)
{
    cord *next_cord;
    while (head != NULL) {
        /* Save the next node before freeing the current one*/
        next_cord = head->next;
        free(head);
        head = next_cord;
    }
}

/* Free a linked list of vectors and their coordinates.
 * Input: pointer to the first vector.
 * Output: no return value*/
void free_vectors(vector *head)
{
    vector *next_vector;
    while (head != NULL) {
        /* Save the next vector before freeing the current one*/
        next_vector = head->next;
        free_cord(head->cords);
        free(head);
        head = next_vector;
    }
}

/* Print the general error message and terminate the program.
 * Input: none.
 * Output: does not return. */
void termination(void){
    printf("An Error Has Occurred\n");
    exit(1);
}

/* Allocate and initialize a new coordinate.
 * Input: value of the new coordinate.
 * Output: new coordinate, or NULL if allocation fails*/
cord *allocate_cord(double value)
{
    cord *new_cord;
    new_cord = malloc(sizeof(cord));
    if (new_cord == NULL) {
        return NULL;
    }
    new_cord->value = value;
    new_cord->next = NULL;
    return new_cord;
}

/* Allocate and initialize a new vector.
 * Input: pointer to its coordinates.
 * Output: new vector, or NULL if allocation fails. */
vector *allocate_vector(cord *cords)
{
    vector *new_vector;
    new_vector = malloc(sizeof(vector));
    if (new_vector == NULL) {
        return NULL;
    }
    new_vector->cords = cords;
    new_vector->next = NULL;
    return new_vector;
}

/* Append a coordinate to the end of a coordinates list.
 * Input: pointers to the list's head and tail, and the new value.
 * Output: 1 on success, or 0 if allocation fails*/
int append_cord(cord **head, cord **tail, double value)
{
    cord *new_cord;
    new_cord = allocate_cord(value);
    if (new_cord == NULL) {
        return 0;
    }
    if (*head == NULL) {
        *head = new_cord;
    }
    else {
        (*tail)->next = new_cord;
    }
    *tail = new_cord;
    return 1;
}

/* Append a vector to the end of a vectors list.
 * Input: pointers to the list's head and tail, and the vector's coordinates.
 * Output: 1 on success, or 0 if allocation fails*/
int append_vector(vector **head, vector **tail, cord *cords)
{
    vector *new_vector;
    new_vector = allocate_vector(cords);
    if (new_vector == NULL) {
        return 0;
    }
    if (*head == NULL) {
        *head = new_vector;
    }
    else {
        (*tail)->next = new_vector;
    }
    *tail = new_vector;
    return 1;
}

/* Free allocated data and close the file after a reading failure.
 * Input: open file, completed vectors, and current coordinates.
 * Output: NULL. */
static vector *failure(FILE *file, vector *vectors, cord *cords)
{
    free_cord(cords);
    free_vectors(vectors);
    fclose(file);
    return NULL;
}


/* Read data points from a file into a linked list.
 * Input: name of the input file.
 * Output: vectors list, or NULL if reading or allocation fails. */
vector *build_vectors(const char *file_name)
{
    FILE *file;
    vector *head_vec = NULL, *tail_vec = NULL;
    cord *head_cord = NULL, *tail_cord = NULL;
    double value;
    char c;
    int res;
    file = fopen(file_name, "r");
    if (file == NULL) {
        return NULL;
    }

    while ((res =fscanf(file, "%lf%c", &value, &c)) >= 1) {
        if (!append_cord(&head_cord, &tail_cord, value)) {
            return failure(file, head_vec, head_cord);
        }
        /* A newline or end of file completes the current vector */
        if (res == 1 || c == '\n') {
            if (!append_vector(&head_vec, &tail_vec, head_cord)) {
                return failure(file, head_vec, head_cord);
            }
            head_cord = NULL;
            tail_cord = NULL;
        }
    }
    fclose(file);
    return head_vec;
}

/* Count the number of vectors in the list.
 * Input: pointer to the first vector.
 * Output: number of vectors in the list*/
int get_rows_num(vector *head)
{
    int count=0;
    for (;head!=NULL; head=head ->next){
        count++;
    }
    return count;
}

/* Allocate a matrix with the requested dimensions.
 * Input: number of rows and columns.
 * Output: allocated matrix, or NULL if allocation fails*/
double **create_mat(int rows, int cols)
{
    double **matrix;
    int i;
    matrix = malloc(rows * sizeof(double *));
    if (matrix == NULL) {
        return NULL;
    }
    for (i = 0; i < rows; i++) {
        matrix[i] = malloc(cols * sizeof(double));

        if (matrix[i] == NULL) {
            /* Only the rows before i were successfully allocated. */
            free_mat(matrix, i);
            return NULL;
        }
    }
    return matrix;
}


/* Print a square matrix with four digits after the decimal point.
 * Input: matrix and its dimension.
 * Output: no return value. */
void print_mat(double **mat, int dim){
    int i, j;
    for (i = 0; i < dim; i++) {
        for (j = 0; j < dim; j++) {
            printf("%.4f", mat[i][j]);
            if (j < dim - 1) {
                printf(",");
            }
        }
        printf("\n");
    }
}

/* Multiply two matrices and store the result.
 * Input: two matrices, result matrix, and their dimensions.
 * Output: updates the result matrix in place*/
void mul_matrix(double **first, double **second, double **result, int firstrows, int firstcols,int secondcols){
    int i,j,k;
    double sum;
     for (i = 0; i < firstrows; i++) {
        for (j = 0; j < secondcols; j++) {
            sum = 0.0;

            /* Compute the dot product of row i and column j. */
            for (k = 0; k < firstcols; k++) {
                sum += first[i][k] * second[k][j];
            }

            result[i][j] = sum;
        }
    }

}