// trans_kr.cpp : DLL 응용 프로그램을 위해 내보낸 함수를 정의합니다.
// test

#include "stdafx.h"
#include "trans_kr.h"

#pragma data_seg("SHAREDATA")
bool g_bReloading=false;
bool g_bTransOn = true;
#pragma data_seg()
#pragma comment(linker, "/SECTION:SHAREDATA,RWS")
FILE* g_untranslated_file=NULL;
bool g_bOutUntrans = false;
bool g_bSubSection = false;
int g_prevIndex = 0;
int g_initialized = 0;
char g_prevBuffer[BUFFER_SIZE_PREV][LINE_BUFFER_SIZE]={NULL,};
TRANSLATION *g_tran[NUM_SECTIONS]={NULL,};
int g_num_trans[NUM_SECTIONS]={0,};
#define NUM_PRETRANS	100
char g_PreTranslationIn[NUM_PRETRANS][MAX_TRANSLATED_LEN];
char g_PreTranslationOut[NUM_PRETRANS][MAX_TRANSLATED_LEN];
char g_PreTransIndex = 0;


#if 0
// 내보낸 변수의 예제입니다.
TRANS_KR_API int ntrans_kr=0;

// 내보낸 함수의 예제입니다.
TRANS_KR_API int fntrans_kr(void)
{
	return 42;
}

// 내보낸 클래스의 생성자입니다.
// 클래스 정의를 보려면 trans_kr.h를 참조하십시오.
Ctrans_kr::Ctrans_kr()
{
	return;
}
#endif

#include "hcode.h"


const char* g_tableJ[NUM_JOSA]={
	"이", "가",
	"은", "는",
	"을", "를",
	"으로", "로",
	"이다", "다"
	"과", "와"
};

int isShiftJIS(int c)
{
	return 0;
}

int isalpha2(int b, int c, int d)
{
	if(d!=0)
	{
		if((c>=0x81 && c<=0x9f)||(c>=0xe0 && c<=0xfc)) // first byte
		{
			if(d>=0x40 && d<=0xfc && d!=0x7f) // second byte
			{
				return 1;
			}
		}
		else if(b!=0)
		{
			if(c>=0x40 && c<=0xfc && c!=0x7f) // second byte
			{
				if((b>=0x81 && b<=0x9f)||(b>=0xe0 && b<=0xfc)) // first byte
				{
					return 1;
				}
			}
		}
	}
	if(b!=0)
	{
		if(c>=0x40 && c<=0xfc && c!=0x7f) // second byte
		{
			if((b>=0x81 && b<=0x9f)||(b>=0xe0 && b<=0xfc)) // first byte
			{
				return 1;
			}
		}
		else if(d!=0)
		{
			if((c>=0x81 && c<=0x9f)||(c>=0xe0 && c<=0xfc)) // first byte
			{
				if(d>=0x40 && d<=0xfc && d!=0x7f) // second byte
				{
					return 1;
				}
			}
		}
	}
	return isalpha(c);
}

void closePcre()
{
	int n, i;
	for(n=0; n<NUM_SECTIONS; n++)
	{
		if(g_num_trans[n]>0)
		{
			for(i=0; i<g_num_trans[n]; i++)
			{
				free (g_tran[n][i].translated);
				g_tran[n][i].translated = NULL;
			}
			free(g_tran[n]);
			g_tran[n] = NULL;
		}
		g_num_trans[n] = 0;
	}
	g_initialized = 0;
	if(g_untranslated_file) fclose(g_untranslated_file);
	g_untranslated_file=NULL;
}

void convert_pattern(const char* s, char* t)
{
	int n;
	*t++ = '^';
	while (1) {
		n = (int)strcspn(s, SPECIAL_CHARS);
		memcpy(t, s, n);
		t += n;
		s += n;
		if (!*s) {
			break;
		} else if (*s == ESCAPE_CHAR) {
			static const char sym[]
				= {STR_CHAR, CHR_CHAR, NUM_CHAR, 0};
			static const struct {
				const char* re;
				int len;
			} tab[] = {
				{STR_RE, STR_RE_LEN},
				{CHR_RE, CHR_RE_LEN},
				{NUM_RE, NUM_RE_LEN},
			};
			const char* r;
			++s;
			r = strchr(sym, *s);
			if (r) {
				++s;
				memcpy(t, tab[r-sym].re, tab[r-sym].len);
				t += tab[r-sym].len;
			}
		} else {
			*t++ = PCRE_ESC_CHAR;
			*t++ = *s++;
		}
	}
	*t++ = '$';
	*t = 0;
}

int add_translation(const char* untranslated, const char* translated, int section)
{
	void *t, *v;
	pcre* u;
	const char* error;
	int erroffset;
	static char converted[MAX_CONVERTED_LEN];
	int result;

	t = realloc(g_tran[section],
			(g_num_trans[section]+1)*sizeof(*g_tran[section]));
	convert_pattern(untranslated, converted);
	u = pcre_compile(converted, NULL, &error, &erroffset, NULL);
	v = malloc(strlen(translated)+1);
	result = t && u && v;
	if (result) {
		g_tran[section] = (TRANSLATION*) t;
		g_tran[section][g_num_trans[section]].untranslated = u;
		g_tran[section][g_num_trans[section]].translated = (char*) v;
		strcpy(g_tran[section][g_num_trans[section]].translated,
				translated);
		++g_num_trans[section];
	} else {
		if (u) {
			free(u);
		}
		if (v) {
			free(v);
		}
	}

	return result;
}

int to_section(char c)
{
	return tolower(c) - 'a' + 1;
}

void load_translations(void)
{
	FILE* translations_file1;
	FILE* translations_file2;
	translations_file1 = fopen(TRANSLATIONS_FILE1, "r");
	translations_file2 = fopen(TRANSLATIONS_FILE2, "r");
	g_bOutUntrans = false;
	g_bSubSection = false;
	if (translations_file1 && translations_file2) {
		char line1[LINE_BUFFER_SIZE];
		char line2[LINE_BUFFER_SIZE];
		int section = 0;
		while (fgets(line1, sizeof(line1), translations_file1)) {
			fgets(line2, sizeof(line2), translations_file2);
			//char* p;
			if (line1[0] == COMMENT_CHAR) {
				continue;
			} else if (line1[0] == SECTION_CHAR) {
				section = to_section(line1[1]);
				continue;
			} else if (line1[0] == PARAM_CHAR) {
				if(strncmp(line1, "$OutUntrans", 11)==0)
				{
					g_bOutUntrans = true;
				}
				if(strncmp(line1, "$OutSubsection", 14)==0)
				{
					g_bSubSection = true;
				}
//				if(strncmp(line, "$OutChat", 8)==0)
//				{
//					bOutChat = true;
//				}
				continue;
			}
//			p = strchr(line, DELIMITER);
//			if (!p) {
//				continue;
//			}
//			*p++ = 0;
			line1[strcspn(line1,"\r\n")] = 0;
			line2[strcspn(line2,"\r\n")] = 0;
			if (!add_translation(line1, line2, section)) {
				break;
			}
		}
		fclose(translations_file1);
		fclose(translations_file2);
	}
}

void ko_initialize(void)
{
	load_translations();
	g_untranslated_file = fopen(UNTRANSLATED_FILENAME, "w");
	if(g_untranslated_file) fclose(g_untranslated_file);
	g_untranslated_file=NULL;

	for(int i=0; i<NUM_PRETRANS; i++)
	{
		g_PreTranslationIn[i][0] = 0;
		g_PreTranslationOut[i][0] = 0;
		g_PreTransIndex = 0;
	}
}

void format(char* s, const char* fmt, const char* p, int* v, int n)
{
	const char* t = fmt;
	while (1) {
		int c = (int)strcspn(t, ESCAPE_STR);
		memcpy(s, t, c);
		s += c;
		t += c;
		if (!*t) {
			break;
		} else if (*t == ESCAPE_CHAR) {
			if (isdigit(*++t)) {	/* \N */
				c = (*t-'0') * 2;
				++t;
				memcpy(s, p+v[c], v[c+1]-v[c]);
				s += v[c+1] - v[c];
			} else if (isalpha(*t) && isdigit(*(t+1))) {
								/* \xN */
				const char* q;
				++t;
				c = *t - '0';
				if (pcre_get_substring(p, v, n, c, &q) <= 0) {
					++t;
					/* TODO: handle error */
					continue;
				}
				if (find_translation(q, s,
							to_section(*(t-1)))) {
					s += strlen(s);
				} else {
					s += pcre_copy_substring(p, v, n, c, s,
							100);
					/* TODO: fix the use of magic number */
				}
				++t;
				pcre_free_substring(q);
			} else {
				++t;
				/* do nothing */
			}
		} else {
			printf("\nformat: impossible!\n");
			system("pause");
		}
	}
	*s = 0;
}

int find_translation(const char* untranslated, char* translated, int section)
{
	int i;
//	const char** params;
	for (i = 0; i < g_num_trans[section]; ++i) {
		int v[(MAX_CAPTURES + 1) * 3];
		const int vlen = sizeof(v) / sizeof(*v);
		int n = pcre_exec(g_tran[section][i].untranslated, NULL,
				untranslated, (int)strlen(untranslated), 0, 0, v,
				vlen);
		if (n > 0) {
			format(translated, g_tran[section][i].translated,
					untranslated, v, n);
			break;
		}
	}
	return i < g_num_trans[section];
}

void save_untranslated(const char* untranslated, int sector)
{
	int i;
	if(g_bOutUntrans==false) return;
	if(g_bSubSection==false && sector!=0) return;
	for(i=0; i<BUFFER_SIZE_PREV; i++)
	{
		if(strcmp(g_prevBuffer[i], untranslated)==0)
			return;
	}
	strcpy(g_prevBuffer[g_prevIndex], untranslated);
	g_prevIndex++;
	if(g_prevIndex>=BUFFER_SIZE_PREV)
		g_prevIndex = 0;

	g_untranslated_file = fopen(UNTRANSLATED_FILENAME, "a+");
	if (g_untranslated_file) {
		if(sector){
			fprintf(g_untranslated_file, "%d:", sector);
		}
		fputs(untranslated, g_untranslated_file);
		fputs("\n", g_untranslated_file);
		fclose(g_untranslated_file);
		g_untranslated_file=NULL;
	}
}

const char* ko_translateSecter(const char* untranslated, int sector, bool* found)
{
	static char result[NUM_SECTIONS][MAX_TRANSLATED_LEN];
	static char tempStr[MAX_TRANSLATED_LEN];
	if(strlen(untranslated)==1) return untranslated;
	if(sector==2)
	{
		memset(result[sector], 0, MAX_TRANSLATED_LEN);
		if (!find_translation(untranslated, result[sector], sector))
		{
			if(strcmp(untranslated+strlen(untranslated)-1, "s")==0)
			{
				strncpy(tempStr, untranslated, strlen(untranslated)-1);
				tempStr[strlen(untranslated)-1] = 0;
				if (!find_translation(tempStr, result[sector], sector))
				{
					if(strcmp(untranslated+strlen(untranslated)-2, "es")==0)
					{
						strncpy(tempStr, untranslated, strlen(untranslated)-2);
						tempStr[strlen(untranslated)-2] = 0;
						if (!find_translation(tempStr, result[sector], sector))
						{
							if(strcmp(untranslated+strlen(untranslated)-3, "ies")==0)
							{
								strncpy(tempStr, untranslated, strlen(untranslated)-3);
								tempStr[strlen(untranslated)-3] = 'y';
								tempStr[strlen(untranslated)-4] = 0;
								if (!find_translation(tempStr, result[sector], sector))
								{
									return result[sector];
								}
							}
							else
							{
								return result[sector];
							}
						}
					}
					else
					{
						return result[sector];
					}
				}
			}
			else
			{
				return result[sector];
			}
		}
	}
	else if(sector==1)
	{
		if (!find_translation(untranslated, result[sector], sector))
		{
			if(strcmp(untranslated+strlen(untranslated)-1, "s")==0)
			{
				strncpy(tempStr, untranslated, strlen(untranslated)-1);
				tempStr[strlen(untranslated)-1] = 0;
				if (!find_translation(tempStr, result[sector], sector))
				{
					if(strcmp(untranslated+strlen(untranslated)-2, "es")==0)
					{
						strncpy(tempStr, untranslated, strlen(untranslated)-2);
						tempStr[strlen(untranslated)-2] = 0;
						if (!find_translation(tempStr, result[sector], sector))
						{
							if(strcmp(untranslated+strlen(untranslated)-3, "ies")==0)
							{
								strncpy(tempStr, untranslated, strlen(untranslated)-3);
								tempStr[strlen(untranslated)-3] = 'y';
								tempStr[strlen(untranslated)-4] = 0;
								if (!find_translation(tempStr, result[sector], sector))
								{
									save_untranslated(untranslated, sector);
									*found = false;
									return untranslated;
								}
							}
							else
							{
								save_untranslated(untranslated, sector);
								*found = false;
								return untranslated;
							}
						}
					}
					else
					{
						save_untranslated(untranslated, sector);
						*found = false;
						return untranslated;
					}
				}
			}
			else
			{
				save_untranslated(untranslated, sector);
				*found = false;
				return untranslated;
			}
		}
	}
	else
	{
		if (!find_translation(untranslated, result[sector], sector))
		{
			save_untranslated(untranslated, sector);
			*found = false;
			return untranslated;
		}
	}
	return result[sector];
}

const char* getIdiom(const char* str, int* index, bool* found)
{
	static char result[MAX_TRANSLATED_LEN];
	static char untrans[MAX_TRANSLATED_LEN];
	int i = (int)strlen(str);
	result[0]=0;
	while(i>0)
	{
		while(i>0)
		{
			if(str[i]==' ')
			{
				break;
			}
			else if(str[i]==NULL && isalpha2((i>1)?str[i-2]:0, str[i-1], str[i]))
			{
				break;
			}
			else if(!isalpha2(str[i-1], str[i], str[i+1]) && isalpha2((i>1)?str[i-2]:0, str[i-1], str[i]))
			{
				break;
			}
			i--;
		}
		if(i<=0)
		{
			break;
		}
		memcpy(untrans, str, i);
		untrans[i]=0;
		result[0] = 0;
		bool bf;
		strcpy(result, ko_translateSecter(untrans, 2, &bf));
//		find_translation(untrans, result, 2);
		if(strcmp(result, "")!=0)
		{
			*index += i;
			if(str[i]==' ')
			{
//				result[strlen(result)+1]=0;
//				result[strlen(result)]=str[i];
				result[strlen(result)]=0;
			}
			return result;
		}
		i--;
	}
	return result;
}

const char* getWord(const char* str, int* index, bool* found, int mode)
{
	static char result[MAX_TRANSLATED_LEN];
	static char untrans[MAX_TRANSLATED_LEN];
	int i;
	if(mode==0)
	{
		i=0;
		while(str[i])
		{
			if(str[i]==' ')break;
			i++;
		}
		*index += i;
		*found = false;
		strncpy(untrans, str, i);
		untrans[i]=0;
		return untrans;
	}

	i=0;
	while(str[i])
	{
		if(isalpha2((i>0)?str[i-1]:0, str[i], str[i+1]))
		{
			i++;
		}
		else
		{
			break;
		}
	}
	*index += i;
	strncpy(untrans, str, i);
	untrans[i]=0;
	result[0] = 0;
	strcpy(result, ko_translateSecter(untrans, 1, found));
//	find_translation(untrans, result, 1);
	if(result[0]==0)
	{
		strcpy(result, untrans);
		if(	(strcmp(result, "an")==0 ||
			strcmp(result, "a")==0 ||
			strcmp(result, "the")==0 ||
			strcmp(result, "The")==0 ||
			strcmp(result, "A")==0 ||
			strcmp(result, "An")==0)
			)
		{
		}
		else
		{
			*found = false;
		}
	}
	return result;
}

int getJosa(char* str, int* index, char* prev)
{
	int type, i, br, len;
	type = -1;
	br = 0;
	len = (int)strlen(str);
	if(len==0)
		return type;
	if(str[0]!='[')
		return type;
	for(i=1; i<len; i++){
		if(str[i]==']')
		{
			br = i;
			break;
		}
	}
	if(br==0){
		return type;
	}

	if(br==3){
		*index += 4;
		if(strncmp("이", str+1, 2)==0){
			type = 0;
		}
		else if(strncmp("가", str+1, 2)==0){
			type = 0;
		}
		else if(strncmp("은", str+1, 2)==0){
			type = 2;
		}
		else if(strncmp("는", str+1, 2)==0){
			type = 2;
		}
		else if(strncmp("을", str+1, 2)==0){
			type = 4;
		}
		else if(strncmp("를", str+1, 2)==0){
			type = 4;
		}
		else if(strncmp("로", str+1, 2)==0){
			type = 6;
		}
		else if(strncmp("다", str+1, 2)==0){
			type = 8;
		}
		else{
			*index -= 4;
		}
	}
	else if(br==5){
		*index += 6;
		if(strncmp("으로", str+1, 4)==0){
			type = 6;
		}
		else if(strncmp("이다", str+1, 4)==0){
			type = 8;
		}
		else{
			*index -= 6;
		}
	}
	if(type>=0)
	{
		unsigned short code = (unsigned char)prev[0]*0x100+(unsigned char)prev[1];
		int code_index = ((code >> 8) - 0xb0) * 94 + (code & 0x00ff) - 0xa1;
		if(code_index<0 || code_index>=2350) code_index=0;
		if((g_hcode[code_index][1]&0x0000001f)==1){//받침없음
			type = type + 1;
		}
		if(type==6){
			if((g_hcode[code_index][1]&0x0000001f)==9){//받침 ㄹ 
				type = type + 1;
			}
		}
	}
	return type;
}

int preSearch(char* untranslated)
{
	for(int i=0; i<NUM_PRETRANS; i++)
	{
		if(strcmp(g_PreTranslationIn[i], untranslated)==0)
		{
			return i;
		}
	}
	return -1;
}

void preAdd(char* inStr, char* outStr)
{
	strcpy(g_PreTranslationIn[g_PreTransIndex], inStr);
	strcpy(g_PreTranslationOut[g_PreTransIndex], outStr);
	g_PreTransIndex++;
	if(g_PreTransIndex>=NUM_PRETRANS)
	{
		g_PreTransIndex = 0;
	}
}

char* trans(char* untranslated, int mode)
{
	static char input_string[MAX_TRANSLATED_LEN];
	static char first_traslated[MAX_TRANSLATED_LEN];
	static char second_traslated[MAX_TRANSLATED_LEN];
	const char* word;
	unsigned int i;
	int index;
	int ret;
	bool found = true;
	int total_len;
	if(!g_bTransOn)
		return untranslated;
	if(g_bReloading) Reload();
	if (!g_initialized) {
		g_initialized = 1;
		ko_initialize();
	}
//	if(untranslated[0]<=0)
	if(untranslated[0]==0)
		return untranslated;
	if((ret = preSearch(untranslated))>=0)
	{
		return g_PreTranslationOut[ret];
	}
	strcpy(input_string, untranslated);
	for(i=0; i<strlen(input_string); i++)
	{
		if(input_string[i]=='\n')
			input_string[i] = 30;
	}
	if (!find_translation(input_string, first_traslated, 0)) {
		save_untranslated(input_string, 0);
		preAdd(untranslated, untranslated);
		return untranslated;
	}
	total_len = (int)strlen(first_traslated);
	memset(second_traslated, 0, MAX_TRANSLATED_LEN);
	index = 0;
	while(index<total_len)
	{
		if(isalpha2((index>0)?first_traslated[index-1]:0, first_traslated[index], first_traslated[index+1]))
		{
			word = getIdiom(first_traslated+index, &index, &found);
			if(word[0]==0)
			{
				word=getWord(first_traslated+index, &index, &found, mode);
			}
			if(	(strcmp(word, "an")==0 || strcmp(word, "a")==0 || strcmp(word, "the")==0 ||	strcmp(word, "The")==0 || strcmp(word, "A")==0 || strcmp(word, "An")==0)
				&& first_traslated[index]==' '
				&& isalpha2(first_traslated[index], first_traslated[index+1], (index<total_len-1)?first_traslated[index+2]:0)
				)
			{
				index++;
			}
			else
			{
				strcat(second_traslated, word);
			}
		}
		else
		{
			int jo = -1;
			int second_len = (int)strlen(second_traslated);
			if(second_len>1)
				jo = getJosa(first_traslated+index, &index, second_traslated+second_len-2);
			if(jo>=0)
			{
				strcat(second_traslated, g_tableJ[jo]);
			}
			else
			{
				int tmp_len = (int)strlen(second_traslated);
				second_traslated[tmp_len] = first_traslated[index];
				second_traslated[tmp_len+1] = 0;
				index++;
			}
		}
	}
	if(found==false) save_untranslated(input_string, -1);
	for(i=0; i<strlen(second_traslated); i++)
	{
		if(second_traslated[i]==30)
			second_traslated[i] = '\n';
	}
	preAdd(untranslated, second_traslated);
	return second_traslated;
}

char* WINAPI ko_translate(char* untranslated)
{
	return trans(untranslated, 1);
}


char* WINAPI ko_translate_wo_word(char* untranslated)
{
	return trans(untranslated, 0);
}

void WINAPI ko_ReloadDic()
{
	g_bReloading = true;
}

void WINAPI TransOn(bool on)
{
	g_bTransOn = on;
}

void Reload()
{
	closePcre();
	for(int i=0; i<BUFFER_SIZE_PREV; i++)
	{
		g_prevBuffer[i][0]=0;
	}
	g_prevIndex = 0;
	ko_initialize();
	g_bReloading = false; 
}

