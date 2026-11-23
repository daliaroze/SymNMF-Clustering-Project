# SymNMF Clustering

Implementation of Symmetric Non-negative Matrix Factorization (SymNMF) for clustering, using C and Python. Includes a K-Means implementation and clustering-quality comparisons.

## Features

- Similarity matrix (SYM), diagonal degree matrix (DDG), and normalized similarity matrix (NORM)
- Iterative SymNMF optimization in C
- Python extension module implemented with the Python C API
- K-Means baseline and comparison using Silhouette score and Adjusted Rand Index (ARI)

## Requirements

- Python 3 with `setuptools`, `numpy`, and `scikit-learn`
- GCC and `make` (for the C executable)
- A C compiler and Python development headers (for the extension)

## Build

```bash
python -m pip install setuptools numpy scikit-learn
python setup.py build_ext --inplace
make
```

## Usage

Input files should contain numeric data points in CSV format, one point per row.

```bash
python symnmf.py <K> <goal> <input.csv>
./symnmf <goal> <input.csv>
python analysis.py <K> <input.csv>
python kmeans.py <K> [max_iter] < input.csv
```

Supported goals:

- `symnmf.py`: `sym`, `ddg`, `norm`, or `symnmf`
- `./symnmf` (C executable): `sym`, `ddg`, or `norm`

`kmeans.py` reads the data points from standard input. `max_iter` is optional (default 400, must be between 2 and 799).

## Project structure

- `symnmf.c`, `symnmf_operations.c`, `support_functions.c`, `symnmf.h`: C implementation
- `symnmfmodule.c`: Python C extension
- `symnmf.py`: Python command-line interface
- `kmeans.py`: K-Means implementation
- `analysis.py`: clustering comparison
- `Makefile`, `setup.py`: build configuration
