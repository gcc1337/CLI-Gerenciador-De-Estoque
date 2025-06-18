#include "listProduct.h"
#include "Estoque.h"
#include <stdlib.h>
#include <stdio.h>

struct listDesc{
    ListElem* first;
    ListElem* last;
    int qtd;
};

struct listElem{
    Product* p;
    ListElem* nodeNext;
    ListElem* nodePrev;
};

ListElem* ListElem_get_position(ListProduct* l, int position);
int listElem_create_object_in_position(ListProduct* l, Product* p, int position);
int listElem_remove_object_in_position(ListProduct* l, int position);

ListProduct* list_product_create(){
    ListProduct* l = (ListProduct*)malloc(sizeof(ListProduct));
    
    l->first = NULL;
    l->last = NULL;
    l->qtd = 0;

    return l;
}

int list_product_insert(ListProduct* l, Product* p){
    if(l == NULL || p == NULL){
        return -1;
    }
    ListElem* holdData = (ListElem*)malloc(sizeof(ListElem));
    if(holdData == NULL){
        return -1;
    }

    holdData->p = p;
    holdData->nodeNext = NULL;

    if(l->qtd == 0){
        l->first = holdData;
        l->last = holdData;
        holdData->nodePrev = NULL;
    }else{
        l->last->nodeNext = holdData;
        holdData->nodePrev = l->last;
        l->last = holdData;
    }
    ++l->qtd;

    return 0;
}

int list_product_remove_by_id(ListProduct* l, int id){
    if(l == NULL){
        return -1;
    }

    ListElem* hold = l->first;
    for(int i = 1; i <= l->qtd; ++i){
        if(product_get_id(hold->p) == id){
            listElem_remove_object_in_position(l, i);
            --l->qtd;
            return id;
        }
        hold = hold->nodeNext;
    }
    return -2;
}

void list_product_add_at_beginning(ListProduct* l, Product* p){
    ListElem* holdData = (ListElem*)malloc(sizeof(ListElem));
    holdData->p = p;
    holdData->nodePrev = NULL;

    if(l->qtd == 0){
        l->first = holdData;
        l->last = holdData;
        holdData->nodeNext = NULL;
    }else{
        l->first->nodePrev = holdData;
        holdData->nodeNext = l->first;
        l->first = holdData;
    }
    ++l->qtd;
}

void list_product_add_at_position(ListProduct* l, int position, Product* p){
    if(position > l->qtd || position <= 0){
        return ;
    }
    
    listElem_create_object_in_position(l, p, position);

    ++l->qtd;
}

int list_product_remove_at_position(ListProduct* l, int position){
    if(position > l->qtd || position <= 0 || l->qtd == 0){
        printf("invalid position\n");
        return -1;
    }

    listElem_remove_object_in_position(l,position);
    --l->qtd;

    return 0;
}

Product* list_product_get_at_position(ListProduct* l, int position){
    return (ListElem_get_position(l, position))->p;
}

void list_product_empty(ListProduct* l){
    ListElem* hold;

    for(hold = l->first; hold; hold = l->first){
        free(product_get_name(hold->p));
        free(product_get_category(hold->p));
        free(hold->p);
        l->first = hold->nodeNext;
        free(hold);
    }
    l->first = NULL;
    l->last = NULL;
    l->qtd = 0;
}

int list_product_get_qtd(ListProduct* l){
    if(l){
        return l->qtd;
    }   
    return -1;
}

int listElem_create_object_in_position(ListProduct* l, Product* p, int position){
    ListElem* hold = (ListElem*)malloc(sizeof(ListElem));
    hold->p = p;

    if(hold == NULL || l == NULL){
        return -1;
    }

    //adiciona ele na posição '2', coloca ele entre o primeiro e o 2(que agora é o terceiro)
    if(position == 1 ){  
        hold->nodePrev = l->first;
        hold->nodeNext = l->first->nodeNext;
        (l->first->nodeNext)->nodePrev = hold;
        l->first->nodeNext = hold;

    //adiciona ele na penultima posição, coloca ele entre o ultimo e o penultimo(que agora é o antepenultimo)    
    }else if(l->qtd == position){
        hold->nodePrev = l->last->nodePrev;
        hold->nodeNext = l->last;
        (l->last->nodePrev)->nodeNext = hold;
        l->last->nodePrev = hold;

    //adciona a um lugar que não é nenhum dos dois casos anterior, no 'meio' da lista entre dois outros
    }else{
        ListElem* tempPtr1 = ListElem_get_position(l, position);
        hold->nodeNext = tempPtr1;
        hold->nodePrev = tempPtr1->nodePrev;
        (tempPtr1->nodePrev)->nodeNext = hold;
        tempPtr1->nodePrev = hold;
    }
    return 0;
}

int listElem_remove_object_in_position(ListProduct* l, int position){
    if(l == NULL){
        return -1;
    }

    if (l->qtd == 1) {
        Product* p = l->first->p;
        free(product_get_name(p));
        free(product_get_category(p));
        free(p);
        free(l->first);
        l->first = NULL;
        l->last = NULL;
        return 0;
    }

    ListElem* tempPtr1 = ListElem_get_position(l, position);

    //ajuste de ponteiro se ele tiver no inicio da lista
    if(position == 1){
        (tempPtr1->nodeNext)->nodePrev = NULL;
        l->first = tempPtr1->nodeNext;

    //ajuste de ponteiro se ele tiver no fim da lista
    }else if(position == l->qtd){
        (tempPtr1->nodePrev)->nodeNext = NULL;
        l->last = tempPtr1->nodePrev;

    //ajuste de ponteiro se ele tiver no meio da lista
    }else{
        (tempPtr1->nodePrev)->nodeNext = tempPtr1->nodeNext;
        (tempPtr1->nodeNext)->nodePrev = tempPtr1->nodePrev;
    }

    Product* p = tempPtr1->p;
    free(product_get_name(p));
    free(product_get_category(p));
    free(p);
    
    free(tempPtr1);

    return 0;
}

ListElem* ListElem_get_position(ListProduct* l, int position){
    int i;
    ListElem* tempPtr = l->first;

    if(l->qtd > position/2){
        for(i = 1; i < position ; ++i){
            tempPtr = tempPtr->nodeNext;
        }
    }else{
        tempPtr = l->last;

        for(i = 1; i < position ; ++i){
            tempPtr = tempPtr->nodePrev;
        }
    }
    return tempPtr;
} 