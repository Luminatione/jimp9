#include <stdio.h>
#include <stdlib.h>

#include "parser.h"

#define MAXINDENTLENGHT 256     // maks długość identyfikatora

functionAnalysisResultArray functionAnalysisResults;

void addFunctionUsage(functionUsage** arr, int* size, char* fileName, int line)
{
	*arr = realloc(*arr, (*size + 1) * sizeof(**arr));
	(*arr)[*size] = createFunctionUsage(fileName, line);
	++* size;
}

functionUsage createFunctionUsage(char* fileName, int line)
{
	functionUsage a;
	a.fileName = malloc((strlen(fileName) + 1) * sizeof(*a.fileName));
	strcpy(a.fileName, fileName);
	a.line = line;
	return a;
}

int getFunctionIndex(char* name)
{
	for (int i = 0; i < functionAnalysisResults.size; ++i)
	{
		if (strcmp(functionAnalysisResults.functionAnalysisResults[i].functionName, name) == 0)
		{
			return i;
		}
	}
	return FUNCTION_NOT_STORED;
}

void addFunctionAnalysisResult(char* functionName)
{
	int index;
	if ((index = getFunctionIndex(functionName)) != FUNCTION_NOT_STORED)
	{
		return;
	}

	functionAnalysisResult result;

	result.functionName = malloc((strlen(functionName) + 1) * sizeof(*functionName));
	strcpy(result.functionName, functionName);
	result.functionUsages = NULL;
	result.usagesAmount = 0;
	result.lineWithDefinition = 0;
	result.lineWithPrototype = 0;
	result.definitionFileName = NULL;
	result.prototypeFileName = NULL;

	functionAnalysisResults.functionAnalysisResults = realloc(functionAnalysisResults.functionAnalysisResults, (functionAnalysisResults.size + 1) * sizeof(*functionAnalysisResults.functionAnalysisResults));
	functionAnalysisResults.functionAnalysisResults[functionAnalysisResults.size] = result;
	++functionAnalysisResults.size;
}

void freeFunctionAnalysisResult(functionAnalysisResult* a)
{
	free(a->functionName);
	free(a->definitionFileName);
	free(a->prototypeFileName);
	if (a->functionUsages)
	{
		for (int i = 0; i < a->usagesAmount; ++i)
		{
			free(a->functionUsages[i].fileName);
		}
		free(a->functionUsages);
	}
}
functionAnalysisResult* getFunctionAnalysisResult(char* functionName)
{
	addFunctionAnalysisResult(functionName);
	return &functionAnalysisResults.functionAnalysisResults[getFunctionIndex(functionName)];
}


void store_add_call(char* functionName, int line, char* fileName)
{
	functionAnalysisResult* a = getFunctionAnalysisResult(functionName);
	addFunctionUsage(&a->functionUsages, &a->usagesAmount, fileName, line);
}

void store_add_proto(char* functionName, int line, char* fileName)
{
	functionAnalysisResult* a = getFunctionAnalysisResult(functionName);
	a->lineWithPrototype = line;
	a->prototypeFileName = malloc((strlen(fileName) + 1) * sizeof(*a->prototypeFileName));
	strcpy(a->prototypeFileName, fileName);
}

void store_add_def(char* functionName, int line, char* fileName)
{
	functionAnalysisResult* a = getFunctionAnalysisResult(functionName);
	a->lineWithDefinition = line;
	a->definitionFileName = malloc((strlen(fileName) + 1) * sizeof(*a->definitionFileName));
	strcpy(a->definitionFileName, fileName);
}

void printFunctionAnalysisResult(void)
{
	for (int i = 0; i < functionAnalysisResults.size; i++)
	{
		printf("funkcja %s\n", functionAnalysisResults.functionAnalysisResults[i].functionName);

		printf("\tprototyp: %s:%d\n", functionAnalysisResults.functionAnalysisResults[i].prototypeFileName,
			functionAnalysisResults.functionAnalysisResults[i].lineWithPrototype + 1);

		printf("\tdefinicja: %s:%d\n", functionAnalysisResults.functionAnalysisResults[i].definitionFileName,
			functionAnalysisResults.functionAnalysisResults[i].lineWithDefinition + 1);

		printf("\tuzycia (%d):\n", functionAnalysisResults.functionAnalysisResults[i].usagesAmount);

		for (int j = 0; j < functionAnalysisResults.functionAnalysisResults[i].usagesAmount; j++)
		{
			printf("\t\t%d: %s:%d\n", j + 1, functionAnalysisResults.functionAnalysisResults[i].functionUsages[j].fileName,
				functionAnalysisResults.functionAnalysisResults[i].functionUsages[j].line + 1);
		}
	}
}

void analizatorSkladni(char* inpname)
{                               // przetwarza plik inpname

	FILE* in = fopen(inpname, "r");

	int nbra = 0;   // bilans nawiasów klamrowych {}
	int npar = 0;   // bilans nawiasów zwykłych ()

	alex_init4file(in);          // ustaw analizator leksykalny, aby czytał in

	functionAnalysisResults.functionAnalysisResults = NULL;

	lexem_t lex = alex_nextLexem();      // pobierz następny leksem
	while (lex != EOFILE) {
		switch (lex) {
		case IDENT: {
			char* iname = alex_ident();   // zapamiętaj identyfikator i patrz co dalej
			lexem_t nlex = alex_nextLexem();
			if (nlex == OPEPAR) {   // nawias otwierający - to zapewne funkcja
				npar++;
				put_on_fun_stack(npar, iname);       // odłóż na stos funkcji
				free(iname);						 // stos f. jest niezbędny, aby poprawnie obsłużyć sytuacje typu// f1( 5, f2( a ), f3( b ) )
			}
			else {                  // nie nawias, czyli nie funkcja
				lex = nlex;
				free(iname);
				continue;
			}
		}
				  break;
		case OPEPAR:
			npar++;
			break;
		case CLOPAR: {              // zamykający nawias - to może być koniec prototypu, nagłówka albo wywołania
			if (top_of_fun_stack() == npar) {       // sprawdzamy, czy liczba nawiasów bilansuje się z wierzchołkiem stosu funkcji
													// jeśli tak, to właśnie wczytany nawias jest domknięciem nawiasu otwartego
													// za identyfikatorem znajdującym się na wierzchołku stosu
				lexem_t nlex = alex_nextLexem();     // bierzemy nast leksem
				char* functionNameBuf = get_from_fun_stack();
				if(!functionNameBuf)
				{
					break;
				}
				if (nlex == OPEBRA)   // nast. leksem to klamra a więc mamy do czynienia z def. funkcji
				{
					++nbra;
					store_add_def(functionNameBuf, alex_getLN(), inpname);
				}
				else if (nbra == 0)   // nast. leksem to nie { i jesteśmy poza blokami - to musi być prototyp
					store_add_proto(functionNameBuf, alex_getLN(), inpname);
				else                  // nast. leksem to nie { i jesteśmy wewnątrz bloku - to zapewne wywołanie
					store_add_call(functionNameBuf, alex_getLN(), inpname);
				free(functionNameBuf);
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
	printFunctionAnalysisResult();
	fclose(in);
	for (int i = 0; i < functionAnalysisResults.size; i++)
	{
		freeFunctionAnalysisResult(functionAnalysisResults.functionAnalysisResults + i);
	}
	free(functionAnalysisResults.functionAnalysisResults);
	functionAnalysisResults.size = 0;
}
