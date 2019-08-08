
SRCDIR := src
BUILDDIR := .build
INTDIR := $(BUILDDIR)/obj
OUTPUT := libOP2Utility.a

CXXFLAGS := -std=c++17 -g -Wall -Wno-unknown-pragmas

DEPFLAGS = -MT $@ -MMD -MP -MF $(INTDIR)/$*.Td

COMPILE.cpp = $(CXX) $(DEPFLAGS) $(CXXFLAGS) $(TARGET_ARCH) -c
POSTCOMPILE = @mv -f $(INTDIR)/$*.Td $(INTDIR)/$*.d && touch $@

SRCS := $(shell find $(SRCDIR) -name '*.cpp')
OBJS := $(patsubst $(SRCDIR)/%.cpp,$(INTDIR)/%.o,$(SRCS))
FOLDERS := $(sort $(dir $(SRCS)))

all: $(OUTPUT)

$(OUTPUT): $(OBJS)
	@mkdir -p ${@D}
	ar rcs $@ $^

$(OBJS): $(INTDIR)/%.o : $(SRCDIR)/%.cpp $(INTDIR)/%.d | build-folder
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

.PHONY: build-folder
build-folder:
	@mkdir -p $(patsubst $(SRCDIR)/%,$(INTDIR)/%, $(FOLDERS))

$(INTDIR)/%.d: ;
.PRECIOUS: $(INTDIR)/%.d

include $(wildcard $(patsubst $(SRCDIR)/%.cpp,$(INTDIR)/%.d,$(SRCS)))

.PHONY: clean clean-all
clean:
	-rm -fr $(INTDIR)
clean-all: clean
	-rm -fr $(BUILDDIR)
	-rm -f $(OUTPUT)


GTESTSRCDIR := /usr/src/googletest/
GTESTINCDIR := /usr/src/googletest/googletest/include/
GTESTBUILDDIR := $(BUILDDIR)/gtest/
GTESTLOCALLIBDIR := $(GTESTBUILDDIR)googlemock/
GTESTLIBDIR := /usr/lib/

.PHONY: gtest gtest-install gtest-clean
gtest:
	mkdir -p "$(GTESTBUILDDIR)"
	cd "$(GTESTBUILDDIR)" && cmake -DCMAKE_CXX_COMPILER="$(CXX)" -DCMAKE_C_COMPILER="$(CC)" -DCMAKE_CXX_FLAGS="-std=c++17" "$(GTESTSRCDIR)"
	make -C "$(GTESTBUILDDIR)"
gtest-install:
	cp $(GTESTBUILDDIR)googlemock/gtest/lib*.a "$(GTESTLIBDIR)"
	cp $(GTESTBUILDDIR)googlemock/lib*.a "$(GTESTLIBDIR)"
gtest-clean:
	rm -rf "$(GTESTBUILDDIR)"


TESTDIR := test
TESTINTDIR := $(BUILDDIR)/testObj
TESTSRCS := $(shell find $(TESTDIR) -name '*.cpp')
TESTOBJS := $(patsubst $(TESTDIR)/%.cpp,$(TESTINTDIR)/%.o,$(TESTSRCS))
TESTFOLDERS := $(sort $(dir $(TESTSRCS)))
TESTCPPFLAGS := -I$(SRCDIR) -I$(GTESTINCDIR)
TESTLDFLAGS := -L./ -L$(GTESTLOCALLIBDIR) -L$(GTESTLOCALLIBDIR)/gtest/
TESTLIBS := -lOP2Utility -lgtest -lgtest_main -lpthread -lstdc++fs
TESTOUTPUT := $(BUILDDIR)/testBin/runTests

TESTDEPFLAGS = -MT $@ -MMD -MP -MF $(TESTINTDIR)/$*.Td
TESTCOMPILE.cpp = $(CXX) $(TESTCPPFLAGS) $(TESTDEPFLAGS) $(CXXFLAGS) $(TARGET_ARCH) -c
TESTPOSTCOMPILE = @mv -f $(TESTINTDIR)/$*.Td $(TESTINTDIR)/$*.d && touch $@

.PHONY: check
check: $(TESTOUTPUT)
	cd test && ../$(TESTOUTPUT)

$(TESTOUTPUT): $(TESTOBJS) $(OUTPUT)
	@mkdir -p ${@D}
	$(CXX) $(TESTOBJS) $(TESTLDFLAGS) $(TESTLIBS) -o $@

$(TESTOBJS): $(TESTINTDIR)/%.o : $(TESTDIR)/%.cpp $(TESTINTDIR)/%.d | test-build-folder
	$(TESTCOMPILE.cpp) $(OUTPUT_OPTION) -I$(SRCDIR) $<
	$(TESTPOSTCOMPILE)

.PHONY: test-build-folder
test-build-folder:
	@mkdir -p $(patsubst $(TESTDIR)/%,$(TESTINTDIR)/%, $(TESTFOLDERS))

$(TESTINTDIR)/%.d: ;
.PRECIOUS: $(TESTINTDIR)/%.d

include $(wildcard $(patsubst $(TESTDIR)/%.cpp,$(TESTINTDIR)/%.d,$(TESTSRCS)))
