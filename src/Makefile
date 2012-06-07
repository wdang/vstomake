
#### Build variables ####
CFLAGS += $(CURRENT_CFLAGS)
CPPFLAGS += $(CURRENT_CPPFLAGS)
CXXFLAGS += --std=gnu++0x $(CURRENT_CXXFLAGS) -I.
LDFLAGS  += $(CURRENT_LDFLAGS)
ARFLAGS   = $(CURRENT_ARFLAGS)
PROJECT_NAME      := vstomake
OUTDIR             = $(CURRENT_OUTDIR)
INTDIR             = $(CURRENT_INTDIR)
TARGET             = $(CURRENT_OUTDIR)/$(CURRENT_TARGET)


#### Build rules ####
default:
	@echo Available build rules:
	@echo Debug
	@echo Release
	@echo DebugWin32
	@echo ReleaseWin32
	@echo all
	@echo clean

.PHONY: all clean

all: Debug Release

clean: 
	@rm -vf $(DebugWin32_OBJS) $(ReleaseWin32_OBJS) 

Debug: DebugWin32

Release: ReleaseWin32

#### Configuration:  ####
####      Platform:  ####
DebugWin32_SRCS :=\
internal.cc\
make_file.cc\
msvc_object_model.cc\
precompiled.cc\
project_parser.cc\
utility.cc\
vcclcompilertool.cc\
vclinkertool.cc\
vcproj_parser.cc\
vcxproj_parser.cc\
vstomake_main.cc
DebugWin32Stats:
	@echo "Configuration: Debug|Win32"
	@echo "CPPFLAGS:     " $(CPPFLAGS)
	@echo "CFLAGS:       " $(CFLAGS)
	@echo "CXXFLAGS:     " $(CXXFLAGS)
	@printf "Intermediates: %s \nOutputs: %s \nTarget: %s \n\n" $(INTDIR) $(OUTDIR) $(TARGET)

DebugWin32_OBJS := $(patsubst %.cc,../bin/Debug/obj/%.o,$(notdir $(DebugWin32_SRCS)))
DebugWin32_OBJS := $(patsubst %.cpp,../bin/Debug/obj/%.o,$(DebugWin32_OBJS))
DebugWin32_OBJS := $(patsubst %.cxx,../bin/Debug/obj/%.o,$(DebugWin32_OBJS))


../bin/Debug/obj/%.o : %.cc
	@echo $<
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $(INTDIR)/$(notdir $@)

DebugWin32Prebuild:
	@mkdir -p ../bin/Debug/obj
	@mkdir -p ../bin/Debug

DebugWin32Build:CURRENT_OUTDIR    :=../bin/Debug
DebugWin32Build:CURRENT_INTDIR    :=../bin/Debug/obj
DebugWin32Build:CURRENT_CXXFLAGS  := -include precompiled.h -I../external
DebugWin32Build:CURRENT_CPPFLAGS  :=-DWIN32 -D_DEBUG -D_CONSOLE -DVSTOMAKE_RUN_TESTS
DebugWin32Build:CURRENT_TARGET    :=$(PROJECT_NAME)
DebugWin32Build:DebugWin32Stats $(DebugWin32_OBJS)
	$(CXX) $(LDFLAGS)  $(DebugWin32_OBJS) -o $(TARGET)


DebugWin32: DebugWin32Prebuild DebugWin32Build 
clean.DebugWin32:
	@rm -vf $(DebugWin32_OBJS)

#### Configuration:  ####
####      Platform:  ####
ReleaseWin32_SRCS :=\
internal.cc\
make_file.cc\
msvc_object_model.cc\
precompiled.cc\
project_parser.cc\
utility.cc\
vcclcompilertool.cc\
vclinkertool.cc\
vcproj_parser.cc\
vcxproj_parser.cc\
vstomake_main.cc
ReleaseWin32Stats:
	@echo "Configuration: Release|Win32"
	@echo "CPPFLAGS:     " $(CPPFLAGS)
	@echo "CFLAGS:       " $(CFLAGS)
	@echo "CXXFLAGS:     " $(CXXFLAGS)
	@printf "Intermediates: %s \nOutputs: %s \nTarget: %s \n\n" $(INTDIR) $(OUTDIR) $(TARGET)

ReleaseWin32_OBJS := $(patsubst %.cc,../bin/Release/obj/%.o,$(notdir $(ReleaseWin32_SRCS)))
ReleaseWin32_OBJS := $(patsubst %.cpp,../bin/Release/obj/%.o,$(ReleaseWin32_OBJS))
ReleaseWin32_OBJS := $(patsubst %.cxx,../bin/Release/obj/%.o,$(ReleaseWin32_OBJS))


../bin/Release/obj/%.o : %.cc
	@echo $<
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $(INTDIR)/$(notdir $@)

ReleaseWin32Prebuild:
	@mkdir -p ../bin/Release/obj
	@mkdir -p ../bin/Release

ReleaseWin32Build:CURRENT_OUTDIR    :=../bin/Release
ReleaseWin32Build:CURRENT_INTDIR    :=../bin/Release/obj
ReleaseWin32Build:CURRENT_CXXFLAGS  := -include precompiled.h -I../external
ReleaseWin32Build:CURRENT_CPPFLAGS  :=-DWIN32 -DNDEBUG -D_CONSOLE
ReleaseWin32Build:CURRENT_TARGET    :=$(PROJECT_NAME)
ReleaseWin32Build:ReleaseWin32Stats $(ReleaseWin32_OBJS)
	$(CXX) $(LDFLAGS)  $(ReleaseWin32_OBJS) -o $(TARGET)


ReleaseWin32: ReleaseWin32Prebuild ReleaseWin32Build 
clean.ReleaseWin32:
	@rm -vf $(ReleaseWin32_OBJS)

