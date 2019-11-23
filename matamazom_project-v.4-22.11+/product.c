//
// Created by eilon on 17/11/2019.
//

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "matamazom.h"
#include "product.h"
#include "amount_set.h"
#include <assert.h>

struct product_t{

    unsigned int ID;
    char *name;
    MatamazomAmountType amountType;
    MtmProductData customData;
    MtmCopyData CopyFunc;
    MtmFreeData FreeFunc;
    MtmGetProductPrice ProductPriceFunc;
    double profit;
};

Product productCreate(unsigned int id, char *name,
                      const double amount, const MatamazomAmountType amountType,
                      const MtmProductData customData, MtmCopyData CopyFunc,
                      MtmFreeData FreeFunc, MtmGetProductPrice ProductPriceFunc) { //maybe need a oopy func for copying the custom data 666

    Product product_new = malloc(sizeof(*product_new));
    if (product_new == NULL){
        return NULL;
    }
    product_new -> name = malloc(sizeof(strlen(name)));
    if (product_new->name == NULL){
        free(product_new);
        return NULL;
    }
    strcpy(product_new -> name, name);
    product_new -> ID = id;
    product_new -> amountType = amountType;
    product_new->customData = CopyFunc(customData);
    product_new -> customData = malloc(sizeof(customData));
    if (product_new->customData == NULL){
        free(product_new->name);
        free(product_new);
        return NULL;
    }
    product_new ->customData = customData; // do i need the copy func or does this work?
    product_new -> ProductPriceFunc = ProductPriceFunc;
    product_new -> profit = 0;
    return product_new;
}


static Product copyProductAUX(Product product){
    Product product_new = productCreate(product->ID, product->name, product->amountType,
                                         product->customData, product->CopyFunc,
                                         product->FreeFunc, product->ProductPriceFunc);
    return product_new;
}

ASElement copyProduct (ASElement product) {
    return copyProductAUX(product);
}

static void freeProductAUX (Product product){
    if (product->name) {
        free(product->name);
    }
    if (product->FreeFunc) {
        product->FreeFunc(product->customData);
    }
    free(product);
    product = NULL;
}

void freeProduct (ASElement product){
    freeProductAUX(product);
}

static int compareProductAUX(Product product1, Product product2){
    return (int)(product1->ID - product2->ID);
}

int compareProduct(ASElement product1, ASElement product2){
    return compareProductAUX(product1, product2);
}



double realProductPrice (Product product, double amount){
    return product -> ProductPriceFunc (product->customData, amount);
}

void changeProfit (Product product, double amount){
    product -> profit += realProductPrice (product, amount);
}

double getCurrentProfitOfProduct(Product product) {
    return (double)(product->profit);
}

Product getPtrToProductForID (AmountSet storage ,unsigned int id){
    Product ptr = asGetFirst(storage);
    AS_FOREACH(Product, )
    return NULL;
}




