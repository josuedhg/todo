#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "task.h"
#include "todotxt.h"

void test_negative_todotxt_get_time_from_string_null(void **status)
{
	time_t time = 0;
	expect_assert_failure(todotxt_get_time_from_string(NULL, &time));
	expect_assert_failure(todotxt_get_time_from_string("2001-02-12", NULL));
	expect_assert_failure(todotxt_get_time_from_string(NULL, NULL));
}

void test_negative_todotxt_get_time_from_string_bad_format(void **state)
{
	time_t time = 0;
	assert_false(todotxt_get_time_from_string("bad format", &time));
	assert_false(todotxt_get_time_from_string("2001/02/12", &time));
	assert_false(todotxt_get_time_from_string("2001-14-01", &time));
	assert_false(todotxt_get_time_from_string("2001-11-34", &time));
}

void test_todotxt_get_time_from_string(void **state)
{
	time_t time = 0;
	assert_true(todotxt_get_time_from_string("2001-02-22", &time));
	assert_true(time > 0);
}

void test_negative_todotxt_get_status_null(void **state)
{
	time_t time = 0;
	expect_assert_failure(todotxt_get_status(NULL, &time));
	expect_assert_failure(todotxt_get_status("x 2001-12-02", NULL));
	expect_assert_failure(todotxt_get_status(NULL, NULL));
}

void test_negative_todotxt_get_status_short(void **status)
{
	time_t time = 0;
	assert_false(todotxt_get_status("x 2001", &time));
}

void test_negative_todotxt_get_status_bad_format(void **status)
{
	time_t time = 0;
	assert_false(todotxt_get_status("bad format status", &time));
	assert_false(todotxt_get_status("xbad format status", &time));
	assert_false(todotxt_get_status("x bad format status", &time));
}

void test_todotxt_get_status(void **status)
{
	time_t time = 0;
	assert_true(todotxt_get_status("x 2002-05-12", &time));
	assert_true(time > 0);
}

void test_todotxt_get_status_with_priority(void **state)
{
	time_t time = 0;
	assert_true(todotxt_get_status("x (A) 2002-05-12", &time));
	assert_true(time > 0);
}

void test_negative_todotxt_get_project_name_from_desc(void **state)
{
	expect_assert_failure(todotxt_get_project_name_from_desc(NULL));
}

void test_negative_todotxt_get_project_name_from_desc_bad_format(void **state)
{
	char *project_name = todotxt_get_project_name_from_desc("something+myproject something");
	assert_null(project_name);

	project_name = todotxt_get_project_name_from_desc("something + myproject something");
	assert_null(project_name);

	project_name = todotxt_get_project_name_from_desc("something+ something");
	assert_null(project_name);

	project_name = todotxt_get_project_name_from_desc("something something +");
	assert_null(project_name);

	project_name = todotxt_get_project_name_from_desc("something something");
	assert_null(project_name);
}

void test_todotxt_get_project_name_from_desc(void **state)
{
	char *project_name = todotxt_get_project_name_from_desc("something +myproject something");
	assert_string_equal(project_name, "myproject");

	char *project_name2 = todotxt_get_project_name_from_desc("+myproject something something");
	assert_string_equal(project_name2, "myproject");

	char *project_name3 = todotxt_get_project_name_from_desc("something something +myproject");
	assert_string_equal(project_name3, "myproject");

	char *project_name4 = todotxt_get_project_name_from_desc("+myproject something something +myproject2");
	assert_string_equal(project_name4, "myproject");
}

int main(int argc, char *argv[])
{
	struct CMUnitTest tests[] = {
		cmocka_unit_test(test_negative_todotxt_get_time_from_string_null),
		cmocka_unit_test(test_negative_todotxt_get_time_from_string_bad_format),
		cmocka_unit_test(test_todotxt_get_time_from_string),
		cmocka_unit_test(test_negative_todotxt_get_status_null),
		cmocka_unit_test(test_negative_todotxt_get_status_short),
		cmocka_unit_test(test_negative_todotxt_get_status_bad_format),
		cmocka_unit_test(test_todotxt_get_status),
		cmocka_unit_test(test_todotxt_get_status_with_priority),
		cmocka_unit_test(test_negative_todotxt_get_project_name_from_desc),
		cmocka_unit_test(test_negative_todotxt_get_project_name_from_desc_bad_format),
		cmocka_unit_test(test_todotxt_get_project_name_from_desc),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
