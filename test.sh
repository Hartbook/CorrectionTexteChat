#! /bin/bash

#valgrind ./main --buildDatabase data/corpus/corrige/corpus_ATH-TV_276_apres_correction.txt data/corpus/brut/corpus_ATH-TV_276_avant_correction.txt

#valgrind --track-origins=yes ./main --buildDatabase toy1 toy2
./main --buildDatabase toy1 toy2
