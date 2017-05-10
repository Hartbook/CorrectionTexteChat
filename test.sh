# /bin/bash

score='/home/franck/Desktop/data_chat/eval/score.sh'

#./main --buildDatabase -lexicons data/corpus/corrige/lexique -corpora data/corpus/brut/toy -pairs data/corpus/brut/trainBrut.txt data/corpus/corrige/trainCorrige.txt

#./main --buildDatabase -lexicons data/corpus/corrige/lexique -corpora data/corpus/brut/corpus_bigchat.txt -pairs data/corpus/brut/trainBrut.txt data/corpus/corrige/trainCorrige.txt
#valgrind ./main --buildDatabase -lexicons data/corpus/corrige/lexique -corpora data/corpus/brut/corpus_bigchat.txt -pairs data/corpus/brut/trainBrut.txt data/corpus/corrige/trainCorrige.txt

./main --correct data/lexicon/brut/trainBrut.txt.lexicon data/lexicon/corrige/trainBrut.txt.lexicon data/gramsCount/trainBrut.txt.grams data/translationTable/trainBrut.txt.table data/corpus/brut/testBrut.txt
#valgrind ./main --correct data/lexicon/brut/trainBrut.txt.lexicon data/lexicon/corrige/trainBrut.txt.lexicon data/gramsCount/trainBrut.txt.grams data/translationTable/trainBrut.txt.table data/corpus/brut/testBrut.txt

$score data/corpus/corrige/testCorrige.txt data/output/testBrut.txt.corrected.layout
