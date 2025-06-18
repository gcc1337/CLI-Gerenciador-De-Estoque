#ifndef __ESTOQUE__H
#define __ESTOQUE__H
#include <stdio.h>

typedef struct estoque Estoque;
typedef struct product Product;

Product* product_create(const char* Name, double purchasePrice, double salePrice, const char* category);
Estoque* estoque_create_and_open(const char* fileName);
Estoque* estoque_open();
int estoque_save(Estoque* e);
int estoque_close(Estoque* e);

int estoque_register_product(Estoque* e, Product* p);
int estoque_remove_product(Estoque* e, Product* p);
Product* estoque_get_product_at_position(Estoque* e, int position);
int estoque_product_purchase(Estoque* e, Product* p, int qtd);
int estoque_product_sell(Estoque* e, Product* p, int qtd);

char* product_get_name(Product* p);
int product_get_name_size(Product* p);
double product_get_purchase_price(Product* p);
double product_get_sale_price(Product* p);
int product_get_id(Product* p);
int product_get_qtd(Product* p);
char* product_get_category(Product* p);
int product_get_category_size(Product* p);
void* estoque_get_products_list(Estoque* e);
int estoque_get_products_registered_qtd(Estoque* e);

#endif