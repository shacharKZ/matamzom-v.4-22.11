#include <stdio.h>
#include <stdbool.h>
#include "matamazom.h"
#include "stdlib.h"
#include "product.h"
#include "order.h"
#include "libmtm/set.h"
#include <string.h>
#include "matamazom_print.h"
#include <assert.h>


struct Matamazom_t{
    AmountSet storage;
    Set orders;
};

static bool valid_name (char* name){
    if (name == NULL){
        return false;
    }
    if ((name[0] <=57) && (name[0]>-48)){ //between 0-9
        return true;
    }
    if (strlen(name)==1 || (name[0]<'A') || (name[0]>'z') || ((name[0]>'Z')&&(name[0]<'a'))){ //not a-z or A-Z
        return false;
    }
    return true;
}

static bool valid_amount (const double amount, MatamazomAmountType amountType){
    if (amount == 0 || amountType == MATAMAZOM_ANY_AMOUNT){
        return  true;
    }
    if (amountType == MATAMAZOM_INTEGER_AMOUNT){
        double x = amount;
        int y = (int)x;
        double z = x-y;
        if (((z<1)&&(z>=0.999)) || ((z>=0)&&(z<=0.001))) {
            return true;
        }
        return false;
    }
    if (amountType == MATAMAZOM_HALF_INTEGER_AMOUNT){
        double x = amount;
        int y = (int)x;
        double z = x-y;
        if (((z<1)&&(z>=0.999)) || ((z>=0)&&(z<=0.001)) || ((z>=0.499)&&(z<=0.501))) {
            return true;
        }
        return false;
    }
}



// add info
static Order mtmFindOrder (Matamazom matamazom, const unsigned int orderId) {
    if (matamazom == NULL) {
        return NULL;
    }

    Order searchTarget = orderCreate(orderId);
    if (searchTarget == NULL) {
        return NULL;
    }

    SET_FOREACH(Order, currentOrder, matamazom->orders) {
        if (orderCompare(currentOrder, searchTarget) == 0) {
            orderFree(searchTarget);
            return currentOrder;
        }
    }

    orderFree(searchTarget);
    return NULL;
}



Matamazom matamazomCreate(){
    Matamazom matamazom_new = malloc(sizeof(*matamazom_new));
    if  (matamazom_new == NULL){
        return NULL;
    }

    matamazom_new -> storage = asCreate(&copyProduct, &freeProduct, &compareProduct);
    if (matamazom_new -> storage == NULL) {
        free (matamazom_new);
        return NULL;
    }

    matamazom_new -> orders = setCreate(&orderCopy, &orderFree, &orderCompare);
    if (matamazom_new -> orders == NULL) {
        asDestroy(matamazom_new->storage);
        free (matamazom_new);
        return NULL;
    }
}

// -----------------------------------------------------------------------
/*
void matamazomDestroy(Matamazom matamazom){
    setDestroy(matamazom->orders);
    asDestroy(matamazom->storage);
    free (matamazom);
}


MatamazomResult mtmNewProduct(Matamazom matamazom, unsigned int id, char *name,
                              const double amount, const MatamazomAmountType amountType,
                              const MtmProductData customData, MtmCopyData CopyFunc,
                              MtmFreeData FreeFunc, MtmGetProductPrice ProductPriceFunc) {

    if (matamazom == NULL || name == NULL || customData == NULL
        || CopyFunc == NULL || FreeFunc == NULL || ProductPriceFunc == NULL) {
        return MATAMAZOM_NULL_ARGUMENT;
    }
    if (valid_name(name) == false) {
        return MATAMAZOM_INVALID_NAME;
    }
    if (valid_amount(amount, amountType) == false || amount < 0) {
        return MATAMAZOM_INVALID_AMOUNT;
    }

    Product product_new = productCreate(id, name, amountType, customData, ProductPriceFunc);

    if (product_new == NULL) {
        return MATAMAZOM_OUT_OF_MEMORY;
    }
    switch (asRegister(matamazom->storage, product_new)) {
        case (AS_SUCCESS): {
            return MATAMAZOM_SUCCESS;
        }
        case (AS_ITEM_ALREADY_EXISTS):{
            return MATAMAZOM_PRODUCT_ALREADY_EXIST;
        }
        case (AS_NULL_ARGUMENT): {
            return MATAMAZOM_NULL_ARGUMENT;
        }
    }
}


MatamazomResult mtmChangeProductAmount(Matamazom matamazom, const unsigned int id, const double amount){

    if (matamazom==NULL){
        return MATAMAZOM_NULL_ARGUMENT;
    }
    Product ptr = getPtrToProductForID(matamazom->storage, id, );
    if (ptr == NULL){
        return MATAMAZOM_PRODUCT_NOT_EXIST;
    }
    if (valid_amount(amount, ptr->amountType) == false){
        return MATAMAZOM_INVALID_AMOUNT;
    }
    switch (asChangeAmount(matamazom->storage, ptr, amount)){
        case AS_NULL_ARGUMENT:
            return MATAMAZOM_NULL_ARGUMENT;
        case AS_INSUFFICIENT_AMOUNT:
            return MATAMAZOM_INSUFFICIENT_AMOUNT;
        case AS_ITEM_DOES_NOT_EXIST:
            return MATAMAZOM_PRODUCT_NOT_EXIST;
        case AS_SUCCESS:
            return MATAMAZOM_SUCCESS;
    }
}

MatamazomResult mtmClearProduct(Matamazom matamazom, const unsigned int id){
    if (matamazom == NULL){
        return MATAMAZOM_NULL_ARGUMENT;
    }
    if (id < 0){
        return MATAMAZOM_PRODUCT_NOT_EXIST;
    }
    Product  temp = getPtrToProductForID(matamazom->storage, id, MtmFreeData); //??????? how do i use the user's free func, whats the name
    switch (asDelete(matamazom->storage,temp)){
        case AS_NULL_ARGUMENT:
            return MATAMAZOM_OUT_OF_MEMORY;
        case AS_ITEM_DOES_NOT_EXIST:
            return MATAMAZOM_PRODUCT_NOT_EXIST;
        case AS_SUCCESS:
            return MATAMAZOM_SUCCESS;
    }

    return MATAMAZOM_SUCCESS;
}

MatamazomResult mtmPrintInventory(Matamazom matamazom, FILE *output){
    if (matamazom == NULL){
        return MATAMAZOM_NULL_ARGUMENT;
    }
    FILE *fp;
    fp = fopen("output_file" , "w");
    fprintf(fp,"Inventory Status:\n");
    for (struct product_t *ptr = asGetFirst(matamazom->storage); ptr ; ptr = asGetNext(matamazom->storage)) {
        mtmPrintProductDetails(getProductName(ptr->currentElement),
                               getProductID(ptr->currentElement), ptr->currentAmount,
                               realProductPrice(ptr->currentElement), fp);
    }
    fclose(fp);
}

unsigned int mtmCreateNewOrder(Matamazom matamazom) {  /// 33333
    unsigned int newOrderId = setGetSize(matamazom->orders)+1;
    Order newOrder = orderCreate(newOrderId);
    while (setIsIn(matamazom->orders, newOrder) == true) {
        newOrderId++;
        orderChangeId(newOrder, newOrderId);
    }
    if (setAdd(matamazom->orders, newOrder) == SET_SUCCESS) {
        return newOrderId;
    }
    return 0;
}


*/
//--------------------------------------------------------------------------------


/**
 * mtmChangeProductAmountInOrder: add/increase/remove/decrease products to an existing order.
 * Only products that exist inside the matamazom can be added to an order.
 * if 'amount' < 0 then this amount should be decreased from the product in the order.
 * if 'amount' > 0 then this amount should be added to the product in the order.
 * if 'amount' = 0 then nothing should be done.
 * please note:
 *  1) If the amount to decrease('amount' < 0) is *larger or equal* then the amount of the product in the
 *     order, then the product is removed entirely from the order.
 *  2) If 'amount' > 0 and the product doesn't exist inside the order then you should add it to the order
 *     with the amount given as argument.
 *
 * @param matamazom - warehouse containing the order and the product.
 * @param orderId - id of the order being modified.
 * @param productId - id of product to add to the order.
 * @param amount - amount of product to add to the order.
 * @return
 *     MATAMAZOM_NULL_ARGUMENT - if a NULL argument is passed.
 *     MATAMAZOM_ORDER_NOT_EXIST - if matamazom does not contain an order with
 *         the given orderId.
 *     MATAMAZOM_PRODUCT_NOT_EXIST - if matamazom does not contain a product with
 *         the given productId.
 *     MATAMAZOM_INVALID_AMOUNT - if amount is not consistent with product's amount type
 *         (@see parameter amountType in mtmNewProduct).
 *     MATAMAZOM_SUCCESS - if product was added/removed/increased/decreased to the order successfully.
 * @note Even if amount is 0 (thus the function will change nothing), still a proper
 *    error code is returned if one of the parameters is invalid, and MATAMAZOM_SUCCESS
 *    is returned if all the parameters are valid.
 */
MatamazomResult mtmChangeProductAmountInOrder(Matamazom matamazom, const unsigned int orderId,
                                              const unsigned int productId, const double amount) {
    // if the structure of the func works i will made it cleaner by separate to sub funcs
    if (matamazom == NULL) {
        return MATAMAZOM_NULL_ARGUMENT;
    }

    Order searchForOrder = orderCreate(orderId);
    if (setIsIn(matamazom->orders, searchForOrder) == false) {
        orderFree(searchForOrder);
        return MATAMAZOM_ORDER_NOT_EXIST;
    }

    Product searchForProduct = productCreate(productId, "NONE", MATAMAZOM_ANY_AMOUNT, NULL, NULL, NULL, NULL);
    if (asContains(matamazom->storage, searchForProduct) == false) {
        orderFree(searchForOrder);
        freeProduct(searchForProduct);
        return MATAMAZOM_PRODUCT_NOT_EXIST;
    }

    // add func compare amount and amountType and if add a case for MATAMAZOM_INVALID_AMOUNT!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1

    bool flagCopyProduct = false;
    AS_FOREACH(Product, currentProduct, matamazom->storage) {
        if (compareProduct(currentProduct, searchForProduct) == true) {
            freeProduct(searchForOrder);
            searchForOrder = copyProduct(currentProduct);
            flagCopyProduct = true;
            if (searchForOrder == NULL) {
                orderFree(searchForOrder);
                freeProduct(searchForProduct);
                return MATAMAZOM_OUT_OF_MEMORY;
            }
            break;
        }
    }
    assert(flagCopyProduct == true); // something went wrong int the last loop

    SET_FOREACH(Order, currentOrder, matamazom->orders) {
        if (orderCompare(currentOrder, searchForOrder) == true) {
            AmountSetResult flag = orderRegisterProductOrChangeAmount(currentOrder, searchForProduct, amount);
            orderFree(searchForOrder);
            freeProduct(searchForProduct);
            if (flag == AS_SUCCESS) {
                return MATAMAZOM_SUCCESS;
            }
            else if (flag == AS_NULL_ARGUMENT || flag == AS_ITEM_DOES_NOT_EXIST) {
                return MATAMAZOM_OUT_OF_MEMORY;
            }
        }
    }

    assert(0);
    return MATAMAZOM_OUT_OF_MEMORY;
}
/**
 * mtmShipOrder: ship an order and remove it from a Matamazom warehouse.
 *
 * All products in the order are removed from the warehouse, and the order is
 * deleted. The amount of each product in the order is the amount of the product
 * that is removed from the warehouse. additionally once order is shipped
 * the profit from the products shipped needs to be updated
 *
 * If the order cannot be shipped for any reason, e.g. some product's amount in
 * the order is larger than its amount in the warehouse, then the entire
 * operation is canceled - the order remains in the warehouse, and the
 * warehouse contents are not modified.
 *
 * @param matamazom - warehouse containing the order and all the products.
 * @param orderId - id of the order being shipped.
 * @return
 *     MATAMAZOM_NULL_ARGUMENT - if a NULL argument is passed.
 *     MATAMAZOM_ORDER_NOT_EXIST - if matamazom does not contain an order with
 *         the given orderId.
 *     MATAMAZOM_INSUFFICIENT_AMOUNT - if the order contains a product with an amount
 *         that is larger than its amount in matamazom.
 *     MATAMAZOM_SUCCESS - if the order was shipped successfully.
 */
MatamazomResult mtmShipOrder(Matamazom matamazom, const unsigned int orderId);

/**
 * mtmCancelOrder: cancel an order and remove it from a Matamazom warehouse.
 *
 * The order is deleted from the warehouse. The products and their amounts in
 * the warehouse is not changed.
 *
 * @param matamazom - warehouse containing the order.
 * @param orderId - id of the order being canceled.
 * @return
 *     MATAMAZOM_NULL_ARGUMENT - if a NULL argument is passed.
 *     MATAMAZOM_ORDER_NOT_EXIST - if matamazom does not contain an order with
 *         the given orderId.
 *     MATAMAZOM_SUCCESS - if the order was shipped successfully.
 */
MatamazomResult mtmCancelOrder(Matamazom matamazom, const unsigned int orderId) {
    if (matamazom == NULL) {
        return MATAMAZOM_NULL_ARGUMENT;
    }
    Order orderToCancel = mtmFindOrder(matamazom, orderId);

    if (orderToCancel == NULL) {
        return MATAMAZOM_ORDER_NOT_EXIST;
    }
    if (setRemove(matamazom->orders, orderToCancel) == SET_SUCCESS) {
        return MATAMAZOM_SUCCESS;
    }

    assert(0); // does not suppose to get here
    return MATAMAZOM_NULL_ARGUMENT;
}


/**
 * matamazomPrintOrder: print a summary of an order from a Matamazom warehouse,
 * as explained in the *.pdf
 *
 * The printout includes the total price of the order.
 *
 * @param matamazom - the Matamazom warehouse containing the order.
 * @param orderId - id of the order in matamazom.
 * @param output - an open, writable output stream, to which the order is printed.
 * @return
 *     MATAMAZOM_NULL_ARGUMENT - if a NULL argument is passed.
 *     MATAMAZOM_ORDER_NOT_EXIST - if matamazom does not contain an order with
 *         the given orderId.
 *     MATAMAZOM_SUCCESS - if printed successfully.
 */
MatamazomResult mtmPrintOrder(Matamazom matamazom, const unsigned int orderId, FILE *output) {
    if (matamazom == NULL || output == NULL) {
        return MATAMAZOM_NULL_ARGUMENT;
    }

    Order currentOrder = mtmFindOrder(matamazom, orderId);
    if (currentOrder == NULL) {
        return MATAMAZOM_ORDER_NOT_EXIST;
    }



}

/**
 * mtmPrintBestSelling: print the best selling products of a Matamazom
 * warehouse, as explained in the *.pdf.
 *
 * Print the products in descending order, from the best selling (highest total
 * income) to the worst selling (lowest total income). For each product, print
 * its name, id and total income.
 *
 * @param matamazom - a Matamazom warehouse.
 * @param output - an open, writable output stream, to which the order is printed.
 * @return
 *     MATAMAZOM_NULL_ARGUMENT - if a NULL argument is passed.
 *     MATAMAZOM_SUCCESS - if printed successfully.
 */
MatamazomResult mtmPrintBestSelling(Matamazom matamazom, FILE *output);

/**
 * mtmPrintFiltered: print some products of a Matamazom warehouse, according to
 * a custom filter, as explained in the *.pdf.
 *
 * Please note: This function filter only products that are inside the warehouses inventory.
 * I.e. this function should not care about what is happening inside the orders or the 'income'
 * mechanism.
 *
 * @param matamazom - a Matamazom warehouse.
 * @param customFilter - a boolean function that receives a product's information and
 *     returns true if it should be printed.
 * @param output - an open, writable output stream, to which the order is printed.
 * @return
 *     MATAMAZOM_NULL_ARGUMENT - if a NULL argument is passed.
 *     MATAMAZOM_SUCCESS - if printed successfully.
 */
MatamazomResult mtmPrintFiltered(Matamazom matamazom, MtmFilterProduct customFilter, FILE *output);


