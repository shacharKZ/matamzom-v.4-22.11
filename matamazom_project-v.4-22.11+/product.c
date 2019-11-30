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

Product productCreate(unsigned int id, const char* name, double amount, MatamazomAmountType datatype,
                      MtmProductData customData, MtmCopyData CopyFunc, MtmFreeData FreeFunc,
                      MtmGetProductPrice ProductPriceFunc){

    if(CopyFunc == NULL || FreeFunc == NULL || ProductPriceFunc == NULL || customData == NULL){
        return NULL;
    }

    Product product = malloc(sizeof(*product));
    if (product == NULL){
        return NULL;
    }
    product -> name = malloc(sizeof(char)*(strlen(name)+1));
    if (product->name == NULL){
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


Product getPtrToProductForID (List list ,unsigned int id) {
    LIST_FOREACH(ListElement, ptr,list) {
        if ((((Product)ptr)->ID) == id) {
            return ((Product)ptr);
        }
    }
    return NULL;
}

Product getPtrToProductForSameProduct (List list ,Product product) {

    LIST_FOREACH(ListElement, ptr,list) {

        if ((((Product)ptr)->ID) == product->ID) {

            return ((Product)ptr);
        }
    }

    return NULL;
}

static Product findProductForID (List storage, unsigned int id){

    for (ListElement ptr = listGetFirst(storage); ptr ; ptr = listGetNext(storage)){
        if(((Product)ptr) -> ID == id){
            return ptr;
        }
    }
    return NULL;
}

void freeProductAUX(Product product){
    if (product == NULL){
        return;
    }
    if (product->name!=NULL){
        free(product->name);
    }
    product->FreeFunc(product->customData);
    free(product);
}

void freeProduct(ListElement product){
    freeProductAUX(product);

    return;
    if (product == NULL){
        return;
    }
    if (((Product)product)->name!=NULL){
        free(((Product)product)->name);
    }
    ((Product)product)->FreeFunc(((Product)product)->customData);
    free(product);
}

void productRemove (List storage, unsigned int id){
    findProductForID(storage, id); //set iterator to wanted product and find the product
    listRemoveCurrent(storage);
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
    Product temp = ((Product)product);
    return temp->ProductPriceFunc(temp->customData, amount);
    //return (((Product)product) -> ProductPriceFunc (((Product)product)->customData, amount));
}

double productGetPrice (ListElement product){
    return (((Product)product) -> ProductPriceFunc (((Product)product)->customData, ((Product)product)->amount));
}

void changeProfitForGivenAmountSold (ListElement product, double amount){
    ((Product)product)->profit += realProductPrice(product, amount);
}

void productPrintDetails (ListElement product, FILE *output) {
    if (product == NULL || output == NULL) {
        return;
    }
    Product temp = ((Product)product);
    double priceTemp = temp->ProductPriceFunc(temp->customData, temp->amount);
    mtmPrintProductDetails(temp->name, temp->ID, temp->amount, priceTemp, output);
}

void productPrintDetailsForOne (ListElement product, FILE *output) {
    if (product == NULL || output == NULL) {
        return;
    }
    Product temp = ((Product)product);
    double priceTemp = temp->ProductPriceFunc(temp->customData, 1);
    mtmPrintProductDetails(temp->name, temp->ID, temp->amount, priceTemp, output);
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

MatamazomResult productChangeAmountForID(List storage, unsigned int id, double amount){

    Product  ptr = findProductForID(storage, id);
    if ( ptr->amount + amount < 0){
        return MATAMAZOM_INSUFFICIENT_AMOUNT;
    } else{
        ptr->amount += amount;
        return MATAMAZOM_SUCCESS;
    }
}

MatamazomResult productChangeAmountInList(List list, unsigned int id, double amount){

    if (list == NULL) {
        return MATAMAZOM_NULL_ARGUMENT;
    }
    Product  ptr = findProductForID(list, id); // also sets the ptr of list to the product
    if (ptr == NULL) {
        return MATAMAZOM_PRODUCT_NOT_EXIST;
    }
    ptr->amount += amount;
    if (ptr -> amount < 0) {
        ListResult flag = listRemoveCurrent(list);
        if (flag == LIST_SUCCESS) {
            return MATAMAZOM_SUCCESS;
        } else {
            return MATAMAZOM_OUT_OF_MEMORY;
        }
    }
    return MATAMAZOM_SUCCESS;
}

MatamazomResult productSetAmountForID(List storage, unsigned int id){
    Product  ptr = findProductForID(storage, id);
    if (ptr == NULL) {
        return MATAMAZOM_ORDER_NOT_EXIST;
    }
    ptr->amount = 0;
    return MATAMAZOM_SUCCESS;
}

MatamazomAmountType productGetAmountType(List storage, unsigned int id){

    Product ptr = findProductForID(storage, id);
    assert(ptr);
    return ((Product)ptr)->amountType;
}

double productGetAmount (Product product){
    if (product == NULL) {
        return 0;
    }
    return ((Product)product)->amount;
}


unsigned int productGetId (ListElement product){
    return ((Product )product)->ID;
}

double getCurrentProfitOfProduct(ListElement product) {
    return ((Product )product) -> profit;
}

void productSetNewAmount (Product product, double amount) {
    if (product == NULL) {
        return;
    }
    product->amount = amount;
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
    if (max == 0){
        fprintf(output, "none\n");
        return;
    }
    for (ListElement ptr = listGetFirst(storage); ptr; ptr = listGetNext(storage)){
        if ((((Product)ptr)->profit) == max){
            mtmPrintIncomeLine(((Product)ptr)->name, ((Product)ptr)->ID, max, output);
            return;
        }
    }

}


MatamazomResult productShipCheckAmount (List storage, List order) {
    if (storage == NULL || order == NULL) {
        return MATAMAZOM_NULL_ARGUMENT;
    }

    LIST_FOREACH(ListElement, currentProduct, order) {
        Product storageProduct = getPtrToProductForSameProduct(storage, ((Product)currentProduct));
        if (storageProduct == NULL) {
            return MATAMAZOM_ORDER_NOT_EXIST;
        }
        if (storageProduct->amount < ((Product)currentProduct)->amount) {
            return MATAMAZOM_INSUFFICIENT_AMOUNT;
        }
    }
    return MATAMAZOM_SUCCESS;
}

MatamazomResult productShipChangeAmountAndProfit(Product product, double amount){
    if (product == NULL) {
        return MATAMAZOM_NULL_ARGUMENT;
    }

    // 777 maybe add amountType check
    product->amount -= amount;
    product->profit += realProductPrice(((List)product), amount);
    return MATAMAZOM_SUCCESS;
}


MatamazomResult productAddToList (List list, Product product) {

    if (product == NULL || list == NULL) {
        return MATAMAZOM_NULL_ARGUMENT;
    }

    if (productAlreadyExists(list, product->ID)){
        return MATAMAZOM_PRODUCT_ALREADY_EXIST;
    }
    ListResult flag = listInsertFirst(list, product);
    if (flag != LIST_SUCCESS) {
        return MATAMAZOM_OUT_OF_MEMORY;
    }
    flag = listSort(list, compareProduct);
    if (flag != LIST_SUCCESS) {
        return MATAMAZOM_OUT_OF_MEMORY;
    }

    return MATAMAZOM_SUCCESS;
}
