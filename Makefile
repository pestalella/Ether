WRKDIR = `pwd`

CXXFLAGS = -Wall -W -O2
LIBS = -lglut -lGLU -lGL -lm
LDFLAGS =

SRC_DIR = src
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
BINARY_NAME = ether
DBG_OBJDIR = debug_build
REL_OBJDIR = release_build

vpath %.cpp $(SRC_DIR)

.PHONY: all release debug clean $(BINARY_NAME)

all: debug

DBG_CXXFLAGS += -Wall -W -g3
DBG_LDFLAGS += -g3
DBG_BINARY = $(DBG_OBJDIR)/$(BINARY_NAME)
DBG_OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(DBG_OBJDIR)/%.o,$(SRC_FILES))
debug: prepare $(DBG_BINARY)

REL_CXXFLAGS += -Wall -W -O2
REL_LDFLAGS =
REL_OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(REL_OBJDIR)/%.o,$(SRC_FILES))
REL_BINARY = $(REL_OBJDIR)/lw
release: prepare $(REL_BINARY)

$(DBG_OBJDIR)/%.o: %.cpp | $(DBG_OBJDIR)
	$(CXX) $(DBG_CXXFLAGS) -c -o $@ $<

$(REL_OBJDIR)/%.o: %.cpp | $(REL_OBJDIR)
	$(CXX) $(REL_CXXFLAGS) -c -o $@ $<

$(DBG_OBJDIR):
	mkdir -p $@

$(REL_OBJDIR):
	mkdir -p $@

prepare: $(DBG_OBJDIR) $(REL_OBJDIR)

$(DBG_BINARY): $(DBG_OBJS)
	$(CXX) -o $(DBG_BINARY) $(DBG_OBJS) $(DBG_LDFLAGS) $(LIBS)

$(REL_BINARY): $(REL_OBJS)
	$(CXX) -o $(REL_BINARY) $(REL_OBJS) $(REL_LDFLAGS) $(LIBS)

clean:
	rm -f $(OBJS) $(OUT_BINARY)
	rm -rf release_build debug_build
