#include <stdio.h>
#include <stdlib.h>
#include "../set.h"
#include "../list.h"
#include "../amount_set.h"
#include "../product.h"
#include <assert.h>

ASElement copyInt(ASElement number) {
    int *copy = malloc(sizeof(*copy));
    if (copy != NULL) {
        *copy = *(int *)number;
    }
    return copy;
}

 void freeInt(ASElement number) { free(number); }

 int compareInts(ASElement lhs, ASElement rhs) {
    return (*(int *)lhs) - (*(int *)rhs);
}

double dummyFunc(MtmProductData basePrice, const double amount) {
    return 0;
}


void testPrintSetInt (Set set) {
    printf("<----Set check: {");
    SET_FOREACH(int*, current, set) {
        printf("%d, ", *current);
    }

    printf("}----->\n");
}

void testPrintListInt (List list) {
    printf("<----List check: {");
    LIST_FOREACH(int*, current, list) {
        printf("%d, ", *current);
    }
    printf("}----->\n");
}

int main() {

    Set set2 = setCreate(&copyProduct, &freeProduct, &compareProduct);
    List list = listCreate(copyProduct, freeProduct);

    int * dummyData;
    int n = -999;
    dummyData = &n;
    Product p1 = productCreate(1, "A prod",1, MATAMAZOM_ANY_AMOUNT, &dummyData, copyInt, freeInt, dummyFunc);
    Product p2 = productCreate(2, "B prod", 1, MATAMAZOM_ANY_AMOUNT, &dummyData, copyInt, freeInt, dummyFunc);
    Product p3 = productCreate(3, "C prod", 1, MATAMAZOM_ANY_AMOUNT, &dummyData, copyInt, freeInt, dummyFunc);
    Product p4 = productCreate(4, "D prod", 1, MATAMAZOM_ANY_AMOUNT, &dummyData, copyInt, freeInt, dummyFunc);
    Product p5 = productCreate(5, "E prod", 1, MATAMAZOM_ANY_AMOUNT, &dummyData, copyInt, freeInt, dummyFunc);

    setAdd(set2, p1);
    setAdd(set2, p2);
    setAdd(set2, p3);
    setAdd(set2, p4);
    setAdd(set2, p5);

    listInsertFirst(list, p5);
    listInsertFirst(list, p3);
    listInsertFirst(list, p1);
    listInsertFirst(list, p2);
    listInsertFirst(list, p4);


    listSort(list, compareProduct);
    printf("this test prove that we can add anywhere elements to list and then sort:\n");
    testPrintListInt(list);

    Set set = setCreate(&copyInt, &freeInt, &compareInts);


    int ids[7] = {2, 5, 7, 3, 8, 9, 40};
    for (int i = 0; i < 7; ++i) {
        setAdd(set, (ids + i));
    }
    printf("this tests prove that we can control the Set element by looping its element with SET_FOREACH\n");
    testPrintSetInt(set);

    int * n1 = setGetFirst(set);
    *n1 = 11;
    testPrintSetInt(set);
    int * n2 = setGetFirst(set);
    *n2 = 22;
    testPrintSetInt(set);
    int * n3 = setGetFirst(set);
    n3 = setGetNext(set);
    n3 = setGetNext(set);
    *n3 = 33;
    testPrintSetInt(set);


    return 0;
}
