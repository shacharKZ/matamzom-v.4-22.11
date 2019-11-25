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


static Product copyProductAUX(Product product){
    Product product_new = productCreate(product->ID, product->name, product->amount,
                                        product->amountType, product->customData, product->CopyFunc,
                                        product->FreeFunc, product->ProductPriceFunc);
    return product_new;
}

ListElement copyProduct (ListElement product) {
    return copyProductAUX(product);
}

static void freeProductAUX (Product product){

    free(product->name);
    product->FreeFunc(product->customData);
    free(product);
}

void freeProduct (ListElement product){
    freeProductAUX(product);
}


static int compareProductAUX(Product product1, Product product2){
    return (int)(product1->ID - product2->ID);
}

int compareProduct(ListElement product1, ListElement product2){
    return compareProductAUX(product1, product2);
}

double realProductPrice (Product product, double amount){
    return (product -> ProductPriceFunc (product->customData, amount));
}

static void changeProfitForGivvenAmountSoldAUX (Product product, double amount) {
    product->profit += realProductPrice(product, amount);
}

void changeProfitForGivvenAmountSold (ListElement product, double amount){
    changeProfitForGivvenAmountSoldAUX(product, amount);
}

static Product findProductForID (List storage, unsigned id){
    for (ListElement ptr = listGetFirst(storage); ptr ; ptr = listGetNext(storage)){
        if(((Product)ptr) -> ID == id){
            return ptr;
        }
    }
}

void productPrintDetails (ListElement product, FILE *output) {
    //product was already checked if null before sent
    double priceTemp = ((Product)product)->
                         ProductPriceFunc(((Product)product)->customData,((Product)product) -> amount);

    mtmPrintProductDetails(((Product)product)->name, ((Product)product)->ID,
                           ((Product)product) -> amount, priceTemp, output);
}

bool productAlreadyExists(List storage, unsigned int id){

    List duplicatedList = listCopy(storage); // so I don't change the iterator
    for (ListElement ptr = listGetFirst(duplicatedList); ptr ; ptr = listGetNext(duplicatedList)){
        if(((Product)ptr) -> ID == id){
            listDestroy(duplicatedList);
            return true;
        }
    }
    listDestroy(duplicatedList);
    return false;
}

void findTheProductBeforeTheNewAndSetCurrentToIt (List storage, ListElement product_new){

    for (ListElement ptr = listGetFirst(storage); ptr; ptr = listGetNext(storage)){
        if (((Product)product_new)->ID > ((Product)ptr)->ID){
            return;
        }
    }
}

void productChangeAmount(List storage,unsigned int id, double amount){

    Product  ptr = findProductForID(storage, id);
    ((Product)ptr) -> amount += amount;
}

void productRemove (List storage, unsigned int id){
    Product ptr = ((Product)findProductForID(storage, id)); //set iterator to wanted product and find the product
    freeProduct(ptr);
    listRemoveCurrent(storage);
}

MatamazomAmountType productGetAmountType(List storage, unsigned int id){

    Product  ptr = findProductForID(storage, id);
    return ((Product) ptr)->amountType;
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

