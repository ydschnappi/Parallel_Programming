CC=c++
MPICC=mpicxx

CXX_FLAGS=-O3 -Wall -Wextra -mavx
MPICXX_FLAG=-DOMPI_SKIP_MPICXX

STD=c++14

OPENMP=-fopenmp
OMPDEFINE=BUILDOMP
MPIDEFINE=BUILDMPI
HYBRIDDEFINE=BUILDHYBRID
SERIALDEFINE=BUILDSERIAL

LIBS=-lm

utility.o: utility.cpp
	$(CC) -std=$(STD) $(CXX_FLAGS) $(LIBS) -c -o $@ $^

serial.o: serial.cpp
	$(CC) -std=$(STD) $(CXX_FLAGS) $(LIBS) -c -o $@ $^

ompkernel.o: ompge.cpp
	$(CC) -std=$(STD) $(CXX_FLAGS) $(LIBS) $(OPENMP) -c -o $@ $^

ompinterface.o: omp_interface.cpp
	$(CC) -std=$(STD) $(CXX_FLAGS) $(LIBS) $(OPENMP) -c -o $@ $^
mpikernel.o: mpige.cpp
	$(MPICC) -std=$(STD) $(CXX_FLAGS) $(MPICXX_FLAG) $(LIBS) -c -o $@ $^
mpiinterface.o: mpi_interface.cpp
	$(MPICC) -std=$(STD) $(CXX_FLAGS) $(MPICXX_FLAG) $(LIBS) -c -o $@ $^
hybridkernel.o: hybridge.cpp
	$(MPICC) -std=$(STD) $(CXX_FLAGS) $(MPICXX_FLAG) $(LIBS) $(OPENMP) -c -o $@ $^

hybridinterface.o: hybrid_interface.cpp
	$(MPICC) -std=$(STD) $(CXX_FLAGS) $(MPICXX_FLAG) $(LIBS) $(OPENMP) -c -o $@ $^

mpibuildinterface.o: interface.cpp
	$(MPICC) -std=$(STD) $(CXX_FLAGS) $(MPICXX_FLAG) $(LIBS) -D$(MPIDEFINE) -c -o $@ $^
ompbuildinterface.o: interface.cpp
	$(CC) -std=$(STD) $(CXX_FLAGS) $(LIBS) $(OPENMP) -D$(OMPDEFINE) -c -o $@ $^

hybridbuildinterface.o: interface.cpp
	$(MPICC) -std=$(STD) $(CXX_FLAGS) $(MPICXX_FLAG) $(LIBS) $(OPENMP) -D$(HYBRIDDEFINE) -c -o $@ $^

serialbuildinterface.o: interface.cpp
	$(CC) -std=$(STD) $(CXX_FLAGS) $(LIBS) -D$(SERIALDEFINE) -c -o $@ $^

ompge: utility.o serial.o ompkernel.o ompinterface.o ompbuildinterface.o main.cpp
	$(CC) -std=$(STD) $(CXX_FLAGS) $(LIBS) $(OPENMP) -D$(OMPDEFINE) -o $@ $^

mpige: utility.o serial.o mpikernel.o mpiinterface.o mpibuildinterface.o main.cpp
	$(MPICC) -std=$(STD) $(CXX_FLAGS) $(MPICXX_FLAG) $(LIBS) -D$(MPIDEFINE) -o $@ $^

hybridge: utility.o serial.o hybridkernel.o hybridinterface.o hybridbuildinterface.o main.cpp
	$(MPICC) -std=$(STD) $(CXX_FLAGS) $(MPICXX_FLAG) $(LIBS) $(OPENMP) -D$(HYBRIDDEFINE) -o $@ $^

serialge: utility.o serial.o serialbuildinterface.o main.cpp
	$(CC) -std=$(STD) $(CXX_FLAGS) $(LIBS) -D$(SERIALDEFINE) -o $@ $^

omptest: ompge
	export OMP_NUM_THREADS=4
	./ompge ./ge_data/size64x64

mpitest: mpige
	mpiexec --oversubscribe -n 4 ./mpige ./ge_data/size64x64


hybridtest: hybridge
	export OMP_NUM_THREADS=2
	mpiexec --oversubscribe -n 2 ./hybridge ./ge_data/size64x64

serialtest: serialge
	./serialge ./ge_data/size64x64

clean:
	rm -rf *.o ompge mpige hybridge serialge *.plist