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

    new_order->productCart = listCreate(copyProduct, freeProduct);
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
    if (target == NULL) {
        return;
    }
    if (target->productCart != NULL) {
        listDestroy(target->productCart);
    }
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


bool isProductIdInOrder (Order order, unsigned int id) {
    return productAlreadyExists(order->productCart, id);
}

MatamazomResult orderChangeProductAmount (Order order, unsigned int id, double amount) {
    return productChangeAmountInList(order->productCart, id, amount);
    }

MatamazomResult orderSetProductAmount (Order order, unsigned int id) {
    return productSetAmountForID(order->productCart, id);
}

MatamazomResult orderAddProductToCart (Order order, Product product) {
    MatamazomResult flag = productAddToList (order->productCart, product);
    Product temp = listGetFirst(order->productCart);
    if (temp) {

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

    LIST_FOREACH(Product, currentProduct, order->productCart) {

        productPrintDetails(currentProduct, output);
    }
    return MATAMAZOM_SUCCESS;
}


double orderGetTotalPrice (Order order) {
    if (order == NULL) {
        assert(0);
        return 0;
    }

    double totalPrice = 0;

    LIST_FOREACH(Product, currentProduct, order->productCart) {
        totalPrice += productGetPrice(currentProduct);
    }
    return totalPrice;
}

List orderGetPtrToProductList (Order order) {
    if (order == NULL) {
        return NULL;
    }
    return order->productCart;
}

