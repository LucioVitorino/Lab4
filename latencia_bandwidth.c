#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
    int id, p;
    int rounds, size;
    double start, end, time;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    if (argc != 3)
    {
        if (id == 0)
            printf("Usage: %s <rounds> <message_size_bytes>\n", argv[0]);
        MPI_Finalize();
        return 1;
    }

    rounds = atoi(argv[1]);
    size = atoi(argv[2]);

    char *buffer = (char *)malloc(size);

    if (buffer == NULL)
    {
        printf("Memory allocation error\n");
        MPI_Finalize();
        return 1;
    }

    MPI_Barrier(MPI_COMM_WORLD);

    start = MPI_Wtime();

    for (int i = 0; i < rounds; i++)
    {
        if (id == 0)
        {
            MPI_Send(buffer, size, MPI_CHAR, 1, i, MPI_COMM_WORLD);
            MPI_Recv(buffer, size, MPI_CHAR, p - 1, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        else
        {
            MPI_Recv(buffer, size, MPI_CHAR, id - 1, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(buffer, size, MPI_CHAR, (id + 1) % p, i, MPI_COMM_WORLD);
        }
    }

    end = MPI_Wtime();

    time = end - start;

    if (id == 0)
    {
        double latency = (time * 1e6) / (2 * rounds * p); // µs por Send/Recv
        double bandwidth = ((double)size * rounds * p) / time; // bytes/s

        printf("Rounds = %d, Processes = %d\n", rounds, p);
        printf("Message size = %d bytes\n", size);
        printf("Total time = %lf sec\n", time);
        printf("Latency per Send/Recv = %lf us\n", latency);
        printf("Bandwidth = %lf MB/s\n", bandwidth / (1024 * 1024));
    }

    free(buffer);

    MPI_Finalize();
    return 0;
}