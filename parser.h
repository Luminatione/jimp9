#ifndef PARSER_H
#define PARSER_H
#include "alex.h"       // analizator leksykalny
#include "fun_stack.h"  // stos funkcji

#include <string.h>

#define FUNTION_NOT_STORED -1

typedef struct functionUsage
{
	char* fileName;
	int line;
} functionUsage;

typedef struct functionIndexesAssociation
{
	char** names;
	int size;
} functionIndexesAssociation;

typedef struct functionAnalysisResult
{
	char* functionName;
	char* prototypeFileName;
	char* definitionFileName;

	int lineWithPrototype;
	int lineWithDefinition;

	functionUsage* functionUsages;
	int usgaesAmount;
} functionAnalysisResult;

typedef struct functionAnalysisResultArray
{
	functionIndexesAssociation functionsIndexes;
	functionAnalysisResult* functionAnalysisResults;
	int size;
} functionAnalysisResultArray;

functionUsage* createFunctionUsage(char* fileName, int line);

void addFunctionUsage(functionUsage** arr, int* size, char* fileName, int line);

int getFunctionIndex(char* name);

void addFunctionToIndexing(char* name);

void addFunctionAnalysisResult(char* functionName);

void freeFunctionAnalysisResult(functionAnalysisResult* a);

void analizatorSkladni(char* inpname);
#endif // PARSER_H
