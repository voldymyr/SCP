CC=g++
CFLAGS= -fPIC
TARGET=concurrent.so
SRC=src/scp_impl.cpp
INC=include/scp_impl.h
RDIR=release


$(TARGET): $(SRC) $(INC)
	@echo "		***** Compiling *****"
	$(CC) -shared -o concurrent.so $(SRC) $(INC) $(CFLAGS)
	mv $(TARGET) $(RDIR)
	@echo "..........................................SUCCESS!"

clean:
	@echo "		*****Cleaning up *****"
	rm -rf *.o $(RDIR)/$(TARGET)
	@echo ".............................................DONE!"
