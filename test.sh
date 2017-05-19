# /bin/bash

score='eval/eval.sh'

#Apprentissage complet
#./main --buildDatabase -lexicons data/corpus/corrige/lexique data/corpus/corrige/vocabulaireSpecial -corpora data/corpus/brut/corpus_bigchat.txt -pairs data/corpus/brut/trainBrut.txt data/corpus/corrige/trainCorrige.txt data/corpus/brut/tableTradCustom data/corpus/corrige/tableTradCustom

#Apprentissage SansGrams
#./main --buildDatabase -lexicons data/corpus/corrige/lexique data/corpus/corrige/vocabulaireSpecial -corpora data/corpus/brut/gramsVoid -pairs data/corpus/brut/trainBrut.txt data/corpus/corrige/trainCorrige.txt data/corpus/brut/tableTradCustom data/corpus/corrige/tableTradCustom

#Correction du fichier d'évaluation
./main --correct data/lexicon/brut/trainBrut.txt.lexicon data/lexicon/corrige/trainBrut.txt.lexicon data/gramsCount/trainBrut.txt.grams data/translationTable/trainBrut.txt.table data/corpus/brut/testBrut.txt

#Correction de input.txt
#./main --correct data/lexicon/brut/trainBrut.txt.lexicon data/lexicon/corrige/trainBrut.txt.lexicon data/gramsCount/trainBrut.txt.grams data/translationTable/trainBrut.txt.table input.txt

#Affiche distance d'édition entre texte corrigé par humain et par le programme
$score data/corpus/corrige/testCorrige.txt data/output/testBrut.txt.corrected.layout

#Affiche distance d'édition entre texte corrigé par humain et celui non corrigé
#$score data/corpus/corrige/testCorrige.txt data/corpus/brut/testBrut.txt
