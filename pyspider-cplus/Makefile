# root directory
ROOT_DIR = $(shell pwd)

SUBDIRS = third-libs/hiredis message_queue test

# src file
SRC =  

MODULE_PATH = modules

LDFLAGS = -L$(ROOT_DIR)/$(MODULE_PATH)/ -lhiredis
LDFLAGS += -lmsgpackc -lpthread
INCLUDE_DIR = -I$(ROOT_DIR)/message_queue/ -I$(ROOT_DIR)/third-libs/hiredis/ -I$(ROOT_DIR)/third-libs/msgpack-c/include
CXXFLAGS = $(INCLUDE_DIR) -rdynamic -Wall -fPIC  -g -Werror
MCXXFLAGS = $(CXXFLAGS)
CXX = g++

# To make it visible
export CXX CCPP ROOT_DIR CXXFLAGS LIBS INCLUDE_DIR LDFLAGS

all: subdir mdl 

#producer: $(SRC:.cpp=.o) 
#	$(CXX) $(MCXXFLAGS) -o $@ $(SRC:.cpp=.o) $(LDFLAGS) $(INCLUDE_DIR)
	
subdir:
	@for dir in $(SUBDIRS) ; \
	   do $(MAKE) -C $$dir || exit 1; \
	done
	
mdl:
	mkdir -p $(MODULE_PATH)
	cp -a message_queue/*.so $(MODULE_PATH)
	cp -a third-libs/hiredis/*.so $(MODULE_PATH)

.PHONY : clean
clean:
	@for dir in $(SUBDIRS) ; do $(MAKE) -C $$dir clean; done
	rm -f test *.o *~ *.log .depend val.* *.expand
	rm -rf $(MODULE_PATH)
	
%.o: %.cpp
	$(CXX) $(MCXXFLAGS) -c -o $@ $< 


.depend: $(SRC)
	$(CXX) -M $(MCXXFLAGS) $(SRC) > $@

sinclude .depend