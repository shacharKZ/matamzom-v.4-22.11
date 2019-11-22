#include <stdio.h>
#include <stdlib.h>
#include "amount_set.h"
#include "order.h"
#include "product.h"
#include <assert.h>


int main() {
    ASElement (*copyFunc)(ASElement) = &orderCopyASElement;
    void (*freeFunc)(ASElement) = &orderFreeASElement;
    int (*compareFunc)(ASElement, ASElement) = &orderCompareASElement;

    AmountSet as = asCreate(copyFunc,freeFunc,compareFunc);


    Order o1 = orderCreate(1);
    Order o2 = orderCreate(2);
    Order o3 = orderCreate(3);
    Order o4 = orderCreate(4);
    Order o5 = orderCreate(4);

    printf("0) size of as is: %d \n", asGetSize(as));
    asRegister(as, o3);
    printf("1) size of as is: %d \n", asGetSize(as));
    asRegister(as, o2);
    printf("2) size of as is: %d \n", asGetSize(as));
    asRegister(as, o1);
    printf("3) size of as is: %d \n", asGetSize(as));
    asRegister(as, o4);
    printf("4) size of as is: %d \n", asGetSize(as));
    asRegister(as, o3);
    printf("5) size of as is: %d \n", asGetSize(as));
    asRegister(as, o5);
    printf("6) size of as is: %d \n", asGetSize(as));
    asClear(as);
    printf("7) size of as is: %d \n", asGetSize(as));
    asRegister(as, o3);
    printf("8) size of as is: %d \n", asGetSize(as));
    asRegister(as, o2);
    printf("9) size of as is: %d \n", asGetSize(as));


    AmountSet a2 = asCopy(as);
    printf("10) size of a2 is: %d \n", asGetSize(a2));
    asRegister(a2, o3);
    printf("11) size of a2 is: %d \n", asGetSize(a2));
    asRegister(a2, o2);
    printf("12) size of a2 is: %d \n", asGetSize(a2));
    asRegister(a2, o1);
    printf("13) size of a2 is: %d \n", asGetSize(a2));

    asClear(as);
    printf("14) size of as is: %d \n", asGetSize(as));
    printf("15) size of a2 is: %d \n", asGetSize(a2));


    printf("16) is as contain o1: %d \n", asContains(as, o1));
    printf("17) is as contain o2: %d \n", asContains(as, o2));
    printf("18) is a2 contain o5: %d \n", asContains(a2, o1));
    printf("19) is a2 contain o4: %d \n", asContains(a2, o4));

    double f1 = 5;
    double f2 = -1;
    printf("20) changeAmount: is pass: %d (0 means it pass ok)\n", asChangeAmount(a2, o2, f1));
    printf("21) getAmount pass: %d (0 means it pass ok)\n", asGetAmount(a2,o2,&f2));
    printf("22) new amount is: %f \n", f2);

    printf("23) changeAmount: is pass: %d (5 means it stops because the object is not there)\n", asChangeAmount(a2, o5, f1));
    printf("24) getAmount pass: %d (5 means it stops because the object is not there)\n", asGetAmount(a2,o5,&f2));

    f1 = 3;
    printf("25) changeAmount: is pass: %d (0 means it pass ok)\n", asChangeAmount(a2, o2, f1));
    printf("26) getAmount pass: %d (0 means it pass ok)\n", asGetAmount(a2,o2,&f2));
    printf("27) new amount is: %f \n", f2);

    f1 = -10;
    printf("28) changeAmount: is pass: %d (6 means to much to decrease)\n", asChangeAmount(a2, o2, f1));
    printf("29) getAmount pass: %d (0 means it pass ok)\n", asGetAmount(a2,o2,&f2)); // carfull using pointers or doubles
    printf("30) new amount is: %f \n", f2);

    asGetFirst(a2);
    asGetFirst(as);
    asGetNext(a2);
    asGetNext(as);

    asRegister(as, o2);
    printf("31) size of as is: %d \n", asGetSize(as));
    asRegister(as, o3);
    printf("32) size of as is: %d \n", asGetSize(as));
    asDestroy(as);
    // printf("size of as is: %d \n", asGetSize(as)); // PROBLEM!!! after internet serching decide its ok. i talk with u about it


    printf("Hello, World2!\n");

    return 0;
}


