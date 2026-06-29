# ============================================================
#  Smart Route Optimizer Pro MAX — Makefile
# ============================================================

CXX      := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra
TARGET   := SmartRouteOptimizer
SRCDIR   := src

SRCS := $(SRCDIR)/main.cpp \
        $(SRCDIR)/Graph/Graph.cpp \
        $(SRCDIR)/Algorithms/BFS.cpp \
        $(SRCDIR)/Algorithms/DFS.cpp \
        $(SRCDIR)/Algorithms/Dijkstra.cpp \
        $(SRCDIR)/Algorithms/AStar.cpp \
        $(SRCDIR)/Algorithms/BellmanFord.cpp \
        $(SRCDIR)/Algorithms/FloydWarshall.cpp \
        $(SRCDIR)/Algorithms/Kruskal.cpp

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $^ -lm -o $@
	@echo "  ✓ Build successful → ./$(TARGET)"

debug: CXXFLAGS := -std=c++17 -g -fsanitize=address,undefined
debug: $(TARGET)

clean:
	rm -f $(TARGET)

run: all
	./$(TARGET)

.PHONY: all debug clean run
