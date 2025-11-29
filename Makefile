CXX = g++
CXXFLAGS = -std=c++17 -O3 -Wall -Wextra -pthread
SRCDIR = src
OBJDIR = obj
BINDIR = bin

CORE_SOURCES = $(wildcard $(SRCDIR)/data_structures/*.cpp)
ALGO_SOURCES = $(SRCDIR)/algorithms/kruskal.cpp $(SRCDIR)/algorithms/prim.cpp $(SRCDIR)/algorithms/kkt.cpp $(SRCDIR)/algorithms/prim_parallel.cpp
UTIL_SOURCES = $(wildcard $(SRCDIR)/utils/*.cpp)
GENERATOR_SOURCES = $(wildcard $(SRCDIR)/generators/*.cpp)

SOURCES = $(CORE_SOURCES) $(ALGO_SOURCES) $(UTIL_SOURCES) $(GENERATOR_SOURCES)
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

TEST_SOURCES = $(SRCDIR)/tests/basic_tests.cpp
TEST_OBJECTS = $(TEST_SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

SIMPLE_EXP_SOURCES = experiments/simple_runner.cpp
SIMPLE_EXP_OBJECTS = $(SIMPLE_EXP_SOURCES:experiments/%.cpp=$(OBJDIR)/%.o)
SIMPLE_EXP_TARGET = $(BINDIR)/simple_experiments

LARGE_EXP_SOURCES = experiments/large_scale_runner.cpp
LARGE_EXP_OBJECTS = $(LARGE_EXP_SOURCES:experiments/%.cpp=$(OBJDIR)/%.o)
LARGE_EXP_TARGET = $(BINDIR)/large_scale_experiments

COMPREHENSIVE_SOURCES = experiments/comprehensive_runner.cpp
COMPREHENSIVE_OBJECTS = $(COMPREHENSIVE_SOURCES:experiments/%.cpp=$(OBJDIR)/%.o)
COMPREHENSIVE_TARGET = $(BINDIR)/comprehensive_experiments

TEST_TARGET = $(BINDIR)/run_tests

.PHONY: all clean tests simple large comprehensive

all: tests simple large comprehensive

tests: $(TEST_TARGET)

simple: $(SIMPLE_EXP_TARGET)
large: $(LARGE_EXP_TARGET)  
comprehensive: $(COMPREHENSIVE_TARGET)

$(TEST_TARGET): $(OBJECTS) $(TEST_OBJECTS)
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(SIMPLE_EXP_TARGET): $(OBJECTS) $(SIMPLE_EXP_OBJECTS)
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(LARGE_EXP_TARGET): $(OBJECTS) $(LARGE_EXP_OBJECTS)
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(COMPREHENSIVE_TARGET): $(OBJECTS) $(COMPREHENSIVE_OBJECTS)
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/%.o: experiments/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

debug: CXXFLAGS += -g -DDEBUG
debug: $(TEST_TARGET) $(SIMPLE_EXP_TARGET) $(LARGE_EXP_TARGET) $(COMPREHENSIVE_TARGET)

clean:
	rm -rf $(OBJDIR) $(BINDIR) *.csv *.png

graph: $(OBJDIR)/data_structures/graph.o
kruskal: $(OBJDIR)/algorithms/kruskal.o
prim: $(OBJDIR)/algorithms/prim.o
kkt: $(OBJDIR)/algorithms/kkt.o
prim_parallel: $(OBJDIR)/algorithms/prim_parallel.o