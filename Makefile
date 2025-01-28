WRKDIR = `pwd`

LIBS = -lglut -lGLU -lGL -lm
SRC_DIR = src
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
BINARY_NAME = ether
DBG_OBJDIR = debug_build
REL_OBJDIR = release_build

vpath %.cpp $(SRC_DIR)

.PHONY: all release debug clean prepare

all: debug

DBG_CXXFLAGS += -Wall -Werror -W -g3
DBG_LDFLAGS += -g3
DBG_BINARY = $(DBG_OBJDIR)/$(BINARY_NAME)
DBG_DEPS = $(patsubst $(SRC_DIR)/%.cpp,$(DBG_OBJDIR)/%.d,$(SRC_FILES))
DBG_OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(DBG_OBJDIR)/%.o,$(SRC_FILES))
debug: prepare $(DBG_BINARY)

REL_CXXFLAGS += -Wall -Werror -Wextra -O3
REL_LDFLAGS =
REL_OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(REL_OBJDIR)/%.o,$(SRC_FILES))
REL_BINARY = $(REL_OBJDIR)/$(BINARY_NAME)
release: prepare $(REL_BINARY)

$(DBG_OBJDIR)/%.o: %.cpp $(DBG_OBJDIR)/%.d $(DBG_OBJDIR)
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

#This is the rule for creating the dependency files
$(DBG_OBJDIR)/%.d: %.cpp | $(DBG_OBJDIR)
	$(CXX) $(DBG_CXXFLAGS) -MM -MT '$(patsubst $(SRC_DIR)/%.cpp,$(DBG_OBJDIR)/%.o,$<)' $< -MF $@
# src/%.d: src/%.cpp
#     $(CXX) $(CXXFLAGS) -MM -MT '$(patsubst src/%.cpp,obj/%.o,$<)' $< -MF $@


clean:
	rm -f $(OBJS) $(OUT_BINARY)
	rm -rf release_build debug_build
