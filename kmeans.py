import sys

# Check whether a string represents a positive integer
# Input: a string
# Output: the integer value if valid, otherwise None
def getIfInt(str_val):
    try:
        val = float(str_val)
        if val > 0 and val == int(val):
            return int(val)
    except ValueError:
        pass
    return None

# Calculate the Euclidean distance between two points
# Input: two points with the same dimension
# Output: the Euclidean distance between them
def euclidean_distance(p, q):
    total_sum = 0
    for i in range(len(p)):
        total_sum += (p[i] - q[i]) ** 2
    return total_sum ** 0.5

# Find the closest centroid to a given point
# Input: a data point and a list of centroids
# Output: the index of the closest centroid
def closest_centroid(point, centroids):
    min_dist = float('inf')
    closest_index = 0

    for i in range(len(centroids)):
        dist = euclidean_distance(point, centroids[i])

        if dist < min_dist:
            min_dist = dist
            closest_index = i

    return closest_index

# Calculate the new centroid of one cluster
# Input: cluster points, previous centroid, and point dimension
# Output: the updated centroid
def update_single_cluster(curr_cluster, old_centroid, d):
    curr_cen_len = len(curr_cluster)
    # Keep the previous centroid if no points were assigned to it
    if curr_cen_len == 0:
        return old_centroid

    # Sum every coordinate separately and calculate its mean
    new_centroid = [0.0] * d
    for point in curr_cluster:
        for i in range(d):
            new_centroid[i] += point[i]
    for i in range(d):
        new_centroid[i] = new_centroid[i] / curr_cen_len
    return new_centroid

# Run the K-means algorithm to find the cluster centroids
# Input: datapoints, number of clusters K, maximum number of iterations,
# and epsilon, which determines when the centroids have stopped moving
# Output: a list of K final centroids
def fit_kmeans(datapoints, K, max_iter=400, epsilon=0.001):
    # Use the first K data points as the initial centroids
    centroids = [list(point) for point in datapoints[:K]]
    d = len(datapoints[0])
    iteration = 0
    while iteration < max_iter:
        clusters = [[] for _ in range(K)]
        # Assign every point to its closest current centroid
        for point in datapoints:
            closest_index = closest_centroid(point, centroids)
            clusters[closest_index].append(point)

        max_delta = 0
        new_centroids = []
        # Update each centroid and track the largest movement
        for k in range(K):
            old_centroid = centroids[k]
            new_centroid = update_single_cluster(clusters[k], old_centroid, d)
            new_centroids.append(new_centroid)
            dist = euclidean_distance(new_centroid, old_centroid)
            max_delta = max(dist, max_delta)

        centroids = new_centroids
        iteration += 1
        # Stop when all centroids move by less than epsilon
        if max_delta < epsilon:
            break
    return centroids

# Run kmeans and assign a cluster label to every point
# Input: data points and the number of clusters
# Output: a list of labels in the same order as the data points
def run_kmeans(datapoints, K):
    centroids = fit_kmeans(datapoints, K, max_iter=300, epsilon=1e-4)
    # A point's label is the index of its closest final centroid
    labels = [closest_centroid(point, centroids) for point in datapoints]
    return labels

# Read data points from standard input
# Input: lines received through sys.stdin
# Output: a list of data points represented as lists of floats
def read_input():
    datapoints = []
    for line in sys.stdin:
        line = line.strip()
        if not line:
            continue
        # Each line represents one comma-separated data point
        point = [float(x) for x in line.split(',')]
        datapoints.append(point)
    return datapoints


# Read arguments, run kmeans, and print the final centroids
# Input: K and an optional maximum iteration value through sys.argv
# Output: prints the final centroids to standard output
def main():
    if len(sys.argv) < 2 or len(sys.argv) > 3:
        print("An Error Has Occurred")
        sys.exit(1)
    K = getIfInt(sys.argv[1])
    if K is None or K <= 1:
        print("Incorrect number of clusters!")
        sys.exit(1)
    
    max_iter = 400
    if len(sys.argv) == 3:
        max_iter = getIfInt(sys.argv[2])
        if max_iter is None or not (1 < max_iter < 800):
            print("Incorrect maximum iteration!")
            sys.exit(1)

    datapoints = read_input()
    N = len(datapoints)
    if N == 0:
        print("An Error Has Occurred")
        sys.exit(1)
    if not (1 < K < N):
        print("Incorrect number of clusters!")
        sys.exit(1)

    centroids = fit_kmeans(datapoints, K, max_iter=max_iter, epsilon=0.001)
    for centroid in centroids:
        print(",".join(['%.4f' % x for x in centroid]))


if __name__ == "__main__":
    try:
        main()
    except Exception:
        print("An Error Has Occurred")
        sys.exit(1)