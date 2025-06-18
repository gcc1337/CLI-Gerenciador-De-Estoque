#ifndef __UTILSCLI_H
#define __UTILSCLI_H
#include "Estoque.h"

void cli_show_menu_file(Estoque* e);
void cli_show_menu_estoque_management(Estoque* e);
void cli_show_product_table(Estoque* e);
Product* cli_get_product(Estoque* e);
void cli_refresh(Estoque* e, int model);
Product* cli_create_product(Estoque* e);
int cli_buy_get_qtd(Estoque* e);
int cli_sell_get_qtd(Estoque* e, Product* p);
char* cli_get_file_name(Estoque *e);

void cli_hide_cursor();
void cli_show_cursor();

#endif 