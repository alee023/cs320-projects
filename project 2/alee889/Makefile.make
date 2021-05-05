CC=g++
CXXFLAGS='-D_GLIBCXX_USE_CXX11_ABI=0'

cache-sim: cache-sim.cpp
	$(CC) -o cache-sim cache-sim.cpp

clean:
	rm -f cache-sim output.txt 