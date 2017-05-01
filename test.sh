# /bin/bash

#./main --buildDatabase data/corpus/corrige/trainCorrige.txt data/corpus/brut/trainBrut.txt
#valgrind ./main --buildDatabase data/corpus/corrige/trainCorrige.txt data/corpus/brut/trainBrut.txt

#valgrind --track-origins=yes ./main --buildDatabase toy1 toy2
#./main --buildDatabase toy1 toy2

#valgrind ./main --correct data/lexicon/brut/toy2.lexicon data/lexicon/corrige/toy1.lexicon data/gramsCount/toy1.grams translationTable

#./main --correct data/lexicon/brut/toy2.lexicon data/lexicon/corrige/toy1.lexicon data/gramsCount/toy1.grams data/translationTable/toy1.table
#valgrind ./main --correct data/lexicon/brut/toy2.lexicon data/lexicon/corrige/toy1.lexicon data/gramsCount/toy1.grams data/translationTable/toy1.table

./main --correct data/lexicon/brut/trainBrut.txt.lexicon data/lexicon/corrige/trainCorrige.txt.lexicon data/gramsCount/trainCorrige.txt.grams data/translationTable/trainCorrige.txt.table data/corpus/brut/testBrut.txt
#valgrind ./main --correct data/lexicon/brut/trainBrut.txt.lexicon data/lexicon/corrige/trainCorrige.txt.lexicon data/gramsCount/trainCorrige.txt.grams data/translationTable/trainCorrige.txt.table data/corpus/brut/testBrut.txt
#valgrind --tool=callgrind ./main --correct data/lexicon/brut/trainBrut.txt.lexicon data/lexicon/corrige/trainCorrige.txt.lexicon data/gramsCount/trainCorrige.txt.grams data/translationTable/trainCorrige.txt.table data/corpus/brut/testBrut.txt
