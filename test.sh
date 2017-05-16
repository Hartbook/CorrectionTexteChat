# /bin/bash

score='/home/franck/Desktop/data_chat/eval/score.sh'

#./main --buildDatabase -lexicons data/corpus/corrige/lexiqueBackup data/corpus/corrige/vocabulaireSpecial -corpora data/corpus/brut/toy -pairs data/corpus/brut/tableTradCustom data/corpus/corrige/tableTradCustom

#./main --buildDatabase -lexicons data/corpus/corrige/lexique data/corpus/corrige/vocabulaireSpecial -corpora data/corpus/brut/corpus_bigchat.txt -pairs data/corpus/brut/trainBrut.txt data/corpus/corrige/trainCorrige.txt data/corpus/brut/tableTradCustom data/corpus/corrige/tableTradCustom
#valgrind ./main --buildDatabase -lexicons data/corpus/corrige/lexiqueBackup data/corpus/corrige/vocabulaireSpecial -corpora data/corpus/brut/corpus_bigchat.txt -pairs data/corpus/brut/trainBrut.txt data/corpus/corrige/trainCorrige.txt

./main --correct data/lexicon/brut/trainBrut.txt.lexicon data/lexicon/corrige/trainBrut.txt.lexicon data/gramsCount/trainBrut.txt.grams data/translationTable/trainBrut.txt.table data/corpus/brut/testBrut.txt
#valgrind --leak-check=full --show-leak-kinds=all ./main --correct data/lexicon/brut/trainBrut.txt.lexicon data/lexicon/corrige/trainBrut.txt.lexicon data/gramsCount/trainBrut.txt.grams data/translationTable/trainBrut.txt.table data/corpus/brut/testBrut.txt

#valgrind --tool=callgrind ./main --correct data/lexicon/brut/trainBrut.txt.lexicon data/lexicon/corrige/trainBrut.txt.lexicon data/gramsCount/trainBrut.txt.grams data/translationTable/trainBrut.txt.table data/corpus/brut/testBrut.txt

$score data/corpus/corrige/testCorrige.txt data/output/testBrut.txt.corrected.layout
#$score data/corpus/corrige/testCorrige.txt data/corpus/brut/testBrut.txt
