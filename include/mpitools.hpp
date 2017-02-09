#ifndef _MPITOOLS_H
#define _MPITOOLS_H


// #include <mpi.h>

#if defined MPICH || defined OPEN_MPI

namespace mpi{

	inline void init(int * argc, char *** argv){
		int ierr;
		ierr = MPI_Init(argc, argv);
	}

	inline void finalize(){
		int ierr;
		ierr = MPI_Finalize();
	}

	inline int master_proc(){
		return 0;
	}

	inline int rank(){
		int ierr, proc;
		ierr = MPI_Comm_rank(MPI_COMM_WORLD, &proc);
		return proc;
	}

	inline bool is_master(){
		return (rank() == master_proc());
	}

	inline int size(){
		int ierr, tproc;
		ierr = MPI_Comm_size(MPI_COMM_WORLD, &tproc);
		return tproc;
	}

	inline void send(void * buf, int count,
					 MPI_Datatype datatype, int dest,
					 int tag, MPI_Comm comm){
		int ierr;
		ierr = MPI_Send(buf, count, datatype, dest, 
						tag, comm);
	}

	inline void isend(void * buf, int count,
					 MPI_Datatype datatype, int dest,
					 int tag, MPI_Comm comm, MPI_Request * req){
		int ierr;
		ierr = MPI_Isend(buf, count, datatype, dest, 
						tag, comm, req);
	}

	inline void recv(void * buf, int count,
					 MPI_Datatype datatype, int source,
					 int tag, MPI_Comm comm,
					 MPI_Status * status){
		int ierr;
		ierr = MPI_Recv(buf, count, datatype, source, 
						tag, comm, status);
	}

	inline void wait(MPI_Request * req, MPI_Status * status){
		int ierr;
		ierr = MPI_Wait(req, status);
	}

	inline void sendrecv(void * sendbuf, int sendcount,
						 MPI_Datatype sendtype, int dest, int sendtag,
						 void * recvbuf, int recvcount, 
						 MPI_Datatype recvtype, int source, int recvtag,
						 MPI_Comm comm, MPI_Status * status){
		int ierr;
		ierr = MPI_Sendrecv(sendbuf, sendcount, sendtype, dest, sendtag,
							recvbuf, recvcount, recvtype, source, recvtag,
							comm, status);
	}

	inline void reduce(void * sendbuf, void * recvbuf, int count, 
					   MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm){
		int ierr;
		ierr = MPI_Reduce(sendbuf, recvbuf, count, datatype, op, root, comm);
	}

	inline void barrier(MPI_Comm comm){
		int ierr;
		ierr = MPI_Barrier(comm);
	}

}

#else
	
namespace mpi{

	typedef unsigned int MPI_Datatype;
	typedef unsigned int MPI_Comm;
	typedef unsigned int MPI_Op;
	typedef unsigned int MPI_Status;
	typedef unsigned int MPI_Request;

	inline void init(int * argc, char *** argv){
		std::cout << "MPI UNAVAILABLE" << std::endl;
	}

	inline void finalize(){
	}

	inline int master_proc(){
		return 0;
	}

	inline int rank(){
	}

	inline bool is_master(){
	}

	inline int size(){
		return 1;
	}

	inline void send(void * buf, int count,
					 MPI_Datatype datatype, int dest,
					 int tag, MPI_Comm comm){
	}

	inline void isend(void * buf, int count,
					 MPI_Datatype datatype, int dest,
					 int tag, MPI_Comm comm, MPI_Request * req){
	}

	inline void recv(void * buf, int count,
					 MPI_Datatype datatype, int source,
					 int tag, MPI_Comm comm,
					 MPI_Status * status){

	}

	inline void wait(MPI_Request * req, MPI_Status * status){

	}

	inline void sendrecv(void * sendbuf, int sendcount,
						 MPI_Datatype sendtype, int dest, int sendtag,
						 void * recvbuf, int recvcount, 
						 MPI_Datatype recvtype, int source, int recvtag,
						 MPI_Comm comm, MPI_Status * status){

	}

	inline void reduce(void * sendbuf, void * recvbuf, int count, 
					   MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm){

	}

	inline void barrier(MPI_Comm comm){

	}

}


#endif

#endif