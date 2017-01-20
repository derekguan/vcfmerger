#!/bin/sh

#c=`pwd`
#select $1 samples to generate database
SAMPLEDIR=$1
SAMPLENUM=$2
GENEFILE=$3
#FILELIST=`find . -name * | shuf`
#FILELIST=`ls`
FILELIST=`find $SAMPLEDIR -name "simu*.truth.vcf" | shuf | sed 's/ /\n/g'`
#echo $FILELIST
SELECTEDFILES=`echo $FILELIST| sed 's/ /\n/g' |head -n $SAMPLENUM` 
let SELECTEDADDNUM=SAMPLENUM+1
echo $FILELIST
echo $SELECTEDADDNUM
ADDFILES=`echo $FILELIST | sed 's/ /\n/g' | tail -n +$SELECTEDADDNUM` 
echo $SELECTEDFILES 
echo $ADDFILES
#echo $e
#vcfmerger $e -t 16 > merged.vcf 2>log &
#generate db file
vcfmerger $SELECTEDFILES -t 16 > db.vcf 2>log &

#generate additional files
vcfmerger $ADDFILES -t 8 > add.vcf 2 >> log &

GENENUM=`wc -l $GENEFILE`
#generate querys 

QUERYFLNAME=`echo $SELECTEDFILES | sed 's/ /\n/g' | shuf | head -n $GENENUM`

for i in $QUERYFLNAME
do
		
done	

#use query to align


#generate  


#wait
#echo $SECONDS >> time.log

