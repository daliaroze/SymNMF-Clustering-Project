import numpy as np
import symnmfmodule
import sys


# randomly initialize the H matrix for the SymNMF algorithm based on the normalized similarity matrix
# input: normalized similarity matrix W, number of data points n, number of clusters k
# output: initialized H matrix of size n x k with values sampled uniformly from [0, 2* sqrt(avg(W) / k)]
def initial_h(W,n,k):
    w_nparr = np.array(W)
    avg = np.mean(w_nparr)
    # Calculate the upper bound required for the random initialization
    max_num= 2 * np.sqrt(avg/ k)
    return np.random.uniform(low=0.0,high=max_num,size=(n, k)).tolist()

# Run all the steps of the SymNMF algorithm.
# Input: the data points and the number of clusters.
# Output: the final H matrix, or None if one of the C functions fails.
def run_symnmf(datapoints, k):
    # Reset the seed here so every SymNMF run starts with the same H
    np.random.seed(1234)
    n = len(datapoints)
    W = symnmfmodule.norm(datapoints)

    # Stop if calculating W failed in the C extension
    if W is None:
        return None
    H = initial_h(W, n, k)
    return symnmfmodule.symnmf(H, W, n, k)

# Check whether a string represents a positive integer
# input: a string object
# output: the integer (if it's a positive int), else None
def getIfInt(str_val):
    try:
        # Converting to float first also allows values such as "3.0".
        val = float(str_val)
        if val > 0 and val == int(val):
            return int(val)
    except ValueError:
        pass
    return None

# terminate the program with an appropriate error message and exit code
# input: reason string indicating the type of error encountered
# output: prints the error message and exit the program with value 1
def termination(reason):
    if reason =="INVALID_K":
        print("Incorrect number of clusters!") 
    else:
        print("An Error Has Occurred") 
    sys.exit(1)

# printing a matrix where the values are formatted to be 4 decimal places
# input: matrix of floats
# output: print formatted matrix rows
def print_matrix(mat):
    for row in mat:
        print(",".join(f"{val:.4f}" for val in row))

# Parse the command-line arguments, execute the requested goal and print the result.
# input: command-line arguments (K, goal, and file_name) via sys.argv
# output: prints formatted resulting matrix to standard output
def main():
    try:
        if len(sys.argv) != 4: 
            termination("GENERAL_ERROR")
        K = getIfInt(sys.argv[1])
        goal = sys.argv[2]
        file_name = sys.argv[3]

        datapoints = np.loadtxt(file_name, delimiter=",",ndmin=2).tolist()
        if K is None or K <= 1 or K >= len(datapoints):
            termination("INVALID_K")
        if goal == "sym":
            result = symnmfmodule.sym(datapoints)
        elif goal == "ddg":
            result = symnmfmodule.ddg(datapoints)
        elif goal == "norm":
            result = symnmfmodule.norm(datapoints)
        elif goal =="symnmf":
            result = run_symnmf(datapoints, K)
        else:
            raise Exception()
        # None result means that the C extension failed
        if result is None:
            raise Exception()

        print_matrix(result)

    except Exception:
        termination("GENERAL_ERROR")

if __name__ == "__main__":
    main()