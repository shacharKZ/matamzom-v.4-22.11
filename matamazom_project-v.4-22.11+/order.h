//
// Created by shach on 15/11/2019.
//

#ifndef UNTITLED_ORDER_H
#define UNTITLED_ORDER_H

#include "libmtm/list.h"
#include "product.h"
/* Implements an order type.
 * the order type includes all fields relevant to managing orders of product
 *
 * The following functions are available:
 * orderCreate                  - create an empty order
 * orderCopy                    - copy an order entirely
 * orderFree                    - free an order entirely
 * orderCompare                 - compare two products by their ID
 * orderChangeId                - change an order's ID
 * orderPrintAllProduct         - print the entire order list of products
 * orderGetTotalPrice           - receive the total price of all products in an order
 * isProductIdInOrder           - check if in the order there is a product matching a given id
 * orderChangeProductAmount     - Add or subtract an amount from a product in an order
 * orderSetProductAmount        - set the amount of a product in an order to requested amount
 * orderAddProductToCart        - add a product entirely to an order
 * orderGetPtrToProductList     - for a given order, returns the list of items it contains
 */
/** Type for defining the set */
typedef struct Order_t *Order;

/**
 * orderCreate: create an empty order.
 *
 * @param id - new product id. Must be non-negative, and unique.

 * @return
 * A new empty order in case of success, and NULL otherwise
 * (e.g. in case of an allocation error or if given a null argument)
 *
 */
Order orderCreate(unsigned int newId);

/* orderCopy: copy a existing order
 * @param orderToCopy - the order we wish to copy
 * @return
 * A duplicate order
 * Or NULL if allocation failed
 */
ListElement orderCopy (ListElement orderToCopy);

/* orderFree: free an order and all its content
 * @param target - the order we wish to free
 */
void orderFree (ListElement target);

/* orderCompare: create a comparison between orders based on their ID
 *
 * @param first - one of the two orders which we want to compare
 * @param second - the other of the two orders which we want to compare
 * @return
 * A positive number if first is "bigger" for our criteria, a negative number if second is bigger,
 * 0 if they are equal (have the same ID, thus in fact, the same order, since its a unique field for each product
 */
int orderCompare(ListElement first, ListElement second);

/* orderChangeId: change a given order's ID
 *
 * @param target - the order we want to change it's id
 * @param newId - the new id we want to assign to the order
 */
void orderChangeId (Order target ,unsigned int newId);

/*
 * orderPrintAllProduct: print all products that are present in the order and their details
 *
 * @param order - the order we will print
 * @param output  - the destination output to which the information will be written
 * @return
 * MATAMAZOM_NULL_ARGUMENT - if one or more of the arguments is NULL
 * MATAMAZOM_SUCCESS - if order was successfully printed
 */
MatamazomResult orderPrintAllProduct (Order order, FILE *output);

/* orderGetTotalPrice: receive the order's total price for all products and their amount
 *
 * @param order - the order we want it's total price
 */
double orderGetTotalPrice (Order order);

/*
 * isProductIdInOrder: return if a product is in an order
 *
 * @param order - the order we're looking in
 * @param id - the id of the product we're searching for
 * @return
 * true  - if the product is in the order
 * false - if the product is NOT in the order
 */
bool isProductIdInOrder (Order order, unsigned int id);

/* orderChangeProductAmount: Add or subtract an amount from a product in an order
 *
 * @param order - the order in which the 'product to change he's amount' is in
 * @param id - the product's id
 * @param amount - the amount to add or subtract
 * MATAMAZOM_NULL_ARGUMENT - if one the arguments sent was NULL
 * MATAMAZOM_PRODUCT_NOT_EXIST - if the product who's change was requested does not exist
 * MATAMAZOM_SUCCESS - if amount was changed successfully
 */
MatamazomResult orderChangeProductAmount (Order order, unsigned int id, double amount);

/* orderSetProductAmount: set the amount of a product in an order to 0
 *
 * @param order - the order in which the 'product to change he's amount' is in
 * @param id - the product's id
 * MATAMAZOM_NULL_ARGUMENT - if one the arguments sent was NULL
 * MATAMAZOM_PRODUCT_NOT_EXIST - if the product who's change was requested does not exist
 * MATAMAZOM_SUCCESS - if amount was changed successfully
 */
MatamazomResult orderSetProductAmount (Order order, unsigned int id);

/* orderAddProductToCart: add an existing product to an order's cart
 *
 * @param order - the order to which we'll add the product
 * @param product - the product to add
 * MATAMAZOM_NULL_ARGUMENT - if one the arguments sent was NULL
 * MATAMAZOM_SUCCESS - if product was added successfully
 */
MatamazomResult orderAddProductToCart (Order order, Product product);

/* orderGetPtrToProductList: ???? 666 leave it this way?
 *
 * @param order -
*/
List orderGetPtrToProductList (Order order);

#endif //UNTITLED_ORDER_H
