//
// Created by shach on 15/11/2019.
//

#include "order.h"
#include "amount_set.h"
#include <stdlib.h>
#include "product.h"
#include <assert.h>


struct Order_t {
    unsigned int id;
    AmountSet productCart;
};

Order orderCreate(unsigned int newId) {
    Order new_order = malloc(sizeof(*new_order));
    if (!new_order) {
        return NULL;
    }

    Product (*copyFunc)(Product product) = &copyProduct;
    void (*freeFunc)(Product product) = &freeProduct;
    int (*compareFunc)(Product first, Product second) = &compareProduct;

    new_order->productCart = asCreate(copyFunc, freeFunc, compareFunc);
    if (!new_order->productCart) {
        return NULL;
    }
    new_order->id = newId;

    return new_order;
}

Order orderCopy (Order target) {
    if (!target) {
        return NULL;
    }

    Order newOrder = orderCreate(target->id);
    return newOrder;
}

void orderFree (Order target) {
    asDestroy(target->productCart);
    target->id = 0;
    free(target);
}

int orderCompare(Order first, Order second) {
    return (int) (first->id - second->id);
}


AmountSetResult orderChangeProductAmount (Order currentOrder, Product productToAdd, const double amount) {
    AmountSetResult flag = asRegister(currentOrder->productCart, productToAdd);
    if (flag==AS_SUCCESS) {
        flag = asChangeAmount(currentOrder->productCart, productToAdd, amount);
    }
    return flag;
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
