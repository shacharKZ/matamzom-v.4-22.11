//
// Created by eilon on 17/11/2019.
//
#ifndef UNTITLED1_PRODUCT17_11_H
#define UNTITLED1_PRODUCT17_11_H
#include <stdbool.h>
#include "libmtm/list.h"
#include "matamazom_print.h"
#include "matamazom.h"

typedef struct product_t *Product;

Product productCreate(unsigned int id, char* name, double amount, MatamazomAmountType datatype, MtmProductData customData,
                      MtmCopyData CopyFunc, MtmFreeData FreeFunc, MtmGetProductPrice ProductPriceFunc);

void changeProfitForGivvenAmountSold (ListElement product, double amount);

ListElement copyProduct(ListElement product);

void freeProduct (ListElement product);

int compareProduct(ListElement product1, ListElement product2);

void productChangeAmount(List storage ,unsigned int id, double amount);

Product getPtrToProductForID (List storage ,unsigned int id, MtmFreeData custom_data_free_func);

bool productAlreadyExists(List storage, unsigned int id);

void findTheProductBeforeTheNewAndSetCurrentToIt (List storage, ListElement product_new);

void productPrintDetails (ListElement product, FILE *output);

MatamazomAmountType productGetAmountType(List storage, unsigned int id);
double productGetCurrentProfitOf(ListElement product);

unsigned int productGetId (ListElement product);

void productRemove (List storage, unsigned int id);


#endif //UNTITLED1_PRODUCT17_11_H

