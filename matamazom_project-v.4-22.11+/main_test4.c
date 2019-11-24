#include <stdio.h>
#include <stdlib.h>
#include "libmtm/set.h"
#include "amount_set.h"
#include <assert.h>

static ASElement copyInt(ASElement number) {
    int *copy = malloc(sizeof(*copy));
    if (copy != NULL) {
        *copy = *(int *)number;
    }
    return copy;
}

static void freeInt(ASElement number) { free(number); }

static int compareInts(ASElement lhs, ASElement rhs) {
    return (*(int *)lhs) - (*(int *)rhs);
}


void testPrintSetInt (Set set) {
    printf("<----Set check: {");
    SET_FOREACH(int*, current, set) {
        printf("%d, ", *current);
    }

    printf("}----->\n");





}

int main() {


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
