LC=pdflatex
BC=bibtex

TEXS=$(shell find . -name '*.tex')
BASE=$(TEXS:.tex=)
PDFS=$(TEXS:.tex=.pdf)
BBLS=$(TEXS:.tex=.bbl)
AUXS=$(TEXS:.tex=.aux)

all : $(AUXS) $(BBLS) $(PDFS)

.PHONY : clean

%.pdf : %.tex %.bbl
	$(LC) $*
	$(LC) $*

%.bbl : %.aux
	$(BC) $*

%.aux : %.tex %.bib
	$(LC) $*

clean :
	rm -f $(AUXS) $(BBLS) $(PDFS) $(BASE).blg $(BASE).log $(BASE).out
