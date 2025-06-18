#include "Estoque.h"
#include "listProduct.h"
#include "FileUtilEstoque.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct estoque{
    ListProduct* products;
    int productsRegisteredQtd;
    int productsTotalQtd;
    long double estoqueTotalPurchaseValue;
    long double estoqueTotalSaleValue;
    float gainPercent;
    char* filePath;
    int idCount;
    FILE* fd;
};

struct product{
    char* name;
    int nameSize;
    double purchasePrice;
    double salePrice;
    int id;
    int qtd;
    char* category;
    int categorySize;
};

void add_product_flag(FILE* f);
void add_log_flag(FILE* f);
int write_products_to_file(ListProduct* l, FILE* f);
int get_products_section(FILE* f);
int get_logs_section(FILE* f);
char* get_file_path(FILE* f);

Product* product_create(const char* name, double purchasePrice, double salePrice, const char* category){
    Product* p = (Product*)malloc(sizeof(Product));
    if(p == NULL){
        return NULL;
    }

    if((strlen(name) > 0) && (purchasePrice > 0) && (salePrice > 0) && (strlen(category) > 0)){
        static int id = 0;
        p->id = id++;
        p->name = (char*)malloc(sizeof(char) * (strlen(name) + 1));
        memset(p->name, 0, strlen(name) + 1);
        p->category = (char*)malloc(sizeof(char) * (strlen(category) + 1));
        memset(p->category, 0, strlen(category) + 1);    
        strcpy(p->name, name);
        p->purchasePrice = purchasePrice;
        p->salePrice = salePrice;
        p->qtd = 0;
        strcpy(p->category, category);
        p->nameSize = strlen(name);
        p->categorySize = strlen(category);

        return p;
    }

    return NULL;
}

Estoque* estoque_create_and_open(const char* fileName){
    Estoque* e = (Estoque*)malloc(sizeof(Estoque));
    if(e == NULL){
        return NULL;
    }

    FILE* f = estq_create(fileName);
    if(f == NULL){
        return NULL;
    }
            
    e->products = list_product_create();
    e->productsRegisteredQtd = 0;
    e->productsTotalQtd = 0;
    e->estoqueTotalPurchaseValue = 0;
    e->estoqueTotalSaleValue = 0;
    e->gainPercent = 0;
    e->filePath = get_file_path(f);
    e->idCount = 0;
    e->fd = f;

    rewind(f);

    fwrite(e, sizeof(Estoque), 1, f);

    add_product_flag(f);
    add_log_flag(f);

    rewind(f);

    return e;

}

Estoque* estoque_open(){
    FILE* f = estq_open();
    if(f == NULL){
        return NULL;
    }

    Estoque* e = (Estoque*)malloc(sizeof(Estoque));
    if(e == NULL){
        return NULL;
    }

    fread(e, sizeof(Estoque), 1, f);
    e->fd = f;
    e->filePath = get_file_path(f);
    e->products = list_product_create();

    fseek(f, get_products_section(f), SEEK_SET);
    for(int i = 0; i < e->productsRegisteredQtd; i++){
        Product* p = (Product*)malloc(sizeof(Product));
        if(p == NULL){
            return NULL;
        }
        fread(p, sizeof(Product), 1, f);
        if(p == NULL){
            return NULL;
        }
    
        p->name = (char*)malloc(sizeof(char) * p->nameSize + 1);
        memset(p->name, 0, p->nameSize + 1);
        if(p->name  == NULL){
            return NULL;
        }
        fread(p->name ,p->nameSize, 1, f);

        p->category = (char*)malloc(sizeof(char) * p->categorySize + 1);
        memset(p->category, 0, p->categorySize + 1);
        if(p->category  == NULL){
            return NULL;
        }

        fread(p->category ,p->categorySize, 1, f);
        list_product_insert(e->products, p);
    }

    e->fd = f;

    rewind(f);
    return e;
}

int estoque_save(Estoque* e) {
    if (e == NULL) {
        return -1;
    }

    if (e->fd == NULL) {
        return -1;
    }

    rewind(e->fd);

    char* backupPath = (char*)malloc(sizeof(char) * (strlen(e->filePath) + 5));
    memset(backupPath, 0, strlen(e->filePath) + 5);
    if (backupPath == NULL) {
        return -1;
    }

    strcpy(backupPath, e->filePath);
    strcat(backupPath, ".bak");
    remove(backupPath);
    fclose(e->fd);
    rename(e->filePath, backupPath);

    e->fd = fopen(e->filePath, "wb+");
    if (e->fd == NULL) {
        return -1;
    }

    fwrite(e, sizeof(Estoque), 1, e->fd);
    
    add_product_flag(e->fd);
    add_log_flag(e->fd);

    rewind(e->fd);
    if (fseek(e->fd, get_products_section(e->fd), SEEK_SET) == 0) {
        if (write_products_to_file(e->products, e->fd) != 0) {
            return -1;
        }
    } else {
        return -1;
    }

    add_log_flag(e->fd);

    rewind(e->fd);

    return 0;
}

int estoque_close(Estoque* e){
    if(e == NULL){
        return -1;
    }

    if(e->fd == NULL){
        return -1;
    }

    fclose(e->fd);
    free(e->filePath);
    list_product_empty(e->products);
    free(e->products);
    free(e);

    return 0;
}

int estoque_register_product(Estoque* e, Product* p){
    if(e == NULL || p == NULL){
        return -1;
    }
    
    p->id = e->idCount++;

    if(list_product_insert(e->products, p) == 0){
        e->productsRegisteredQtd++;
        e->productsTotalQtd += p->qtd;
        e->estoqueTotalPurchaseValue += p->purchasePrice * p->qtd;
        e->estoqueTotalSaleValue += p->salePrice * p->qtd;
        e->gainPercent = 1.00;
        return 0;
    }

    return -1;
}

int estoque_remove_product(Estoque* e, Product* p){
    if(e == NULL || p == NULL){
        return -1;
    }

    int id = product_get_id(p);
    int qtd = product_get_qtd(p);
    double purchasePrice = product_get_purchase_price(p);
    double salePrice = product_get_sale_price(p);

    if(list_product_remove_by_id(e->products,id) == id){
        e->productsRegisteredQtd--;
        e->productsTotalQtd -= qtd;
        e->estoqueTotalPurchaseValue -= purchasePrice * qtd;
        e->estoqueTotalSaleValue -= salePrice * qtd;
        e->gainPercent = 1.00;
        return 0;
    }

    return -1;
}

Product* estoque_get_product_at_position(Estoque* e,int position){
    if(e == NULL){
        return NULL;
    }

    return list_product_get_at_position(e->products, position);
}

int estoque_product_purchase(Estoque* e, Product* p, int qtd){
    if(e == NULL || p == NULL){
        return -1;
    }

    long double purchasePrice = product_get_purchase_price(p);
    long double salePrice = product_get_sale_price(p);
    p->qtd +=qtd;

    e->productsTotalQtd += qtd;
    e->estoqueTotalPurchaseValue += purchasePrice * qtd;
    e->estoqueTotalSaleValue += salePrice * qtd;
    e->gainPercent = 1.00;
    return 0;
    
}

int estoque_product_sell(Estoque* e, Product* p, int qtd){
    if(e == NULL || p == NULL){
        return -1;
    }

    long double purchasePrice = product_get_purchase_price(p);
    long double salePrice = product_get_sale_price(p);
    p->qtd -=qtd;

    e->productsTotalQtd -= qtd;
    e->estoqueTotalPurchaseValue -= purchasePrice * qtd;
    e->estoqueTotalSaleValue -= salePrice * qtd;
    e->gainPercent = 1.00;
    return 0;

}

void add_product_flag(FILE* f){
    short productFlag = -23;

    fwrite(&productFlag, sizeof(short), 1, f);
    fwrite("Produto", 1, 7, f);

}

void add_log_flag(FILE* f){
    short logFlag = -65;

    fwrite(&logFlag, sizeof(short), 1, f);
    fwrite("Log", 1, 3, f);
}

int write_products_to_file(ListProduct* l, FILE* f){
    if(f == NULL || l == NULL){
        return -1;
    }
    
    int qtd = list_product_get_qtd(l);
    for (int i = 1; i <= qtd; ++i) {
        Product* p = list_product_get_at_position(l, i);
        if (p == NULL) {
            return -1;
        }
        fwrite(p, sizeof(Product), 1, f);
        fwrite(p->name, 1, p->nameSize, f);
        fwrite(p->category, 1, p->categorySize, f);

    }

    return 0;
}

int get_products_section(FILE* f){
    if(f == NULL){
        return -1;
    }

    fseek(f, 0, SEEK_END);
    int fileSize = ftell(f), section;
    rewind(f);

    short holdFlag;
    char buffer[8] = {0};

    for (int i = 0; i < fileSize; i++){
        fread(&holdFlag, sizeof(short), 1, f);
        if (holdFlag == -23) {
            fread(buffer, 1, 7, f);
            buffer[7] = '\0';
            if (strcmp("Produto", buffer) == 0) {
                section = ftell(f);
                rewind(f);
                return section;
            }
        }
    }
    return -1;
}

int get_logs_section(FILE* f){
    if(f == NULL){
        return -1;
    }

    fseek(f, 0, SEEK_END);
    int fileSize = ftell(f), section;
    rewind(f);

    short holdFlag;
    char buffer[4] = {0};

    for (int i = 0; i < fileSize; i++){
        fread(&holdFlag, sizeof(short), 1, f);
        if (holdFlag == -65) {
            fread(buffer, 1, 3, f);
            buffer[3] = '\0';
            if (strcmp("Log", buffer) == 0) {
                section = ftell(f);
                rewind(f);
                return section;
            }
        }
    }
    return -1;
}

char* get_file_path(FILE* f){
    if(f == NULL){
        return NULL;
    }

    int pathSize;
    fseek(f, -4, SEEK_END);
    fread(&pathSize, sizeof(int), 1, f);
    rewind(f);

    char hold;
    char hold2;

    fseek(f, 0, SEEK_END);
    int fileSize = ftell(f);
    rewind(f);

    char* path = (char*)malloc(sizeof(char) * pathSize + 1);
    memset(path, 0, pathSize + 1);
    if(path == NULL){
        return NULL;
    }

    rewind(f);

    for(int i = 0; i < fileSize; i++){
        fread(&hold, sizeof(char), 1, f);
        if(hold > 'A' && hold < 'Z'){
            fread(&hold2, sizeof(char), 1, f);
            if(hold2 == ':'){
                path[0] = (char)hold;
                path[1] = ':';
                fread(&path[2], 1, pathSize - 2, f);
                return path;
            }
        }
    }

    return NULL;
}


/*         GETTERS          */


char* product_get_name(Product* p) {
    return p->name;
}

int product_get_name_size(Product* p) {
    return p->nameSize;
}

double product_get_purchase_price(Product* p) {
    return p->purchasePrice;
}

double product_get_sale_price(Product* p) {
    return p->salePrice;
}

int product_get_id(Product* p) {
    return p->id;
}

int product_get_qtd(Product* p) {
    return p->qtd;
}

char* product_get_category(Product* p) {
    return p->category;
}

int product_get_category_size(Product* p) {
    return p->categorySize;
}

void* estoque_get_products_list(Estoque* e) {
    return e->products;
}

int estoque_get_products_registered_qtd(Estoque* e) {
    return e->productsRegisteredQtd;
}

