#include <stdlib.h>
#include "order.h"
#include "product.h"
#include "matamazom.h"
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
    if (new_order->productCart == NULL) {
        return NULL;
    }
    new_order->id = newId;

    return new_order;
}

static Order orderCopyUAX (Order target) {
    if (target == NULL || target->productCart == NULL) {
        return NULL;
    }
    Order neWorder = malloc(sizeof(*neWorder));
    if (neWorder == NULL) {
        return NULL;
    }
    neWorder->productCart = listCopy(target->productCart);
    if (neWorder->productCart == NULL) {
        free(neWorder);
        return NULL;
    }
    neWorder->id = target->id;
    return neWorder;
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

void orderChangeId (Order target ,unsigned int newId) {
    target->id = newId;
}

bool isProductIdInOrder (Order order, unsigned int id) {
    return productAlreadyExists(order->productCart, id);
}

MatamazomResult orderChangeProductAmount (Order order, unsigned int id, double amount) {
    if (order == NULL) {
        return MATAMAZOM_NULL_ARGUMENT;
    }
    return productChangeAmountInOrder(order->productCart, id, amount);
}

MatamazomResult orderAddProductToCart (Order order, Product product) {
    if (order == NULL || product == NULL) {
        return MATAMAZOM_NULL_ARGUMENT;
    }
    return productAddToList (order->productCart, product);
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

