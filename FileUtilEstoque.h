#ifndef __FILEUTILESTOQUE__H
#define __FILEUTILESTOQUE__H
#include "stdio.h"
#include "Estoque.h"

FILE* estq_create(const char* fileName);
FILE* estq_open();
int estq_close(FILE* f);

#endif