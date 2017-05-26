# src file
SRC = consumer.cpp 

# To make it visible
export CXX CCPP ROOT_DIR CXXFLAGS LIBS INCLUDE_DIR LDFLAGS

all: consumer 

consumer: $(SRC:.cpp=.o) 
	$(CXX) $(CXXFLAGS) -o $@ $(SRC:.cpp=.o) $(LDFLAGS) $(INCLUDE_DIR)
	
clean:
	rm -f consumer *.o *~ *.log .depend val.* *.expand

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $< 

.depend: $(SRC)
	$(CXX) -M $(CXXFLAGS) $(SRC) > $@

sinclude .depend