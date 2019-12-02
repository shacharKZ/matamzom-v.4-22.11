#include <stdbool.h>
#include "matamazom.h"
#include <stdlib.h>
#include <string.h>
#include "product.h"
#include "order.h"
#include "libmtm/list.h"
#include "libmtm/set.h"
#include <assert.h>


struct Matamazom_t{
    List storage;
    Set orders;
    unsigned int ordersCount;
};

static bool validName (const char* name){
    if (name == NULL){
        return false;
    }
    if ((name[0] <='9') && (name[0]>='0')){ //between 0-9
        return true;
    }
    if (strlen(name)==0 || (name[0]<'A') || (name[0]>'z') || ((name[0]>'Z')&&(name[0]<'a'))){ //not a-z or A-Z
        return false;
    }
    return true;
}

static bool validAmount (const double amount, MatamazomAmountType amountType){

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

    return true;
}


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
    if (matamazom_new -> storage == NULL) {
        free (matamazom_new);
        return NULL;
    }

    matamazom_new -> orders = setCreate(&orderCopy, &orderFree, &orderCompare);
    if (matamazom_new -> orders == NULL) {
        if (matamazom_new->storage !=NULL){
            listDestroy(matamazom_new->storage);
        }
        free (matamazom_new);
        return NULL;
    }
    matamazom_new->ordersCount=1;
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
    if (validName(name) == false) {
        return MATAMAZOM_INVALID_NAME;
    }
    if (validAmount(amount, amountType) == false || amount < 0) {
        return MATAMAZOM_INVALID_AMOUNT;
    }
    if (productAlreadyExists(matamazom->storage, id)){
        return MATAMAZOM_PRODUCT_ALREADY_EXIST;
    }

    Product product_new = productCreate(id, name, amount, amountType,
                               customData, CopyFunc, FreeFunc, ProductPriceFunc);
    if (product_new == NULL) {
        return MATAMAZOM_OUT_OF_MEMORY;
    }

    MatamazomResult flag = productAddToList(matamazom->storage, product_new);
    freeProduct(product_new);
    return flag;

    /*
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
     */

}


MatamazomResult mtmChangeProductAmount(Matamazom matamazom, const unsigned int id, const double amount){
    if (matamazom==NULL){
        return MATAMAZOM_NULL_ARGUMENT;
    }
    if (productAlreadyExists(matamazom->storage, id) == false){
        return MATAMAZOM_PRODUCT_NOT_EXIST;
    }
    if (validAmount(amount, productGetAmountType(matamazom->storage, id)) == false){
        return MATAMAZOM_INVALID_AMOUNT;
    }

    return productChangeAmountInStorage(matamazom->storage, id, amount);
}

MatamazomResult mtmClearProduct(Matamazom matamazom, const unsigned int id){
    if (matamazom == NULL){
        return MATAMAZOM_NULL_ARGUMENT;
    }
    if (id < 0 || productAlreadyExists(matamazom->storage, id) == false){
        return MATAMAZOM_PRODUCT_NOT_EXIST;
    }
    productRemove(matamazom->storage, id);

    SET_FOREACH(Order, currentOrder, matamazom->orders) {
        productRemove(orderGetPtrToProductList(currentOrder), id);
    }
    return MATAMAZOM_SUCCESS;
}

MatamazomResult mtmPrintInventory(Matamazom matamazom, FILE *output){
    if (matamazom == NULL || output == NULL){
        return MATAMAZOM_NULL_ARGUMENT;
    }
    fprintf(output,"Inventory Status:\n");
    for (ListElement ptr = listGetFirst(matamazom->storage); ptr ; ptr = listGetNext(matamazom->storage)) {
        productPrintDetailsForOne(ptr, output);
    }
    return MATAMAZOM_SUCCESS;
}

unsigned int mtmCreateNewOrder(Matamazom matamazom) {
    if(matamazom == NULL) {
        return 0;
    }
    unsigned int newOrderId = matamazom->ordersCount++;
    Order newOrder = orderCreate(newOrderId);
    if (setAdd(matamazom->orders, newOrder) == SET_SUCCESS) {
        orderFree(newOrder);
        return newOrderId;
    }
    orderFree(newOrder);
    assert(0); // if the func got to here something went wrong
    return 0;




    /*
    unsigned int newOrderId = setGetSize(matamazom->orders)+1;
    Order newOrder = orderCreate(newOrderId);
    if (newOrder == NULL) {
        return 0;
    }
    while (setIsIn(matamazom->orders, newOrder) == true) {
        newOrderId++;
        orderChangeId(newOrder, newOrderId);
    }
    if (setAdd(matamazom->orders, newOrder) == SET_SUCCESS) {
        orderFree(newOrder);
        return newOrderId;
    }
    orderFree(newOrder);
    assert(0); // if the func got to here something went wrong
    return 0;
     */
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

    if (validAmount(amount, productGetAmountType(matamazom->storage, productId)) == false) {
        return MATAMAZOM_INVALID_AMOUNT;
    }

    if (isProductIdInOrder(currentOrder, productId) == false) {
        if (amount < 0) {
            return MATAMAZOM_INSUFFICIENT_AMOUNT;
        }
        Product newProduct = getCopyOfProductForId(matamazom->storage ,productId);
        if (newProduct == NULL) {
            return MATAMAZOM_OUT_OF_MEMORY;
        }

        productSetNewAmount(newProduct, amount);
        MatamazomResult flag = orderAddProductToCart(currentOrder,newProduct);
        freeProduct(newProduct);
        return flag;
    }
    return orderChangeProductAmount(currentOrder, productId, amount);
}

static MatamazomResult mtmShipCheckAmounts (List storage, List order) {
    if (storage == NULL || order == NULL) {
        return MATAMAZOM_NULL_ARGUMENT;
    }

    LIST_FOREACH(ListElement, orderProduct, order) {
        Product storageProduct = CheckIfProductFromOrderIsInStorage(storage, orderProduct);
        if (storageProduct == NULL) {
            return MATAMAZOM_PRODUCT_NOT_EXIST;
        }
        if (productGetAmount(storageProduct) < productGetAmount(((Product)orderProduct))) {
            return MATAMAZOM_INSUFFICIENT_AMOUNT;
        }
    }
    return MATAMAZOM_SUCCESS;
}


static MatamazomResult mtmShipOrderExecute(List storage, List order) {
    if (storage == NULL || order == NULL) {
        return MATAMAZOM_NULL_ARGUMENT;
    }

    LIST_FOREACH(Product, currentOrderProduct, order) {
        Product storageProduct = CheckIfProductFromOrderIsInStorage(storage, currentOrderProduct);
        if (productShipChangeAmountAndProfit(storageProduct, productGetAmount(currentOrderProduct)) != MATAMAZOM_SUCCESS) {
            assert(0);
            return MATAMAZOM_OUT_OF_MEMORY;
        }
    }
    return MATAMAZOM_SUCCESS;
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

    MatamazomResult flag = mtmShipCheckAmounts(matamazom->storage, orderGetPtrToProductList(orderToShip));
    if (flag != MATAMAZOM_SUCCESS) {
        return flag;
    }

    flag = mtmShipOrderExecute(matamazom->storage, orderGetPtrToProductList(orderToShip));
    if (flag != MATAMAZOM_SUCCESS) {
        assert(0);
        return MATAMAZOM_OUT_OF_MEMORY;
    }
    
    if (setRemove(matamazom->orders, orderToShip) != SET_SUCCESS) {
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

    return MATAMAZOM_NULL_ARGUMENT; // if the function got to here something went wrong
}


MatamazomResult mtmPrintOrder(Matamazom matamazom, const unsigned int orderId, FILE *output) {
    if (matamazom == NULL || output == NULL) {
        return MATAMAZOM_NULL_ARGUMENT;
    }

    Order currentOrder = mtmFindOrder(matamazom, orderId);
    if (currentOrder == NULL) {
        return MATAMAZOM_ORDER_NOT_EXIST;
    }

    fprintf(output,"Order %u Details:\n", orderId);
    MatamazomResult flag = orderPrintAllProduct (currentOrder, output);
    fprintf(output, "----------\n");
    double totalPrice = orderGetTotalPrice(currentOrder);
    fprintf(output, "Total Price: %0.3f\n", totalPrice);
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
            productPrintDetailsForOne(ptr, output);
        }
    }
    return MATAMAZOM_SUCCESS;
}

