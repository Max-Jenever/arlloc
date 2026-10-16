#ifndef MAIN_TEST_H
#define MAIN_TEST_H

#include <check.h>

Suite* suite_lifecycle(void);  // create, destroy
Suite* suite_alloc(void);
Suite* suite_reset(void);
Suite* suite_stats(void);

#endif // MAIN_TEST_H
