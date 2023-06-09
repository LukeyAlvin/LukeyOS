# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/alvin/workspace/LukeyOS/LukeyOS/start

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/alvin/workspace/LukeyOS/LukeyOS/start/build

# Include any dependencies generated for this target.
include source/boot/CMakeFiles/boot.dir/depend.make

# Include the progress variables for this target.
include source/boot/CMakeFiles/boot.dir/progress.make

# Include the compile flags for this target's objects.
include source/boot/CMakeFiles/boot.dir/flags.make

source/boot/CMakeFiles/boot.dir/start.S.o: source/boot/CMakeFiles/boot.dir/flags.make
source/boot/CMakeFiles/boot.dir/start.S.o: ../source/boot/start.S
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/alvin/workspace/LukeyOS/LukeyOS/start/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building ASM object source/boot/CMakeFiles/boot.dir/start.S.o"
	cd /home/alvin/workspace/LukeyOS/LukeyOS/start/build/source/boot && /usr/bin/gcc $(ASM_DEFINES) $(ASM_INCLUDES) $(ASM_FLAGS) -o CMakeFiles/boot.dir/start.S.o -c /home/alvin/workspace/LukeyOS/LukeyOS/start/source/boot/start.S

source/boot/CMakeFiles/boot.dir/boot.c.o: source/boot/CMakeFiles/boot.dir/flags.make
source/boot/CMakeFiles/boot.dir/boot.c.o: ../source/boot/boot.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/alvin/workspace/LukeyOS/LukeyOS/start/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object source/boot/CMakeFiles/boot.dir/boot.c.o"
	cd /home/alvin/workspace/LukeyOS/LukeyOS/start/build/source/boot && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/boot.dir/boot.c.o   -c /home/alvin/workspace/LukeyOS/LukeyOS/start/source/boot/boot.c

source/boot/CMakeFiles/boot.dir/boot.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/boot.dir/boot.c.i"
	cd /home/alvin/workspace/LukeyOS/LukeyOS/start/build/source/boot && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/alvin/workspace/LukeyOS/LukeyOS/start/source/boot/boot.c > CMakeFiles/boot.dir/boot.c.i

source/boot/CMakeFiles/boot.dir/boot.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/boot.dir/boot.c.s"
	cd /home/alvin/workspace/LukeyOS/LukeyOS/start/build/source/boot && /usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/alvin/workspace/LukeyOS/LukeyOS/start/source/boot/boot.c -o CMakeFiles/boot.dir/boot.c.s

# Object files for target boot
boot_OBJECTS = \
"CMakeFiles/boot.dir/start.S.o" \
"CMakeFiles/boot.dir/boot.c.o"

# External object files for target boot
boot_EXTERNAL_OBJECTS =

source/boot/boot: source/boot/CMakeFiles/boot.dir/start.S.o
source/boot/boot: source/boot/CMakeFiles/boot.dir/boot.c.o
source/boot/boot: source/boot/CMakeFiles/boot.dir/build.make
source/boot/boot: source/boot/CMakeFiles/boot.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/alvin/workspace/LukeyOS/LukeyOS/start/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking C executable boot"
	cd /home/alvin/workspace/LukeyOS/LukeyOS/start/build/source/boot && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/boot.dir/link.txt --verbose=$(VERBOSE)
	cd /home/alvin/workspace/LukeyOS/LukeyOS/start/build/source/boot && objcopy -O binary boot.elf /home/alvin/workspace/LukeyOS/LukeyOS/start/../image/boot.bin
	cd /home/alvin/workspace/LukeyOS/LukeyOS/start/build/source/boot && objdump -x -d -S -m i8086 /home/alvin/workspace/LukeyOS/LukeyOS/start/build/source/boot/boot.elf > boot_dis.txt
	cd /home/alvin/workspace/LukeyOS/LukeyOS/start/build/source/boot && readelf -a /home/alvin/workspace/LukeyOS/LukeyOS/start/build/source/boot/boot.elf > boot_elf.txt

# Rule to build all files generated by this target.
source/boot/CMakeFiles/boot.dir/build: source/boot/boot

.PHONY : source/boot/CMakeFiles/boot.dir/build

source/boot/CMakeFiles/boot.dir/clean:
	cd /home/alvin/workspace/LukeyOS/LukeyOS/start/build/source/boot && $(CMAKE_COMMAND) -P CMakeFiles/boot.dir/cmake_clean.cmake
.PHONY : source/boot/CMakeFiles/boot.dir/clean

source/boot/CMakeFiles/boot.dir/depend:
	cd /home/alvin/workspace/LukeyOS/LukeyOS/start/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/alvin/workspace/LukeyOS/LukeyOS/start /home/alvin/workspace/LukeyOS/LukeyOS/start/source/boot /home/alvin/workspace/LukeyOS/LukeyOS/start/build /home/alvin/workspace/LukeyOS/LukeyOS/start/build/source/boot /home/alvin/workspace/LukeyOS/LukeyOS/start/build/source/boot/CMakeFiles/boot.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : source/boot/CMakeFiles/boot.dir/depend

