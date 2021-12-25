#
# Author - VenkataDurgaPrasad.B
#

# Beautify output
# ---------------------------------------------------------------------------
#
# Normally the whole command before executing it.
#
# A simple variant is to prefix commands with $(Q) - that's useful
# for commands that shall be hidden in non-verbose mode.
#
#   $(Q)$(RM) $@
#
# To put more focus on warnings, be less verbose as default
# Use 'make V=1' to see the full commands

ifeq ("$(origin V)", "command line")
	BUILD_VERBOSE = $(V)
endif
ifndef BUILD_VERBOSE
	BUILD_VERBOSE = 0
endif

ifeq ($(BUILD_VERBOSE),1)
	Q =
else
	Q = @
endif

include Makefile.in

# Find all the C and C++ files we want to compile
# Note the single quotes around the * expressions. Make will incorrectly expand these otherwise.
SRCS := $(shell find $(SRC_DIRS) -name '*.cpp' -or -name '*.c' -or -name '*.s')

# String substitution for every C/C++ file.
# As an example, hello.cpp turns into ./build/hello.o
OBJS := $(SRCS:%.c=$(BUILD_DIR)/%.o)

# Another way of defining OBJS
#OBJS	:= $(patsubst $(SRC_DIR)/%,$(BUILD_DIR)/%,$(SRCS:.c=.o))

# String substitution (suffix version without %).
# As an example, ./build/hello.o turns into ./build/hello.d
DEPS := $(OBJS:.o=.d)

# Every folder in ./src will need to be passed to GCC so that it can find header files
INC_DIRS := $(shell find $(SRC_DIR) -type d)
# Add a prefix to INC_DIRS. So moduleA would become -ImoduleA. GCC understands this -I flag
INC_FLAGS := $(addprefix -I,$(INC_DIR))

# Default Make
all:	$(TARGET)

#Make the Directories
directories:
	$(Q) $(MK_DIR) -p $(TARGET_DIR)
	$(Q) $(MK_DIR) -p $(BUILD_DIR)

# Remake
remake:	cleaner	all

# Full clean, objs and target binaries
cleaner:	clean
	$(Q) $(RM_DIR) $(TARGET_DIR)
	@echo "	[RM]	$(TARGET_DIR)"


$(TARGET):	directories	$(OBJS)
	$(Q) $(CC) $(OBJS) $(O_FLAGS) $(TARGET_DIR)/$@ $(LD_FLAGS)
	@echo "	[LD]	$(TARGET)"


# Build step for C source
$(BUILD_DIR)/%.o: %.c
	$(Q) $(MK_DIR) -p $(dir $@)
	$(Q) $(CC) $(INC_FLAGS) $(C_FLAGS) $< $(O_FLAGS) $@
	@echo "	[CC]	$<"

clean:
	$(Q) $(RM_DIR) $(BUILD_DIR)
	@echo "	[RM]	$(BUILD_DIR)"

# Make help rule to display available rules
help:
	@echo "	make		-	To make all target(s)"
	@echo "	make all	-	To make all target(s)"
	@echo "	make httpd	-	To make httpd binary"
	@echo "	make V=1	-	Use V=1 option to enable command verbose"


.PHONY:	all	remake	clean	cleaner	directories

# Include the .d makefiles. The - at the front suppresses the errors of missing
# Makefiles. Initially, all the .d files will be missing, and we don't want those
# errors to show up.
-include $(DEPS)
