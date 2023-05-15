BINARY			?= stm32-vga
BUILD_DIR		?= build
FIRMWARE		?= $(BUILD_DIR)/$(BINARY).bin
BUILD_TYPE 		?= Debug
DEVICE			?= STM32L476RG

PLATFORM 		= $(if $(OS),$(OS),$(shell uname -s))

.PHONY: all format build cmake clean flash st-flash monitor

# Verify platform
ifeq ($(PLATFORM),Windows_NT)
	@echo "Unsuported platform"
	exit 1
else
    ifeq ($(PLATFORM),Linux)
        BUILD_SYSTEM ?= Unix Makefiles
    else
        @echo "Unsuported platform"
        exit 1
    endif
endif

all: build

# Formats all user modified source files (add ones that are missing)
SRCS := $(shell find app -not -name 'font.c' -name '*.[ch]' -or -name '*.[ch]pp')
format: $(addsuffix .format, $(SRCS))

%.format: %
	clang-format -i $<

build: cmake
	$(MAKE) -C $(BUILD_DIR) -j8 --no-print-directory

cmake: $(BUILD_DIR)/Makefile

$(BUILD_DIR)/Makefile: cmake/CMakeLists.txt cmake/stm32l4.cmake cmake/toolchain-arm-none-eabi.cmake
	rm -f $(BUILD_DIR)/CMakeCache.txt
	cmake \
		-G "$(BUILD_SYSTEM)" \
		-B$(BUILD_DIR) \
		-DBINARY=$(BINARY) \
		-DCMAKE_BUILD_TYPE=$(BUILD_TYPE) \
		cmake/

clean:
	rm -rf $(BUILD_DIR)
 
flash: build | $(BUILD_DIR)/jlink-script
	JLinkExe -commanderScript $(BUILD_DIR)/jlink-script

$(BUILD_DIR)/jlink-script:
	touch $@
	@echo device $(DEVICE) > $@
	@echo si 1 >> $@
	@echo speed 4000 >> $@
	@echo loadfile $(FIRMWARE),0x08000000 >> $@
	@echo -e "r\ng\nqc" >> $@

st-flash: build
	st-flash --reset write $(FIRMWARE) 0x8000000

monitor:
	JLinkSWOViewer -device $(DEVICE) -cpufreq 80000000 -itmport 0
