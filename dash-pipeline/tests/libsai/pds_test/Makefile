all: pds_test
pds_test: cfg.cpp /SAI/lib/libsai.so
	echo "building $@ ..."
	g++ \
	    -I /SAI/SAI/inc \
	    -I /SAI/SAI/experimental/ \
	    -o pds_test \
	    cfg.cpp \
	    -Wl,-rpath,/SAI/lib \
	    -L/SAI/lib/ \
	    -lsaipds \
	    -lpthread \
	    -g

clean:
	rm -rf pds_test
