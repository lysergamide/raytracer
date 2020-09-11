CC       := clang++
CFLAGS   := -Wall -Wextra -pedantic-errors -std=c++20 -O3
LFLAGS   := -lfmt
SRCDIR   := ./src
BINDIR   := ./bin
ONAME    := 'out'

default: $(SRCDIR)/main.cpp
	$(CC) $(CFLAGS) $(LFLAGS) $^ -o $(BINDIR)/$(ONAME)

run:
	./bin/out
