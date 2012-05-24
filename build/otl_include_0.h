#ifndef __OTL_INCLUDE_ZERO_H__
#define __OTL_INCLUDE_ZERO_H__

#include <ostream>

// OTLv4 config:


#undef OTL_INCLUDE_0
#define OTL_INCLUDE_0 1


#define OTL_DESTRUCTORS_DO_NOT_THROW  1
#define OTL_EXCEPTION_ENABLE_ERROR_OFFSET 1
#define OTL_EXTENDED_EXCEPTION 1



#define OTL_ODBC_MULTI_MODE 1
// #define OTL_ODBC_MSSQL_2005 1

#define OTL_DEFAULT_STRING_NULL_TO_VAL ""
// #define OTL_ODBC_TIME_ZONE


#define OTL_ODBC_POSTGRESQL 1

#define OTL_ODBC_MYSQL 1

// #define OTL_ODBC_UNIX 1


#define OTL_STL                         1


#define OTL_TRACE_ENABLE_STREAM_LABELS                  1

#define OTL_FUNC_THROW_SPEC_ON                                  1

#define OTL_UNCAUGHT_EXCEPTION_ON                               1

#define OTL_VALUE_TEMPLATE_ON                                   1
#define OTL_VALUE_TEMPLATE

#define OTL_BIND_VAR_STRICT_TYPE_CHECKING_ON    1

#define OTL_DEFAULT_DATETIME_NULL_TO_VAL                otl_datetime(0, 0, 0, 0, 0, 0, 0, 0)

#define OTL_ODBC_STRING_TO_TIMESTAMP(str, tm)    otl_str_to_tm(str, tm)
#define OTL_ODBC_TIMESTAMP_TO_STRING(tm, str)    otl_ts_to_str(tm, str)

#define OTL_THROWS_ON_SQL_SUCCESS_WITH_INFO                     1



#define OTL_TRACE_LEVEL                         otlv4_trace_global
#define OTL_TRACE_STREAM                        get_otlv4_trace_stream()


class otl_datetime;


extern int otlv4_trace_global;

ostream &get_otlv4_trace_stream(void);
void otl_str_to_tm(const char *str, otl_datetime &tm);
void otl_ts_to_str(const otl_datetime &tm, char str[]);



#define OTL_BIGINT int64_t



#endif // ifndef __OTL_INCLUDE_ZERO_H__

