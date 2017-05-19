#! /bin/bash

function formatText
{
	local filename=$(basename $1)

	cat $1 | tr -d '[:punct:]' > eval/tmp/$filename.clean
	#cat $1 > eval/tmp/$filename.clean
	awk '{printf("%s (a_%d)\n", $0, NR)}' eval/tmp/$filename.clean > eval/tmp/$filename
	echo eval/tmp/$filename
}

function printUsageAndExit
{
	echo USAGE $0 expectedTextFile actualTextFile
	exit 1
}

if [ "$#" -ne 2 ]; then
	printUsageAndExit
fi

if [ ! -f $1 ]; then
    echo Cannot find file $1
	printUsageAndExit
fi

if [ ! -f $2 ]; then
    echo Cannot find file $2
	printUsageAndExit
fi

ref=$(formatText $1)
hyp=$(formatText $2)

./eval/sclite -r $ref trn -h $hyp trn -i rm -o rsum stdout
./eval/sclite -r $ref trn -h $hyp trn -i rm -o sum stdout
