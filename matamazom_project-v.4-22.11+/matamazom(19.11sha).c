#include <stdio.h>
#include <stdbool.h>
#include "matamazom(19.11sha).h"
#include "product.h"
#include "order.h"


struct Matamazom_t{
    AmountSet storage;
    AmountSet order;
};

/**
 * matamazomCreate: create an empty Matamazom warehouse.
 *
 * @return A new Matamazom warehouse in case of success, and NULL otherwise (e.g.
 *     in case of an allocation error)
 */
Matamazom matamazomCreate(){ // eilon 666!!!!
    Matamazom matamazom_new = malloc(sizeof(*matamazom_new));
    if  (matamazom_new == NULL){
        return NULL;
    }
    matamazom_new -> storage = asCreate(&copyProduct, &freeProduct, &compareProduct);
    matamazom_new -> order = asCreate(&orderCopy, &orderFree, &orderCompare); // compare with eilon
}
/**
 * matamazomDestroy: free a Matamazom warehouse, and all its contents, from
 * memory.
 *
 * @param matamazom - the warehouse to free from memory. A NULL value is
 *     allowed, and in that case the function does nothing.
 */
void matamazomDestroy(Matamazom matamazom);

/**
 * mtmNewProduct: add a new product to a Matamazom warehouse.
 *
 * @param matamazom - warehouse to add the product to. Must be non-NULL.
 * @param id - new product id. Must be non-negative, and unique.
 * @param name - name of the product, e.g. "apple". Must be non-empty.
 * @param amount - the initial amount of the product when added to the warehouse.
 * @param amountType - defines what are valid amounts for this product.
 * @return
 *     MATAMAZOM_NULL_ARGUMENT - if matamazom or name are NULL.
 *     MATAMAZOM_INVALID_NAME - if name is empty, or doesn't start with a
 *         letter (a -z, A -Z) or a digit (0 -9).
 *     MATAMAZOM_INVALID_AMOUNT - if amount < 0, or is not consistent with amountType
 *         (@see MatamazomAmountType documentation above)
 *     MATAMAZOM_PRODUCT_ALREADY_EXIST - if a product with the given id already exist.
 *     MATAMAZOM_SUCCESS - if product was added successfully.
 */
MatamazomResult mtmNewProduct(Matamazom matamazom, const unsigned int id, const char *name,
                              const double amount, const MatamazomAmountType amountType,
                              const MtmProductData, MtmCopyData, MtmFreeData, MtmGetProductPrice);
/**
 * mtmChangeProductAmount: increase or decrease the amount of an *existing* product in a Matamazom warehouse.
 * if 'amount' < 0 then this amount should be decreased from the matamazom warehouse.
 * if 'amount' > 0 then this amount should be added to the matamazom warehouse.
 * if 'amount' = 0 then nothing should be done.
 * please note:
 * If the amount to decrease is larger than the product's amount in the
 * warehouse, then the product's amount is not changed, and a proper error-code
 * is returned.
 * If the amount is equal to the product's amount in the
 * warehouse,then the product will remain inside the warehouse
 * with amount of zero.
 *
 * @param matamazom - warehouse to add the product to. Must be non-NULL.
 * @param id - existing product id. Must exist in the warehouse.
 * @param amount - the amount of the product to increase/decrease to the warehouse.
 * @return
 *     MATAMAZOM_NULL_ARGUMENT - if a NULL argument is passed.
 *     MATAMAZOM_PRODUCT_NOT_EXIST - if matamazom does not contain a product with
 *         the given id.
 *     MATAMAZOM_INVALID_AMOUNT - if amount is not consistent with product's amount type
 *         (@see parameter amountType in mtmNewProduct).
 *     MATAMAZOM_INSUFFICIENT_AMOUNT - if 'amount' < 0 and the amount to be decreased
 *         is bigger than product's amount in the warehouse.
 *     MATAMAZOM_SUCCESS - if product amount was increased/decreased successfully.
 * @note Even if amount is 0 (thus the function will change nothing), still a proper
 *    error code is returned if one of the parameters is invalid, and MATAMAZOM_SUCCESS
 *    is returned if all the parameters are valid.
 */
MatamazomResult mtmChangeProductAmount(Matamazom matamazom, const unsigned int id, const double amount);

/**
 * mtmClearProduct: clear a product from a Matamazom warehouse.
 *
 * The entire amount of the product is removed, and the product is erased
 * completely from the warehouse, from all existing orders and from the
 * 'income' mechanism(holding the profits for each existing product).
 * For example, after clearing a product with
 * mtmClearProduct, calling mtmAddProduct on that product will fail.
 *
 * @param matamazom - warehouse to remove the product from.
 * @param id - id of product to be removed.
 * @return
 *     MATAMAZOM_NULL_ARGUMENT - if a NULL argument is passed.
 *     MATAMAZOM_PRODUCT_NOT_EXIST - if matamazom does not contain a product with
 *         the given id.
 *     MATAMAZOM_SUCCESS - if product was cleared successfully.
 */
MatamazomResult mtmClearProduct(Matamazom matamazom, const unsigned int id);

/**
 * mtmCreateNewOrder: create a new empty order in a Matamazom warehouse, and
 * return the order's id.
 *
 * @param matamazom - a Matamazom warehouse
 * @return
 *     Positive id of the new order, if successful.
 *     0 in case of failure.
 */
unsigned int mtmCreateNewOrder(Matamazom matamazom) {  /// 33333
    return mtmInsertNewOrder(matamazom, asGetSize(matamazom->order)+1);
    // sends to func that create and inserts new order with an id that is bigger than the amount of current orders
}

static unsigned int mtmInsertNewOrder(Matamazom matamazom, unsigned int newOrderId) {  /// 33333
    Order newOrder = orderCreate(newOrderId);
    AmountSetResult flag = asRegister(matamazom->order, newOrder);
    unsigned int valueToReturn = newOrderId;
    if(asRegister(matamazom->order)!=AS_ITEM_ALREADY_EXISTS) { // if newOrderId is already in use by another order
        valueToReturn = mtmInsertNewOrder(matamazom, newOrderId+1); // try insert new Order with bigger num as id
    }
    else if (flag!=AS_SUCCESS) { // if something went wrong
        valueToReturn = 0;
    }
    orderFree(newOrder); // the new order as getting copied inside asRegister so this one is useless and delete
    return valueToReturn;
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
                                              const unsigned int productId, const double amount);
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
MatamazomResult mtmCancelOrder(Matamazom matamazom, const unsigned int orderId);

/**
 * mtmPrintInventory: print a Matamazom warehouse and its contents as
 * explained in the *.pdf
 *
 * @param matamazom - a Matamazom warehouse to print.
 * @param output - an open, writable output stream, to which the contents are printed.
 * @return
 *     MATAMAZOM_NULL_ARGUMENT - if a NULL argument is passed.
 *     MATAMAZOM_SUCCESS - if printed successfully.
 */
MatamazomResult mtmPrintInventory(Matamazom matamazom, FILE *output);

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
MatamazomResult mtmPrintOrder(Matamazom matamazom, const unsigned int orderId, FILE *output);

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


