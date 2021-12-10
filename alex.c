#include "alex.h"

#include <ctype.h>
#include <string.h>

static int  ln = 0;
static char ident[256];
static FILE* ci = NULL;

void    alex_init4file(FILE* in) {
	ln = 0;
	ci = in;
}

int isKeyword(char* str)
{
	char* keywords[] = { "while", "for", "do", "switch", "return", "sizeof", "auto", "break",
						"case", "const", "continue", "default", "if", "else", "enum", "extern",
						"static", "signed", "goto", "inline", "short", "long", "struct", "typedef",
						"union", "unsigned", "char", "double", "float", "int" };
	int i;
	for (i = 0; i < sizeof(keywords) / sizeof(keywords[0]); ++i)
	{
		if (strcmp(str, keywords[i]) == 0)
		{
			return 1;
		}
	}
	return 0;
}

lexem_t alex_nextLexem(void) {
	int c;
	while ((c = fgetc(ci)) != EOF) {
		if (isspace(c))
			continue;
		else if (c == '\n')
			ln++;
		else if (c == '(')
			return OPEPAR;
		else if (c == ')')
			return CLOPAR;
		else if (c == '{')
			return OPEBRA;
		else if (c == '}')
			return CLOBRA;
		else if (isalpha(c)) {
			int i = 1;
			ident[0] = c;
			while (isalnum(c = fgetc(ci)))
				ident[i++] = c;
			ident[i] = '\0';
			return isKeyword(ident) ? OTHER : IDENT;
		}
		else if (c == '"') {
			/* TODO: Uwaga: tu trzeba jeszcze poprawic obsluge nowej linii w trakcie napisu
			   i \\ w napisie
			*/
			int cp = c;
			while ((c = fgetc(ci)) != EOF && c != '"' && cp == '\\') {
				cp = c;
			}
			return c == EOF ? EOFILE : OTHER;
		}
		else if (c == '/') {
			/* moze byc komentarz */
		} if (isdigit(c) || c == '.') {
			/* liczba */
		}
		else {
			return OTHER;
		}
	}
	return EOFILE;
}

char* alex_ident(void) {
	return ident;
}

int alex_getLN() {
	return ln;
}

