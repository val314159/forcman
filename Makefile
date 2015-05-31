
forcman: main.cpp options.c
	$(CXX) -o forcman main.cpp

# for this rule to work, you have to install teh docopt.c package like so:
# git clone https://guthub.com/docopt/docopt.c
options.c: options.docopt
	cd docopt.c ; python docopt_c.py -o ../options.c ../options.docopt

clean:
	rm -fr forcman *.o *.d

realclean: clean
	rm -fr options.c *~ \#* .\#*
