//
// Created by eilon on 17/11/2019.
//
#ifndef UNTITLED1_PRODUCT17_11_H
#define UNTITLED1_PRODUCT17_11_H
#include <stdbool.h>
#include "amount_set.h"
#include "matamazom.h"

typedef struct product_t *Product;

double getCurrentProfitOfProduct(Product product);
double realProductPrice (Product product, double amount);
void changeProfit (Product product, double amount);
ASElement copyProduct(ASElement product);
void freeProduct (ASElement product);
int compareProduct(ASElement product1, ASElement product2);


Product productCreate(unsigned int id, char* name, const MatamazomAmountType datatype,
                      const MtmProductData customData,
                      MtmCopyData CopyFunc, MtmFreeData FreeFunc, MtmGetProductPrice ProductPriceFunc);


#endif //UNTITLED1_PRODUCT17_11_H

