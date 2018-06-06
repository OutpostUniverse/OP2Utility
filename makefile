
# Set compiler default to mingw
# Can still override from command line or environment variables
ifeq ($(origin CXX),default)
	CXX := i686-w64-mingw32-g++
endif

SRCDIR := .
BUILDDIR := build
BINDIR := $(BUILDDIR)/bin
OBJDIR := $(BUILDDIR)/obj
DEPDIR := $(BUILDDIR)/deps
EXE := $(BINDIR)/op2utility

CFLAGS := -std=c++14 -g -Wall -Wno-unknown-pragmas
LDFLAGS := -lstdc++ -lm

DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td

COMPILE.cpp = $(CXX) $(DEPFLAGS) $(CFLAGS) $(TARGET_ARCH) -c
POSTCOMPILE = @mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d && touch $@

SRCS := $(shell find $(SRCDIR) -name '*.cpp')
OBJS := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS))
FOLDERS := $(sort $(dir $(SRCS)))

all: $(EXE)

$(EXE): $(OBJS)
	@mkdir -p ${@D}
	$(CXX) $^ $(LDFLAGS) -o $@

$(OBJS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp $(DEPDIR)/%.d | build-folder
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

.PHONY:build-folder
build-folder:
	@mkdir -p $(patsubst $(SRCDIR)/%,$(OBJDIR)/%, $(FOLDERS))
	@mkdir -p $(patsubst $(SRCDIR)/%,$(DEPDIR)/%, $(FOLDERS))

$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d

include $(wildcard $(patsubst $(SRCDIR)/%.cpp,$(DEPDIR)/%.d,$(SRCS)))

.PHONY:clean, clean-deps, clean-all
clean:
	-rm -fr $(OBJDIR)
	-rm -fr $(DEPDIR)
	-rm -fr $(BINDIR)
clean-deps:
	-rm -fr $(DEPDIR)
clean-all:
	-rm -rf $(BUILDDIR)