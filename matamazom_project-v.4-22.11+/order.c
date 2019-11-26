//
// Created by shach on 15/11/2019.
//

#include "order.h"
#include <stdlib.h>
#include "product.h"
#include "matamazom_print.h"
#include "libmtm/list.h"
#include <assert.h>


struct Order_t {
    unsigned int id;
    List productCart;
};

Order orderCreate(unsigned int newId) {
    Order new_order = malloc(sizeof(*new_order));
    if (!new_order) {
        return NULL;
    }

    /*
    Product (*copyFunc)(Product product) = &copyProduct;
    void (*freeFunc)(Product product) = &freeProduct;
    int (*compareFunc)(Product first, Product second) = &compareProduct;
     */

    new_order->productCart = listCreate(orderCopy, orderFree);
    if (!new_order->productCart) {
        return NULL;
    }
    new_order->id = newId;

    return new_order;
}



static Order orderCopyUAX (Order target) {
    if (!target) {
        return NULL;
    }
    Order newOrder = orderCreate(target->id);
    return newOrder;
}

ListElement orderCopy (ListElement target) {
    ListElement copiedOrder = orderCopyUAX (target);
    return copiedOrder;
}

static void orderFreeUAX (Order target) {
    listDestroy(target->productCart);
    target->id = 0;
    free(target);
}

void orderFree (ListElement target) {
    orderFreeUAX(target);
}


static int orderCompareAUX(Order first, Order second) {
    return (int) (first->id - second->id);
}

int orderCompare(ListElement first, ListElement second) {
    return orderCompareAUX(first, second);
}


ListResult isProductIdInOrder (Order order, unsigned int id) {
    return (productAlreadyExists(order->productCart, id));
}

void orderChangeProductAmount (Order order, unsigned int id, double amount) { // 666 not complete
    if (productChangeAmount(order->productCart, id, amount) <=0 ) {
        productRemove (order->productCart, id);
    }
}

MatamazomResult addProductToOrder (Order order, Product product) {
    return addProductToList (order->productCart, product);
}


MatamazomResult orderRegisterProductOrChangeAmount (Order currentOrder, Product productToAdd, const double amount) {
    if (currentOrder == NULL || productToAdd == NULL) {
        return MATAMAZOM_NULL_ARGUMENT;
    }

    MatamazomResult check = addProductToList(currentOrder->productCart, productToAdd);

    if ( check == MATAMAZOM_ORDER_NOT_EXIST

    }

    ListResult flag =


    AmountSetResult flag = asRegister(currentOrder->productCart, productToAdd);
    if (flag==AS_SUCCESS || flag == AS_ITEM_ALREADY_EXISTS) {
        flag = asChangeAmount(currentOrder->productCart, productToAdd, amount);
        if (flag == AS_INSUFFICIENT_AMOUNT) {
            flag = asDelete(currentOrder->productCart, productToAdd);
        }
    }
    return flag;
}



void orderChangeId (Order target ,unsigned int newId) {
    target->id = newId;
}


MatamazomResult orderPrintAllProduct (Order order, FILE *output) {
    if (order == NULL || output == NULL) {
        return MATAMAZOM_NULL_ARGUMENT;
    }

    AS_FOREACH(Product , currentProduct, order->productCart) {
        double* currentAmount;
        if (asGetAmount(order->productCart, currentProduct, currentAmount) != AS_SUCCESS) { // make sure this is the right syntax with currentAmount
            assert(0);
            return MATAMAZOM_OUT_OF_MEMORY;
        }
        if (productPrintDetails(currentProduct, *currentAmount, output) != MATAMAZOM_SUCCESS) { // make sure this is the right syntax with currentAmount
            assert(0);
            return MATAMAZOM_OUT_OF_MEMORY;
        }
    }
    return MATAMAZOM_SUCCESS;
}


double orderGetTotalPrice (Order order) {
    if (order == NULL) {
        assert(0);
        return 0;
    }

    double totalPrice = 0;
    AS_FOREACH(Product , currentProduct, order->productCart) {
        double* currentAmount;
        if (asGetAmount(order->productCart, currentProduct, currentAmount) != AS_SUCCESS) { // make sure this is the right syntax with currentAmount
            assert(0);
            return MATAMAZOM_OUT_OF_MEMORY;
        }
        totalPrice += realProductPrice(currentProduct, *currentAmount); // // make sure this is the right syntax with currentAmount
    }
    return totalPrice;
}



/** testing:
unsigned int productIdCopy (unsigned int *id) {
    unsigned int copied = *id;
    return copied;
}

void productIdFree (unsigned int *id) {
    *id = 0; // !!! it is just int.
}

int productIdCompare (unsigned int *first, unsigned int *second) {
    return (int) (*first - *second);
}
 */
