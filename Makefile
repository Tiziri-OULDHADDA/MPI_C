
all:
	mpicc -Wall projet.c -o projet
run:
	mpirun -np ${np} projet
clean:
	rm -rf projet
