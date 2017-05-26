# capture name
CAPT_NAME = libqueue

#library (.so)
CAPT_SO = $(CAPT_NAME).so

# sub directory
SUBDIRS = 

# src file
SRC = queue_test.cpp

# compilation
LDFLAGS = 

# To make it visible
export CXX CCPP ROOT_DIR CXXFLAGS LIBS INCLUDE_DIR

all: subdir $(CAPT_SO)

$(CAPT_SO): $(SRC:.cpp=.o)
	$(CXX) $(CXXFLAGS) -shared -Wl,-soname,$@ -o $@ $(SRC:.cpp=.o) $(LDFLAGS)

subdir:
	@for dir in $(SUBDIRS) ; \
	   do $(MAKE) -C $$dir || exit 1; \
	 done \

.PHONY : clean	
clean:
	@for dir in $(SUBDIRS) ; do $(MAKE) -C $$dir clean || exit 1; done
	rm -f *.o *.so *~ include/*~ .depend *.expand

.depend: $(SRC)
	$(CXX) -M $(CXXFLAGS) $(SRC) > $@

sinclude .depend