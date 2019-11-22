//
// Created by shach on 15/11/2019.
//

#ifndef UNTITLED_ORDER_H
#define UNTITLED_ORDER_H

#include "amount_set.h"

/** Type for defining the set */
typedef struct Order_t *Order;

Order orderCreate(unsigned int newId);

Order orderCopy (Order target);
void orderFree (Order target);
int orderCompare(Order first, Order second);

#endif //UNTITLED_ORDER_H
