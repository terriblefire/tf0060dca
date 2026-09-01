# Top-level Makefile for TF0060DCA.
# eagle/    - Gerbers, schematic PDF, BOM/CPL (terriblefire78/eagle:v1, eagle-pdf)
# firmware/ - CH32V203 firmware (terriblefire78/mrs:latest, MounRiver RISC-V GCC)

MRS_DOCKER := docker run --rm --platform linux/amd64 -v $(CURDIR):/work -w /work/firmware/CH32V203C8T6/obj terriblefire78/mrs:latest

.PHONY: all eagle firmware firmware-clean clean

all: eagle firmware

eagle:
	$(MAKE) -C eagle all

firmware:
	$(MRS_DOCKER) make -j4
	@echo "Firmware built: firmware/CH32V203C8T6/obj/CH32V203C8T6.hex"

firmware-clean:
	$(MRS_DOCKER) make clean

clean: firmware-clean
	$(MAKE) -C eagle clean
