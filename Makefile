
forcman: main.cpp options.c
	$(CXX) -o forcman main.cpp

options.c: options.docopt
	cd docopt.c ; python docopt_c.py -o ../options.c ../options.docopt

clean:
	rm -fr forcman *.o *.d

realclean: clean
	rm -fr options.c *~ \#* .\#*
