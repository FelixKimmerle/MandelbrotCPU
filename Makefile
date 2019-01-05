#
# Compiler flags
#
CC     = g++
CFLAGS = -Wall -std=c++17
LIBS	= -lsfml-system -lsfml-window -lsfml-graphics -lpthread
#
# Project files
#
SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)
EXE  = MEXP

#
# Debug build settings
#
DBGDIR = debug
DBGEXE = $(DBGDIR)/$(EXE)
DBGOBJS = $(addprefix $(DBGDIR)/, $(OBJS))
DBGCFLAGS = -g -O0 -DDEBUG

#
# Release build settings
#
RELDIR = release
RELEXE = $(RELDIR)/$(EXE)
RELOBJS = $(addprefix $(RELDIR)/, $(OBJS))
RELCFLAGS = -O3 -Ofast -ffast-math -DNDEBUG -Werror -Wextra

.PHONY: all clean debug prep release remake run rund test

# Default build
all: prep release

#
# Debug rules
#
debug: $(DBGEXE)

$(DBGEXE): $(DBGOBJS)
	$(CC) $(CFLAGS) $(DBGCFLAGS) -o $(DBGEXE) $^ $(LIBS)

$(DBGDIR)/%.o: %.cpp
	$(CC) -c $(CFLAGS) $(DBGCFLAGS) -o $@ $<

#
# Release rules
#
release: $(RELEXE)

$(RELEXE): $(RELOBJS)
	$(CC) $(CFLAGS) $(RELCFLAGS) -o $(RELEXE) $^ $(LIBS)

$(RELDIR)/%.o: %.cpp
	$(CC) -c $(CFLAGS) $(RELCFLAGS) -o $@ $<

#
# Other rules
#
prep:
	@mkdir -p $(DBGDIR) $(RELDIR)

remake: clean all

clean:
	rm -f $(RELEXE) $(RELOBJS) $(DBGEXE) $(DBGOBJS) $(RELDIR)/*.o $(DBGDIR)/*.o
run:
	$(RELEXE)

rund:
	$(DBGEXE)

test:
	valgrind --leak-check=full -v $(DBGEXE) -d test.txt -o test2.txt
