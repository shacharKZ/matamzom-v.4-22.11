#include <stdbool.h>
#include "matamazom.h"
#include <stdlib.h>
#include "product.h"
#include "order.h"
#include "list.h"
#include "set.h"
#include <assert.h>


struct Matamazom_t {
    List storage;
    Set orders;
    unsigned int ordersCount;
};


/**
 * mtmFindOrder: Checks if an order exists in the matamazom and return its pointer
 *
 * @param matamazom - The matamazom to search in.
 * @param orderId - The order unique id to look for. Will be compared using the
 *     Order comparison function.
 * @return
 *     return a pointer to the order if there is an order with the given id inside matamazom
 *     and return null otherwise
 */
static Order mtmFindOrder(Matamazom matamazom, const unsigned int orderId) {
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

Matamazom matamazomCreate() {
    Matamazom matamazom_new = malloc(sizeof(*matamazom_new));
    if (matamazom_new == NULL) {
        return NULL;
    }

    matamazom_new->storage = listCreate(copyProduct, freeProduct);
    if (matamazom_new->storage == NULL) {
        free(matamazom_new);
        return NULL;
    }

    matamazom_new->orders = setCreate(&orderCopy, &orderFree, &orderCompare);
    if (matamazom_new->orders == NULL) {
        if (matamazom_new->storage != NULL) {
            listDestroy(matamazom_new->storage);
        }
        free(matamazom_new);
        return NULL;
    }
    matamazom_new->ordersCount = 1; // the first order will be given the id "1"
    return matamazom_new;
}

void matamazomDestroy(Matamazom matamazom) {
    if (matamazom == NULL) {
        return;
    }
    setDestroy(matamazom->orders);
    listDestroy(matamazom->storage);
    free(matamazom);
}

MatamazomResult mtmNewProduct(Matamazom matamazom, const unsigned int id, const char *name,
                              const double amount, const MatamazomAmountType amountType,
                              const MtmProductData customData, MtmCopyData CopyFunc,
                              MtmFreeData FreeFunc, MtmGetProductPrice ProductPriceFunc) {

    if (matamazom == NULL || matamazom->storage == NULL || name == NULL
    || customData == NULL || CopyFunc == NULL || FreeFunc == NULL
    || ProductPriceFunc == NULL) {
        return MATAMAZOM_NULL_ARGUMENT;
    }
    if (productCheckValidName(name) == false) {
        return MATAMAZOM_INVALID_NAME;
    }
    if (productCheckValidAmount(amount, amountType) == false || amount < 0) {
        return MATAMAZOM_INVALID_AMOUNT;
    }
    if (productAlreadyExists(matamazom->storage, id)) {
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
}

MatamazomResult mtmChangeProductAmount(Matamazom matamazom, const unsigned int id, const double amount) {
    if (matamazom == NULL) {
        return MATAMAZOM_NULL_ARGUMENT;
    }
    if (productAlreadyExists(matamazom->storage, id) == false) {
        return MATAMAZOM_PRODUCT_NOT_EXIST;
    }
    if (productCheckValidAmount(amount,
            productGetAmountType(matamazom->storage, id)) == false) {
        return MATAMAZOM_INVALID_AMOUNT;
    }

    return productChangeAmountInStorage(matamazom->storage, id, amount);
}

MatamazomResult mtmClearProduct(Matamazom matamazom, const unsigned int id) {
    if (matamazom == NULL) {
        return MATAMAZOM_NULL_ARGUMENT;
    }
    if (id < 0 || productAlreadyExists(matamazom->storage, id) == false) {
        return MATAMAZOM_PRODUCT_NOT_EXIST;
    }
    productRemove(matamazom->storage, id);

    SET_FOREACH(Order, currentOrder, matamazom->orders) {
        productRemove(orderGetPtrToProductList(currentOrder), id);
    }
    return MATAMAZOM_SUCCESS;
}

MatamazomResult mtmPrintInventory(Matamazom matamazom, FILE *output) {
    if (matamazom == NULL || output == NULL) {
        return MATAMAZOM_NULL_ARGUMENT;
    }
    fprintf(output, "Inventory Status:\n");
    for (ListElement ptr = listGetFirst(matamazom->storage);
                ptr; ptr = listGetNext(matamazom->storage)) {
        productPrintDetailsForOne(ptr, output);
    }
    return MATAMAZOM_SUCCESS;
}

unsigned int mtmCreateNewOrder(Matamazom matamazom) {
    if (matamazom == NULL) {
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
}


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

    if (productCheckValidAmount(amount,
            productGetAmountType(matamazom->storage, productId)) == false) {
        return MATAMAZOM_INVALID_AMOUNT;
    }

    if (isProductIdInOrder(currentOrder, productId) == false) {
        if (amount < 0) {
            return MATAMAZOM_SUCCESS;
        }
        Product newProduct = getCopyOfProductForId
                             (matamazom->storage, productId);
        if (newProduct == NULL) {
            return MATAMAZOM_OUT_OF_MEMORY;
        }

        productSetNewAmount(newProduct, amount);
        MatamazomResult flag = orderAddProductToCart(currentOrder, newProduct);
        freeProduct(newProduct);
        return flag;
    }
    return orderChangeProductAmount(currentOrder, productId, amount);
}

static MatamazomResult mtmShipCheckAmounts(List storage, List order) {
    if (storage == NULL || order == NULL) {
        return MATAMAZOM_NULL_ARGUMENT;
    }

    LIST_FOREACH(ListElement, orderProduct, order) {
        Product storageProduct =
                CheckIfProductFromOrderIsInStorage(storage, orderProduct);
        if (storageProduct == NULL) {
            return MATAMAZOM_PRODUCT_NOT_EXIST;
        }
        if (productGetAmount(storageProduct) <
                            productGetAmount(((Product) orderProduct))) {
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
        Product storageProduct =CheckIfProductFromOrderIsInStorage
                                       (storage, currentOrderProduct);
        if (productShipChangeAmountAndProfit(storageProduct,
                productGetAmount(currentOrderProduct)) != MATAMAZOM_SUCCESS) {
            assert(0);
            return MATAMAZOM_OUT_OF_MEMORY;
        }
    }
    return MATAMAZOM_SUCCESS;
}


MatamazomResult mtmShipOrder(Matamazom matamazom, const unsigned int orderId) {
    if (matamazom == NULL) {
        return MATAMAZOM_NULL_ARGUMENT;
    }

    Order orderToShip = mtmFindOrder(matamazom, orderId);
    if (orderToShip == NULL) {
        return MATAMAZOM_ORDER_NOT_EXIST;
    }

    MatamazomResult flag = mtmShipCheckAmounts(matamazom->storage,
                              orderGetPtrToProductList(orderToShip));
    if (flag != MATAMAZOM_SUCCESS) {
        return flag;
    }

    flag = mtmShipOrderExecute(matamazom->storage,
                   orderGetPtrToProductList(orderToShip));
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

    fprintf(output, "Order %u Details:\n", orderId);
    MatamazomResult flag = orderPrintAllProduct(currentOrder, output);
    fprintf(output, "----------\n");
    double totalPrice = orderGetTotalPrice(currentOrder);
    fprintf(output, "Total Price: %0.3f\n", totalPrice);
    return flag;
}


MatamazomResult mtmPrintBestSelling(Matamazom matamazom, FILE *output) {

    if (matamazom == NULL || output == NULL) {
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

