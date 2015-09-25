LDFLAGS=`pkg-config dataharvester --libs`
CPPFLAGS=`pkg-config dataharvester --cflags`

all: read write

read: read.o
	${CXX} ${LDFLAGS} -o read read.o
write: write.o
	${CXX} ${LDFLAGS} -o write write.o

clean:
	rm -f read.o read write.o write
