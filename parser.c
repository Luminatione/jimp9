#include <stdio.h>
#include <stdlib.h>

#include "parser.h"

#define MAXINDENTLENGHT 256     // maks długość identyfikatora

functionAnalysisResultArray functionAnalysisResults;

void addFunctionUsage(functionUsage** arr, int* size, char* fileName, int line)
{
	if(*arr)
	{
		*arr = realloc(*arr, (*size + 1) * sizeof(**arr));
	}
	else
	{
		*arr = malloc(sizeof(**arr));
	}
	(*arr)[*size] = *createFunctionUsage(fileName, line);
	++*size;
}

functionUsage* createFunctionUsage(char* fileName, int line)
{
	functionUsage* a = malloc(sizeof(*a));
	a->fileName = malloc(strlen(fileName) * sizeof(*a->fileName));
	strcpy(a->fileName, fileName);
	a->line = line;
	return a;
}

int getFunctionIndex(char* name)
{
	for (int i = 0; i < functionAnalysisResults.functionsIndexes.size; ++i)
	{
		if (strcmp(functionAnalysisResults.functionsIndexes.names[i], name) == 0)
		{
			return i;
		}
	}
	return FUNTION_NOT_STORED;
}

void addFunctionToIndexing(char* name)
{
	functionAnalysisResults.functionsIndexes.names = realloc(functionAnalysisResults.functionsIndexes.names, functionAnalysisResults.functionsIndexes.size * sizeof(*functionAnalysisResults.functionsIndexes.names));
	strcpy(functionAnalysisResults.functionsIndexes.names[functionAnalysisResults.functionsIndexes.size], name);
	++functionAnalysisResults.functionsIndexes.size;
}

void addFunctionAnalysisResult(char* functionName)
{
	int index;
	if ((index = getFunctionIndex(functionName)) != FUNTION_NOT_STORED)
	{
		return;
	}

	functionAnalysisResult result;

	result.functionName = malloc(strlen(functionName) * sizeof(*functionName));
	strcpy(result.functionName, functionName);
	addFunctionToIndexing(functionName);

	functionAnalysisResults.functionAnalysisResults = realloc(functionAnalysisResults.functionAnalysisResults, (functionAnalysisResults.size + 1) * sizeof(*functionAnalysisResults.functionAnalysisResults));
	functionAnalysisResults.functionAnalysisResults[functionAnalysisResults.size] = result;
	++functionAnalysisResults.size;
}

void freeFunctionAnalysisResult(functionAnalysisResult* a)
{
	free(a->functionName);
	if (a->functionUsages)
	{
		int i;
		for (i = 0; i < a->usgaesAmount; ++i)
		{
			free(a->functionUsages[i].fileName);
		}
		free(a->functionUsages);
	}
	free(a);
}
functionAnalysisResult* getFunctionAnalysisResult(char* functionName)
{
	int index;
	if ((index = getFunctionIndex(functionName)) != FUNTION_NOT_STORED)
	{
		return &functionAnalysisResults.functionAnalysisResults[index];
	}
	return NULL;
}


void store_add_call(char* functionName, int line, char* fileName)
{
	functionAnalysisResult* a = getFunctionAnalysisResult(functionName);
	addFunctionUsage(&a->functionUsages, &a->usgaesAmount, fileName, line);
}

void store_add_proto(char* functionName, int line, char* fileName)
{
	functionAnalysisResult* a = getFunctionAnalysisResult(functionName);
	a->lineWithPrototype = line;
	a->prototypeFileName = fileName;
}

void store_add_def(char* functionName, int line, char* fileName)
{
	functionAnalysisResult* a = getFunctionAnalysisResult(functionName);
	a->lineWithDefinition = line;
	a->definitionFileName = fileName;
}

void analizatorSkladni(char* inpname)
{                               // przetwarza plik inpname

	FILE* in = fopen(inpname, "r");

	int nbra = 0;   // bilans nawiasów klamrowych {}
	int npar = 0;   // bilans nawiasów zwykłych ()

	alex_init4file(in);          // ustaw analizator leksykalny, aby czytał in

	lexem_t lex = alex_nextLexem();      // pobierz następny leksem
	while (lex != EOFILE) {
		switch (lex) {
		case IDENT: {
			char* iname = alex_ident();   // zapamiętaj identyfikator i patrz co dalej
			lexem_t nlex = alex_nextLexem();
			if (nlex == OPEPAR) {   // nawias otwierający - to zapewne funkcja
				npar++;
				put_on_fun_stack(npar, iname);       // odłóż na stos funkcji
													  // stos f. jest niezbędny, aby poprawnie obsłużyć sytuacje typu
													  // f1( 5, f2( a ), f3( b ) )
			}
			else {                  // nie nawias, czyli nie funkcja
				lex = nlex;
				continue;
			}
		}
				  break;
		case OPEPAR:
			npar++;
			break;
		case CLOPAR: {              // zamykający nawias - to może być koniec prototypu, nagłówka albo wywołania
			if (top_of_funstack() == npar) {       // sprawdzamy, czy liczba nawiasów bilansuje się z wierzchołkiem stosu funkcji
													// jeśli tak, to właśnie wczytany nawias jest domknięciem nawiasu otwartego
													// za identyfikatorem znajdującym się na wierzchołku stosu
				lexem_t nlex = alex_nextLexem();     // bierzemy nast leksem
				if (nlex == OPEBRA)   // nast. leksem to klamra a więc mamy do czynienia z def. funkcji
					store_add_def(get_from_fun_stack(), alex_getLN(), inpname);
				else if (nbra == 0)   // nast. leksem to nie { i jesteśmy poza blokami - to musi być prototyp
					store_add_proto(get_from_fun_stack(), alex_getLN(), inpname);
				else                  // nast. leksem to nie { i jesteśmy wewnątrz bloku - to zapewne wywołanie
					store_add_call(get_from_fun_stack(), alex_getLN(), inpname);
			}
			npar--;
		}
				   break;
		case OPEBRA:
			nbra++;
			break;
		case CLOBRA:
			nbra--;
			break;
		case ERROR: {
			fprintf(stderr, "\nBUUUUUUUUUUUUUUUUUUUUUU!\n"
				"W pliku %s (linia %d) są błędy składni.\n"
				"Kończę!\n\n", inpname, alex_getLN());
			exit(1);               // to nie jest najlepsze, ale jest proste ;-)
		}
				  break;
		default:
			break;
		}
		lex = alex_nextLexem();
	}
}
