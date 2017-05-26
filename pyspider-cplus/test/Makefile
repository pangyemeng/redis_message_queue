# sub directory
SUBDIRS = consumer producer

# sec file
SRC = 

# To make it visible
export CXX CCPP ROOT_DIR CXXFLAGS LIBS INCLUDE_DIR 

all: subdir

subdir:
	@for dir in $(SUBDIRS) ; \
	   do $(MAKE) -C $$dir || exit 1; \
	 done \
	 
clean:
	@for dir in $(SUBDIRS) ; do $(MAKE) -C $$dir clean || exit 1; done
	rm -f *.o *.so *.a *~ include/*~ .depend *.expand

