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

Product productCreate(unsigned int id, char* name, const MatamazomAmountType datatype,
                      const MtmProductData customData,
                      MtmCopyData CopyFunc, MtmFreeData FreeFunc, MtmGetProductPrice ProductPriceFunc){

    Product product = malloc(sizeof(*product));
    if (product == NULL){
        return NULL;
    }
    product -> name = malloc(sizeof(strlen(name)));
    strcpy(product -> name, name);
    product -> ID = id;
    product -> amountType = datatype;
    product -> customData = customData;
    product -> CopyFunc = CopyFunc;
    product -> FreeFunc = FreeFunc;
    product -> ProductPriceFunc = ProductPriceFunc;
    product -> profit = 0;
    return product;
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
    return (product1->ID - product2->ID);
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

