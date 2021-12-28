#include "teststack.h"
#include "fun_stack.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void test_top_of_fun_stack(void) {
	int par = 7;
	char* n = "fun1";

	printf("top_of_fun_stack() test:\n");

	put_on_fun_stack(par, n);

	if (top_of_fun_stack() == par)
		printf("test success\n\n");
	else printf("test failure\n\n");
}

void test_put_on_fun_stack(void) {
	int par = 7;
	char* n = "fun2";

	printf("put_on_fun_stack() test:\n");

	put_on_fun_stack(par, n);

	if ((top_of_fun_stack() == par) && strcmp(get_from_fun_stack(), n) == 0)
		printf("test success\n\n");
	else
		printf("test failure\n\n");
}

void test_get_from_fun_stack(void) {
	int par1 = 7;
	int par2 = 14;
	char* n1 = "fun3";
	char* n2 = "fun4";

	printf("get_from_fun_stack() tests:\n");

	put_on_fun_stack(par1, n1);

	if ((top_of_fun_stack() == par1) && strcmp(get_from_fun_stack(), n1) == 0)
		printf("test 1 success\n");
	else
		printf("test 1 failure\n");

	put_on_fun_stack(par2, n2);

	if ((top_of_fun_stack() == par1) || strcmp(get_from_fun_stack(), n2) != 0)
		printf("test 2 failure\n\n");
	else
		printf("test 2 success\n\n");
}