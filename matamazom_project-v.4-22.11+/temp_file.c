//
// Created by eilon on 15/11/2019.
//
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define FALSE 0 // 666
#define TRUE 1 /// 666

// we do need all the info that came with the instructions
typedef enum MatamazomResult_t {
    MATAMAZOM_SUCCESS = 0,
    MATAMAZOM_NULL_ARGUMENT,
    MATAMAZOM_OUT_OF_MEMORY,
    MATAMAZOM_INVALID_NAME,
    MATAMAZOM_INVALID_AMOUNT,
    MATAMAZOM_PRODUCT_ALREADY_EXIST,
    MATAMAZOM_PRODUCT_NOT_EXIST,
    MATAMAZOM_ORDER_NOT_EXIST,
    MATAMAZOM_INSUFFICIENT_AMOUNT,
} MatamazomResult;

typedef enum MatamazomAmountType_t {
    MATAMAZOM_INTEGER_AMOUNT,
    MATAMAZOM_HALF_INTEGER_AMOUNT,
    MATAMAZOM_ANY_AMOUNT,
} MatamazomAmountType;

typedef void *MtmProductData;
typedef MtmProductData (*MtmCopyData)(MtmProductData);
typedef double (*MtmGetProductPrice)(MtmProductData, const double amount);
typedef void (*MtmFreeData)(MtmProductData);
typedef bool (*MtmFilterProduct)(const unsigned int id, const char *name,
                                 const double amount, MtmProductData customData);

typedef struct Order_t *Order;

struct product_t{

    unsigned int ID; // 666 id
    char *name;
    MatamazomAmountType amountType;
    double amount;
    MtmProductData customData;
    MtmCopyData copyDataFunc; // 666 no needs to make that an obj-value. the struct will take care of that
    MtmFreeData freeDataFunc; // 666 no needs to make that an obj-value. the struct will take care of that
    MtmGetProductPrice getProductPriceFunc;
    MtmFilterProduct filterProductFunc; // 666 no needs to make that an obj-value. the struct will take care of that
    int profit; // 666 needs to be double. maybe the right name is sold or sells?
    struct product_t first; // 666 no need. th AmountSort takes care of that
    struct product_t previous; // 666 no need. th AmountSort takes care of that
    struct product_t next; // 666 no need. th AmountSort takes care of that
};

typedef struct product_t *Product; // 666 that needs to be in the .h file


struct Matamazom_t{ // 666 we need to split between files: product.c, product.h, matamzon.h, matamazon.c
    Product product; // 666 the type is AmountSet
    Order order; // 666 the type is AmountSet
};

typedef struct Matamazom_t *Matamazom;

static Product productCreate(){
    Product product = malloc(sizeof(*product));
    if (product == NULL){
        return NULL;
    }
    product -> ID = 0;
    product -> name = NULL;
    product -> amountType = 0;
    product -> amount = 0;
    product -> customData = NULL;
    product -> copyDataFunc = NULL;
    product -> freeDataFunc = NULL; // 666 V^
    product -> getProductPriceFunc = NULL;
    product -> filterProductFunc = NULL;
    product -> profit = 0;
    product -> first = NULL;
    product -> previous = NULL;
    product -> next = NULL;

    return product;
}

static Product product_find (Matamazom matamazom, const unsigned int id){ // 666 why do we need it?
    Product ptr = matamazom->product;//the first
    // 666 use the AmountSet for that!
    if (ptr == NULL){
        return NULL;
    }
    while (ptr){
        if (ptr->ID == id){
            return ptr; // 666 do we want to return the pinter or a copy?
        }
        ptr = ptr->next;
    }
    return NULL;
}

static Product find_last(Matamazom matamazom){ // 666
    Product ptr = matamazom->product;//the first
    if (ptr==NULL){
        return NULL;
    }
    while (ptr->next){
        ptr = ptr->next;
    }
    return ptr;
}

static Product find_first(Matamazom matamazom){
    return matamazom->product;
}

static bool valid_name (char* name){ // 666 why do we need that?
    if ((name[0] <=57) && (name[0]>-48)){ //between 0-9. // 666 so use <='9', >='0'
        return TRUE; // 666 again, true
    }
    if (strlen(name)==1 || (name[0]<'A') || (name[0]>'z') || ((name[0]>'Z')&&(name[0]<'a'))){ //not a-z or A-Z
        return FALSE; // 666
    }
    return TRUE; // 666 use true
}

static bool valid_amount (const double amount, MatamazomAmountType amountType){ // 666
    if (amount < 0){
        return  FALSE;
    }
    if (amountType == MATAMAZOM_ANY_AMOUNT){
        return TRUE;
    }
    if (amountType == MATAMAZOM_INTEGER_AMOUNT){
        double x = amount;
        int y = (int)x;
        double z = x-y;
        if (((z<1)&&(z>=0.999)) || ((z>=0)&&(z<=0.001))) {
            return TRUE;
        }
        return FALSE;
    }
    if (amountType == MATAMAZOM_HALF_INTEGER_AMOUNT){
        double x = amount;
        int y = (int)x;
        double z = x-y;
        if (((z<1)&&(z>=0.999)) || ((z>=0)&&(z<=0.001)) || ((z>=0.499)&&(z<=0.501))) {
            return TRUE;
        }
        return FALSE;
    }
}

Matamazom matamazomCreate(){
    Matamazom matamazom = malloc(sizeof(*matamazom));
    if (!matamazom){
        return  NULL;
    }
    matamazom -> order = NULL;
    matamazom -> product = NULL;
    return matamazom;
}

MatamazomResult mtmClearProduct(Matamazom matamazom, const unsigned int id){

    if (matamazom == NULL){
        return MATAMAZOM_NULL_ARGUMENT;
    }

    if (id < 1){
        return MATAMAZOM_PRODUCT_NOT_EXIST;
    }

    Product ptr = product_find(matamazom, id);

    if (ptr == NULL){
        return MATAMAZOM_PRODUCT_NOT_EXIST;
    }

    ptr -> freeDataFunc (ptr->customData);
    ptr -> freeDataFunc (ptr->copyDataFunc);
    ptr -> freeDataFunc (ptr->getProductPriceFunc);
    ptr -> freeDataFunc (ptr->filterProductFunc);
    ptr -> freeDataFunc (ptr->next);
    ptr -> freeDataFunc (ptr->freeDataFunc);

    if (ptr -> previous == NULL){
        assert(matamazom->product == ptr)
        matamazom->product = ptr->next;
        return MATAMAZOM_SUCCESS;
    }

    Product ptr2 =  ptr -> previous;
    ptr2 -> next = ptr -> next;

    if (ptr -> next == NULL){
        return MATAMAZOM_SUCCESS;
    }

    Product ptr3 = ptr->next;
    ptr3 -> previous = ptr2;

    free (ptr);
    return MATAMAZOM_SUCCESS;
}

static void destroy_all_products_AUX(Product ptr){
    if (ptr == NULL){
        return;
    }
    destroy_all_products_AUX(ptr->next);
    mtmClearProduct (ptr);
}

static void destroy_all_products(Matamazom matamazom){

    Product ptr = matamazom->product;
    destroy_all_products_AUX (ptr);
}

void matamazomDestroy(Matamazom matamazom){

    if (matamazom == NULL){
        return;
    }
    destroy_all_orders (matamazom -> order); // 666 ????????????????shachar should make????
    destroy_all_products (matamazom -> product);
    free (matamazom);
}



MatamazomResult mtmNewProduct(Matamazom matamazom, const unsigned int id,
                              const char *name, const double amount, const MatamazomAmountType
                              amountType, const MtmProductData customData, MtmCopyData CopyData,
                              MtmFreeData FreeData, MtmGetProductPrice ProdPrice){

    if (matamazom == NULL || name == NULL){
        return MATAMAZOM_NULL_ARGUMENT;
    }
    if (valid_name(name) == FALSE){
        return MATAMAZOM_INVALID_NAME;
    }
    if (product_find(matamazom, id) != NULL){
        return MATAMAZOM_PRODUCT_ALREADY_EXIST;
    }

    if (valid_amount(amount, amountType)==FALSE || amount < 0){
        return MATAMAZOM_INVALID_AMOUNT;
    }

    Product product_new = productCreate();
    if (product_new== NULL){
        return MATAMAZOM_OUT_OF_MEMORY;
    }
    product_new -> ID = id;
    product_new -> name = malloc(sizeof(*name)*strlen(name));
    strcpy(product_new->name, name);
    product_new -> amountType = amountType
    product_new -> amount = amount;
    product_new -> customData = customData;
    product_new -> copyDataFunc = CopyData;
    product_new -> freeDataFunc = FreeData;
    product_new -> getProductPriceFunc = ProdPrice;
    product_new -> filterProductFunc = NULL;
    product_new -> profit = 0;
    product_new -> next = NULL;

    if (matamazom -> product == NULL){
        matamazom -> product = product_new;
        product_new -> first = product_new;
        product_new -> previous = NULL;
        return MATAMAZOM_SUCCESS;
    }

    else{
        product_new -> previous = find_last(matamazom);;
        product_new -> first = find_first(matamazom);

        Product ptr = product_find(matamazom, x);
        ptr -> next = product_new;

        return MATAMAZOM_SUCCESS;
    }
}

MatamazomResult mtmChangeProductAmount(Matamazom matamazom, const unsigned int id, const double amount){

    if (!matamazom){
        return MATAMAZOM_NULL_ARGUMENT;
    }
    Product ptr = product_find(matamazom id);
    if (ptr==NULL){
        return MATAMAZOM_PRODUCT_ALREADY_EXIST;
    }
    if (valid_amount(amount, ptr->amountType) == FALSE){
        return MATAMAZOM_INVALID_AMOUNT;
    }
    if ((ptr->amount+amount)<0){
        return MATAMAZOM_INSUFFICIENT_AMOUNT;
    }
    ptr -> amount += amount;
    return MATAMAZOM_SUCCESS;
}
