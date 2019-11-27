//
// Created by shach on 15/11/2019.
//

#ifndef UNTITLED_ORDER_H
#define UNTITLED_ORDER_H

#include "amount_set.h"
#include "product.h"

/** Type for defining the set */
typedef struct Order_t *Order;

Order orderCreate(unsigned int newId);

ASElement orderCopy (ASElement orderToCopy);
void orderFree (ASElement target);
int orderCompare(ASElement first, ASElement second);
void orderChangeId (Order target ,unsigned int newId);
MatamazomResult orderRegisterProductOrChangeAmount (Order currentOrder, Product productToAdd, const double amount);
MatamazomResult orderPrintAllProduct (Order order, FILE *output);
double orderGetTotalPrice (Order order);
bool isProductIdInOrder (Order order, unsigned int id);
MatamazomResult orderChangeProductAmount (Order order, unsigned int id, double amount);
MatamazomResult orderSetProductAmount (Order order, unsigned int id);
MatamazomResult orderAddProductToCart (Order order, Product product);
List orderGetPtrToProductList (Order order);

#endif //UNTITLED_ORDER_H
