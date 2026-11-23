#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "symnmf.h"


/* Run the requested goal. Input: goal and file name. Output: prints the result.*/
int main(int argc, char **argv) {
    vector *head;
    double **matrix;
    int N;
    /* Extract goal argument safely and check validity */
    char *goal = (argc == 3) ? argv[1] : NULL;
    if (!goal || (strcmp(goal, "sym") && strcmp(goal, "ddg") && strcmp(goal, "norm"))) {
        termination();
    }
    /* Build data points (vector = struct from lecture) from input file */
    if (!(head = build_vectors(argv[2]))) {
        termination();
    }
    N = get_rows_num(head);
    matrix = create_mat(N, N);
    if (matrix == NULL) {
        free_vectors(head);
        termination();
    }
    /* Execute the specific matrix calculation based on the goal */
    if (strcmp(goal, "sym") == 0) {
        sym(head, N, matrix);
    }
    else if (strcmp(goal, "ddg") == 0) {
        ddg(head, N, matrix);
    }
    else {
        if (norm(head, N, matrix) == NULL) {
            free_mat(matrix, N);
            free_vectors(head);
            termination();
    } 
}
    print_mat(matrix, N);
    free_mat(matrix, N);
    free_vectors(head);
    return 0;
}