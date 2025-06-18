#include "utilsCLI.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "Estoque.h"
#include "listProduct.h"

typedef enum {
    BLACK = 0,
    BLUE = 1,
    GREEN = 2,
    CYAN = 3,
    RED = 4,
    PURPLE = 5,
    YELLOW = 6,
    WHITE = 7,
    GRAY = 8,
    LIGHT_BLUE = 9,
    LIGHT_GREEN = 10,
    LIGHT_CYAN = 11,
    LIGHT_RED = 12,
    LIGHT_PURPLE = 13,
    LIGHT_YELLOW = 14,
    BRIGHT_WHITE = 15
} TerminalColor;

int isKeyPressedCLI(char key) {
    static SHORT lastState[256] = {0};
    SHORT currentState = GetAsyncKeyState(key);
    int pressed = (currentState & 0x8000) && !(lastState[key] & 0x8000);
    lastState[key] = currentState;
    return pressed;
}

TerminalColor set_terminal_color(TerminalColor backgroundColor, TerminalColor textColor,HANDLE hConsole);
void print_separator(int columns[], int num_columns);
void print_row(char *data[], int columns[], int num_columns);
void print_header(int num_columns);
void print_product(Product* p, int columns[], int num_columns);
void print_options(Estoque* e, int model);
void move_arrow_up(int pos);
void move_arrow_down(int pos);
void move_arrow(int pos);
char* get_string(int len);


void cli_show_product_table(Estoque* e) {
    int margin[] = {5, 20, 10, 10, 15, 15};
    print_header(6);

    if(e == NULL){
        return;
    }

    int qtd = estoque_get_products_registered_qtd(e);
    ListProduct* l = (ListProduct*)estoque_get_products_list(e);
    if(l == NULL){
        return;
    }

    for(int i = 1; i <= qtd; i++) {
        Product* p = list_product_get_at_position(l, i);
        print_product(p, margin, 6);
    }

    print_separator(margin, 6);

}

TerminalColor set_terminal_color(TerminalColor backgroundColor, TerminalColor textColor,HANDLE hConsole) {
    int cor = backgroundColor * 16 + textColor ;
    SetConsoleTextAttribute(hConsole, cor);

    return textColor;
}

void print_separator(int columns[], int num_columns) {
    printf("+");
    for (int i = 0; i < num_columns; i++) {
        for (int j = 0; j < columns[i] + 2; j++) {
            printf("-");
        }
        printf("+");
    }
    printf("\n");
}

void print_row(char *data[], int columns[], int num_columns) {
    for (int i = 0; i < num_columns; i++) {
        int len = strlen(data[i]);
        int padding = (columns[i] - len) / 2;
        printf("| %*s%-*s ", padding, "", columns[i] - padding, data[i]);
    }
    printf("|\n");
}

void print_header(int num_columns) {
    char *header[] = {"ID", "Nome", "Compra", "Venda", "Quantidade", "Categoria"};
    int margin[] = {5, 20, 10, 10, 15, 15};

    print_separator(margin, num_columns);
    print_row(header, margin, num_columns);
    print_separator(margin, num_columns);
}

void print_product(Product* p, int columns[], int num_columns) {
    char id[10] = {0};
    char purchasePrice[15] = {0};
    char salePrice[15]  = {0};
    char qtd[15] = {0};
    sprintf(id, "%d", product_get_id(p));
    sprintf(purchasePrice, "%.5g", product_get_purchase_price(p));
    sprintf(salePrice, "%.5g", product_get_sale_price(p));
    sprintf(qtd, "%d", product_get_qtd(p));

    char *data[] = {id, product_get_name(p), purchasePrice, salePrice, qtd, product_get_category(p)};
    print_row(data, columns, num_columns);
}

Product* cli_get_product(Estoque* e) {
    if (e == NULL) {
        return NULL;
    }

    cli_refresh(e,2);

    int minQtd = 3;
    int maxQtd = estoque_get_products_registered_qtd(e) + 2;

    int qtd = minQtd;  
    move_arrow(qtd);  

    while (1) {
        if (isKeyPressedCLI(VK_UP)) {
            if (qtd > minQtd) {  
                move_arrow_up(qtd);
                qtd--;  
            }
        }
        if (isKeyPressedCLI(VK_DOWN)) {
            if (qtd < maxQtd) {  
                move_arrow_down(qtd);
                qtd++;  
            }
        }
        if (isKeyPressedCLI(VK_RETURN)) {   
            break;
        }
    }

    Product* p = list_product_get_at_position(estoque_get_products_list(e),qtd - 2);

    return p;
}


void cli_refresh(Estoque* e, int model){
    system("cls");

    if(e == NULL){

        char *estoqueNULL[] = {"Abra um Estoque usando 'A' ou 'C'."};

        print_header(6);
        print_row(estoqueNULL,(int[]){90},1);
        print_separator((int[]){90}, 1);

    }

    if(e != NULL && estoque_get_products_registered_qtd(e) == 0){
        char *estoqueEmpty[] = {"Registre produtos usando '1'."};

        print_header(6);
        print_row(estoqueEmpty,(int[]){90},1);
        print_separator((int[]){90}, 1);

    }

    if(e != NULL && estoque_get_products_registered_qtd(e) > 0){
        cli_show_product_table(e);
    }

    print_options(e,model);

}

void print_options(Estoque* e, int model){
    char open[] =   "A - Abrir arquivo";
    char create[] = "C - Criar arquivo";
    char save[] =   "S - Salvar arquivo";
    char close[] =  "F - Fechar arquivo";

    char registry[]="1 - Registrar um produto";
    char remove[]=  "2 - Remover um produto";
    char buy[]=     "3 - Comprar produto(s)";
    char sell[]=    "4 - Vender produto(s)";

    char up[]=      "↑ - Subir";
    char down[]=    "↓ - Descer";
    char select[]=  "Enter - Selecionar";

    printf("\n\n");
    if(e == NULL && model == 1){
        printf("● %s\t\t",open);
        printf("● %s\t\t",create);
    }
    
    if(e != NULL && model == 1){
        printf("● %s\t\t",save);
        printf("● %s\t\t",close);
    }

    if(e != NULL && model == 2){
        printf("● %s\t",up);
        printf("● %s\t",down);
        printf("● %s\t",select);
    }

    printf("\n");
    print_separator((int[]){26,26}, 2);
    printf("\n");

    if(e != NULL && estoque_get_products_registered_qtd(e) > 0 && model == 1){
        printf("● %s\t",registry);
        printf("● %s\n",remove);
        printf("● %s\t",buy);
        printf("● %s\n",sell);
        print_separator((int[]){26,26}, 2);

    }
    if(e != NULL && estoque_get_products_registered_qtd(e) == 0 && model == 1){
        printf("● %s\n",registry);
        print_separator((int[]){26,26}, 2);
    }

}


void move_arrow_up(int pos) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD written;

    // Limpa a posição anterior
    COORD cord1 = {94, pos};
    SetConsoleCursorPosition(hConsole, cord1);
    WriteConsoleOutputCharacter(hConsole, "   ", 3, cord1, &written);

    // Move para a nova posição (para cima)
    COORD cord = {94, pos - 1};
    SetConsoleCursorPosition(hConsole, cord);
    WriteConsoleOutputCharacter(hConsole, "<--", 3, cord, &written);
}

void move_arrow_down(int pos) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD written;

    // Limpa a posição anterior
    COORD cord1 = {94, pos};
    SetConsoleCursorPosition(hConsole, cord1);
    WriteConsoleOutputCharacter(hConsole, "   ", 3, cord1, &written);

    // Move para a nova posição (para baixo)
    COORD cord = {94, pos + 1};
    SetConsoleCursorPosition(hConsole, cord);
    WriteConsoleOutputCharacter(hConsole, "<--", 3, cord, &written);
}

void move_arrow(int pos) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD written;

    // Move para a posição inicial
    COORD cord = {94, pos};
    SetConsoleCursorPosition(hConsole, cord);
    WriteConsoleOutputCharacter(hConsole, "<--", 3, cord, &written);
}

void cli_hide_cursor() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.dwSize = 1;  // Tamanho do cursor (1 é o menor tamanho)
    cursorInfo.bVisible = FALSE;  // Torna o cursor invisível
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

void cli_show_cursor() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.dwSize = 1;
    cursorInfo.bVisible = TRUE;  // Torna o cursor visível novamente
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

Product* cli_create_product(Estoque* e){
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);

    char name[20] = {0};
    double purchasePrice = 0.0;    
    double salePrice = 0.0;
    char category[15] = {0};

    char productName[]=       "Nome do Produto: ";
    char purchase[]=     "Preço de compra do Produto: ";
    char sale[]=         "Preço de venda do Produto: ";
    char productCategory[]=   "Categoria do Produto: ";

    int ch;

    fflush(stdin);
    rewind(stdin);

    FlushConsoleInputBuffer(hStdin);
    cli_refresh(e,1);
    printf("\n● %s", productName);
    strcpy(name,get_string(19));


    FlushConsoleInputBuffer(hStdin);
    cli_refresh(e,1);
    printf("\n● %s", purchase);
    scanf("%lf",&purchasePrice);

    FlushConsoleInputBuffer(hStdin);
    if(purchasePrice == 0){
        return NULL;
    }
    cli_refresh(e,1);
    printf("\n● %s", sale);
    scanf("%lf",&salePrice);

    FlushConsoleInputBuffer(hStdin);
    if(salePrice == 0){
        return NULL;
    }
    getchar();
    cli_refresh(e,1);
    printf("\n● %s", productCategory);
    strcpy(category,get_string(14));

    Product* p = product_create(name,purchasePrice,salePrice,category);
    return p;
}

char* get_string(int len) {
    char* buf = (char*)malloc(sizeof(char)* len + 1);  
    int i = 0;
    int ch;

    fgets(buf, len, stdin);

    // Remover o '\n' que o fgets captura quando pressiona Enter
    if (buf[strlen(buf) - 1] == '\n') {
        buf[strlen(buf) - 1] = '\0';
    }

    fflush(stdin);

    return buf;
}

int cli_buy_get_qtd(Estoque* e){
    char buy[] =   "Comprar: ";

    int qtd = 0;
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);


    cli_refresh(e,1);
    printf("\n● %s", buy);
    fflush(stdin);
    rewind(stdin);
    FlushConsoleInputBuffer(hStdin);

    scanf("%d",&qtd);

    fflush(stdin);
    rewind(stdin);

    if(qtd <= 0){
        return -1;
    }

    return qtd;

}
int cli_sell_get_qtd(Estoque* e, Product* p){
    if(p == NULL){
        return -1;
    }
    char sell[] =   "Vender: ";
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);

    
    int qtd = 0;
    int maxQtd = 0;

    maxQtd = product_get_qtd(p);

    FlushConsoleInputBuffer(hStdin);
    cli_refresh(e,1);
    printf("\n● %s", sell);
    fflush(stdin);
    rewind(stdin);
    FlushConsoleInputBuffer(hStdin);
    scanf("%d",&qtd);
    getchar();

    if(qtd <= 0 || qtd > maxQtd){
        return -1;
    }

    return qtd;
}

char* cli_get_file_name(Estoque *e){
    char fileName[] =   "Nome do Arquivo: ";

    char* buf = (char*)malloc(sizeof(char)* 40);
    memset(buf, 0, 40);
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);

    fflush(stdin);
    rewind(stdin);

    FlushConsoleInputBuffer(hStdin);
    cli_refresh(e,1);
    printf("\n● %s", fileName);
    strcpy(buf,get_string(39));

    return buf;
}