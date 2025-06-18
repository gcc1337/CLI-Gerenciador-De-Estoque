#include <stdio.h>
#include <Windows.h>
#include "locale.h"
#include "Estoque.h"
#include "utilsCLI.h"

int isKeyPressed(char key) {
    static SHORT lastState[256] = {0};
    SHORT currentState = GetAsyncKeyState(key);
    int pressed = (currentState & 0x8000) && !(lastState[key] & 0x8000);
    lastState[key] = currentState;
    return pressed;
}

int isEstoqueOpen(Estoque* e) {
    if (e == NULL) {
        return 0;
    }
    return 1;
}

int main() {
    setlocale(LC_ALL, "pt_BR.UTF-8");
    SetConsoleOutputCP(CP_UTF8);
    Estoque* e = NULL;
    Product* p = NULL;
    int qtd = 0;
    char* fileName = NULL;

    cli_hide_cursor();
    cli_refresh(e,1);

    while (1) {
        if (isKeyPressed('A')) {
            if(e == NULL){
                e = estoque_open();
                if (e == NULL) {
                    printf("Erro ao abrir o estoque\n");
                    Sleep(500);
                }
                cli_refresh(e,1);
            }
        }

        if (isKeyPressed('C')) {
            if(e == NULL){
                fileName = cli_get_file_name(e);
                e = estoque_create_and_open(fileName);
                free(fileName);
                if (e == NULL) {
                    printf("Erro ao criar o estoque\n");
                    Sleep(500);
                }
                cli_refresh(e,1);

            }   
        }

        if (isKeyPressed('F')) {
            if (isEstoqueOpen(e)) {
                if (estoque_close(e) == 0) {
                    e = NULL;
                    p = NULL;
                    cli_refresh(e,1);
                }
            }
        }

        if (isKeyPressed('S')) {
            if (isEstoqueOpen(e)) {
                if (estoque_save(e) == 0) {
                    printf("Estoque salvo\n");
                    Sleep(500);
                    cli_refresh(e,1);
                }
            }
        }

        if (isKeyPressed('1')) {
            if (isEstoqueOpen(e)) {
                cli_show_cursor();
                p = cli_create_product(e);
                if(p != NULL){
                    estoque_register_product(e,p);
                }
                cli_refresh(e,1);
                cli_hide_cursor();
            }

        }

        if (isKeyPressed('2')) {
            if (isEstoqueOpen(e) && estoque_get_products_registered_qtd(e) != 0) {
                p = cli_get_product(e);
                if (estoque_remove_product(e, p) == 0) {
                    cli_refresh(e,1);
                }
            }
        }

        if (isKeyPressed('3')) {
            if (isEstoqueOpen(e) && estoque_get_products_registered_qtd(e) != 0) {
                p = cli_get_product(e);
                cli_show_cursor();
                qtd = cli_buy_get_qtd(e);
                if(qtd != -1){
                    estoque_product_purchase(e,p,qtd);
                }
                cli_refresh(e,1);
                cli_hide_cursor();
            }
        }

        if (isKeyPressed('4')) {
            if (isEstoqueOpen(e) && estoque_get_products_registered_qtd(e) != 0) {
                p = cli_get_product(e);
                cli_show_cursor();
                qtd = cli_sell_get_qtd(e,p);
                if(qtd != -1){
                    estoque_product_sell(e,p,qtd);
                }
                cli_refresh(e,1);
                cli_hide_cursor();
            }
        }
        Sleep(50);
    }

    return 0;
}