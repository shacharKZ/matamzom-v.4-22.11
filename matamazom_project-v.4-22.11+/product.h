//
// Created by eilon on 17/11/2019.
//
#ifndef UNTITLED1_PRODUCT17_11_H
#define UNTITLED1_PRODUCT17_11_H
#include <stdbool.h>
#include "libmtm/list.h"
#include "matamazom_print.h"
#include "matamazom.h"

/**
 * Generic List Container
 *
 * Implements a product type.
 * the product type includes all fields relevant to managing a storage of products

 *
 * The following functions are available:
 *   productCreate                      - Creates a new detailed product
 *   productRemove                      - Deletes an existing product and removes from storage
 *   compareProduct                      - Compare product based on their ID
 *   productChangeAmountInStorage       - Add or subtract an amount from a product in the storage
 *   productChangeAmountInOrder         - Add or subtract an amount from a product in an order
 *   productAlreadyExists               - Check if a product exists in a storage
 *   realProductPrice                   - Receive the price of a certain amount of a product
 *   productGetPrice                    - Receive the price all amount existing of a product
 *   productGetAmountType               - Receive the product's amount type
 *   productGetCurrentProfitOf          - Receive the product's current accumulated profit
 *   productCustomFilter                - Return rather a product answers a certain criteria or not
 *   productAddToList                   - Add an existing product to a storage
 *   productGetAmount                   - Return the product's current amount
 *   productSetNewAmount                - Set the product's amount to a specific, requested amount
 *   productShipChangeAmountAndProfit   - Subtract the order's product amount from storage and update it's profit
 *   CheckIfProductFromOrderIsInStorage - for a given product in an order, check if it's in the storage
 *   productPrintIncomeLine             - print most profitable product's details and accumulated profit
 *   productPrintDetails                - print product's details and price for a given amount
 *   productPrintDetailsForOne          - print product's details and price for a single unit
 */




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

//function for copying a product's fields and create a copy of the product
ListElement copyProduct(ListElement product);

/*
 * compareProduct: create a comparison between products based on their ID
 *
 * @param product1 - one of the two product which we want to compare
 * @param product2
 * @return
 * A positive number if product 1 is "bigger" for our criteria, a negative number if product 2 is bigger,
 * 0 if they are equal (have the same ID, thus in fact, the same product, since its a unique field for each product
 */
int compareProduct(ListElement product1, ListElement product2);


/*
 * productChangeAmountInStorage: add or subtract from product's amount in storage , identification of said product is
 * based on his ID
 *
 * @param storage - the storage in which the product is kept (the adt holding it)
 * @param id - the product unique ID field of data
 * @param amount - the amount we want to add or subtract to the product's current amount
 * @return
 * MATAMAZOM_INSUFFICIENT_AMOUNT - if subtraction of wanted amount will cause --> new_amount<0
 * MATAMAZOM_SUCCESS - if product's amount was successfully updated
 */
MatamazomResult productChangeAmountInStorage(List storage, unsigned int id, double amount);

/*
 * productChangeAmountInOrder: add or subtract from product's amount in order, identification of said product is
 * based on his ID
 *
 * @param list - the order in which the product is kept (the adt holding it)
 * @param id - the product unique ID field of data
 * @param amount - the amount we want to add or subtract to the product's current amount
 * @return
 * MATAMAZOM_INSUFFICIENT_AMOUNT - if subtraction of wanted amount will cause --> new_amount<0
 * MATAMAZOM_SUCCESS - if product's amount was successfully updated
 */
MatamazomResult productChangeAmountInOrder(List list, unsigned int id, double amount);

/*
 * productAlreadyExists: check if a certain product exists in a given storage
 *
 * @param storage - the storage in which we want to look for the product
 * @param id - the product unique ID field of data
 * @return
 * true - if the product is in the storage
 * false - if the product is NOT in the storage
 */
bool productAlreadyExists(List storage, unsigned int id);

/*
 * realProductPrice: get the price of a product for a given amount
 *
 * @param product - the product to check price for
 * @param amount - the amount of the product, which we wish to know the price for
 */
double realProductPrice (ListElement product, double amount);

/*
 * productGetPrice: get the price of a product for all amount currently available
 *
 * @param product - the product to check price for
 */
double productGetPrice (ListElement product);

/*
 * productGetAmountType: Receive the product's amount type
 *
 * @param storage - the storage in which we want to look for the product
 * @param id - the product unique ID field of data
 * @return
 * MATAMAZOM_INTEGER_AMOUNT - if product can be calculated only for integer amounts
 * MATAMAZOM_HALF_INTEGER_AMOUNT - if product can be calculated only for integer amounts and half integer amounts
 * MATAMAZOM_ANY_AMOUNT - if product can be calculated for any amount
 */
MatamazomAmountType productGetAmountType(List storage, unsigned int id);

/*
 * productGetCurrentProfitOf: Receive the product's current accumulated profit
 *
 * @param product - the product which we want to receive it's profit
 */
double productGetCurrentProfitOf(ListElement product);

/*
 * productCustomFilter: return if a product should be included in a filtered storage, according to custom qualifier
 *
 * @param product - the storage in which we want to look for the product
 * @param customFilter - pointer to a function that  sets apart products by a certain quality
 * @return
 * true  - if the product should be included according to the custom filter
 * false - if the product should NOT be included according to the custom filter
 */
bool productCustomFilter (ListElement product, MtmFilterProduct customFilter);

/*
 *  * productAddToList: add an existing product to a list (storage or order)
 *
 * @param list    - the storage ot order tp which we want to add the product
 * @param product - the element to add
 * @return
 * MATAMAZOM_NULL_ARGUMENT - if one or more of the arguments is NULL
 * MATAMAZOM_PRODUCT_ALREADY_EXIST - if this product is already included in the order/storage
 * MATAMAZOM_OUT_OF_MEMORY - if a segmentation fault occurred or an other memory failure
 * MATAMAZOM_SUCCESS - if product was added successfully
 */
MatamazomResult productAddToList (List list, Product product);

/*
 * productGetAmount: Receive the product's current amount in a storage ot order
 *
 * @param product - the product which we want to receive it's amount
 */
double productGetAmount (Product product);

/*
 *  * productAddToList: add an existing product to a list (storage or order)
 *
 * @param list    - the storage ot order tp which we want to add the product
 * @param product - the element to add
 * @return
 * MATAMAZOM_NULL_ARGUMENT - if one or more of the arguments is NULL
 * MATAMAZOM_ORDER_NOT_EXIST
 * MATAMAZOM_SUCCESS - if product's amount was successfully set to 0
 */
MatamazomResult productSetAmountForID(List storage, unsigned int id);

/*
 * productSetNewAmount: set a specific new amount to a product
 *
 * @param amount  - the requested new amount for said product
 * @param product - the product, who's amount we want to change
 */
void productSetNewAmount (Product product, double amount);

/*
 *  * productShipChangeAmountAndProfit: Subtract the order's product amount from storage and update it's profit
 *
 * @param product - the product sold
 * @param amount  - the amount sold, and to subtracted from storage
 * @return
 * MATAMAZOM_NULL_ARGUMENT - if one or more of the arguments is NULL
 * MATAMAZOM_SUCCESS - if product amount and profit were updated successfully
 */
MatamazomResult productShipChangeAmountAndProfit(Product product, double amount);

/**
 * CheckIfProductFromOrderIsInStorage: for a given product in an order, check if it's in the storage
 *
 * @param list - the storage to search in
 * @param product - the product in the order to check for in storage
 * @return
 * The same product sent
 */
Product CheckIfProductFromOrderIsInStorage (List list ,Product product); //666 formerly known as getPtrToProductForSameProduct 666


/**
 * getCopyOfProductForId: for a given product in an order, check if it's in the storage
 *
 * @param list - the List to search in
 * @param id - the id of the product to look for in list
 * @return
 * if the list contain a product with the same id return a copy of the product
 * otherwise return NULL
 */
Product getCopyOfProductForId (List list ,unsigned int id);


/**
 * productPrintIncomeLine: print the income line of a product, as required from
 * mtmPrintBestSelling.
 */
void productPrintIncomeLine (List storage, FILE *output);

/**
 * productPrintDetails: print product's details and price for a given amount
 */
void productPrintDetails (ListElement product, FILE *output);

/**
 * productPrintDetailsForOne: print product's details and price for a single unit
 * mtmPrintBestSelling.
 */
void productPrintDetailsForOne (ListElement product, FILE *output);


#endif //UNTITLED1_PRODUCT17_11_H

