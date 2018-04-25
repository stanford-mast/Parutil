###############################################################################
# Parutil
#   Multi-platform library of parallelized utility functions.
###############################################################################
# Authored by Samuel Grossman
# Department of Electrical Engineering, Stanford University
# Copyright (c) 2016-2017
###############################################################################
# Makefile
#   Build script for GNU-compatible Linux operating systems.
###############################################################################


# --------- PROJECT PROPERTIES ------------------------------------------------

PROJECT_NAME                = parutil
PLATFORM_NAME               = linux

SOURCE_DIR                  = source
INCLUDE_DIR                 = include/$(PROJECT_NAME)
ASSEMBLY_SOURCE_DIR         = $(OUTPUT_DIR)/asm/source
ASSEMBLY_INCLUDE_DIR        = $(OUTPUT_DIR)/asm/include

OUTPUT_BASE_DIR             = output
OUTPUT_DOCS_DIR             = $(OUTPUT_BASE_DIR)/docs
OUTPUT_DIR                  = $(OUTPUT_BASE_DIR)/$(PLATFORM_NAME)
OUTPUT_FILE                 = lib$(PROJECT_NAME).a
INTERMEDIATE_DIR            = $(OUTPUT_DIR)/build

C_SOURCE_SUFFIX             = .c
CXX_SOURCE_SUFFIX           = .cpp

MASM_SOURCE_SUFFIX          = .asm
MASM_HEADER_SUFFIX          = .inc
ASSEMBLY_SOURCE_SUFFIX      = .s
ASSEMBLY_HEADER_SUFFIX      = .S


# --------- TOOL SELECTION AND CONFIGURATION ----------------------------------

CC                          = gcc
CXX                         = g++
AS                          = as
AR                          = ar

CCFLAGS                     = -O3 -Wall -fPIC -std=c11 -masm=intel -march=core-avx-i -mno-vzeroupper -I$(INCLUDE_DIR) -D_GNU_SOURCE -DPARUTIL_LINUX
CXXFLAGS                    = -O3 -Wall -fPIC -std=c++0x -masm=intel -march=core-avx-i -mno-vzeroupper -I$(INCLUDE_DIR) -DPARUTIL_LINUX
ASFLAGS                     = --64 -mmnemonic=intel -msyntax=intel -mnaked-reg -I$(ASSEMBLY_INCLUDE_DIR) --defsym PARUTIL_LINUX=1
ARFLAGS                     = 


# --------- FILE ENUMERATION --------------------------------------------------

OBJECT_FILE_SUFFIX          = .o
DEP_FILE_SUFFIX             = .d

C_SOURCE_FILES              = $(filter-out $(wildcard $(SOURCE_DIR)/*-*$(C_SOURCE_SUFFIX)), $(wildcard $(SOURCE_DIR)/*$(C_SOURCE_SUFFIX))) $(wildcard $(SOURCE_DIR)/*-$(PLATFORM_NAME)$(C_SOURCE_SUFFIX))
CXX_SOURCE_FILES            = $(filter-out $(wildcard $(SOURCE_DIR)/*-*$(CXX_SOURCE_SUFFIX)), $(wildcard $(SOURCE_DIR)/*$(CXX_SOURCE_SUFFIX))) $(wildcard $(SOURCE_DIR)/*-$(PLATFORM_NAME)$(CXX_SOURCE_SUFFIX))
ALL_SOURCE_FILES            = $(C_SOURCE_FILES) $(CXX_SOURCE_FILES)

MASM_SOURCE_FILES           = $(wildcard $(SOURCE_DIR)/*$(MASM_SOURCE_SUFFIX))
MASM_HEADER_FILES           = $(wildcard $(INCLUDE_DIR)/*$(MASM_HEADER_SUFFIX))
ASSEMBLY_SOURCE_FILES       = $(patsubst $(SOURCE_DIR)/%$(MASM_SOURCE_SUFFIX), $(ASSEMBLY_SOURCE_DIR)/%$(ASSEMBLY_SOURCE_SUFFIX), $(MASM_SOURCE_FILES))
ASSEMBLY_HEADER_FILES       = $(patsubst $(INCLUDE_DIR)/%$(MASM_HEADER_SUFFIX), $(ASSEMBLY_INCLUDE_DIR)/%$(ASSEMBLY_HEADER_SUFFIX), $(MASM_HEADER_FILES))

OBJECT_FILES_FROM_SOURCE    = $(patsubst $(SOURCE_DIR)/%, $(INTERMEDIATE_DIR)/%$(OBJECT_FILE_SUFFIX), $(ALL_SOURCE_FILES))
OBJECT_FILES_FROM_ASSEMBLY  = $(patsubst $(ASSEMBLY_SOURCE_DIR)/%, $(INTERMEDIATE_DIR)/%$(OBJECT_FILE_SUFFIX), $(ASSEMBLY_SOURCE_FILES))
DEP_FILES_FROM_SOURCE       = $(patsubst $(SOURCE_DIR)/%, $(INTERMEDIATE_DIR)/%$(DEP_FILE_SUFFIX), $(ALL_SOURCE_FILES))


# --------- TOP-LEVEL RULE CONFIGURATION --------------------------------------

.PHONY: parutil docs clean help

.SECONDARY: $(ASSEMBLY_SOURCE_FILES) $(ASSEMBLY_HEADER_FILES)


# --------- TARGET DEFINITIONS ------------------------------------------------

parutil: $(OUTPUT_DIR)/$(OUTPUT_FILE)

docs: | $(OUTPUT_DOCS_DIR)
	@doxygen

help:
	@echo ''
	@echo 'Usage: make [target]'
	@echo ''
	@echo 'Targets:'
	@echo '    parutil'
	@echo '        Default target.'
	@echo '        Builds Parutil as a static library.'
	@echo '    docs'
	@echo '        Builds HTML and LaTeX documentation using Doxygen.'
	@echo '    clean'
	@echo '        Removes all output files, including binary and documentation.'
	@echo '    help'
	@echo '        Shows this information.'
	@echo ''


# --------- BUILDING AND CLEANING RULES ---------------------------------------

$(OUTPUT_DIR)/$(OUTPUT_FILE): $(OBJECT_FILES_FROM_SOURCE) $(OBJECT_FILES_FROM_ASSEMBLY)
	@echo '   AR        $@'
	@$(AR) $(ARFLAGS) rcs $@ $^
	@echo 'Build completed: $(PROJECT_NAME).'

clean:
	@echo '   RM        $(OUTPUT_BASE_DIR)'
	@rm -rf $(OUTPUT_BASE_DIR)
	@echo 'Clean completed: $(PROJECT_NAME).'


# --------- COMPILING AND ASSEMBLING RULES ------------------------------------

$(INTERMEDIATE_DIR):
	@mkdir -p $(INTERMEDIATE_DIR)

$(OUTPUT_DOCS_DIR):
	@mkdir -p $(OUTPUT_DOCS_DIR)

$(INTERMEDIATE_DIR)/%$(ASSEMBLY_SOURCE_SUFFIX)$(OBJECT_FILE_SUFFIX): $(ASSEMBLY_SOURCE_DIR)/%$(ASSEMBLY_SOURCE_SUFFIX) $(ASSEMBLY_HEADER_FILES) | $(INTERMEDIATE_DIR)
	@echo '   AS        $@'
	@$(AS) $(ASFLAGS) $< -o $@

$(INTERMEDIATE_DIR)/%$(C_SOURCE_SUFFIX)$(OBJECT_FILE_SUFFIX): $(SOURCE_DIR)/%$(C_SOURCE_SUFFIX) | $(INTERMEDIATE_DIR)
	@echo '   CC        $@'
	@$(CC) $(CCFLAGS) -MD -MP -c -o $@ -Wa,-adhlms=$(patsubst %$(OBJECT_FILE_SUFFIX),%$(ASSEMBLY_SOURCE_SUFFIX),$@) $<

$(INTERMEDIATE_DIR)/%$(CXX_SOURCE_SUFFIX)$(OBJECT_FILE_SUFFIX): $(SOURCE_DIR)/%$(CXX_SOURCE_SUFFIX) | $(INTERMEDIATE_DIR)
	@echo '   CXX       $@'
	@$(CXX) $(CXXFLAGS) -MD -MP -c -o $@ -Wa,-adhlms=$(patsubst %$(OBJECT_FILE_SUFFIX),%$(ASSEMBLY_SOURCE_SUFFIX),$@) $<

-include $(DEP_FILES_FROM_SOURCE)


# --------- ASSEMBLY SOURCE FILE TRANSFORMATION RULES -------------------------

$(ASSEMBLY_SOURCE_DIR):
	@mkdir -p $(ASSEMBLY_SOURCE_DIR)

$(ASSEMBLY_INCLUDE_DIR):
	@mkdir -p $(ASSEMBLY_INCLUDE_DIR)

$(ASSEMBLY_SOURCE_DIR)/%$(ASSEMBLY_SOURCE_SUFFIX): $(SOURCE_DIR)/%$(MASM_SOURCE_SUFFIX) | $(ASSEMBLY_SOURCE_DIR)
	@echo '   X-AS      $@'
	@cat $< | $(X-AS) > $@

$(ASSEMBLY_INCLUDE_DIR)/%$(ASSEMBLY_HEADER_SUFFIX): $(INCLUDE_DIR)/%$(MASM_HEADER_SUFFIX) | $(ASSEMBLY_INCLUDE_DIR)
	@echo '   X-AS      $@'
	@cat $< | $(X-AS) > $@

-include buildhelpers/x-$(AS).mk
