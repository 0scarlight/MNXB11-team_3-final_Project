CXX := g++
CXXWARNINGS := -Wall -Wextra -Werror
CXXOPT := -O3
CXXSTD := -std=c++17
INCLUDES := -I include
ROOTCFLAGS = $(shell root-config --cflags)
ROOTLIBS = $(shell root-config --libs)
CXXFLAGS := $(CXXWARNINGS) $(CXXSTD) $(CXXOPT) $(INCLUDES)
LDLIBS = $(ROOTLIBS) -lTreePlayer

.PHONY: all clean

all: main

main: CXXFLAGS += $(ROOTCFLAGS)
main: main.o src/daily_means.o src/day_analysis.o src/cold_day_count.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

CLEANER_DIR := scripts/Data Cleaning - C++

smhi_cleaner: scripts/Data\ Cleaning\ -\ C++/src/smhi_cleaner_main.cxx \
              scripts/Data\ Cleaning\ -\ C++/src/smhi_cleaner.cxx \
              scripts/Data\ Cleaning\ -\ C++/include/smhi_cleaner.h
	$(CXX) $(CXXFLAGS) -I "$(CLEANER_DIR)/include" $(LDFLAGS) -o $@ \
	    "$(CLEANER_DIR)/src/smhi_cleaner_main.cxx" "$(CLEANER_DIR)/src/smhi_cleaner.cxx"

main.o: include/daily_means.h include/day_analysis.h include/cold_day_count.h
src/daily_means.o: include/daily_means.h
src/day_analysis.o: include/day_analysis.h
src/cold_day_count.o: include/cold_day_count.h

%.o: %.cxx
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -fv src/*.o main.o main smhi_cleaner
