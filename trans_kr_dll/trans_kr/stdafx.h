// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
// Windows ��� ����:
#include <windows.h>

// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.
#include <stdio.h>
#include "pcre.h"
#define TRANSLATIONS_FILE1	"puten.txt"
#define TRANSLATIONS_FILE2	"outkr.txt"
#define UNTRANSLATED_FILENAME	"untrans.txt"
#define LINE_BUFFER_SIZE	2048
#define MAX_TRANSLATED_LEN	2048
#define MAX_CONVERTED_LEN	2048
#define DELIMITER	'='
#define ESCAPE_CHAR	'\\'
#define ESCAPE_STR	"\\"
#define MAX_PARAMS	5
#define MAX_PARAM_LEN	15
#define WILD_CARD	"\\s"
#define SPECIAL_CHARS	".*^$|()[]\\+?"
#define STR_CHAR	's'
#define STR_RE	"(.*)"
#define STR_RE_LEN	(sizeof(STR_RE) - 1)
#define CHR_CHAR	'c'
#define CHR_RE	"(.)"
#define CHR_RE_LEN	(sizeof(CHR_RE) - 1)
#define NUM_CHAR	'#'
#define NUM_RE	"([0-9]+)"
#define NUM_RE_LEN	(sizeof(NUM_RE) - 1)
#define PCRE_ESC_CHAR	'\\'
#define MAX_CAPTURES	10
#define NUM_SECTIONS	('z' - 'a' + 2)
#define COMMENT_CHAR	'#'
#define PARAM_CHAR	'$'
#define SECTION_CHAR	':'
#define BUFFER_SIZE_PREV 10
#define NUM_JOSA 12

typedef struct {
	pcre* untranslated;
	char* translated;
//	int sortIndex[10000];
}TRANSLATION;
//int find_translation(const char*, char*, int);
void format(char* s, const char* fmt, const char* p, int* v, int n);
void closePcre();
void convert_pattern(const char* s, char* t);
int add_translation(const char* untranslated, const char* translated, int section);
int to_section(char c);
void load_translations(void);
void ko_initialize(void);
int find_translation(const char* untranslated, char* translated, int section);
void save_untranslated(const char* untranslated, int sector);
const char* ko_translateSecter(const char* untranslated, int sector, bool* found);
const char* getIdiom(const char* str, int* index, bool* found);
const char* getWord(const char* str, int* index, bool* found, int mode);
int getJosa(char* str, int* index, char* prev);
void Reload();
