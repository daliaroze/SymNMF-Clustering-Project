from sklearn.metrics import silhouette_score 
from sklearn.metrics import adjusted_rand_score 
import numpy as np
import sys
import symnmfmodule
from symnmf import run_symnmf
from kmeans import run_kmeans

# Compare the clustering results of symnmf and kmeans
# Input: number of clusters and input file name through sys.argv
# Output: prints the Silhouette scores and ARI score
def main():
    try:
        if len(sys.argv) != 3:
            raise ValueError()
        k = int(sys.argv[1])
        file_name = sys.argv[2]
        matrix = np.loadtxt(file_name, delimiter=",", ndmin=2).tolist()

        # Run kmeans and get one cluster label for every point
        kmeans_label = run_kmeans(matrix, k)

       # Run Symmnf and check that the calculation succeeded
        H_final = run_symnmf(matrix, k)
        if H_final is None:
            raise Exception()

        # Assign each point to the largest value in its row of H
        symnmf_label = np.argmax(np.array(H_final), axis=1)
        # Calculate both clustering scores and compare their labels
        kmeans_score = silhouette_score(matrix, kmeans_label)
        symnmf_score = silhouette_score(matrix, symnmf_label)  
        ari_score = adjusted_rand_score(symnmf_label, kmeans_label)

        print(f"nmf: {symnmf_score:.4f}")
        print(f"kmeans: {kmeans_score:.4f}")
        print(f"ari: {ari_score:.4f}")

    except Exception:
        print("An Error Has Occurred")
        sys.exit(1)

if __name__ == "__main__":
    main()