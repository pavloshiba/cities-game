CC            = gcc
CXX           = g++
CXXFLAGS      = -std=c++11 -g -Wall
ifeq ($(OS),Windows_NT)
LDLIBS        = -lws2_32
endif

SOURCES=  cities.cpp gamebot.cpp bots.cpp BotManager.cpp pssocket.cpp  citiesapp.cpp main.cpp
OBJECTS=$(SOURCES:%.cpp=%.o)
TARGET        = cities-game

all:    $(TARGET)


$(TARGET):  $(OBJECTS) 
ifeq ($(OS),Windows_NT)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDLIBS)
else	
	$(CXX) $(CXXFLAGS) $^ -o $@
endif 
	
.cpp.o:
	$(CXX) -c $(CXXFLAGS) -o "$@" "$<"

.PHONY: clean	
clean:
	rm $(OBJECTS) $(TARGET) 
