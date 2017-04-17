FOLDERS=data data/corpus data/lexicon data/corpus/brut data/corpus/corrige data/lexicon/brut data/lexicon/corrige data/gramsCount data/corpus/temp

all:
	mkdir -p $(FOLDERS)
	cd src && $(MAKE)

display:
	cd src && $(MAKE) display

clean:
	cd src && $(MAKE) clean
