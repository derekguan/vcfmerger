#!/bin/sh

#c=`pwd`
#select $1 samples to generate database
if [ $# -lt 3 ]
then
	echo "merge.sh SAMPLEDIR SAMPLENUM GENEFILE"
	exit 1
fi
SAMPLEDIR=$1
SAMPLENUM=$2
GENEFILE=$3
#FILELIST=`find . -name * | shuf`
#FILELIST=`ls`
#FILELIST=`find $SAMPLEDIR -name "simu*.truth.vcf" | shuf`
#echo $FILELIST
#echo $FILELIST| sed 's/ /\n/g' |head -n $SAMPLENUM > SELECTEDFILES 
#let SELECTEDADDNUM=SAMPLENUM+1
#echo $FILELIST
#echo $SELECTEDADDNUM
#echo $FILELIST | sed 's/ /\n/g' | tail -n +$SELECTEDADDNUM > ADDFILES 
#echo $SELECTEDFILES 
#echo $ADDFILES
#echo $e
#vcfmerger $e -t 16 > merged.vcf 2>log &
#generate db file
./vcfmerger SELECTEDFILES  16 > db.vcf 2>log 

#generate additional files
./vcfmerger ADDFILES  16 > add.vcf 2 >> log  

GENENUM=`wc -l $GENEFILE | cut -d' ' -f1`
#generate querys 
cat SELECTEDFILES | shuf | head -n $GENENUM > SSAMPLES

paste -d'\t' SSAMPLES $GENEFILE > HSAMPLES

while read -r samplePath chrm reverse start end geneName
do
	prefix=`basename $samplePath`
	#echo $samplePath $chrm $reverse $start $end $geneName	
	bcftools view -r $chrm:$start-$end > "$prefix".vcf 	
done < HSAMPLES	

rm -f SSAMPLES HSAMPLES 
#use query to align


#generate    


#wait
#echo $SECONDS >> time.log

