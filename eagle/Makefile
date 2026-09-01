# Makefile for TF0060DCA - CAM (Gerbers), schematic PDF, and JLCPCB assembly files.
# Gerbers + PDF use the terriblefire78 Eagle Docker images (headless Eagle 9.6.2).
# BOM/CPL use plain python3 (extract_bom.py / extract_cpl.py).

BOARDPREFIX := TF0060DCA
BOARD       := $(BOARDPREFIX).brd
SCH_FILE    := $(BOARDPREFIX).sch
CAMFILE     := jlcpcb_4_layer_v9.cam
CHECK_POLYGONS_SCRIPT := check_polygons.py

GITVERSION  := $(shell git rev-parse --short HEAD)$(shell git diff --quiet HEAD || echo -dirty)
DATESTRING  := $(shell date +"%Y_%m_%d")
GERBERS     := $(BOARDPREFIX)_$(GITVERSION)_$(DATESTRING)
PDF_FILE    := $(BOARDPREFIX)_$(GITVERSION)_$(DATESTRING).pdf

EAGLE_DOCKER     := docker run --rm --platform linux/amd64 -v $(CURDIR):/work -w /work terriblefire78/eagle:v1 eagle
EAGLE_PDF_DOCKER := docker run --rm --platform linux/amd64 -v $(CURDIR):/work -w /work terriblefire78/eagle-pdf python /app/generate_schematic_pdf.py

.PHONY: all
all: gerbers pdf assembly

# ---- Board sanity checks (run before Gerber generation) ----
.PHONY: check
check: $(BOARD) $(CHECK_POLYGONS_SCRIPT)
	python3 $(CHECK_POLYGONS_SCRIPT) $(BOARD)

# ---- Gerbers (CAM) ----
.PHONY: gerbers camfiles
gerbers: $(GERBERS).zip
camfiles: $(GERBERS)

$(GERBERS): $(BOARD) $(CAMFILE) $(CHECK_POLYGONS_SCRIPT)
	@echo "Checking $(BOARD) for zero-width pour polygons..."
	python3 $(CHECK_POLYGONS_SCRIPT) $(BOARD)
	@echo "Generating Gerbers for $(BOARD) using $(CAMFILE)..."
	mkdir -p $(GERBERS)
	$(EAGLE_DOCKER) -X -dCAMJOB -j$(CAMFILE) -o$(GERBERS) $(BOARD)
	@echo "Gerbers generated in $(GERBERS)/"

$(GERBERS).zip: $(GERBERS)
	@echo "Creating Gerber archive..."
	zip -r $@ $(GERBERS)   # recursive: captures the drill file in its nested subfolder
	@echo "Gerber archive created: $@"

# ---- Schematic PDF ----
.PHONY: pdf
pdf: $(PDF_FILE)

$(PDF_FILE): $(SCH_FILE)
	@echo "Generating PDF schematic from $(SCH_FILE)..."
	$(EAGLE_PDF_DOCKER) $(SCH_FILE) $(PDF_FILE)
	@echo "PDF schematic generated: $(PDF_FILE)"

# ---- Assembly (JLCPCB BOM + CPL) ----
.PHONY: bom cpl assembly
assembly: bom cpl
	@echo "Generated BOM and CPL files for assembly"

bom: $(BOARDPREFIX)_bom.csv
cpl: $(BOARDPREFIX)_cpl.csv

$(BOARDPREFIX)_bom.csv: $(SCH_FILE) extract_bom.py
	@echo "Generating BOM from $(SCH_FILE)..."
	python3 extract_bom.py $(SCH_FILE) $@

$(BOARDPREFIX)_cpl.csv: $(BOARD) extract_cpl.py
	@echo "Generating CPL from $(BOARD)..."
	python3 extract_cpl.py $(BOARD) $@

# ---- Clean ----
.PHONY: clean clean-gerbers clean-pdf clean-assembly distclean
clean: clean-gerbers clean-pdf clean-assembly

distclean:
	rm -rf *.b#* *.s#* *.l#*

clean-gerbers:
	rm -rf $(BOARDPREFIX)_*_*/ ; rm -f $(BOARDPREFIX)_*_*.zip
	@echo "Cleaned Gerbers"

clean-pdf:
	rm -f $(BOARDPREFIX)_*_*.pdf
	@echo "Cleaned PDF"

clean-assembly:
	rm -f $(BOARDPREFIX)_bom.csv $(BOARDPREFIX)_cpl.csv
	@echo "Cleaned assembly files"

.PHONY: help
help:
	@echo "TF0060DCA Makefile targets:"
	@echo "  check     - check board for zero-width pour polygons"
	@echo "  gerbers   - CAM -> Gerber ZIP (Docker)"
	@echo "  camfiles  - Gerbers only, no ZIP (Docker)"
	@echo "  pdf       - schematic PDF (Docker)"
	@echo "  bom       - JLCPCB BOM CSV from .sch"
	@echo "  cpl       - JLCPCB CPL CSV from .brd"
	@echo "  assembly  - bom + cpl"
	@echo "  all       - gerbers + pdf + assembly"
	@echo "  clean     - remove generated outputs"
	@echo "  distclean - remove EAGLE backup files (*.b#*, *.s#*, *.l#*)"
