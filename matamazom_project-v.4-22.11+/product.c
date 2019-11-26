//
// Created by eilon on 17/11/2019.
//

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "product.h"
#include <assert.h>

struct product_t{

    unsigned int ID;
    double amount;
    char *name;
    MatamazomAmountType amountType;
    MtmProductData customData;
    MtmCopyData CopyFunc;
    MtmFreeData FreeFunc;
    MtmGetProductPrice ProductPriceFunc;
    double profit;
};

Product productCreate(unsigned int id, char* name, double amount, MatamazomAmountType datatype,
                      MtmProductData customData, MtmCopyData CopyFunc, MtmFreeData FreeFunc,
                      MtmGetProductPrice ProductPriceFunc){

    if(CopyFunc == NULL || FreeFunc == NULL || ProductPriceFunc == NULL || customData == NULL){
        return NULL;
    }

    Product product = malloc(sizeof(*product));
    if (product == NULL){
        return NULL;
    }
    product -> name = malloc(sizeof(strlen(name)));
    if (name == NULL){
        freeProduct(product);
        return NULL;
    }
    strcpy(product -> name, name);
    product -> ID = id;
    product -> amount = amount;
    product -> amountType = datatype;
    product -> customData = CopyFunc(customData);
    product -> CopyFunc = CopyFunc;
    product -> FreeFunc = FreeFunc;
    product -> ProductPriceFunc = ProductPriceFunc;
    product -> profit = 0;
    return product;
}

/*
Product getPtrToProductForID (List storage ,unsigned int id) { // 777

    for (ListElement ptr = listGetFirst(storage); ptr; ptr = listGetNext(storage)) {
        if ((((Product)ptr)->ID) == id){
            return ((Product)ptr);
        }
    }
    return NULL;
} */ //illigal, cancels out encapsulation

static Product findProductForID (List storage, unsigned int id){

    for (ListElement ptr = listGetFirst(storage); ptr ; ptr = listGetNext(storage)){
        if(((Product)ptr) -> ID == id){
            return ptr;
        }
    }
    return NULL;
}

void freeProduct(ListElement product){

    free(((Product)product)->name);
    ((Product)product)->FreeFunc(((Product)product)->customData);
    free(product);
}

void productRemove (List storage, unsigned int id){
    Product ptr = ((Product)findProductForID(storage, id)); //set iterator to wanted product and find the product
    listRemoveCurrent(storage);
    freeProduct(ptr);
}

static Product copyProductAUX(Product product){
    Product product_new = productCreate(product->ID, product->name, product->amount,
                                        product->amountType, product->customData, product->CopyFunc,
                                        product->FreeFunc, product->ProductPriceFunc);
    return product_new;
}

ListElement copyProduct (ListElement product) {
    return copyProductAUX(product);
}

int compareProduct(ListElement product1, ListElement product2){
    return (int)((((Product)product1)->ID) - (((Product)product2)->ID));
}

double realProductPrice (ListElement product, double amount){
    return (((Product)product) -> ProductPriceFunc (((Product)product)->customData, amount));
}

double productGetPrice (ListElement product){
    return (((Product)product) -> ProductPriceFunc (((Product)product)->customData, ((Product)product)->amount));
}

void changeProfitForGivenAmountSold (ListElement product, double amount){
    ((Product)product)->profit += realProductPrice(product, amount);
}

void productPrintDetails (ListElement product, FILE *output) {
    //product and output were already checked if null before sent
    double priceTemp = ((Product)product)->
                         ProductPriceFunc(((Product)product)->customData,((Product)product) -> amount);

    mtmPrintProductDetails(((Product)product)->name, ((Product)product)->ID,
                           ((Product)product) -> amount, priceTemp, output); // 777 maybe we need to check the funcs return SUCCESS
                                                                            //666this is a void func, given to us at matamazom_print.c
}

bool productAlreadyExists(List storage, unsigned int id){

    for (ListElement ptr = listGetFirst(storage); ptr ; ptr = listGetNext(storage)){
        if(((Product)ptr) -> ID == id){
            return true;
        }
    }
    return false;
}

bool findTheProductAfterTheNewAndSetCurrentToIt (List storage, ListElement product_new){
 /// 777 does not work
 // 666 modified
    for (ListElement ptr = listGetFirst(storage); ptr; ptr = listGetNext(storage)){
        if ( ( ((Product)product_new)->ID ) < ( ((Product)ptr)->ID ) ){
            return true;
        }
    }
    return false;
}

MatamazomResult productChangeAmount(List storage,unsigned int id, double amount){

    Product  ptr = findProductForID(storage, id);
    if ( ( (((Product)ptr) -> amount) + amount) < 0){
        return MATAMAZOM_INSUFFICIENT_AMOUNT;
    } else{
        ((Product)ptr) -> amount += amount;
        return MATAMAZOM_SUCCESS;
    }

}

MatamazomAmountType productGetAmountType(List storage, unsigned int id){

    Product ptr = findProductForID(storage, id);
    assert(ptr);
    return ((Product)ptr)->amountType;
}

double productGetAmount (ListElement product){
    return ((Product)product)->amount;
}


unsigned int productGetId (ListElement product){
    return ((Product )product)->ID;
}

double getCurrentProfitOfProduct(ListElement product) {
    return ((Product )product) -> profit;
}

bool productCustomFilter (ListElement product, MtmFilterProduct customFilter){

    return  customFilter((((Product)product)->ID), ((Product)product)->name,
                           ((Product)product) -> amount, ((Product)product)->customData);

}

static double maxOfTwo (double x, double y){
    return x > y ? x : y;
}

void productPrintIncomeLine (List storage, FILE *output){

    double max = 0;
    for (ListElement ptr = listGetFirst(storage); ptr; ptr = listGetNext(storage)){
        max = maxOfTwo(max, ((Product)ptr)->profit);
    }
    for (ListElement ptr = listGetFirst(storage); ptr; ptr = listGetNext(storage)){
        if ((((Product)ptr)->profit) == max){
            mtmPrintIncomeLine(((Product)ptr)->name, ((Product)ptr)->ID, max, output);
        }
    }

}

MatamazomResult addProductToList (List list, Product product) {

    if (product == NULL || list == NULL) {
        return MATAMAZOM_NULL_ARGUMENT;
    }

    if (productAlreadyExists(list, product->ID)){
        return MATAMAZOM_PRODUCT_ALREADY_EXIST;
    }

    if (listGetFirst(list) == NULL){
        ListResult check = listInsertFirst(list, ((ListElement)list));

        if(check == LIST_OUT_OF_MEMORY){
            return MATAMAZOM_OUT_OF_MEMORY;
        } else{
            assert(check == LIST_SUCCESS);
            return MATAMAZOM_SUCCESS;
        }
    } else {
        findTheProductAfterTheNewAndSetCurrentToIt (list, product);

        ListResult check = listInsertAfterCurrent(list, product);

        if(check == LIST_OUT_OF_MEMORY){
            return MATAMAZOM_OUT_OF_MEMORY;
        } else{
            assert(check == LIST_SUCCESS);
            return MATAMAZOM_SUCCESS;
        }
    }
}
