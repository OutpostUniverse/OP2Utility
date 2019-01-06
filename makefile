
# Set compiler default to mingw
# Can still override from command line or environment variables
ifeq ($(origin CXX),default)
	CXX := clang++-6.0
endif

SRCDIR := src
BUILDDIR := .build
BINDIR := $(BUILDDIR)/bin
OBJDIR := $(BUILDDIR)/obj
DEPDIR := $(BUILDDIR)/deps
OUTPUT := libOP2Utility.a

CXXFLAGS := -std=c++14 -g -Wall -Wno-unknown-pragmas

DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td

COMPILE.cpp = $(CXX) $(DEPFLAGS) $(CXXFLAGS) $(TARGET_ARCH) -c
POSTCOMPILE = @mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d && touch $@

SRCS := $(shell find $(SRCDIR) -name '*.cpp')
OBJS := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS))
FOLDERS := $(sort $(dir $(SRCS)))

all: $(OUTPUT)

$(OUTPUT): $(OBJS)
	@mkdir -p ${@D}
	ar rcs $@ $^

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
	-rm -f $(OUTPUT)
clean-deps:
	-rm -fr $(DEPDIR)
clean-all:
	-rm -rf $(BUILDDIR)


# Either of these should be a complete combined package. Only build one.
GTESTSRCDIR := /usr/src/gtest/
GMOCKSRCDIR := /usr/src/gmock/
GTESTDIR := $(BUILDDIR)/gtest
GMOCKDIR := $(BUILDDIR)/gmock

.PHONY:gtest
gtest:
	mkdir -p $(GTESTDIR)
	cd $(GTESTDIR) && cmake -DCMAKE_CXX="$(CXX)" -DCMAKE_CXX_FLAGS="-std=c++17" $(GTESTSRCDIR)
	make -C $(GTESTDIR)

.PHONY:gmock
gmock:
	mkdir -p $(GMOCKDIR)
	cd $(GMOCKDIR) && cmake -DCMAKE_CXX="$(CXX)" -DCMAKE_CXX_FLAGS="-std=c++17" $(GMOCKSRCDIR)
	make -C $(GMOCKDIR)

# This is used to detect if a separate GMock library was built, in which case, use it
GMOCKLIB := $(wildcard $(GMOCKDIR)/libgmock.a)

TESTDIR := test
TESTOBJDIR := $(BUILDDIR)/testObj
TESTSRCS := $(shell find $(TESTDIR) -name '*.cpp')
TESTOBJS := $(patsubst $(TESTDIR)/%.cpp,$(TESTOBJDIR)/%.o,$(TESTSRCS))
TESTFOLDERS := $(sort $(dir $(TESTSRCS)))
TESTCPPFLAGS := -I$(SRCDIR) -I$(GMOCKSRCDIR)/gtest/include
TESTLDFLAGS := -L./ -L$(GMOCKDIR) -L$(GMOCKDIR)/gtest/ -L$(GTESTDIR)
TESTLIBS := -lOP2Utility -lgtest -lgtest_main -lpthread -lstdc++fs
TESTOUTPUT := $(BUILDDIR)/testBin/runTests
# Conditionally add GMock if we built it separately
# This is conditional to avoid errors in case the library is not found
ifneq ($(strip $(GMOCKLIB)),)
	TESTLIBS := -lgmock $(TESTLIBS)
endif

TESTDEPFLAGS = -MT $@ -MMD -MP -MF $(TESTOBJDIR)/$*.Td
TESTCOMPILE.cpp = $(CXX) $(TESTCPPFLAGS) $(TESTDEPFLAGS) $(CXXFLAGS) $(TARGET_ARCH) -c
TESTPOSTCOMPILE = @mv -f $(TESTOBJDIR)/$*.Td $(TESTOBJDIR)/$*.d && touch $@

.PHONY:check
check: $(TESTOUTPUT)
	cd test && ../$(TESTOUTPUT)

$(TESTOUTPUT): $(TESTOBJS) $(OUTPUT)
	@mkdir -p ${@D}
	$(CXX) $(TESTOBJS) $(TESTLDFLAGS) $(TESTLIBS) -o $@

$(TESTOBJS): $(TESTOBJDIR)/%.o : $(TESTDIR)/%.cpp $(TESTOBJDIR)/%.d | test-build-folder
	$(TESTCOMPILE.cpp) $(OUTPUT_OPTION) -I$(SRCDIR) $<
	$(TESTPOSTCOMPILE)

.PHONY:test-build-folder
test-build-folder:
	@mkdir -p $(patsubst $(TESTDIR)/%,$(TESTOBJDIR)/%, $(TESTFOLDERS))

$(TESTOBJDIR)/%.d: ;
.PRECIOUS: $(TESTOBJDIR)/%.d

include $(wildcard $(patsubst $(TESTDIR)/%.cpp,$(TESTOBJDIR)/%.d,$(TESTSRCS)))
