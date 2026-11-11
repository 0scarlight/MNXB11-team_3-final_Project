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
main: main.o src/daily_means.o src/day_analysis.o src/lund_cold_days.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

smhi_cleaner: src/smhi_cleaner_main.o src/smhi_cleaner.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

src/smhi_cleaner_main.o src/smhi_cleaner.o: include/smhi_cleaner.h
main.o: include/daily_means.h include/day_analysis.h include/lund_cold_days.h
src/daily_means.o: include/daily_means.h
src/day_analysis.o: include/day_analysis.h
src/lund_cold_days.o: include/lund_cold_days.h

%.o: %.cxx
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -fv src/*.o main.o main smhi_cleaner
