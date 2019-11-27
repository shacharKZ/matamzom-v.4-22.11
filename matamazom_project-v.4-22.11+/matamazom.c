#include <stdio.h>
#include <stdbool.h>
#include "matamazom.h"
#include "stdlib.h"
#include "product.h"
#include "order.h"
#include "libmtm/list.h"
#include "libmtm/set.h"
#include <string.h>
#include "matamazom_print.h"
#include <assert.h>
#include "matamazom_print.h"



struct Matamazom_t{
    List storage;
    Set orders;
};

static bool valid_name (char* name){
    if (name == NULL){
        return false;
    }
    if ((name[0] <=57) && (name[0]>-48)){ //between 0-9
        return true;
    }
    if (strlen(name)==0 || (name[0]<'A') || (name[0]>'z') || ((name[0]>'Z')&&(name[0]<'a'))){ //not a-z or A-Z
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


    return true; /// 777 need a defult return
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

    matamazom_new -> storage = listCreate(copyProduct, freeProduct);
    matamazom_new -> orders = setCreate(&orderCopy, &orderFree, &orderCompare);

    if (matamazom_new -> storage == NULL) {
        if (matamazom_new->orders !=NULL){
            setDestroy(matamazom_new->orders);
        }
        free (matamazom_new);
        return NULL;
    }

    if (matamazom_new -> orders == NULL) {
        if (matamazom_new->storage !=NULL){
            listDestroy(matamazom_new->storage);
        }
        free (matamazom_new);
        return NULL;
    }
    return matamazom_new;
}

void matamazomDestroy(Matamazom matamazom){
    setDestroy(matamazom->orders);
    listDestroy(matamazom->storage);
    free (matamazom);
}
MatamazomResult mtmNewProduct(Matamazom matamazom, const unsigned int id, const char *name,
                              const double amount, const MatamazomAmountType amountType,
                              const MtmProductData customData, MtmCopyData CopyFunc,
                              MtmFreeData FreeFunc, MtmGetProductPrice ProductPriceFunc) {

    if (matamazom == NULL || matamazom->storage == NULL|| name == NULL || customData == NULL
        || CopyFunc == NULL || FreeFunc == NULL || ProductPriceFunc == NULL) {
        return MATAMAZOM_NULL_ARGUMENT;
    }
    if (valid_name(name) == false) {
        return MATAMAZOM_INVALID_NAME;
    }
    if (valid_amount(amount, amountType) == false || amount < 0) {
        return MATAMAZOM_INVALID_AMOUNT;
    }
    if (productAlreadyExists(matamazom->storage, id)){
        return MATAMAZOM_PRODUCT_ALREADY_EXIST;
    }

    Product product_new = productCreate(id, name, amount, amountType,
                               customData, CopyFunc, FreeFunc, ProductPriceFunc);
    /////// 777 copy to product
    //666 what?
    if (product_new == NULL) {
        return MATAMAZOM_OUT_OF_MEMORY;
    }
    if (listGetFirst(matamazom->storage) == NULL){ //case of first product
        ListResult check = listInsertFirst(matamazom -> storage, ((ListElement)product_new));

        if(check == LIST_OUT_OF_MEMORY){
            return MATAMAZOM_OUT_OF_MEMORY;
        } else{
            assert(check == LIST_SUCCESS);
            return MATAMAZOM_SUCCESS;
        }
    }
    if (findTheProductAfterTheNewAndSetCurrentToIt(matamazom->storage, product_new) == true){
        //check if he is not the last
        ListResult check = listInsertBeforeCurrent(matamazom->storage, product_new);

        if(check == LIST_OUT_OF_MEMORY){
            return MATAMAZOM_OUT_OF_MEMORY;
        } else{
            assert(check == LIST_SUCCESS);
            return MATAMAZOM_SUCCESS;
        }
    } else{ //this means this product will be the last, and has the biggest id so far
        ListResult check = listInsertLast(matamazom->storage, product_new);

        if(check == LIST_OUT_OF_MEMORY){
            return MATAMAZOM_OUT_OF_MEMORY;
        } else{
            assert(check == LIST_SUCCESS);
            return MATAMAZOM_SUCCESS;
        }
    }

}


MatamazomResult mtmChangeProductAmount(Matamazom matamazom, const unsigned int id, const double amount){

    if (matamazom==NULL){
        return MATAMAZOM_NULL_ARGUMENT;
    }
    if (productAlreadyExists(matamazom->storage, id) == false){
        return MATAMAZOM_PRODUCT_NOT_EXIST;
    }
    if (valid_amount(amount, productGetAmountType(matamazom->storage, id)) == false){
        return MATAMAZOM_INVALID_AMOUNT;
    }

    return productChangeAmountForID(matamazom->storage, id, amount);
    // 777 if the amount is under 0 we need to make another if/case
    // 666 - updated and now checks for change legality
}

MatamazomResult mtmClearProduct(Matamazom matamazom, const unsigned int id){
    if (matamazom == NULL){
        return MATAMAZOM_NULL_ARGUMENT;
    }
    if (id < 0 || productAlreadyExists(matamazom->storage, id) == false){
        return MATAMAZOM_PRODUCT_NOT_EXIST;
    }
    productRemove(matamazom->storage, id); // 777 i think we should put this func inside matamzom
                                            // 666 we can't because it uses findProductForID which is static
    return MATAMAZOM_SUCCESS;
}

MatamazomResult mtmPrintInventory(Matamazom matamazom, FILE *output){
    if (matamazom == NULL || output == NULL){
        return MATAMAZOM_NULL_ARGUMENT;
    }
    fprintf(output,"Inventory Status:\n");
    for (ListElement ptr = listGetFirst(matamazom->storage); ptr ; ptr = listGetNext(matamazom->storage)) {
        productPrintDetails(ptr, output);
    }
    return MATAMAZOM_SUCCESS;
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

    Order currentOrder = mtmFindOrder(matamazom, orderId);
    if (currentOrder == NULL) {
        return MATAMAZOM_ORDER_NOT_EXIST;
    }

    if (productAlreadyExists(matamazom->storage, productId) == false) {
        return MATAMAZOM_PRODUCT_NOT_EXIST;
    }

    if (valid_amount(amount, productGetAmountType(matamazom->storage, productId) == false)){
        return MATAMAZOM_INVALID_AMOUNT;
    }

    if (isProductIdInOrder(currentOrder, productId) == false) {
        return MATAMAZOM_PRODUCT_NOT_EXIST;
    } else{
        return orderChangeProductAmount(currentOrder, productId, amount);
    }
}

MatamazomResult mtmShipCheckAmount (List storage, List order) {
    if (storage == NULL || order == NULL) {
        return MATAMAZOM_NULL_ARGUMENT;
    }

    LIST_FOREACH(ListElement, currentProduct, order) {
        Product storageProduct = getPtrToProductForSameProduct(storage, currentProduct);
        if (storageProduct == NULL) {
            return MATAMAZOM_ORDER_NOT_EXIST;
        }
        if ( productGetAmount(storageProduct) < productGetAmount(((Product)currentProduct))) {
            return MATAMAZOM_INSUFFICIENT_AMOUNT;
        }
    }
    return MATAMAZOM_SUCCESS;
}


MatamazomResult mtmShipOrderExecut(List storage, List order) {
    if (storage == NULL || order == NULL) {
        return MATAMAZOM_NULL_ARGUMENT;
    }

    LIST_FOREACH(ListElement, currentProduct, order) {
        Product storageProduct = getPtrToProductForSameProduct(storage, currentProduct);
        if (productShipChangeAmountProfit(storageProduct, productGetAmount(((Product)currentProduct))) != MATAMAZOM_SUCCESS) {
            assert(0);
            return MATAMAZOM_OUT_OF_MEMORY;
        }
    }

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
MatamazomResult mtmShipOrder(Matamazom matamazom, const unsigned int orderId) {
    if (matamazom == NULL) {
        return MATAMAZOM_NULL_ARGUMENT;
    }
    Order orderToShip = mtmFindOrder(matamazom, orderId);

    if (orderToShip == NULL) {
        return MATAMAZOM_ORDER_NOT_EXIST;
    }

    MatamazomResult flag = mtmShipCheckAmount(matamazom->storage, orderGetPtrToProductList(orderToShip));
    if (flag != MATAMAZOM_SUCCESS) {
        return flag;
    }

    flag = mtmShipOrderExecut(matamazom->storage, orderGetPtrToProductList(orderToShip));
    if (flag != MATAMAZOM_SUCCESS) {
        assert(0);
        return MATAMAZOM_OUT_OF_MEMORY;
    }

    return MATAMAZOM_SUCCESS;
}



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

    //fprintf(output,"Inventory Status:\n"); changed this to that below 666

    fprintf(output,"Order %u Details:\n", orderId);
    MatamazomResult flag = orderPrintAllProduct (currentOrder, output); //you can use my print storage 666
    fprintf(output, "----------\n");
    double totalPrice = orderGetTotalPrice(currentOrder);
    fprintf(output, "Total Price: %f\n", totalPrice);
    return flag;
}


MatamazomResult mtmPrintBestSelling(Matamazom matamazom, FILE *output){

    if(matamazom == NULL || output == NULL){
        return MATAMAZOM_NULL_ARGUMENT;
    }
    fprintf(output, "Best Selling Product:\n");
    productPrintIncomeLine(matamazom->storage, output);
    return MATAMAZOM_SUCCESS;
}


MatamazomResult mtmPrintFiltered(Matamazom matamazom, MtmFilterProduct customFilter, FILE *output) {
    if (matamazom == NULL || customFilter == NULL || output == NULL) {
        return MATAMAZOM_NULL_ARGUMENT;
    }
    for (ListElement ptr = listGetFirst(matamazom->storage); ptr; ptr = listGetNext(matamazom->storage)) {

        if (productCustomFilter(ptr, customFilter) == true) {
            productPrintDetails(ptr, output);
        }
    }
    return MATAMAZOM_SUCCESS;
}

