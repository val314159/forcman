
forcman: main.cpp
	$(CXX) -o forcman main.cpp

clean:
	rm -fr forcman *~ \#* .\#* *.o *.d
