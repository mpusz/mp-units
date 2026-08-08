FIGURES=$(patsubst %.dot,%.pdf,$(wildcard assets/*.dot))
EXAMPLES=$(patsubst %.tex,%.pdf,$(wildcard assets/*.tex))

default: full

clean:
	rm -f *.aux std.pdf std-gram.ext *.idx *.ilg *.ind *.log *.lot *.lof *.tmp *.out *.glo *.gls *.fls *.fdb* *.toc *.xtr

refresh:
	pdflatex std

full:
	latexmk -pdf std

quiet:
	latexmk -pdf std -e '$$max_repeat = 1;' -silent || ( rm std.pdf; latexmk -pdf std -e '$$max_repeat = 4;' )

%.pdf: %.dot
	dot -o $@ -Tpdf $< -Nfontname=NewComputerModernSans10

clean-figures:
	rm -f $(FIGURES)

figures: $(FIGURES)

%.pdf: %.tex
	lualatex -output-directory assets $<

clean-examples:
	rm -f $(EXAMPLES)

examples: $(EXAMPLES)
