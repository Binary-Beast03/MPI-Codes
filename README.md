# MPI Basic Codes Repository

![MPI Logo](https://www.hpcwire.com/wp-content/uploads/2017/05/MPIlogo2.gif)

This repository contains a collection of basic codes demonstrating the use of MPI (Message Passing Interface) for parallel programming. MPI is a widely-used standard for message-passing libraries that allows developers to write parallel applications for distributed-memory systems.

## Table of Contents

- [Introduction](#introduction)
- [Prerequisites](#prerequisites)
- [How to Use](#how-to-use)
- [Examples](#examples)
- [Contributing](#contributing)
- [License](#license)

## Introduction

MPI is a powerful tool for writing parallel programs, especially for high-performance computing and distributed systems. This repository provides simple MPI code examples to help users get started with parallel programming using MPI.

## Prerequisites

Before running the MPI codes in this repository, you'll need the following:

- A working MPI installation on your system. You can download MPI from [mpich.org](https://www.mpich.org/) or [open-mpi.org](https://www.open-mpi.org/).
- A C/C++ compiler (e.g., GCC) compatible with your MPI installation.

## How to Use

Follow these steps to compile and run the MPI codes in this repository:

1. Clone this repository to your local machine:  
  `git clone https://github.com/Binary-Beast03/MPI-Codes.git`  
  `cd MPI-Codes.git`
2. Compile the MPI code using the MPI compiler wrapper (e.g., `mpicc`):  
   `mpicc -o mpi_example mpi_example.c`
3. Run the compiled executable using the MPI launcher (e.g., `mpirun` or `mpiexec`):  
   `mpirun -np <num_processes> ./mpi_example`  
   Replace `<num_processes>` with the number of processes (e.g., 4) you want to use in the MPI program.
## Examples

1. **Hello World**: A simple MPI program that prints "Hello, World!" from each process.

2. **Pi**: Calculate an approximation of pi using the Monte Carlo method with MPI.

3. **Matrix Multiplication**: Perform matrix multiplication in parallel using MPI.

4. **Merge Sorting Parallel**: Implement parallel sorting algorithm with MPI.

## Contributing

Contributions to this repository are welcome! If you have any MPI code examples you'd like to add or find any issues that need fixing, feel free to submit a pull request. Please ensure your code is well-documented and follows the repository's coding conventions.

## License

This repository is licensed under the MIT License. You can find more information in the [LICENSE](LICENSE) file.
   
