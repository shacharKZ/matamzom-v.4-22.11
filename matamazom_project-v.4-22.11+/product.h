//
// Created by eilon on 17/11/2019.
//
#ifndef UNTITLED1_PRODUCT17_11_H
#define UNTITLED1_PRODUCT17_11_H
#include <stdbool.h>
#include "matamazom(19.11sha).h"

typedef struct product_t *Product;

double getCurrentProfitOfProduct(Product product);
double realProductPrice (Product product, double amount);
int compareProduct(Product product1, Product product2);
void changeProfit (Product product, double amount);
Product copyProduct(Product product);
void freeProduct (Product product);

Product productCreate(unsigned int id, char* name, const MatamazomAmountType datatype,
                      const MtmProductData customData,
                      MtmCopyData CopyFunc, MtmFreeData FreeFunc, MtmGetProductPrice ProductPriceFunc);


#endif //UNTITLED1_PRODUCT17_11_H

