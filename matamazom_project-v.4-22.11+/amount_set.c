#include <stdbool.h>
#include <stdlib.h>
#include "amount_set.h"
//#include <stdio.h> // used only for testing
#include <assert.h>

/**
 * Generic Amount Set Container
 *
 * Implements a sorted amount set container type.
 * The set has an internal iterator for external use. For all functions
 * where the state of the iterator after calling that function is not stated,
 * it is undefined. That is you cannot assume anything about it.
 * The set is sorted in ascending order - iterating over the set is done in the
 * same order.
 *
 * The following functions are available:
 *   asCreate           - Creates a new empty set
 *   asDestroy          - Deletes an existing set and frees all resources
 *   asCopy             - Copies an existing set
 *   asGetSize          - Returns the size of the set
 *   asContains         - Checks if an element exists in the set
 *   asGetAmount         - Returns the amount of an element in the set
 *   asRegister         - Add a new element into the set
 *   asChangeAmount     - Increase or decrease the amount of an element in the set
 *   asDelete           - Delete an element completely from the set
 *   asClear            - Deletes all elements from target set
 *   asGetFirst         - Sets the internal iterator to the first element
 *                        in the set, and returns it.
 *   asGetNext          - Advances the internal iterator to the next element
 *                        and returns it.
 *   AS_FOREACH         - A macro for iterating over the set's elements
 */

/** Type for defining the set:
 * AmountSet is a set hold generic elements (void *),
 * and for each elements also holds a double represents its amount.
 *
 * the set based on a "chain" of connected nodes: the set holdes a pointer to the first
 * node (in this "chain of nodes") and a pointer to another node in the chain.
 * in order to manipulate the nodes and the data inside of them the AmountSet passes
 * between the nodes using the first node and its (build in) nextNode parameter, all that
 * by points one of the nodes by pointer ("current" node).
 *
 * */


// a generic connected node that holds an element and a double represent its amount
typedef struct amountSetNode_t {
    ASElement element;
    struct amountSetNode_t *nextNode;
    double amount;
} *ASNode;

// a generic set base on connected nodes
struct AmountSet_t {
    // the first node contain the first element (first by compareElementFunc)
    ASNode firstNode;
    // current used for manipulate all the data inside the connected nodes
    ASNode current;
    CopyASElement copyElementFunc;
    FreeASElement freeElementFunc;
    CompareASElements compareElementFunc;
};

/**
 * asCreate: Allocates a new empty amount set.
 *
 * @param copyElement - Function pointer to be used for copying elements into
 *     the set or when copying the set.
 * @param freeElement - Function pointer to be used for removing data elements from
 *     the set.
 * @param compareElements - Function pointer to be used for comparing elements
 *     inside the set. Used to check if new elements already exist in the set.
 * @return
 *     NULL - if one of the parameters is NULL or allocations failed.
 *     A new amount set in case of success.
 */
AmountSet asCreate(CopyASElement copyElement, FreeASElement freeElement, CompareASElements compareElements) {
    if (copyElement==NULL || freeElement==NULL || compareElements==NULL) {
        return NULL;
    }

    AmountSet ptr = malloc(sizeof(*ptr));
    if (ptr == NULL) {
        return NULL;
    }

    ptr->current = NULL;
    ptr->firstNode = NULL;
    ptr->copyElementFunc = copyElement;
    ptr->freeElementFunc = freeElement;
    ptr->compareElementFunc = compareElements;

    return ptr;
}

/**
 * asNode: Allocates a new node and make a copy of element to be its data.
 *
 * @param set - The set to that the node needs to be from its node type
 *      (having the same type of element). used the set copyElementFunc
 *      to make a copy of element
 * @param element - The element to copy and make the node data
 * @return
 *     NULL - if one of the parameters is NULL or allocations failed.
 *     A new node contain the copied element in case of success.
 */
static ASNode nodeCreate (AmountSet set, ASElement element) {
    ASNode newNode = malloc(sizeof(*newNode));
    if (newNode == NULL) {
        return NULL;
    }
    newNode->element = set->copyElementFunc(element);
    if (newNode->element == NULL) {
        free(newNode);
        return NULL;
    }
    newNode->amount = 0;
    newNode->nextNode = NULL;
    return newNode;
}


/**
 * asDestroy: Deallocates an existing amount set. Clears all elements by using
 * the stored free functions.
 *
 * @param set - Target set to be deallocated. If set is NULL nothing will be done.
 */
void asDestroy(AmountSet set) {
    // the clear function doing the same as destroy
    // except realising the whole set param
    // (basically realising all the set's nodes
    asClear(set);
    free(set);
}

/**
 * asCopy: Creates a copy of target set.
 *
 * Iterator values for both sets are undefined after this operation.
 *
 * @param set - Target set.
 * @return
 *     NULL if a NULL was sent or a memory allocation failed.
 *     An amount set containing the same elements (and amounts) as set, otherwise.
 */
AmountSet asCopy(AmountSet set) {
    if (set == NULL) {
        return NULL;
    }

    AmountSet duplicated_set = asCreate(set->copyElementFunc, set->freeElementFunc, set->compareElementFunc);
    if (duplicated_set == NULL) {
        return NULL;
    }
    if (set->firstNode == NULL){
        return  duplicated_set;
    }

    ASNode copy_node = nodeCreate(set, set->firstNode->element);
    if (copy_node == NULL){
        asDestroy(duplicated_set);
        return NULL; // if segmentation failed, then the whole function failed
    }
    copy_node->amount = set->firstNode->amount;
    duplicated_set->firstNode = copy_node;
    duplicated_set->current = copy_node;

    set->current = set->firstNode->nextNode;

    for (ASNode temp_node_for_copying = NULL; (set->current != NULL) ;
         copy_node = copy_node-> nextNode, asGetNext(set)) {
        temp_node_for_copying = nodeCreate(set, set->current->element);
        if (temp_node_for_copying == NULL){
            asDestroy(duplicated_set);
            return NULL;
            // if segmentation failed, then the whole function failed
        }
        temp_node_for_copying -> amount = set->current->amount;
        copy_node -> nextNode = temp_node_for_copying;
    }

    return duplicated_set;
}


/**
 * asGetSize: Returns the number of elements in a set.
 *
 * Iterator's state is unchanged after this operation.
 *
 * @param set - The set which size is requested.
 * @return
 *     -1 if a NULL pointer was sent.
 *     Otherwise the number of elements in the set.
 */
int asGetSize(AmountSet set) {
    if (set==NULL) {
        return -1;
    }
    ASNode tempLoop = set->firstNode;
    int counter = 0;
    while (tempLoop != NULL) {
        counter++;
        tempLoop = tempLoop->nextNode;
    }
    return counter;
}

/**
 * asContains: Checks if an element exists in the set.
 *
 * The element will be considered in the set if one of the elements in the set
 * it determined equal using the comparison function used to initialize the set.
 * Iterator's state is unchanged after this operation.
 *
 * @param set - The set to search in.
 * @param element - The element to look for. Will be compared using the
 *     comparison function.
 * @return
 *     false - if the input set is null, or if the element was not found.
 *     true - if the element was found in the set.
 */
bool asContains(AmountSet set, ASElement element) {
    if (set==NULL) {
        return false;
    }

    ASNode tempLoop = set->firstNode;
    while (tempLoop != NULL) {
        if(set->compareElementFunc(tempLoop->element, element) == 0) {
            return true; // in case find an identical element in set
        }
        tempLoop = tempLoop->nextNode;
    }
    return false; // in case did find identical element in set
}

/**
 * asGetAmount: Returns the amount of an element in the set.
 *
 * The function returns an error code indicating wether the operation succeeded,
 * and in case of success also returns the element's amount via the outAmount
 * pointer.
 * Iterator's state is unchanged after this operation.
 *
 * @param set - The set which contains the element.
 * @param element - The element whose amount is requested.
 * @param outAmount - Pointer to the location where the amount is returned, in case
 *     of success. In case of failure, the contents of outAmount are unchanged.
 * @return
 *     AS_NULL_ARGUMENT - if a NULL argument was passed.
 *     AS_ITEM_DOES_NOT_EXIST - if the element doesn't exist in the set.
 *     AS_SUCCESS - if the amount was returned successfully.
 *
 * For example, to print the amount of the first item in the set:
 * @code
 * double firstAmount;
 * ASElement first = asGetFirst(set);
 * if (asGetAmount(set, first, &firstAmount) == AS_SUCCESS) {
 *     printf("First amount: %f\n", firstAmount);
 * }
 * @endcode
 */
AmountSetResult asGetAmount(AmountSet set, ASElement element, double *outAmount) {
    if (set==NULL || element==NULL) {
        return AS_NULL_ARGUMENT;
    }

    ASNode tempLoop = set->firstNode;
    while (tempLoop != NULL) {
        if(set->compareElementFunc(tempLoop->element, element) == 0) {
            *outAmount = (tempLoop->amount);
            return AS_SUCCESS; // in case find an identical element in set
        }
        tempLoop = tempLoop->nextNode;
    }
    return AS_ITEM_DOES_NOT_EXIST;
}


/**
 * asRegister: Add a new element into the set.
 *
 * The element is added with an initial amount of 0.
 * Iterator's value is undefined after this operation.
 *
 * @param set - The target set to which the element is added.
 * @param element - The element to add.
 * @return
 *     AS_NULL_ARGUMENT - if a NULL argument was passed.
 *     AS_ITEM_ALREADY_EXISTS - if an equal element already exists in the set.
 *     AS_SUCCESS - if the element was added successfully.
 */
AmountSetResult asRegister(AmountSet set, ASElement element) {
    if (set == NULL || element == NULL) {
        return AS_NULL_ARGUMENT;
    }

    if (set->firstNode == NULL) { // first element in set
        ASNode newNode = nodeCreate(set, element);
        if (newNode == NULL) {
            return AS_OUT_OF_MEMORY;
        }
        set->firstNode = newNode;
        return AS_SUCCESS;
    }

    ASNode prevNode = NULL; // the node to insert the new node after
    ASNode tempLoop = set->firstNode;
    while (tempLoop != NULL) {
        int positionFlag = set->compareElementFunc(tempLoop->element, element);
        if (positionFlag == 0) {
            return AS_ITEM_ALREADY_EXISTS;
        }
        else if (positionFlag > 0) {
            ASNode newNode = nodeCreate(set, element);
            if (newNode == NULL) {
                return AS_OUT_OF_MEMORY;
            }

            if (prevNode == NULL) {
                // insert the new node as first in the set
                newNode->nextNode = tempLoop;
                set->firstNode = newNode;
            }
            else {
                // insert the new node between the prevNode and its nextNode
                newNode->nextNode = tempLoop;
                prevNode->nextNode = newNode;
            }
            return AS_SUCCESS;
        }
        prevNode = tempLoop;
        tempLoop = tempLoop->nextNode;
    }

    assert(prevNode->nextNode==NULL);
    // must be null because prev supposed to be the last node inside the set
    // (before adding newNode).if the assert fails than we have a logic problem

    // if the func got to here so the newNode supposed to be last in the set
    ASNode newNode = nodeCreate(set, element);
    if (newNode == NULL) {
        return AS_OUT_OF_MEMORY;
    }
    prevNode->nextNode = newNode;
    return AS_SUCCESS;
}

/**
 * asChangeAmount: Increase or decrease the amount of an element in the set.
 *
 * Iterator's state is unchanged after this operation.
 *
 * @param set - The target set containing the element.
 * @param element - The element whose amount is increased.
 * @param amount - How much to change the element's amount. A positive value
 *     increases element's amount, while a negative value decreases it. A value
 *     of 0 means don't change it.
 * @return
 *     AS_NULL_ARGUMENT - if a NULL argument was passed.
 *     AS_ITEM_DOES_NOT_EXIST - if the element doesn't exist in the set.
 *     AS_SUCCESS - if the element's amount was changed successfully.
 *
 * @note parameter amount doesn't affect the return value. Even if amount is 0,
 *     still AS_ITEM_DOES_NOT_EXIST is returned if element doesn't exist in set,
 *     and AS_SUCCESS is returned if element exists in set (assuming set is not
 *     NULL).
 */
AmountSetResult asChangeAmount(AmountSet set, ASElement element, const double amount) {
    if (set==NULL || element==NULL) {
        return AS_NULL_ARGUMENT;
    }

    ASNode tempLoop = set->firstNode;
    while (tempLoop != NULL) {
        if (set->compareElementFunc(tempLoop->element, element) == 0) {
            if (tempLoop->amount + amount <= 0) {
                return AS_INSUFFICIENT_AMOUNT;
            }
            else {
                tempLoop->amount = tempLoop->amount + amount;
                return AS_SUCCESS;
            }
        }
        tempLoop = tempLoop->nextNode;
    }

    // default return in case did find an identical element
    return AS_ITEM_DOES_NOT_EXIST;
}


/**
 * asDelete: Delete an element completely from the set.
 *
 * This is the opposite of asRegister. The element is deallocated using the
 * stored free function.
 * Iterator's value is undefined after this operation.
 *
 * @param set - The target set from which the element is deleted.
 * @param element - The element to delete.
 * @return
 *     AS_NULL_ARGUMENT - if a NULL argument was passed.
 *     AS_ITEM_DOES_NOT_EXIST - if the element doesn't exist in the set.
 *     AS_SUCCESS - if the element was deleted successfully.
 */
AmountSetResult asDelete(AmountSet set, ASElement element) {
    if (set==NULL || element==NULL) {
        return AS_NULL_ARGUMENT;
    }
    ASNode prevNode = NULL;
    ASNode tempLoop = set->firstNode;
    while (tempLoop != NULL) { /// 333
        if(set->compareElementFunc(tempLoop->element, element) == 0) {
            if (prevNode == NULL) {
                set->firstNode = tempLoop->nextNode;
            }
            else {
                prevNode->nextNode = tempLoop->nextNode;

            }
            set->freeElementFunc(tempLoop->element);
            free(tempLoop);
            return AS_SUCCESS;
        }
        prevNode = tempLoop;
        tempLoop = tempLoop->nextNode;
    }
    return AS_ITEM_DOES_NOT_EXIST;
}

/**
 * asClear: Deletes all elements from target set.
 *
 * The elements are deallocated using the stored free function.
 * Iterator's value is undefined after this operation.
 *
 * @param set - Target set to delete all elements from.
 * @return
 *     AS_NULL_ARGUMENT - if a NULL pointer was sent.
 *     AS_SUCCESS - Otherwise.
 */
AmountSetResult asClear(AmountSet set) {
    if (set == NULL) {
        return AS_NULL_ARGUMENT;
    }

    ASNode tempLoop = set->firstNode;
    ASNode nextTarget = NULL;

    set->current=NULL;
    set->firstNode=NULL;

    while(tempLoop != NULL) { // release all the set's nodes
        nextTarget = tempLoop->nextNode;
        set->freeElementFunc(tempLoop->element);
        free(tempLoop);
        tempLoop=nextTarget;
    }
    return AS_SUCCESS;
}

/**
 * asGetFirst: Sets the internal iterator (also called current element) to
 * the first  element in the set. There doesn't need to be an internal order
 * of the elements so the "first" element is any element.
 * Use this to start iterating over the set.
 * To continue iteration use asGetNext
 *
 * @param set - The set for which to set the iterator and return the first
 *     element.
 * @return
 *     NULL if a NULL pointer was sent or the set is empty.
 *     The first element of the set otherwise
 */
ASElement asGetFirst(AmountSet set) {
    if (set == NULL || set->firstNode == NULL) {
        return NULL;
    }
    set->current = set->firstNode;
    return set->current->element;
}

/**
 * asGetNext: Advances the set iterator to the next element and returns it.
 * The next element is any element not previously returned by the iterator.
 *
 * @param set - The set for which to advance the iterator
 * @return
 *     NULL if reached the end of the set, or the iterator is at an invalid state
 *     or a NULL sent as argument
 *     The next element on the set in case of success
 */
ASElement asGetNext(AmountSet set) {
    if (set == NULL || set->current == NULL) {
        return NULL;
    }
    set->current = set->current->nextNode;
    if (set->current != NULL) {
        return set->current->element;
    }
    return NULL;
}
