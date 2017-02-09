/*
 * =============================================================================
========
 *
 *       Filename:  splitMA.c
 *
 *    Description:  split multiple alleles into single allele in vcf
 *
 *        Version:  1.0
 *        Created:  02/03/2017 08:41:39 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dengfeng Guan (D. Guan), dfguan@hit.edu.cn
 *   Organization:  Center for Bioinformatics, Harbin Institute of Technology
 *
 * =============================================================================
========
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LENGTH 1024
typedef struct line{
        char *str;
        int str_len;
        int max_str_len;
}ln;

typedef struct INFO {
        char *chrom;
        char *pos;
        char *id;
        char *ref;
        char *alt;
        char *info;
        char *format;
        char *genos;
}info;

typedef struct commaStruct{
	int 	pos;
	char 	*ref;
	char 	*alt;
	char 	*sv_len;
	char 	genos[3];
}comStr;

typedef struct commas{
	comStr *cms;
	int 	cms_num;
	int 	max_cms_num;
}coms;



coms *initiateCommas()
{
	coms * temp_coms = (coms *)malloc(sizeof(coms));
	
	if (temp_coms) {
		temp_coms->cms = (comStr *)malloc(sizeof(comStr)* 3);
		if (temp_coms->cms) {
			temp_coms->max_cms_num = 3;
			return temp_coms;
		} else 
		return NULL;	
	}

	return NULL;
}

int extendCommas(coms *_cms, int max_cms_num)
{
	if (max_cms_num > _cms->max_cms_num) {
		comStr *temp_cms = (comStr *)malloc(sizeof(comStr) * max_cms_num);
		
		if (temp_cms && _cms && _cms->cms) {
			memcpy(temp_cms, _cms->cms, _cms->max_cms_num);		
			free(_cms->cms);
			_cms->cms = temp_cms;	
		}else 
			return -1;	
	}
	return 0;
}

int destroyCommas(coms *t)
{
	if (t) {
		if (t->cms) {
			free(t->cms);
		}
		free(t);
	}
	return 0;
}

ln *initiateLn(int len)
{
        ln *temp = (ln *) malloc(sizeof(ln));
        if (temp) {
                temp->str = (char *)malloc(sizeof(char) * len);
                if (temp->str) {
                        temp->max_str_len = len;
                        return temp;
                }

        }

        return NULL;

}
int destroyLn(ln *l)
{
        if (l) {
                if (l->str) free(l->str);
                free(l);
        } 
        return 0;

}

int extendLn(ln *l)
{
	if (!l)
		return -1;
        int len = l->max_str_len << 1;
        char *str = (char *) malloc(sizeof(char) * len);
        if (str && l->str) {
		memcpy(str, l->str, l->max_str_len);
		free(l->str);
		l->str = str;
		l->max_str_len = len;
	} else 
		return -1;
	
	return 0;
}
int readheader(FILE *fl, ln *lin)
{
        char c;
        int len = 0;
        while (!feof(fl) && (c = fgetc(fl))!='\n') {
                lin->str[len++] = c;
                if (len >= lin->max_str_len) {
                        if (extendLn(lin))
                                return -1;//error for extend storage space for lin
                }
        }
        if (len >= lin->max_str_len) {
                        if (extendLn(lin))
                                return -1;//error for extend storage space for lin
        }

        lin->str[len] = 0;
        return len;
}

int readbody(FILE *fl, ln *lin)
{
        char c;
        int len = 0;
        while (!feof(fl) && (c = fgetc(fl))!='\n') {
                if (c == '\t') c = 0;
		lin->str[len++] = c;
                if (len >= lin->max_str_len) {
                        if (extendLn(lin))
                                return -1;//error for extend storage space for lin
                }
        }
        if (len >= lin->max_str_len) {
                        if (extendLn(lin))
                                return -1;//error for extend storage space for lin
        }

        lin->str[len] = 0;
        return len;
}
int processHeader(FILE *fl)
{       
        long fl_pos;
        ln *ltm = initiateLn(LENGTH);
        if (ltm) {
                while (!feof(fl)) {
                        fl_pos = ftell(fl);
                        int l_len = readheader(fl, ltm);
                        
                        if (~l_len) 
                                return -1;
                        if (ltm->str[0] == '#')
                                fprintf(stdout,"%s\n", ltm->str);
                        else 
                                break;
                }
        } else {
                return -1;
        }

        fseek(fl, 0L, fl_pos);
	destroyLn(ltm);

        return 0;
}
int processComma(char *pos, char *ref, char *alt, char *svlen,char *genos, coms *ccms)
{
	int i_pos = atoi(pos);		

	int len_alt = strlen(alt);
	
	int i;
	
	int count_comma = 1;


	for (i=0; i < len_alt; ++i) {
		if (alt[i] == ',') {
			alt[i] = 0;
			++count_comma;	
		}
		if (!extendCommas(ccms, count_comma)) {
			int j;
			int k = 0;
			for (j = 0; j < len_alt; ++j) {
				ccms->cms[k++].alt = alt + j;	
				while (alt[j] != 0) ++j;			
			}			
		}		
	}

	
	for (i = 0; i < count_comma; ++i) {
		int extendLength = posOfLastSameChar(ref, ccms->cms[i].alt);
		ccms->cms[i].pos = i_pos + extendLength;	
		ccms->cms[i].ref = ref + extendLength;	
		ccms->	
	}


	return 0;
}
int processBody(FILE *fl)
{       
	ln *ln = initiateLn(LENGTH);
	coms *Cms = initiateCommas();
		
	char *part[10];
        while (!feof(fl)) {
		int llen = readbody(fl,ln);	
		int j = 0;
		for (int i=0; i< llen; ++i) {
			part[j++] = ln->str + i;
			while (ln->str[i] != 0) ++i;
		}	
		processComma(part+1, part+3, part + 4, part + 7, part + 8, Cms);
        
	
	}

	destroyLn(ln);	
	destroyCommas(Cms);
        return 0;
}


int posOfLastSameChar(char *s1, char *s2)
{

	int len_s1 = strlen(s1);
	int len_s2 = strlen(s2);
	
	int min_len = len_s1 < len_s2? len_s1: len_s2;

	int i;
	for (i=0; i < min_len;++i) {
		if (s1[i] != s2[i])
			break;	
	}
        
	return i?0:i-1;

}





int main()
{
	



}
