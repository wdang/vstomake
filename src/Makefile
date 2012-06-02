#### This Makefile has been machine generated from vstomake 0.1.0 ####

#### Builtin variables ####
CFLAGS   += $(CURRENT_CFLAGS)
CPPFLAGS += $(CURRENT_CPPFLAGS)
CXXFLAGS += --std=gnu++0x -I. $(CURRENT_CXXFLAGS)
LDFLAGS  += $(CURRENT_LDFLAGS)
ARFLAGS   = -rcs $(CURRENT_ARFLAGS)

#### Project variables ####
ProjectName       := vstomake
TargetDir          = $(OutDir)/$(ProjectName)
OutDir             = $(CurrentOutDir)
IntDir             = $(CurrentIntDir)

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
vcclcompilertool.cc\
vclinkertool.cc\
vcproject.cc\
vcproject_tests.cc\
vstomake_main.cc

../bin/Debug/obj/%.o : %.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $(IntDir)/$(notdir $@)


DebugWin32Objects := $(patsubst %.cc,../bin/Debug/obj/%.o,$(notdir $(DebugWin32Sources)))
DebugWin32Objects := $(patsubst %.cpp,../bin/Debug/obj/%.o,$(DebugWin32Objects))
DebugWin32Objects := $(patsubst %.cxx,../bin/Debug/obj/%.o,$(DebugWin32Objects))

precompiled.h.gch: precompiled.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c precompiled.h

DebugWin32Prebuild:
	@mkdir -p ../bin/Debug
	@mkdir -p ../bin/Debug/obj

DebugWin32Build:CurrentOutDir    :=../bin/Debug
DebugWin32Build:CurrentIntDir    :=../bin/Debug/obj
DebugWin32Build:CURRENT_CXXFLAGS :=-I../external
DebugWin32Build:CURRENT_CPPFLAGS :=-DWIN32 -D_DEBUG -D_CONSOLE
DebugWin32Build:precompiled.h.gch $(DebugWin32Objects)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS)  $(DebugWin32Objects) -o $(TargetDir)

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
vcproject_tests.cc\
vstomake_main.cc

../bin/Release/obj/%.o : %.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $(IntDir)/$(notdir $@)


ReleaseWin32Objects := $(patsubst %.cc,../bin/Release/obj/%.o,$(notdir $(ReleaseWin32Sources)))
ReleaseWin32Objects := $(patsubst %.cpp,../bin/Release/obj/%.o,$(ReleaseWin32Objects))
ReleaseWin32Objects := $(patsubst %.cxx,../bin/Release/obj/%.o,$(ReleaseWin32Objects))

precompiled.h.gch: precompiled.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c precompiled.h

ReleaseWin32Prebuild:
	@mkdir -p ../bin/Release
	@mkdir -p ../bin/Release/obj

ReleaseWin32Build:CurrentOutDir    :=../bin/Release
ReleaseWin32Build:CurrentIntDir    :=../bin/Release/obj
ReleaseWin32Build:CURRENT_CXXFLAGS :=-I../external
ReleaseWin32Build:CURRENT_CPPFLAGS :=-DWIN32 -DNDEBUG -D_CONSOLE
ReleaseWin32Build:precompiled.h.gch $(ReleaseWin32Objects)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS)  $(ReleaseWin32Objects) -o $(TargetDir)

ReleaseWin32:ReleaseWin32Prebuild ReleaseWin32Build



