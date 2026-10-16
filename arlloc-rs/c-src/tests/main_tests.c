#include <check.h>
#include <stdio.h>
#include "main_tests.h"

int main(void)
{
    // Создаём единый раннер, в который добавляются все сьюты
    SRunner* sr = srunner_create(suite_lifecycle());
    srunner_add_suite(sr, suite_alloc());
    srunner_add_suite(sr, suite_reset());
    srunner_add_suite(sr, suite_stats());

    // Запуск с выводом в стандартный поток (NORMAL — баланс подробности и читаемости)
    // Альтернативы: CK_VERBOSE (подробно), CK_SILENT (только итог), CK_SUBUNIT (для CI)
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
