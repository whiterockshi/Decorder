CXX = g++
CXXFLAGS = -Wall -Wextra -O2
ROOTFLAGS = $(shell root-config --cflags)
ROOTLIBS = $(shell root-config --libs)

SRCDIR = src
INCDIR = inc
OBJDIR = obj
BINDIR = bin

SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS))
EXE = $(BINDIR)/decorder

all: directories $(EXE)

$(EXE): $(OBJS)
	$(CXX) $(CXXFLAGS) $(ROOTFLAGS) $(ROOTLIBS) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(ROOTFLAGS) -I$(INCDIR) -c $< -o $@

directories:
	mkdir -p $(BINDIR)
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(BINDIR) $(OBJDIR)
