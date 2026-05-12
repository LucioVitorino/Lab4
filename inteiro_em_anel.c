#include <stdio.h>
#include <mpi.h>

// (id + 1) % p processo seguinte
// (id + p - 1) % p processo anterior 

int main(int argc, char **argv)
{
    int id, p;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    int n = 42;
    if (id == 0)
    {
        MPI_Send(&n, 1, MPI_INT, (id + 1) % p, 0, MPI_COMM_WORLD);
        MPI_Recv(&n, 1, MPI_INT, (id + p - 1) % p, 0, MPI_COMM_WORLD, &status);
        printf(" Valor Final : %d\n", n);
    }
    else
    {
        MPI_Recv(&n, 1, MPI_INT, (id + p - 1) % p, 0, MPI_COMM_WORLD, &status);
        n += id;
        MPI_Send(&n, 1, MPI_INT, (id + 1) % p, 0, MPI_COMM_WORLD);
    }
    MPI_Finalize();
}