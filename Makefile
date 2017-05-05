FOLDERS=data data/corpus data/lexicon data/corpus/brut data/corpus/corrige data/lexicon/brut data/lexicon/corrige data/gramsCount data/corpus/temp data/translationTable output

all:
	mkdir -p $(FOLDERS)
	cd src && $(MAKE)

doc:
	cd src && $(MAKE) doc

display:
	cd src && $(MAKE) display

clean:
	cd src && $(MAKE) clean
