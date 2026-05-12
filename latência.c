#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main (int argc, char *argv[]) {

    MPI_Status status;
    int id, p,
	i, rounds;
    double secs, latencia;

    MPI_Init (&argc, &argv);

    MPI_Comm_rank (MPI_COMM_WORLD, &id);
    MPI_Comm_size (MPI_COMM_WORLD, &p);

    if(argc != 2){
		if (!id) printf ("Command line: %s <n-rounds>\n", argv[0]);
		MPI_Finalize();
		exit (1);
    }
    rounds = atoi(argv[1]);

    MPI_Barrier (MPI_COMM_WORLD);
    secs = - MPI_Wtime();

    for(i = 0; i < rounds; i++){
		if(!id){
			latencia = MPI_Wtime();
		    MPI_Send(&latencia, 1, MPI_DOUBLE, 1, i, MPI_COMM_WORLD);
		    MPI_Recv(&latencia, 1, MPI_DOUBLE, p-1, i, MPI_COMM_WORLD, &status);
			printf("%d ---- > %d : %.12f\n",p - 1, id, MPI_Wtime() - latencia);
		}
		else{
		    MPI_Recv(&latencia, 1, MPI_DOUBLE, id-1, i, MPI_COMM_WORLD, &status);
			printf("%d ---- > %d : %.12f\n",id - 1, id, MPI_Wtime() - latencia);
			latencia = MPI_Wtime();
		    MPI_Send(&latencia, 1, MPI_DOUBLE, (id+1) % p, i, MPI_COMM_WORLD);
		}
    }

    MPI_Barrier (MPI_COMM_WORLD);
    secs += MPI_Wtime();

    if(!id){
		printf("Rounds= %d, N Processes = %d, Time = %12.6f sec,\n",
		       rounds, p, secs);
		printf ("Average time per Send/Recv = %6.2f us\n",
			secs * 1e6 / (2*rounds*p));
   }
   MPI_Finalize();
   return 0;
}