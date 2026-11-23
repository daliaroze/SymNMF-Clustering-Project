#ifndef SYMNMF_H
#define SYMNMF_H
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* Algorithm constants */
#define MAX_ITER 300
#define BETA 0.5
#define EPSILON 1e-4

/* Data structures */
typedef struct cord
{
    double value;
    struct cord *next;
} cord;

typedef struct vector
{
    struct vector *next;
    struct cord *cords;
} vector;

/* Functions implemented in support_functions.c */

/* Print the general error message and terminate the program */
void termination(void);

/* Allocate a new coordinate with the given value */
cord *allocate_cord(double value);

/* Allocate a new vector containing the given coordinates */
vector *allocate_vector(cord *cords);

/* Append a coordinate to the end of a coordinates list */
int append_cord(cord **head, cord **tail, double value);

/* Append a vector to the end of a vectors list */
int append_vector(vector **head, vector **tail, cord *cords);

/* Free a linked list of coordinates */
void free_cord(cord *head);

/* Free a linked list of vectors and their coordinates */
void free_vectors(vector *head);

/* Read the data points from the given input file */
vector *build_vectors(const char *file_name);

/* Count the number of vectors in a linked list */
int get_rows_num(vector *head);

/* Allocate a matrix with the requested number of rows and columns */
double **create_mat(int rows, int cols);

/* Free a matrix with the given number of rows */
void free_mat(double **matrix, int rows);

/* Print a square matrix with four digits after the decimal point */
void print_mat(double **matrix, int dim);

/* Multiply two matrices and store the result in a third matrix */
void mul_matrix(double **first, double **second, double **result, int firstrows, int firstcols, int secondcols);

/* Functions implemented in symnmf_operations.c */

/* Calculate the similarity matrix */
void sym(vector *head, int size, double **matrix);

/* Calculate the diagonal degree matrix */
void ddg(vector *head, int size, double **matrix);

/* Calculate and return the normalized similarity matrix */
double **norm(vector *head, int size, double **matrix);

/* Optimize H using the SymNMF update rule. */
double **symnmf(double **h, double **w, int n, int k);

#endif