//
// Created by eilon on 17/11/2019.
//
#ifndef UNTITLED1_PRODUCT17_11_H
#define UNTITLED1_PRODUCT17_11_H
#include <stdbool.h>
#include "libmtm/list.h"
#include "matamazom_print.h"
#include "matamazom.h"

//type for representing a single product, holding all fields of data needed for a product
typedef struct product_t *Product;

/**
 * productCreate: create a product.
 *
 * @param id - new product id. Must be non-negative, and unique.
 * @param name - name of the product, e.g. "apple". Must be non-empty.
 * @param amount - the initial amount of the product when added to the warehouse.
 * @param amountType - defines what are valid amounts for this product.
 * @param datatype -
 * @param customData - pointer to an additional custom data of a product
 * @param CopyFunc - pointer to a function used for copying the custom data of the product
 * @param FreeFunc - pointer to a function used for freeing the custom data of the product
 * @param ProductPriceFunc - pointer to a function used for calculating the price if given amount of the product
 * @return
 * A new product in case of success, and NULL otherwise
 * (e.g. in case of an allocation error or if given a null argument)
 *
 */
Product productCreate(unsigned int id, const char* name, double amount, MatamazomAmountType datatype, MtmProductData customData,
                      MtmCopyData CopyFunc, MtmFreeData FreeFunc, MtmGetProductPrice ProductPriceFunc);

//function for removing a lists element - product
void freeProduct (ListElement product);

//function for deleting a product and taking him off the list
void productRemove (List storage, unsigned int id);


void changeProfitForGivenAmountSold (ListElement product, double amount);

ListElement copyProduct(ListElement product);

int compareProduct(ListElement product1, ListElement product2);

MatamazomResult productChangeAmountForID(List storage, unsigned int id, double amount);

bool productAlreadyExists(List storage, unsigned int id);

bool findTheProductAfterTheNewAndSetCurrentToIt (List storage, ListElement product_new);

void productPrintDetails (ListElement product, FILE *output);

double realProductPrice (ListElement product, double amount);

double productGetPrice (ListElement product);

MatamazomAmountType productGetAmountType(List storage, unsigned int id);

double productGetCurrentProfitOf(ListElement product);

unsigned int productGetId (ListElement product);

bool productCustomFilter (ListElement product, MtmFilterProduct customFilter);

void productPrintIncomeLine (List storage, FILE *output);

MatamazomResult productAddToList (List list, Product product);

double productGetAmount (Product product);

double productChangeAmount(Product product, double amount);

MatamazomResult productSetAmountForID(List storage, unsigned int id);

void productSetNewAmount (Product product, double amount);

MatamazomResult productShipChangeAmountAndProfit(Product product, double amount);

Product getPtrToProductForID (List list ,unsigned int id);

Product getPtrToProductForSameProduct (List list ,Product product);

MatamazomResult productChangeAmountInList(List list, unsigned int id, double amount);

#endif //UNTITLED1_PRODUCT17_11_H

