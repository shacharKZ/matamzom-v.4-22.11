//
// Created by eilon on 24/11/2019.
//
#include "matamazom_project-v.4-22.11+/amount_set.h"
#include "matamazom_project-v.4-22.11+/matamazom.h"
#include "matamazom_project-v.4-22.11+/product.h"

MtmGetProductPrice simple (MtmProductData customData){
    return customData;
}


int main(){

    ASElement (*copyFunc)(ASElement) = &copyProduct;
    void (*freeFunc)(ASElement) = &freeProduct;
    int (*compareFunc)(ASElement, ASElement) = &compareProduct;


    Matamazom matamazom_new = matamazomCreate();
    FILE *fp = fopen("test5_output", "w");
    mtmPrintInventory(matamazom_new, fp);

    mtmNewProduct (matamazom_new, 1, "eilon",
                   10, MATAMAZOM_ANY_AMOUNT, (MtmProductData) 10, copyFunc, freeFunc, simple);
    mtmNewProduct (matamazom_new, 1, "dror",
                   20, MATAMAZOM_ANY_AMOUNT, (MtmProductData) 10, copyFunc, freeFunc, simple);
    mtmNewProduct (matamazom_new, 2, "dror",
                   20, MATAMAZOM_ANY_AMOUNT, (MtmProductData) 10, copyFunc, freeFunc, simple);

    mtmPrintInventory(matamazom_new, fp);

    matamazomDestroy(matamazom_new);

    mtmPrintInventory(matamazom_new, fp);

    fclose(fp);

}