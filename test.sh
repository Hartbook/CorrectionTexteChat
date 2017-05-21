# /bin/bash

score='eval/eval.sh'

#Apprentissage complet
#./main --buildDatabase -lexicons data/corpus/corrige/lexique data/corpus/corrige/vocabulaireSpecial -corpora data/corpus/brut/corpus_bigchat.txt -pairs data/corpus/brut/trainBrut.txt data/corpus/corrige/trainCorrige.txt data/corpus/brut/testBrut.txt data/corpus/corrige/testCorrige.txt data/corpus/brut/tableTradCustom data/corpus/corrige/tableTradCustom

#Apprentissage complet autre Grams
#./main --buildDatabase -lexicons data/corpus/corrige/lexique data/corpus/corrige/vocabulaireSpecial data/corpus/corrige/germinal.txt data/corpus/corrige/bizet.txt -corpora data/corpus/corrige/germinal.txt data/corpus/brut/corpus_bigchat.txt data/corpus/corrige/bizet.txt -pairs data/corpus/brut/trainBrut.txt data/corpus/corrige/trainCorrige.txt data/corpus/brut/testBrut.txt data/corpus/corrige/testCorrige.txt data/corpus/brut/tableTradCustom data/corpus/corrige/tableTradCustom

#Apprentissage SansGrams
#./main --buildDatabase -lexicons data/corpus/corrige/lexique data/corpus/corrige/vocabulaireSpecial -corpora data/corpus/brut/gramsVoid -pairs data/corpus/brut/trainBrut.txt data/corpus/corrige/trainCorrige.txt data/corpus/brut/testBrut.txt data/corpus/corrige/testCorrige.txt data/corpus/brut/tableTradCustom data/corpus/corrige/tableTradCustom

#Apprentissage SansTable
#./main --buildDatabase -lexicons data/corpus/corrige/lexique data/corpus/corrige/vocabulaireSpecial -corpora data/corpus/brut/gramsFull -pairs data/corpus/brut/tableTradCustom data/corpus/corrige/tableTradCustom

#Correction AVEC grams
#./main --correct data/lexicon/brut/trainBrut.txt.lexicon data/lexicon/corrige/trainBrut.txt.lexicon data/gramsCount/full.grams data/translationTable/full.table data/corpus/brut/corpus_devt_brut.txt

#Correction DU TEXTE EVAL AVEC grams
./main --correct data/lexicon/brut/trainBrut.txt.lexicon data/lexicon/corrige/trainBrut.txt.lexicon data/gramsCount/full.grams data/translationTable/full.table data/corpus/brut/corpus_eval_brut.txt

#Correction SANS grams
#./main --correct data/lexicon/brut/trainBrut.txt.lexicon data/lexicon/corrige/trainBrut.txt.lexicon data/gramsCount/void.grams data/translationTable/germinal.table data/corpus/brut/corpus_devt_brut.txt

#Correction SANS grams SANS table
#./main --correct data/lexicon/brut/trainBrut.txt.lexicon data/lexicon/corrige/trainBrut.txt.lexicon data/gramsCount/void.grams data/translationTable/void.table data/corpus/brut/corpus_devt_brut.txt

#Correction de input.txt
#./main --correct data/lexicon/brut/trainBrut.txt.lexicon data/lexicon/corrige/trainBrut.txt.lexicon data/gramsCount/full.grams data/translationTable/full.table input.txt

#./main --correct data/lexicon/brut/trainBrut.txt.lexicon data/lexicon/corrige/trainBrut.txt.lexicon data/gramsCount/void.grams data/translationTable/void.table input.txt

#eval/sclite -r data/corpus/corrige/corpus_devt_corr.txt -h data/corpus/brut/corpus_devt_brut.txt -i wsj

#eval/sclite -r data/corpus/corrige/corpus_devt_corr.txt -h data/output/corpus_devt_brut.txt.corrected.layout -i wsj
