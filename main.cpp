/* File: main.cpp
 * Author: CRE
 * Last Edited: Mon Jan 16 13:09:54 2017
 */

#include "vcfmerger.h"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <string>
#include <string.h>
#include "crelib/crelib.h"
#include <omp.h>

using namespace std;
using namespace cre;

static inline void printHelp()
{
	die("Usage:\n"\
			"\tvcfmerger vcf1 vcf2 ... > merged.vcf\n"\
			"or\tvcfmerger vcf1 vcf2 ... -o merged.vcf\n"\
			"and\t-t n for multithreading\n"\
			"Warning: This program may generate or overwrite several temporary files(less than input files) in this directory, they are %s* and %s*. If the program ends normal, they would be deleted as well. Although I don't think you have those files in your directory, just be aware of it.", TEMP_FILE_NAME1, TEMP_FILE_NAME2);
}

static inline void printHelpD()
{
	die("Usage:\n"
			"vcfmerger vcfNamesFile ThreadsNum"
			"Warning: This program may generate or overwrite several temporary files(less than input files) in this directory, they are %s* and %s*. If the program ends normal, they would be deleted as well. Although I don't think you have those files in your directory, just be aware of it.", TEMP_FILE_NAME1, TEMP_FILE_NAME2);
}
string getTempFileName(const char * Prefix, int n)
{
	string Name(Prefix);
	Name+=to_string(n);
	return Name;
}

uint MostFileNumber=0;
void removeTempFiles()
{
	for (uint i=0;i<MostFileNumber;++i)
	{
		remove(getTempFileName(TEMP_FILE_NAME1,i).c_str());
		remove(getTempFileName(TEMP_FILE_NAME2,i).c_str());
	}
}
void removeTempFiles(vector<string> &rlist, uint num)
{
	for (uint i=0; i < num; ++i) {
		const char *flname = rlist[i].c_str();
		if (flname[0] == '.') {
			remove(flname);	
			//printf("%s", flname);		
		}
		//remove(rlist[i].c_str());
	}
}
struct EmptyString
{
	bool operator() (const string &s) 
	{
		return s.empty();
	}
};

uint Threads=1;

int main (int argc, char *argv[])
{
	if (argc<=2) printHelpD();

	atexit(removeTempFiles);

	vector<string> InFileNames;
	vector<string> toBeRemoved;
	char *samplesNameListFile = argv[1];

	ifstream samplesNamesFile;
	samplesNamesFile.open(samplesNameListFile);

	if (!samplesNamesFile.is_open()) {
		fprintf(stderr, "please specify samples file\n");
		exit(1);		
	}
	
	int z = 0;
	string line;
	while (getline(samplesNamesFile, line)) {
		//fprintf(stderr,"%s\n",line.c_str());
		InFileNames.push_back(line);	
		toBeRemoved.push_back("");
	} 
	samplesNamesFile.close();	
	
	char OutFile[NAME_BUFFER_SIZE];
	OutFile[0]='\0';

	Threads = atoi(argv[2]);
	//for (int i=1;i<argc;++i)
	//{
		//if (argv[i][0]=='-')
		//{
			//if (argv[i][1]=='o')
			//{
				//if (i+1>=argc) printHelp();
				//strcpy(OutFile, argv[++i]);
			//}
			//else if (argv[i][1]=='t'&& argv[i][2]=='\0')
			//{
				//if (i+1>=argc) printHelp();
				//Threads=atoi(argv[++i]);
			//}
		//}
		//else
		//{
			//InFileNames.push_back(argv[i]);
		//}
	//}
	//for (int i=0; i < InFileNames.size(); ++i)
		//fprintf(stderr,"%s\n",InFileNames[i].c_str());

	if (InFileNames.size()<2) printHelp();
	uint FilesToMerge=InFileNames.size();
	MostFileNumber=FilesToMerge/2;
	const char *Prefix=TEMP_FILE_NAME1, *LPrefix=TEMP_FILE_NAME2;
	while(FilesToMerge>1)
	{
		if (FilesToMerge==2)
		{
			toBeRemoved[0] = InFileNames[0];
			toBeRemoved[1] = InFileNames[1];
			merge2(InFileNames[0].c_str(),InFileNames[1].c_str(),OutFile);
			if (InFileNames[0].c_str()[0] =='.')
				remove(InFileNames[0].c_str());
			if (InFileNames[1].c_str()[0] =='.')
				remove(InFileNames[1].c_str());
			break;
		}
		omp_set_num_threads(Threads);
		//#pragma omp parallel num_threads(Threads)
		#pragma omp parallel for
		for (uint i=0;i<FilesToMerge/2;++i)
		{
			string TempFileName=getTempFileName(Prefix,i);
			if (merge2(InFileNames[i].c_str(), InFileNames[FilesToMerge-1-i].c_str(), TempFileName.c_str())) {
				if (InFileNames[i].c_str()[0] =='.')
					remove(InFileNames[i].c_str());
				if (InFileNames[FilesToMerge - 1 -i].c_str()[0] =='.')
					remove(InFileNames[FilesToMerge - i - 1].c_str());
				InFileNames[i] = InFileNames[FilesToMerge - 1 -i] = ""; 
				//InFileNames[FilesToMerge - i - 1] = "";
			} else {
				if (InFileNames[i].c_str()[0] =='.')
					remove(InFileNames[i].c_str());
				if (InFileNames[FilesToMerge - 1 -i].c_str()[0] =='.')
					remove(InFileNames[FilesToMerge - i - 1].c_str());
				InFileNames[i] = TempFileName;
				InFileNames[FilesToMerge - i - 1] = "";
			}
		}
		//fprintf(stderr,"stop here");	
		InFileNames.erase(remove_if(InFileNames.begin(), InFileNames.end(), EmptyString()), InFileNames.end());
		FilesToMerge = InFileNames.size();
		const char * Temp=Prefix;
		Prefix=LPrefix;
		LPrefix=Temp;
	}
	return 0;
}
