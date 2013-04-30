CC=g++
CFLAGS= -fPIC
TARGET=concurrent.so
SRC=src/scp_impl.cpp
INC=include/scp_impl.h
LDIR=lib


$(TARGET): $(SRC) $(INC)
	@echo "		***** Compiling *****"
	$(CC) -shared -o concurrent.so $(SRC) $(INC) $(CFLAGS)
	mv $(TARGET) $(LDIR)
	@echo "..........................................SUCCESS!"

clean:
	@echo "		*****Cleaning up *****"
	rm -rf *.o $(LDIR)/$(TARGET)
	@echo ".............................................DONE!"
