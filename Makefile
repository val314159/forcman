
options.c: options.docopt
	cd docopt.c ; python docopt_c.py -o ../options.c ../options.docopt

forcman: main.cpp options.c
	$(CXX) -o forcman main.cpp

clean:
	rm -fr forcman *.o *.d

realclean: clean
	rm -fr options.c *~ \#* .\#*
