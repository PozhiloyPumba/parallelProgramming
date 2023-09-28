#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
	MPI_Comm client;
	MPI_Status status;
	MPI_Info info;

	char port_name[MPI_MAX_PORT_NAME];
	int size;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Info_create(&info);

	MPI_Info_set(info, "ompi_global_scope", "true");
	printf("lol\n");
	MPI_Open_port(info, port_name);

	printf("port name = %s\n", port_name);
	int error = MPI_Publish_name("server", info, port_name);
	printf("loh %d\n", error);
	fflush(stdout);

	MPI_Comm_accept(port_name, MPI_INFO_NULL, 0, MPI_COMM_SELF, &client);
	printf("client connected\n");

	//MPI_Unpublish_name("server", info, port_name);

	MPI_Finalize();
	return 0;
}
