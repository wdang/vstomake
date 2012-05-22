#### This Makefile has been machine generated from vstomake 0.1.0 ####

#### Builtin variables ####
CFLAGS   += $(CURRENT_CFLAGS)
CPPFLAGS += $(CURRENT_CPPFLAGS)
CXXFLAGS += $(CURRENT_CXXFLAGS) -std=gnu++0x
LDFLAGS  +=
ARFLAGS  +=

#### Project variables ####
ProjectName       := vstomake
TargetDir          = $(OutDir)/$(ProjectName)

#### Build rules ####
default:
	@echo Available build rules:
	@echo Debug
	@echo Release
	@echo DebugWin32
	@echo ReleaseWin32
	@echo all
	@echo clean
all:  Debug Release
clean:
	@rm -vf $(DebugWin32Objects) $(ReleaseWin32Objects) 
Debug: DebugWin32
Release: ReleaseWin32

#### Configuration: Debug ####
####      Platform: Win32 ####
DebugWin32Sources :=\
makefile.cc\
precompiled.cc\
utility.cc\
vcclcompilertool.cc\
vclinkertool.cc\
vcproject.cc\
vstomake_main.cc

DebugWin32Objects := $(patsubst %.cc, ../bin/Debug/obj/%.o,$(DebugWin32Sources))
DebugWin32Objects := $(patsubst %.cpp,../bin/Debug/obj/%.o,$(DebugWin32Objects))
DebugWin32Objects := $(patsubst %.cxx,../bin/Debug/obj/%.o,$(DebugWin32Objects))

precompiled.h.gch: precompiled.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c precompiled.h

DebugWin32Prebuild:
	@mkdir -p ../bin/Debug
	@mkdir -p ../bin/Debug/obj

../bin/Debug/obj/%.o: %.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -include precompiled.h -o $@
../bin/Debug/obj/%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -include precompiled.h -o $@
../bin/Debug/obj/%.o: %.cxx
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -include precompiled.h -o $@

DebugWin32Build:OutDir           :=../bin/Debug
DebugWin32Build:IntDir           :=../bin/Debug/obj
DebugWin32Build:CURRENT_CXXFLAGS := -I. -I../external
DebugWin32Build:CURRENT_CPPFLAGS := -DWIN32 -D_DEBUG -D_CONSOLE -DVSTOMAKE_RUN_TESTS
DebugWin32Build:precompiled.h.gch $(DebugWin32Objects)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(DebugWin32Objects) -o $(TargetDir)

DebugWin32:DebugWin32Prebuild DebugWin32Build


#### Configuration: Release ####
####      Platform: Win32 ####
ReleaseWin32Sources :=\
makefile.cc\
precompiled.cc\
utility.cc\
vcclcompilertool.cc\
vclinkertool.cc\
vcproject.cc\
vstomake_main.cc

ReleaseWin32Objects := $(patsubst %.cc, ../bin/Release/obj/%.o,$(ReleaseWin32Sources))
ReleaseWin32Objects := $(patsubst %.cpp,../bin/Release/obj/%.o,$(ReleaseWin32Objects))
ReleaseWin32Objects := $(patsubst %.cxx,../bin/Release/obj/%.o,$(ReleaseWin32Objects))

precompiled.h.gch: precompiled.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c precompiled.h

ReleaseWin32Prebuild:
	@mkdir -p ../bin/Release
	@mkdir -p ../bin/Release/obj

../bin/Release/obj/%.o: %.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -include precompiled.h -o $@
../bin/Release/obj/%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -include precompiled.h -o $@
../bin/Release/obj/%.o: %.cxx
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -include precompiled.h -o $@

ReleaseWin32Build:OutDir           :=../bin/Release
ReleaseWin32Build:IntDir           :=../bin/Release/obj
ReleaseWin32Build:CURRENT_CXXFLAGS := -I. -I../external
ReleaseWin32Build:CURRENT_CPPFLAGS := -DWIN32 -DNDEBUG -D_CONSOLE
ReleaseWin32Build:precompiled.h.gch $(ReleaseWin32Objects)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(ReleaseWin32Objects) -o $(TargetDir)

ReleaseWin32:ReleaseWin32Prebuild ReleaseWin32Build



