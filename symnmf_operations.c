#include <math.h>
#include "symnmf.h"

/* Calculate the similarity between two data points */
static double similarity(vector *head, int i, int j);

/* Return the coordinates of the vector at the requested index. */
static cord *get_cord(int index, vector *head);

/* Calculate H transpose multiplied by H */
static void mul_matrix_ht_h(double **h, double **ht_h, int n, int k);

/* Perform one update of H and return its squared change. */
static double update_h(double **h, double **wh, double **denominator, int n, int k);

/*Compute the similarity matrix.
 * Input: data points, number of points, and an allocated result matrix.
 * Output: updates the result matrix in place.*/
void sym(vector* head, int size, double** matrix) {
    int i,j;
    double sim;
    for (i = 0; i < size; i++) {
        matrix[i][i] = 0.0;
        /* Calculate only the upper half since the matrix is symmetric*/
        for (j = i + 1; j < size; j++) {
            sim = similarity(head, i, j);
            matrix[i][j] = sim;
            matrix[j][i] = sim;
        }
    }

}

/*Calculate the similarity between two data points.
 * Input: data points and the indices of the two requested points.
 * Output: their similarity according to exp(-||xi-xj||^2 / 2).*/
static double similarity(vector* head, int i, int j){
    double diff,total=0;
    cord *cord1, *cord2;
    cord1= get_cord(i,head);
    cord2= get_cord(j,head);

    /* Add the squared difference of every matching coordinate. */
    while (cord1 != NULL && cord2 !=NULL)
    {
        diff = (cord1->value) -(cord2->value);
        diff *=diff;
        total +=diff;
        cord1= cord1-> next;
        cord2 = cord2-> next;
    }
    return exp(-(total/2.0));

}

/*Find the coordinates of a data point.
 * Input: index of the requested point and the data-points list
 * Output: its coordinates, or NULL if the index is invalid */
static cord *get_cord(int i, vector *head)
{
    int curr=0;
    if (i < 0) {
        return NULL;
    }
    while (head != NULL && curr < i) {
        head = head->next;
        curr++;
    }
    if (head == NULL) {
        return NULL;
    }
    return head->cords;
}

/* Compute the diagonal degree matrix.
 * Input: data points, number of points, and an allocated result matrix.
 * Output: updates the result matrix in place. */
void ddg(vector* head, int size, double** matrix) {
    double sum;
    int i,j;
    /* The diagonal values of D are the row sums of the similarity matrix */
    sym(head, size, matrix);
    for (i = 0; i < size; i++) {
        sum = 0.0;
        for (j = 0; j < size; j++) {
            sum += matrix[i][j];
        }
        matrix[i][i] = sum;
    }
    /* All values outside the diagonal of D are zero */
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            if (i != j) {
                matrix[i][j] = 0.0;
            }
        }
    }
}

/* Compute the normalized similarity matrix.
 * Input: data points, number of points, and an allocated result matrix.
 * Output: the normalized matrix, or NULL if allocation fails. */
double **norm(vector *head, int size, double **matrix)
{
    int i,j;
    double left_factor, right_factor;
    double **degree;

    sym(head, size, matrix);
    /* Allocate degree matrix */
    degree = create_mat(size, size); 
    if (degree == NULL) {
        return NULL;
    }
    ddg(head, size, degree);

    /* Store the diagonal values of D^(-1/2) */
    for (i = 0; i < size; i++) {
        degree[i][i] = 1.0 / sqrt(degree[i][i]);
    }

    /* Calculate W = D^(-1/2) * A * D^(-1/2) */
    for (i = 0; i < size; i++) {
        left_factor = degree[i][i];

        for (j = 0; j < size; j++) {
            right_factor = degree[j][j];
            matrix[i][j] *= left_factor * right_factor;
        }
    }
    free_mat(degree, size);
    return matrix;
}

/*Optimize H using the SymNMF update rule.
 * Input: initial H, normalized matrix W, number of points and clusters.
 * Output: optimized H, or NULL if allocation fails. */
double** symnmf(double** h, double** w,int n, int k){
    double **wh, **ht_h, **denominator;
    double frobenius_norm;
    int iter;
    /* Allocate the temporary matrices reused in every iteration*/
    wh = create_mat(n, k);
    ht_h = create_mat(k, k);
    denominator = create_mat(n, k);
    if (wh == NULL || ht_h == NULL || denominator == NULL) {
        free_mat(wh, n);
        free_mat(ht_h, k);
        free_mat(denominator, n);
        return NULL;
    }
    /* Iterative update loop until convergence or max iterations reached */
    for (iter = 0; iter < MAX_ITER; iter++) {
        /* Compute the numerator W*H and denominator H*(H^T*H)*/
        mul_matrix(w, h, wh, n, n, k);
        mul_matrix_ht_h(h, ht_h, n, k);
        mul_matrix(h, ht_h, denominator, n, k, k);

        frobenius_norm =update_h(h, wh, denominator, n, k);
        if (frobenius_norm < EPSILON) {
            break;
        }
    }
    free_mat(wh, n);
    free_mat(ht_h, k);
    free_mat(denominator, n);
    return h;
}

/* Compute H transpose multiplied by H.
 * Input: H, result matrix, number of rows, and number of columns of H.
 * Output: stores H^T * H in the result matrix*/
static void mul_matrix_ht_h(double **h, double **ht_h, int n, int k)
{
    int i, j, r;
    double sum;

    for (i = 0; i < k; i++) {
        for (j = 0; j < k; j++) {
            sum = 0.0;

             /* Each entry is the dot product of two columns of H*/
            for (r = 0; r < n; r++) {
                sum += h[r][i] * h[r][j];
            }
            ht_h[i][j] = sum;
        }
    }
}

/*Perform one SymNMF update and calculate the change in H.
 * Input: H, W*H, H*(H^T*H), and the dimensions of H.
 * Output: squared Frobenius norm of the change in H*/
static double update_h(double **h, double **wh,double **denominator, int n, int k){
    int i, j;
    double prev, curr, diff,frobenius_norm= 0.0;
    for (i = 0; i < n; i++) {
        for (j = 0; j < k; j++) {
            prev = h[i][j];
            /* Apply the update rule and prevent division by zero*/
            curr = prev *((1.0 - BETA) + BETA * wh[i][j] /   (denominator[i][j] + 1e-12));
            diff = curr - prev;
            frobenius_norm += diff * diff;
            h[i][j] = curr;
        }
    }
    return frobenius_norm;
}