# /bin/bash

#./main --buildDatabase data/corpus/corrige/corpus_ATH-TV_276_apres_correction.txt data/corpus/brut/corpus_ATH-TV_276_avant_correction.txt

#valgrind --track-origins=yes ./main --buildDatabase toy1 toy2
#./main --buildDatabase toy1 toy2

#valgrind ./main --correct data/lexicon/brut/toy2.lexicon data/lexicon/corrige/toy1.lexicon data/gramsCount/toy1.grams translationTable

#./main --correct data/lexicon/brut/toy2.lexicon data/lexicon/corrige/toy1.lexicon data/gramsCount/toy1.grams translationTable

./main --correct data/lexicon/brut/corpus_ATH-TV_276_avant_correction.txt.lexicon data/lexicon/corrige/corpus_ATH-TV_276_apres_correction.txt.lexicon data/gramsCount/corpus_ATH-TV_276_apres_correction.txt.grams data/translationTable/corpus_ATH-TV_276_apres_correction.txt.table
