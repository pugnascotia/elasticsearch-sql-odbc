/*
 * Copyright Elasticsearch B.V. and/or licensed to Elasticsearch B.V. under one
 * or more contributor license agreements. Licensed under the Elastic License;
 * you may not use this file except in compliance with the Elastic License.
 */

#include <gtest/gtest.h>
#include "connected_dbc.h"

#include <string.h>

namespace test {

class ConvertC2SQL_Interval : public ::testing::Test, public ConnectedDBC {
};

TEST_F(ConvertC2SQL_Interval, Bit2Interval_year)
{
	prepareStatement();

	SQLCHAR val = 1;
	ret = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_BIT,
			SQL_INTERVAL_YEAR, /*size*/0, /*decdigits*/0, &val, sizeof(val),
			/*IndLen*/NULL);
	ASSERT_TRUE(SQL_SUCCEEDED(ret));

	assertRequest("[{\"type\": \"INTERVAL_YEAR\", "
		"\"value\": \"P1Y\"}]");
}

TEST_F(ConvertC2SQL_Interval, Short2Interval_month)
{
	prepareStatement();

	SQLSMALLINT val = -2;
	ret = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_SHORT,
			SQL_INTERVAL_MONTH, /*size*/0, /*decdigits*/0, &val, sizeof(val),
			/*IndLen*/NULL);
	ASSERT_TRUE(SQL_SUCCEEDED(ret));

	assertRequest("[{\"type\": \"INTERVAL_MONTH\", "
		"\"value\": \"P-2M\"}]");
}

TEST_F(ConvertC2SQL_Interval, Short2Interval_month_all_0)
{
	prepareStatement();

	SQLSMALLINT val = 0;
	ret = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_SHORT,
			SQL_INTERVAL_MONTH, /*size*/0, /*decdigits*/0, &val, sizeof(val),
			/*IndLen*/NULL);
	ASSERT_TRUE(SQL_SUCCEEDED(ret));

	assertRequest("[{\"type\": \"INTERVAL_MONTH\", "
		"\"value\": \"P0M\"}]");
}

TEST_F(ConvertC2SQL_Interval, Integer2Interval_day)
{
	prepareStatement();

	SQLINTEGER val = -3;
	ret = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_LONG,
			SQL_INTERVAL_DAY, /*size*/0, /*decdigits*/0, &val, sizeof(val),
			/*IndLen*/NULL);
	ASSERT_TRUE(SQL_SUCCEEDED(ret));

	assertRequest("[{\"type\": \"INTERVAL_DAY\", "
		"\"value\": \"P-3D\"}]");
}

TEST_F(ConvertC2SQL_Interval, UBigInt2Interval_minute)
{
	prepareStatement();

	SQLUBIGINT val = 12345678;
	ret = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_UBIGINT,
			SQL_INTERVAL_MINUTE, /*size*/0, /*decdigits*/0, &val, sizeof(val),
			/*IndLen*/NULL);
	ASSERT_TRUE(SQL_SUCCEEDED(ret));

	assertRequest("[{\"type\": \"INTERVAL_MINUTE\", "
		"\"value\": \"PT12345678M\"}]");
}

TEST_F(ConvertC2SQL_Interval, SBigInt2Interval_second)
{
	prepareStatement();

	SQLUBIGINT val = -123456789;
	ret = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_SBIGINT,
			SQL_INTERVAL_SECOND, /*size*/0, /*decdigits*/0, &val, sizeof(val),
			/*IndLen*/NULL);
	ASSERT_TRUE(SQL_SUCCEEDED(ret));

	assertRequest("[{\"type\": \"INTERVAL_SECOND\", "
		"\"value\": \"PT-123456789S\"}]");
}

TEST_F(ConvertC2SQL_Interval, SBigInt2Interval_second_all_0)
{
	prepareStatement();

	SQLUBIGINT val = 0;
	ret = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_SBIGINT,
			SQL_INTERVAL_SECOND, /*size*/0, /*decdigits*/0, &val, sizeof(val),
			/*IndLen*/NULL);
	ASSERT_TRUE(SQL_SUCCEEDED(ret));

	assertRequest("[{\"type\": \"INTERVAL_SECOND\", "
		"\"value\": \"PT0S\"}]");
}

TEST_F(ConvertC2SQL_Interval, WStr2Interval_day_to_second)
{
	prepareStatement();

	SQLWCHAR val[] = L"INTERVAL -'2 03:04:05.678' DAY TO SECOND";
	ret = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR,
			SQL_INTERVAL_DAY_TO_SECOND, /*size*/2, /*decdigits*/3, val,
			SQL_NTSL, /*IndLen*/NULL);
	ASSERT_TRUE(SQL_SUCCEEDED(ret));

	assertRequest("[{\"type\": \"INTERVAL_DAY_TO_SECOND\", "
		"\"value\": \"P-2DT-3H-4M-5.678S\"}]");
}

TEST_F(ConvertC2SQL_Interval, CStr2Interval_hour_to_second)
{
	prepareStatement();

	SQLCHAR val[] = "INTERVAL '03:04:05.678' HOUR TO SECOND";
	ret = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR,
			SQL_INTERVAL_HOUR_TO_SECOND, /*size*/2, /*decdigits*/3, val,
			sizeof(val) - 1, /*IndLen*/NULL);
	ASSERT_TRUE(SQL_SUCCEEDED(ret));

	assertRequest("[{\"type\": \"INTERVAL_HOUR_TO_SECOND\", "
		"\"value\": \"PT3H4M5.678S\"}]");
}

TEST_F(ConvertC2SQL_Interval, CStr2Interval_hour_to_second_force_alloc)
{
	prepareStatement();

	SQLCHAR val[] = "INTERVAL '03:04:05.678' HOUR TO "
		"                                                           "
		"                                                           "
		"                                                           "
		"SECOND";
	SQLLEN osize = sizeof(val) - 1;
	ret = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR,
			SQL_INTERVAL_HOUR_TO_SECOND, /*size*/2, /*decdigits*/3, val,
			0, &osize);
	ASSERT_TRUE(SQL_SUCCEEDED(ret));

	assertRequest("[{\"type\": \"INTERVAL_HOUR_TO_SECOND\", "
		"\"value\": \"PT3H4M5.678S\"}]");
}

TEST_F(ConvertC2SQL_Interval, Interval2Interval_year_to_month)
{
	prepareStatement();

	SQL_INTERVAL_STRUCT val;
	val.interval_type = SQL_IS_YEAR_TO_MONTH;
	val.interval_sign = SQL_TRUE;
	val.intval.year_month.year = 12;
	val.intval.year_month.month = 11;

	ret = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT,
			SQL_C_INTERVAL_YEAR_TO_MONTH, SQL_INTERVAL_YEAR_TO_MONTH,
			/*size*/2, /*decdigits*/3, &val, sizeof(val), /*IndLen*/NULL);
	ASSERT_TRUE(SQL_SUCCEEDED(ret));

	assertRequest("[{\"type\": \"INTERVAL_YEAR_TO_MONTH\", "
		"\"value\": \"P-12Y-11M\"}]");
}

TEST_F(ConvertC2SQL_Interval, Interval_binary2Interval_year_to_month)
{
	prepareStatement();

	SQL_INTERVAL_STRUCT val;
	val.interval_type = SQL_IS_YEAR_TO_MONTH;
	val.interval_sign = SQL_TRUE;
	val.intval.year_month.year = 12;
	val.intval.year_month.month = 11;

	ret = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT,
			SQL_C_BINARY, SQL_INTERVAL_YEAR_TO_MONTH,
			/*size*/2, /*decdigits*/3, &val, sizeof(val), /*IndLen*/NULL);
	ASSERT_TRUE(SQL_SUCCEEDED(ret));

	assertRequest("[{\"type\": \"INTERVAL_YEAR_TO_MONTH\", "
		"\"value\": \"P-12Y-11M\"}]");
}



} // test namespace

/* vim: set noet fenc=utf-8 ff=dos sts=0 sw=4 ts=4 : */
