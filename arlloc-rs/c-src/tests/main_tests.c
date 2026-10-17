#include <check.h>
#include <stdio.h>
#include "main_tests.h"

int main(void)
{
    SRunner* sr = srunner_create(suite_lifecycle());
    srunner_add_suite(sr, suite_alloc());
    srunner_add_suite(sr, suite_reset());
    srunner_add_suite(sr, suite_stats());

    srunner_run_all(sr, CK_NORMAL);

    int number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    if (number_failed != 0) {
        printf("\n[FAIL] %d test(s) failed.\n", number_failed);
        return 1;
    }

    printf("\n[PASS] All tests passed.\n");
    return 0;
}
