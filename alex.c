#include "alex.h"

#include <ctype.h>
#include <stdlib.h>
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
	for (size_t i = 0; i < sizeof(keywords) / sizeof(keywords[0]); ++i)
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
		if (c == '\n')
		{
			ln++;
			continue;
		}
		else if (isspace(c))
			continue;
		else if (c == '(')
			return OPEPAR;
		else if (c == ')')
			return CLOPAR;
		else if (c == '{')
			return OPEBRA;
		else if (c == '}')
			return CLOBRA;
		else if (isalpha(c) || c == '_') {
			int i = 1;
			ident[0] = c;
			while (isalnum(c = fgetc(ci)) || c == '_')
				ident[i++] = c;
			ident[i] = '\0';
			ungetc(c, ci);
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
			ungetc(c, ci);
			return c == EOF ? EOFILE : OTHER;
		}
		else if (c == '/') {
			/* moze byc komentarz */
		}
		if (isdigit(c) || c == '.') {
			/* liczba */
		}
		else {
			return OTHER;
		}
	}
	return EOFILE;
}

char* alex_ident(void) {
	char* result = malloc((strlen(ident) + 1) * sizeof(*ident));
	strcpy(result, ident);
	return result;
}

int alex_getLN() {
	return ln;
}

