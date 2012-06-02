#### This Makefile has been machine generated from vstomake 0.1.0 ####

#### Builtin variables ####
CFLAGS   += $(CURRENT_CFLAGS)
CPPFLAGS += $(CURRENT_CPPFLAGS)
CXXFLAGS += --std=gnu++0x -IC:/dev/github/vstomake/src/testing/vcclcompilertool_check $(CURRENT_CXXFLAGS)
LDFLAGS  += $(CURRENT_LDFLAGS)
ARFLAGS   = -rcvs $(CURRENT_ARFLAGS)

#### Project variables ####
ProjectName       := vcclcompilertool_check
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
precompiled.cc\
vcclcompilertool_check.cpp

DebugWin32Objects := $(patsubst %.cc,%.o,$(DebugWin32Sources))
DebugWin32Objects := $(patsubst %.cpp,%.o,$(DebugWin32Objects))
DebugWin32Objects := $(patsubst %.cxx,%.o,$(DebugWin32Objects))

DebugWin32Prebuild:
	@mkdir -p Debug
	@mkdir -p Debug

%.o : %.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $<  -o $(IntDir)$(notdir $@)
%.o : %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $<  -o $(IntDir)$(notdir $@)
%.o : %.cxx
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $<  -o $(IntDir)$(notdir $@)

DebugWin32Build:OutDir           :=Debug
DebugWin32Build:IntDir           :=Debug
DebugWin32Build:CURRENT_CXXFLAGS :=
DebugWin32Build:CURRENT_CPPFLAGS :=-DWIN32 -D_DEBUG -D_WINDOWS
DebugWin32Build:Objects          :=$(patsubst %.o, $(IntDir)/%.o, $(notdir $(DebugWin32Objects)))
DebugWin32Build: $(DebugWin32Objects)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(Objects) -o $(TargetDir)

DebugWin32:DebugWin32Prebuild DebugWin32Build


#### Configuration: Release ####
####      Platform: Win32 ####
ReleaseWin32Sources :=\
precompiled.cc\
vcclcompilertool_check.cpp

ReleaseWin32Objects := $(patsubst %.cc,%.o,$(ReleaseWin32Sources))
ReleaseWin32Objects := $(patsubst %.cpp,%.o,$(ReleaseWin32Objects))
ReleaseWin32Objects := $(patsubst %.cxx,%.o,$(ReleaseWin32Objects))

ReleaseWin32Prebuild:
	@mkdir -p Release
	@mkdir -p Release

%.o : %.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $<  -o $(IntDir)$(notdir $@)
%.o : %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $<  -o $(IntDir)$(notdir $@)
%.o : %.cxx
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $<  -o $(IntDir)$(notdir $@)

ReleaseWin32Build:OutDir           :=Release
ReleaseWin32Build:IntDir           :=Release
ReleaseWin32Build:CURRENT_CXXFLAGS :=
ReleaseWin32Build:CURRENT_CPPFLAGS :=-DWIN32 -DNDEBUG -D_WINDOWS
ReleaseWin32Build:Objects          :=$(patsubst %.o, $(IntDir)/%.o, $(notdir $(ReleaseWin32Objects)))
ReleaseWin32Build: $(ReleaseWin32Objects)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(Objects) -o $(TargetDir)

ReleaseWin32:ReleaseWin32Prebuild ReleaseWin32Build



