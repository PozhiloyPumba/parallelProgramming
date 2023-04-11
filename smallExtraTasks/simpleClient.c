#include <stdio.h>

#include "mpi.h"

int main(int argc, char *argv[]) {
	MPI_Comm server;
	char port_name[MPI_MAX_PORT_NAME];
	MPI_Info info;

	char global[] = "ompi_lookup_order";
	char true_global[] = "local,global";

	MPI_Init(&argc, &argv);
	MPI_Info_create(&info);

	int error = MPI_Info_set(info, global, true_global);
	printf("loh %d\n", error);

	error =	MPI_Lookup_name("server", info, port_name);
	printf("lol %d %s\n", error, port_name);
	printf("port name = %s\n", port_name);
	fflush(stdout);
    MPI_Comm_connect(port_name, MPI_INFO_NULL, 0, MPI_COMM_SELF, &server);

	MPI_Comm_disconnect(&server);
	MPI_Finalize();
	return 0;
}
