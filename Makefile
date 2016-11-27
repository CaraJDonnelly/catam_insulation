insulated_box:
	g++ -O2 main.cc insulated_box.h insulated_box.cc -std=c++11

clean:
	rm -rf *.o *.out
