# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.17

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = "/Users/jonny/Library/Application Support/JetBrains/Toolbox/apps/CLion/ch-0/202.7319.72/CLion.app/Contents/bin/cmake/mac/bin/cmake"

# The command to remove a file.
RM = "/Users/jonny/Library/Application Support/JetBrains/Toolbox/apps/CLion/ch-0/202.7319.72/CLion.app/Contents/bin/cmake/mac/bin/cmake" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/jonny/CLionProjects/TCPIP

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/jonny/CLionProjects/TCPIP/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/TCPIP.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/TCPIP.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/TCPIP.dir/flags.make

CMakeFiles/TCPIP.dir/main.cpp.o: CMakeFiles/TCPIP.dir/flags.make
CMakeFiles/TCPIP.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/jonny/CLionProjects/TCPIP/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/TCPIP.dir/main.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/TCPIP.dir/main.cpp.o -c /Users/jonny/CLionProjects/TCPIP/main.cpp

CMakeFiles/TCPIP.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/TCPIP.dir/main.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/jonny/CLionProjects/TCPIP/main.cpp > CMakeFiles/TCPIP.dir/main.cpp.i

CMakeFiles/TCPIP.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/TCPIP.dir/main.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/jonny/CLionProjects/TCPIP/main.cpp -o CMakeFiles/TCPIP.dir/main.cpp.s

# Object files for target TCPIP
TCPIP_OBJECTS = \
"CMakeFiles/TCPIP.dir/main.cpp.o"

# External object files for target TCPIP
TCPIP_EXTERNAL_OBJECTS =

TCPIP: CMakeFiles/TCPIP.dir/main.cpp.o
TCPIP: CMakeFiles/TCPIP.dir/build.make
TCPIP: CMakeFiles/TCPIP.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/jonny/CLionProjects/TCPIP/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable TCPIP"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/TCPIP.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/TCPIP.dir/build: TCPIP

.PHONY : CMakeFiles/TCPIP.dir/build

CMakeFiles/TCPIP.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/TCPIP.dir/cmake_clean.cmake
.PHONY : CMakeFiles/TCPIP.dir/clean

CMakeFiles/TCPIP.dir/depend:
	cd /Users/jonny/CLionProjects/TCPIP/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/jonny/CLionProjects/TCPIP /Users/jonny/CLionProjects/TCPIP /Users/jonny/CLionProjects/TCPIP/cmake-build-debug /Users/jonny/CLionProjects/TCPIP/cmake-build-debug /Users/jonny/CLionProjects/TCPIP/cmake-build-debug/CMakeFiles/TCPIP.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/TCPIP.dir/depend

