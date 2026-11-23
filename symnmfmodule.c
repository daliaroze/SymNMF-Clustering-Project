#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "symnmf.h"

/* Convert a Python list of lists into a C matrix
 * Input: Python list and the expected number of rows and columns
 * Output: allocated C matrix, or NULL if conversion fails */
static double **py_list_to_mat(PyObject *py_list, int rows, int cols);

/* Convert a C matrix into a Python list of lists
 * Input: C matrix and its number of rows and columns
 * Output: new Python list, or NULL if creation fails */
static PyObject *mat_to_py_list(double **mat, int rows, int cols);

/* Convert one Python row into a linked list of coordinates
 * Input: Python list representing one data point
 * Output: coordinates list, or NULL if conversion fails */
static cord *py_row_to_cords(PyObject *row_py);

/* Convert Python data points into a linked list of vectors
 * Input: Python list of data points and a pointer for storing their number
 * Output: vectors list, or NULL if conversion fails */
static vector *py_list_to_vectors(PyObject *py_list, int *n);

/* Python wrapper for calculating the similarity matrix */
static PyObject* py_sym(PyObject*, PyObject*);

/* Python wrapper for calculating the diagonal degree matrix */
static PyObject* py_ddg(PyObject*, PyObject*);

/* Python wrapper for calculating the normalized similarity matrix */
static PyObject* py_norm(PyObject*, PyObject*);

/* Python wrapper for running the SymNMF optimization */
static PyObject* py_symnmf(PyObject*, PyObject*);


/* Calculate the similarity matrix for Python data points.
 * Input: Python arguments containing a list of data points.
 * Output: similarity matrix as a Python list, or NULL on failure. */
static PyObject* py_sym(PyObject *self, PyObject *args)
{
    PyObject *datapoints_py,*result_py;
    vector *datapoints;
    double **matrix;
    int n;
    if (!PyArg_ParseTuple(args, "O", &datapoints_py)) {
        return NULL;
    }
    datapoints = py_list_to_vectors(datapoints_py, &n);
    if (datapoints == NULL) {
        return NULL;
    }
    matrix = create_mat(n, n);
    if (matrix == NULL) {
        free_vectors(datapoints);
        return NULL;
    }
    /* Run the C calculation and convert its result back to Python if not NULL */
    sym(datapoints, n, matrix);
    result_py = mat_to_py_list(matrix, n, n);
    free_mat(matrix, n);
    free_vectors(datapoints);
    return result_py;
}

/* Calculate the degree matrix for Python data points.
 * Input: Python arguments containing a list of data points.
 * Output: degree matrix as a Python list, or NULL on failure. */
static PyObject *py_ddg(PyObject *self, PyObject *args)
{
    PyObject *datapoints_py, *result_py;
    vector *datapoints;
    double **matrix;
    int n;
    if (!PyArg_ParseTuple(args, "O", &datapoints_py)) {
        return NULL;
    }
    datapoints = py_list_to_vectors(datapoints_py, &n);
    if (datapoints == NULL) {
        return NULL;
    }
    matrix = create_mat(n, n);
    if (matrix == NULL) {
        free_vectors(datapoints);
        return NULL;
    }
    /* Run the C calculation and convert its result back to Python. */
    ddg(datapoints, n, matrix);
    result_py = mat_to_py_list(matrix, n, n);
    free_mat(matrix, n);
    free_vectors(datapoints);
    return result_py;
}

/* Calculate the normalized matrix for Python data points.
 * Input: Python arguments containing a list of data points.
 * Output: normalized matrix as a Python list, or NULL on failure. */
static PyObject *py_norm(PyObject *self, PyObject *args)
{
    PyObject *datapoints_py, *result_py;
    vector *datapoints;
    double **matrix;
    int n;
    if (!PyArg_ParseTuple(args, "O", &datapoints_py)) {
        return NULL;
    }
    datapoints = py_list_to_vectors(datapoints_py, &n);
    if (datapoints == NULL) {
        return NULL;
    }
    matrix = create_mat(n, n);
    if (matrix == NULL) {
        free_vectors(datapoints);
        return NULL;
    }
    /* Run the C calculation and convert its result back to Python if not Null */
    if (norm(datapoints, n, matrix) == NULL) {
        free_mat(matrix, n);
        free_vectors(datapoints);
        return NULL;
    }
    result_py = mat_to_py_list(matrix, n, n);
    free_mat(matrix, n);
    free_vectors(datapoints);
    return result_py;
}

/* Run symnmf with matrices received from Python.
 * Input: Python matrices H and W, number of points, and clusters.
 * Output: optimized H as a Python list, or NULL on failure. */
static PyObject *py_symnmf(PyObject *self, PyObject *args)
{
    PyObject *h_py, *w_py, *result_py;
    double **h, **w, **result;
    int n, k;
    if (!PyArg_ParseTuple(args, "OOii", &h_py, &w_py, &n, &k)) {
        return NULL;
    }
    h = py_list_to_mat(h_py, n, k);
    if (h == NULL) {
        return NULL;
    }
    w = py_list_to_mat(w_py, n, n);
    if (w == NULL) {
        free_mat(h, n);
        return NULL;
    }
     /* symnmf updates H directly and returns the same matrix */
    result = symnmf(h, w, n, k);
    if (result == NULL) {
        free_mat(h, n);
        free_mat(w, n);
        return NULL;
    }
    result_py = mat_to_py_list(result, n, k);
    free_mat(h, n);
    free_mat(w, n);
    return result_py;
}

/* Connect each Python method name to its C wrapper function. */
static PyMethodDef symnmfMethods[] = {
    {
        "symnmf",
        (PyCFunction)py_symnmf,
        METH_VARARGS,
        PyDoc_STR("Perform the Symnmf optimization")
    },
    {
        "sym",
        (PyCFunction)py_sym,
        METH_VARARGS,
        PyDoc_STR("Calculate the similarity matrix")
    },
    {
        "ddg",
        (PyCFunction)py_ddg,
        METH_VARARGS,
        PyDoc_STR("Calculate the degree matrix")
    },
    {
        "norm",
        (PyCFunction)py_norm,
        METH_VARARGS,
        PyDoc_STR("Calculate normalized similarity")
    },
    {NULL, NULL, 0, NULL}
};

/* Define the Python module */
static struct PyModuleDef symnmfmodule = {
    PyModuleDef_HEAD_INIT,
    "symnmfmodule",
    NULL,
    -1,
    symnmfMethods
};

/* Create the module when Python imports symnmfmodule.
 * Input: none.
 * Output: the created Python module, or NULL if creation fails. */
PyMODINIT_FUNC PyInit_symnmfmodule(void){
    PyObject* m; 
    m = PyModule_Create(&symnmfmodule);
    if(m == NULL) {
    return NULL;
    }
    return m;
}

/* Convert a Python list of lists into a C matrix. */
static double **py_list_to_mat(PyObject *py_list, int rows, int cols)
{
    int i, j;
    double **mat;
    PyObject *row_py,*item;
    mat = create_mat(rows, cols);
    if (mat == NULL) {
        return NULL;
    }
    /* Read each Python row and copy its values into the C matrix */
    for (i = 0; i < rows; i++) {
        row_py = PyList_GetItem(py_list, i);
        if (row_py == NULL) { 
        free_mat(mat, rows);
        return NULL;
    }
        for (j = 0; j < cols; j++) {
            item = PyList_GetItem(row_py, j);
            if (item == NULL) { 
            free_mat(mat, rows);
            return NULL;
        }
            mat[i][j] = PyFloat_AsDouble(item);
        }
    }
    return mat;
}

/* Convert a C matrix into a Python list of lists. */
static PyObject *mat_to_py_list(double **mat, int rows, int cols)
{
    int i, j;
    PyObject *python_result, *python_row;

    /* Create the list that will contain all matrix rows */
    python_result = PyList_New(rows);
    if (python_result == NULL) return NULL; 
    for (i = 0; i < rows; i++) {
        python_row = PyList_New(cols);
        if (python_row == NULL) return NULL;
        /* Convert every C value into a Python float */
        for (j = 0; j < cols; j++) {
            PyList_SetItem(python_row,j,PyFloat_FromDouble(mat[i][j]));
        }
        /* Add the completed row to the Python list */
        PyList_SetItem(python_result, i, python_row);
    }
    return python_result;
}

/* Convert one Python row into a linked list of coordinates. */
static cord *py_row_to_cords(PyObject *row_py)
{
    Py_ssize_t cols,j;
    cord *head = NULL,*tail = NULL;
    PyObject *item;
    double value;
    /* The number of items in the row is the point's dimension */
    cols = PyList_Size(row_py);
    if (cols < 0) {
    return NULL;
    }
    for (j = 0; j < cols; j++) {
        item = PyList_GetItem(row_py, j);
        if (item == NULL) {
            free_cord(head);
            return NULL;
        }
        /* Convert the Python value and append it as a coordinate */
        value = PyFloat_AsDouble(item);
        if (!append_cord(&head, &tail, value)) {
            free_cord(head);
            return NULL;
        }
    }
    return head;
}

/* Convert Python data points into a linked list of vectors. */
static vector *py_list_to_vectors(PyObject *py_list, int *n)
{
    Py_ssize_t rows,i;
    vector *head = NULL,*tail = NULL;
    cord *cords;
    PyObject *row_py;
    /* Each row in the Python list represents one data point */
    rows = PyList_Size(py_list);
    if (rows < 0) {
    return NULL;
}
    *n = (int)rows;
    for (i = 0; i < rows; i++) {
        row_py = PyList_GetItem(py_list, i);
          if (row_py == NULL) {
            free_vectors(head);
            return NULL;
        }
        /* Convert the row to coordinates and add it as a vector */
        cords = py_row_to_cords(row_py);

        if (cords == NULL) {
            free_vectors(head);
            return NULL;
        }
        if (!append_vector(&head, &tail, cords)) {
            free_cord(cords);
            free_vectors(head);
            return NULL;
        }
    }
    return head;
}