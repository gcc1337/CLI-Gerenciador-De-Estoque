#ifndef __LISTPRODUCT__H
#define __LISTPRODUCT__H
#include <stdio.h>
#include "Estoque.h"

typedef struct listDesc ListProduct;

typedef Product ListType;
typedef struct listElem ListElem;

ListProduct* list_product_create();

int list_product_insert(ListProduct* l, Product* p);

void list_product_add_at_end(ListProduct* l, Product* p);
void list_product_add_at_beginning(ListProduct* l, Product* p);
void list_product_add_at_position(ListProduct* l, int possition, Product* p);

int list_product_remove_at_position(ListProduct* l, int possition);
int list_product_remove_by_id(ListProduct* l, int id);

Product* list_product_get_at_position(ListProduct* l, int possition);

void list_product_empty(ListProduct* l);
int list_product_get_qtd(ListProduct* l);

#endif