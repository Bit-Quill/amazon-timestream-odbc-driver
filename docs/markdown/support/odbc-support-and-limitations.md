# ODBC Support and Limitations

## Topics
- [ODBC API Support](#odbc-api-support)
- [Supported Connection Information Types](#supported-connection-information-types)
- [Supported Connection Attributes](#supported-connection-attributes)
- [Supported Statements Attributes](#supported-statements-attributes)
- [SQLPrepare, SQLExecute and SQLExecDirect](#sqlprepare-sqlexecute-and-sqlexecdirect)
- [SQLTables](#sqltables)
- [Database Reporting Differences Between Timestream JDBC Driver and ODBC Driver](#database-reporting-differences-between-timestream-jdbc-driver-and-odbc-driver)

## ODBC API Support
Note that the following table describes the planned functionality for the GA release. As the driver is still in development, not all functions marked as "yes" under the "Support" column below are supported at this time.

| Function | Support? <br /> yes/error/empty | Notes |
|----------|---------------------------------|-------|
| SQLAllocHandle | yes |
| SQLAllocConnect | yes |
| SQLAllocEnv | yes |
| SQLAllocStmt | yes |
| SQLBrowseConnect | no |
| SQLConnect | yes |
| SQLDriverConnect | yes |
| SQLColumnPrivileges | no (empty results) |
| SQLColumns | yes |
| SQLForeignKeys | no (empty results) |
| SQLPrimaryKeys | no (empty results) |
| SQLProcedureColumns | no (empty results) |
| SQLProcedures | no (empty results) |
| SQLSpecialColumns | no (empty results) |
| SQLStatistics | no (empty results) |
| SQLTablePrivileges | no (empty results) |
| SQLTables | yes |
| SQLCopyDesc | yes |
| SQLGetDescField | yes |
| SQLGetDescRec | yes |
| SQLSetDescField | yes |
| SQLSetDescRec | yes |
| SQLGetConnectAttr | yes |
| SQLGetEnvAttr | yes |
| SQLGetStmtAttr | yes |
| SQLSetConnectAttr | yes |
| SQLSetStmtAttr | yes |
| SQLGetFunctions | yes |
| SQLGetInfo | yes |
| SQLGetTypeInfo | yes |
| SQLDisconnect | yes |
| SQLCancel | yes |
| SQLCancelHandle | no | ODBC 3.8+ only
| SQLCloseCursor | yes |
| SQLEndTrans | no (error) | Driver will not support transactions
| SQLFreeHandle | yes |
| SQLDescribeParam | no (empty results) | No parameter support
| SQLExecDirect | yes |
| SQLExecute | yes |
| SQLNativeSql | yes | Will return same SQL
| SQLNumParams | no (empty results) | No parameter support
| SQLParamData | no (error) | No parameter support
| SQLPutData | no (error) | No parameter support
| SQLBindParameter | no (error) | No parameter support
| SQLGetCursorName | yes |
| SQLPrepare | yes | No parameter support, so will just ready query for execution
| SQLSetCursorName | yes |
| SQLBindCol | yes |
| SQLBulkOperations | no |
| SQLColAttribute | yes |
| SQLDescribeCol | yes |
| SQLFetch | yes |
| SQLFetchScroll | yes | Only forward-only cursors supported, so this will match the behavior of SQLFetch
| SQLGetData | yes |
| SQLGetDiagField | yes |
| SQLGetDiagRec | yes |
| SQLMoreResults | yes |
| SQLNumResultCols | yes |
| SQLSetPos | no (error) |
| SQLColAttributes | yes |
| SQLError | yes |
| SQLExtendedFetch | yes | Only forward-only cursors supported, so this will match the behavior of SQLFetch
| SQLFreeConnect | yes |
| SQLFreeEnv | yes |
| SQLFreeStmt | yes |
| SQLGetConnectOption | yes |
| SQLGetStmtOption | yes |
| SQLParamOptions | yes |
| SQLRowCount | yes | Defined as SQL_ROWCOUNT_UNKNOWN
| SQLSetConnectOption | yes |
| SQLSetParam | no (error) | No parameter support
| SQLSetScrollOptions | no | Deprecated
| SQLSetStmtOption | yes |
| SQLTransact | no (error) | Driver does not support transactions

## Supported Connection Information Types
| Connection Information Types | Default | Support Value Change|
|--------|------|-------|
| SQL_DRIVER_NAME | 'Amazon Timestream ODBC Driver' | no |
| SQL_DBMS_NAME | 'Amazon Timestream' | no |
| SQL_DRIVER_ODBC_VER | '03.00' | no |
| SQL_DRIVER_VER | MM.mm.pppp (MM = major, mm = minor, pppp = patch) | no |
| SQL_DBMS_VER | MM.mm.pppp (MM = major, mm = minor, pppp = patch) | no |
| SQL_COLUMN_ALIAS | 'Y' | no |
| SQL_IDENTIFIER_QUOTE_CHAR | '\"' | no |
| SQL_CATALOG_NAME_SEPARATOR | '.' | no |
| SQL_SPECIAL_CHARACTERS | '' | no |
| SQL_CATALOG_TERM | 'database' (default case). If [`DATABASE_AS_SCHEMA`](../setup/developer-guide.md/#database-reporting) set to `TRUE`, it's '' | no |
| SQL_TABLE_TERM | 'table' | no |
| SQL_SCHEMA_TERM | 'schema' | no |
| SQL_NEED_LONG_DATA_LEN | 'Y' | no |
| SQL_ACCESSIBLE_PROCEDURES | 'Y' | no |
| SQL_ACCESSIBLE_TABLES | 'Y' | no |
| SQL_CATALOG_NAME | 'Y' (default case). If [`DATABASE_AS_SCHEMA`](../setup/developer-guide.md/#database-reporting) set to `TRUE`, it's 'N' | no |
| SQL_COLLATION_SEQ | 'UTF-8' | no |
| SQL_DATA_SOURCE_NAME | '\<dsn\>" (if available) | no |
| SQL_DATA_SOURCE_READ_ONLY | 'Y' | no |
| SQL_DATABASE_NAME | '\<database\>' | no |
| SQL_DESCRIBE_PARAMETER | 'N' | no |
| SQL_EXPRESSIONS_IN_ORDERBY | 'Y' | no |
| SQL_INTEGRITY | 'N' | no |
| SQL_KEYWORDS | 'ABS,ALLOW,ARRAY,ARRAY_MAX_CARDINALITY,ASYMMETRIC,ATOMIC,BEGIN_FRAME,BEGIN_PARTITION,BIGINT,BINARY,BLOB,BOOLEAN,CALL,CALLED,CARDINALITY,CEIL,CEILING,CHAR_LENGTH,CLASSIFIER,CLOB,COLLECT,CONDITION,CORR,COVAR_POP,COVAR_SAMP,CUBE,CUME_DIST,CURRENT_CATALOG,CURRENT_DEFAULT_TRANSFORM_GROUP,CURRENT_PATH,CURRENT_ROLE,CURRENT_ROW,CURRENT_SCHEMA,CURRENT_TRANSFORM_GROUP_FOR_TYPE,CYCLE,DENSE_RANK,DEREF,DETERMINISTIC,DISALLOW,DYNAMIC,EACH,ELEMENT,EMPTY,END_FRAME,END_PARTITION,EQUALS,EVERY,EXP,EXPLAIN,EXTEND,FILTER,FIRST_VALUE,FLOOR,FRAME_ROW,FREE,FUNCTION,FUSION,GROUPING,GROUPS,HOLD,IMPORT,INITIAL,INOUT,INTERSECTION,JSON_ARRAY,JSON_ARRAYAGG,JSON_EXISTS,JSON_OBJECT,JSON_OBJECTAGG,JSON_QUERY,JSON_VALUE,LAG,LARGE,LAST_VALUE,LATERAL,LEAD,LIKE_REGEX,LIMIT,LN,LOCALTIME,LOCALTIMESTAMP,MATCHES,MATCH_NUMBER,MATCH_RECOGNIZE,MEASURES,MEMBER,MERGE,METHOD,MINUS,MOD,MODIFIES,MULTISET,NCLOB,NEW,NORMALIZE,NTH_VALUE,NTILE,OCCURRENCES_REGEX,OFFSET,OLD,OMIT,ONE,OUT,OVER,OVERLAY,PARAMETER,PARTITION,PATTERN,PER,PERCENT,PERCENTILE_CONT,PERCENTILE_DISC,PERCENT_RANK,PERIOD,PERMUTE,PORTION,POSITION_REGEX,POWER,PRECEDES,PREV,RANGE,RANK,READS,RECURSIVE,REF,REFERENCING,REGR_AVGX,REGR_AVGY,REGR_COUNT,REGR_INTERCEPT,REGR_R2,REGR_SLOPE,REGR_SXX,REGR_SXY,REGR_SYY,RELEASE,RESET,RESULT,RETURN,RETURNS,ROLLUP,ROW,ROW_NUMBER,RUNNING,SAVEPOINT,SCOPE,SEARCH,SEEK,SENSITIVE,SHOW,SIMILAR,SKIP,SPECIFIC,SPECIFICTYPE,SQLEXCEPTION,SQRT,START,STATIC,STDDEV_POP,STDDEV_SAMP,STREAM,SUBMULTISET,SUBSET,SUBSTRING_REGEX,SUCCEEDS,SYMMETRIC,SYSTEM,SYSTEM_TIME,TABLESAMPLE,TINYINT,TRANSLATE_REGEX,TREAT,TRIGGER,TRIM_ARRAY,TRUNCATE,UESCAPE,UNNEST,UPSERT,VALUE_OF,VARBINARY,VAR_POP,VAR_SAMP,VERSIONING,WIDTH_BUCKET,WINDOW,WITHIN,WITHOUT' | no |
| SQL_LIKE_ESCAPE_CLAUSE | 'N' | no |
| SQL_MAX_ROW_SIZE_INCLUDES_LONG | 'Y' | no |
| SQL_MULT_RESULT_SETS | 'N' | no |
| SQL_MULTIPLE_ACTIVE_TXN | 'Y' | no |
| SQL_ORDER_BY_COLUMNS_IN_SELECT | 'N' | no |
| SQL_PROCEDURE_TERM | 'stored procedure' | no |
| SQL_PROCEDURES | 'N' | no |
| SQL_ROW_UPDATES | 'N' | no |
| SQL_SEARCH_PATTERN_ESCAPE | '\\\\' | no |
| SQL_SERVER_NAME | 'Amazon Timestream' | no |
| SQL_USER_NAME | '\<user\>' | no |
| SQL_ASYNC_DBC_FUNCTIONS | SQL_ASYNC_DBC_NOT_CAPABLE | no |
| SQL_ASYNC_MODE | SQL_AM_NONE | no |
| SQL_ASYNC_NOTIFICATION | SQL_ASYNC_NOTIFICATION_NOT_CAPABLE | no |
| SQL_BATCH_ROW_COUNT | SQL_BRC_ROLLED_UP, SQL_BRC_EXPLICIT | no |
| SQL_BATCH_SUPPORT | 0 (not supported) | no |
| SQL_BOOKMARK_PERSISTENCE | 0 (not supported) | no |
| SQL_CATALOG_LOCATION | SQL_CL_START (default case). If [`DATABASE_AS_SCHEMA`](../setup/developer-guide.md/#database-reporting) set to `TRUE`, it's 0 (not supported) | no |
| SQL_QUALIFIER_LOCATION | 0 (not supported) | no |
| SQL_GETDATA_EXTENSIONS|SQL_GD_ANY_COLUMN, SQL_GD_ANY_ORDER, SQL_GD_BOUND | no |
| SQL_ODBC_INTERFACE_CONFORMANCE | SQL_OIC_CORE | no |
| SQL_SQL_CONFORMANCE | SQL_SC_SQL92_ENTRY | no |
| SQL_CATALOG_USAGE | SQL_CU_DML_STATEMENTS (default case). If [`DATABASE_AS_SCHEMA`](../setup/developer-guide.md/#database-reporting) set to `TRUE`, it's 0 (not supported) | no |
| SQL_SCHEMA_USAGE | 0 (not supported) (default case). If [`DATABASE_AS_SCHEMA`](../setup/developer-guide.md/#database-reporting) set to `TRUE`, it's SQL_SU_DML_STATEMENTS, SQL_SU_TABLE_DEFINITION, SQL_SU_PRIVILEGE_DEFINITION, SQL_SU_INDEX_DEFINITION | no |
| SQL_AGGREGATE_FUNCTIONS | SQL_AF_AVG, SQL_AF_COUNT, SQL_AF_MAX, SQL_AF_MIN, SQL_AF_SUM, SQL_AF_DISTINCT, SQL_AF_ALL | no |
| SQL_NUMERIC_FUNCTIONS | SQL_FN_NUM_MOD | no |
| SQL_STRING_FUNCTIONS|SQL_FN_STR_CHAR_LENGTH, SQL_FN_STR_CONCAT, SQL_FN_STR_LEFT, SQL_FN_STR_LOCATE, SQL_FN_STR_CHARACTER_LENGTH, SQL_FN_STR_POSITION, SQL_FN_STR_RIGHT, SQL_FN_STR_SUBSTRING, SQL_FN_STR_LCASE, SQL_FN_STR_UCASE | no |
| SQL_TIMEDATE_FUNCTIONS|SQL_FN_TD_CURRENT_TIME, SQL_FN_TD_CURRENT_DATE, SQL_FN_TD_CURRENT_TIMESTAMP, SQL_FN_TD_EXTRACT, SQL_FN_TD_YEAR, SQL_FN_TD_QUARTER, SQL_FN_TD_MONTH, SQL_FN_TD_WEEK, SQL_FN_TD_DAYOFYEAR, SQL_FN_TD_DAYOFMONTH, SQL_FN_TD_DAYOFWEEK, SQL_FN_TD_HOUR, SQL_FN_TD_MINUTE, SQL_FN_TD_SECOND, SQL_FN_TD_DAYNAME, SQL_FN_TD_MONTHNAME | no |
| SQL_TIMEDATE_ADD_INTERVALS|SQL_FN_TSI_FRAC_SECOND, SQL_FN_TSI_SECOND, SQL_FN_TSI_MINUTE, SQL_FN_TSI_HOUR, SQL_FN_TSI_DAY, SQL_FN_TSI_WEEK, SQL_FN_TSI_MONTH, SQL_FN_TSI_QUARTER, SQL_FN_TSI_YEAR | no |
| SQL_TIMEDATE_DIFF_INTERVALS|SQL_FN_TSI_FRAC_SECOND, SQL_FN_TSI_SECOND, SQL_FN_TSI_MINUTE, SQL_FN_TSI_HOUR, SQL_FN_TSI_DAY, SQL_FN_TSI_WEEK, SQL_FN_TSI_MONTH, SQL_FN_TSI_QUARTER, SQL_FN_TSI_YEAR | no |
| SQL_DATETIME_LITERALS|SQL_DL_SQL92_DATE, SQL_DL_SQL92_TIME, SQL_DL_SQL92_TIMESTAMP | no |
| SQL_SYSTEM_FUNCTIONS|SQL_FN_SYS_USERNAME, SQL_FN_SYS_DBNAME, SQL_FN_SYS_IFNULL | no |
| SQL_CONVERT_FUNCTIONS | 0 (not supported) | no |
| SQL_OJ_CAPABILITIES|SQL_OJ_LEFT, SQL_OJ_NOT_ORDERED, SQL_OJ_ALL_COMPARISON_OPS | no |
| SQL_POS_OPERATIONS | 0 (not supported) | no |
| SQL_SQL92_NUMERIC_VALUE_FUNCTIONS|SQL_SNVF_CHARACTER_LENGTH, SQL_SNVF_EXTRACT, SQL_SNVF_POSITION | no |
| SQL_SQL92_STRING_FUNCTIONS|SQL_SSF_LOWER, SQL_SSF_UPPER, SQL_SSF_SUBSTRING | no |
| SQL_SQL92_DATETIME_FUNCTIONS|SQL_SDF_CURRENT_DATE, SQL_SDF_CURRENT_TIMESTAMP | no |
| SQL_SQL92_VALUE_EXPRESSIONS|SQL_SVE_CASE, SQL_SVE_CAST, SQL_SVE_COALESCE, SQL_SVE_NULLIF | no |
| SQL_SQL92_PREDICATES|SQL_SP_BETWEEN, SQL_SP_COMPARISON, SQL_SP_EXISTS, SQL_SP_IN, SQL_SP_ISNOTNULL, SQL_SP_ISNULL, SQL_SP_LIKE, SQL_SP_MATCH_FULL, SQL_SP_MATCH_PARTIAL, SQL_SP_MATCH_UNIQUE_FULL, SQL_SP_MATCH_UNIQUE_PARTIAL, SQL_SP_OVERLAPS, SQL_SP_UNIQUE, SQL_SP_QUANTIFIED_COMPARISON | no |
| SQL_SQL92_RELATIONAL_JOIN_OPERATORS|SQL_SRJO_CORRESPONDING_CLAUSE, SQL_SRJO_CROSS_JOIN, SQL_SRJO_EXCEPT_JOIN, SQL_SRJO_INNER_JOIN, SQL_SRJO_LEFT_OUTER_JOIN, SQL_SRJO_RIGHT_OUTER_JOIN, SQL_SRJO_NATURAL_JOIN, SQL_SRJO_INTERSECT_JOIN, SQL_SRJO_UNION_JOIN | no |
| SQL_STATIC_CURSOR_ATTRIBUTES1 | SQL_CA1_NEXT | no |
| SQL_STATIC_CURSOR_ATTRIBUTES2 | 0 (not supported) | no |
| SQL_CONVERT_BIGINT|SQL_CVT_CHAR, SQL_CVT_VARCHAR, SQL_CVT_LONGVARCHAR, SQL_CVT_WCHAR, SQL_CVT_WLONGVARCHAR, SQL_CVT_WVARCHAR, SQL_CVT_NUMERIC, SQL_CVT_TIMESTAMP, SQL_CVT_TINYINT, SQL_CVT_SMALLINT, SQL_CVT_INTEGER, SQL_CVT_BIGINT, SQL_CVT_BIT | no |
| SQL_CONVERT_BINARY|SQL_CVT_CHAR, SQL_CVT_VARCHAR, SQL_CVT_LONGVARCHAR, SQL_CVT_BIT, SQL_CVT_WCHAR, SQL_CVT_WLONGVARCHAR, SQL_CVT_WVARCHAR, SQL_CVT_NUMERIC, SQL_CVT_LONGVARBINARY, SQL_CVT_FLOAT, SQL_CVT_SMALLINT, SQL_CVT_INTEGER, SQL_CVT_BIGINT, SQL_CVT_REAL, SQL_CVT_DATE, SQL_CVT_TINYINT, SQL_CVT_DOUBLE, SQL_CVT_BINARY, SQL_CVT_DECIMAL, SQL_CVT_TIME, SQL_CVT_GUID, SQL_CVT_TIMESTAMP, SQL_CVT_VARBINARY | no |
| SQL_CONVERT_BIT|SQL_CVT_BIGINT, SQL_CVT_VARCHAR, SQL_CVT_LONGVARCHAR, SQL_CVT_WVARCHAR, SQL_CVT_NUMERIC, SQL_CVT_BIT, SQL_CVT_CHAR, SQL_CVT_SMALLINT, SQL_CVT_INTEGER, SQL_CVT_TINYINT | no |
| SQL_CONVERT_CHAR|SQL_CVT_CHAR, SQL_CVT_VARCHAR, SQL_CVT_LONGVARCHAR, SQL_CVT_VARBINARY, SQL_CVT_WCHAR, SQL_CVT_WLONGVARCHAR, SQL_CVT_WVARCHAR, SQL_CVT_NUMERIC, SQL_CVT_DECIMAL, SQL_CVT_BIT, SQL_CVT_TINYINT, SQL_CVT_SMALLINT, SQL_CVT_INTEGER, SQL_CVT_BIGINT, SQL_CVT_REAL, SQL_CVT_FLOAT, SQL_CVT_DOUBLE, SQL_CVT_BINARY, SQL_CVT_TIMESTAMP, SQL_CVT_DATE, SQL_CVT_TIME, SQL_CVT_LONGVARBINARY, SQL_CVT_GUID | no |
| SQL_CONVERT_VARCHAR|SQL_CVT_CHAR, SQL_CVT_VARCHAR, SQL_CVT_LONGVARCHAR, SQL_CVT_WCHAR, SQL_CVT_WLONGVARCHAR, SQL_CVT_WVARCHAR, SQL_CVT_NUMERIC, SQL_CVT_DECIMAL, SQL_CVT_TINYINT, SQL_CVT_SMALLINT, SQL_CVT_INTEGER, SQL_CVT_BIGINT, SQL_CVT_REAL, SQL_CVT_FLOAT, SQL_CVT_BIT, SQL_CVT_DOUBLE, SQL_CVT_BINARY, SQL_CVT_VARBINARY, SQL_CVT_GUID, SQL_CVT_DATE, SQL_CVT_TIME, SQL_CVT_LONGVARBINARY, SQL_CVT_TIMESTAMP | no |
| SQL_CONVERT_LONGVARCHAR|SQL_CVT_CHAR, SQL_CVT_VARCHAR, SQL_CVT_DATE, SQL_CVT_TIME, SQL_CVT_LONGVARCHAR, SQL_CVT_WCHAR, SQL_CVT_WLONGVARCHAR, SQL_CVT_WVARCHAR, SQL_CVT_NUMERIC, SQL_CVT_DECIMAL, SQL_CVT_BIT, SQL_CVT_REAL, SQL_CVT_SMALLINT, SQL_CVT_INTEGER, SQL_CVT_GUID, SQL_CVT_BIGINT, SQL_CVT_LONGVARBINARY, SQL_CVT_DOUBLE, SQL_CVT_BINARY, SQL_CVT_VARBINARY, SQL_CVT_TINYINT, SQL_CVT_FLOAT, SQL_CVT_TIMESTAMP | no |
| SQL_CONVERT_WCHAR|SQL_CVT_CHAR, SQL_CVT_VARCHAR, SQL_CVT_LONGVARCHAR, SQL_CVT_WCHAR, SQL_CVT_WLONGVARCHAR, SQL_CVT_WVARCHAR, SQL_CVT_NUMERIC, SQL_CVT_DECIMAL, SQL_CVT_VARBINARY, SQL_CVT_BIT, SQL_CVT_TINYINT, SQL_CVT_SMALLINT, SQL_CVT_INTEGER, SQL_CVT_BIGINT, SQL_CVT_REAL, SQL_CVT_FLOAT, SQL_CVT_DOUBLE, SQL_CVT_BINARY, SQL_CVT_LONGVARBINARY, SQL_CVT_TIMESTAMP, SQL_CVT_DATE, SQL_CVT_TIME, SQL_CVT_GUID | no |
| SQL_CONVERT_WVARCHAR|SQL_CVT_CHAR, SQL_CVT_VARCHAR, SQL_CVT_LONGVARCHAR, SQL_CVT_REAL, SQL_CVT_WCHAR, SQL_CVT_WLONGVARCHAR, SQL_CVT_WVARCHAR, SQL_CVT_NUMERIC, SQL_CVT_LONGVARBINARY, SQL_CVT_DECIMAL, SQL_CVT_BIT, SQL_CVT_TINYINT, SQL_CVT_SMALLINT, SQL_CVT_DATE, SQL_CVT_TIME, SQL_CVT_INTEGER, SQL_CVT_BIGINT, SQL_CVT_FLOAT, SQL_CVT_DOUBLE, SQL_CVT_BINARY, SQL_CVT_GUID, SQL_CVT_VARBINARY, SQL_CVT_TIMESTAMP | no |
| SQL_CONVERT_WLONGVARCHAR|SQL_CVT_CHAR, SQL_CVT_VARCHAR, SQL_CVT_BIGINT, SQL_CVT_REAL, SQL_CVT_LONGVARCHAR, SQL_CVT_WCHAR, SQL_CVT_WLONGVARCHAR, SQL_CVT_WVARCHAR, SQL_CVT_VARBINARY, SQL_CVT_NUMERIC, SQL_CVT_DECIMAL, SQL_CVT_BIT, SQL_CVT_TINYINT, SQL_CVT_DATE, SQL_CVT_FLOAT, SQL_CVT_INTEGER, SQL_CVT_SMALLINT, SQL_CVT_DOUBLE, SQL_CVT_BINARY, SQL_CVT_LONGVARBINARY, SQL_CVT_TIME, SQL_CVT_TIMESTAMP, SQL_CVT_GUID | no |
| SQL_CONVERT_GUID|SQL_CVT_CHAR, SQL_CVT_VARCHAR, SQL_CVT_LONGVARCHAR, SQL_CVT_WVARCHAR, SQL_CVT_WCHAR, SQL_CVT_WLONGVARCHAR, SQL_CVT_BINARY, SQL_CVT_VARBINARY, SQL_CVT_LONGVARBINARY, SQL_CVT_GUID | no |
| SQL_CONVERT_DATE|SQL_CVT_CHAR, SQL_CVT_VARCHAR, SQL_CVT_LONGVARCHAR, SQL_CVT_WCHAR, SQL_CVT_WLONGVARCHAR, SQL_CVT_WVARCHAR, SQL_CVT_NUMERIC, SQL_CVT_INTEGER, SQL_CVT_BIGINT, SQL_CVT_BINARY, SQL_CVT_VARBINARY, SQL_CVT_LONGVARBINARY, SQL_CVT_DATE, SQL_CVT_TIMESTAMP | no |
| SQL_CONVERT_DECIMAL|SQL_CVT_CHAR, SQL_CVT_VARCHAR, SQL_CVT_LONGVARCHAR, SQL_CVT_WCHAR, SQL_CVT_WLONGVARCHAR, SQL_CVT_WVARCHAR, SQL_CVT_NUMERIC, SQL_CVT_DECIMAL, SQL_CVT_TIMESTAMP, SQL_CVT_BIT, SQL_CVT_TINYINT, SQL_CVT_SMALLINT, SQL_CVT_INTEGER, SQL_CVT_BIGINT, SQL_CVT_REAL, SQL_CVT_FLOAT, SQL_CVT_DOUBLE, SQL_CVT_BINARY, SQL_CVT_VARBINARY, SQL_CVT_LONGVARBINARY | no |
| SQL_CONVERT_DOUBLE|SQL_CVT_CHAR, SQL_CVT_VARCHAR, SQL_CVT_LONGVARCHAR, SQL_CVT_WCHAR, SQL_CVT_WLONGVARCHAR, SQL_CVT_WVARCHAR, SQL_CVT_NUMERIC, SQL_CVT_DECIMAL, SQL_CVT_TIMESTAMP, SQL_CVT_TINYINT, SQL_CVT_BIGINT, SQL_CVT_INTEGER, SQL_CVT_FLOAT, SQL_CVT_REAL, SQL_CVT_DOUBLE, SQL_CVT_BINARY, SQL_CVT_VARBINARY, SQL_CVT_SMALLINT, SQL_CVT_LONGVARBINARY | no |
| SQL_CONVERT_FLOAT|SQL_CVT_CHAR, SQL_CVT_VARCHAR, SQL_CVT_LONGVARCHAR, SQL_CVT_WVARCHAR, SQL_CVT_WLONGVARCHAR, SQL_CVT_NUMERIC, SQL_CVT_DECIMAL, SQL_CVT_TINYINT, SQL_CVT_SMALLINT, SQL_CVT_INTEGER, SQL_CVT_BIGINT, SQL_CVT_REAL, SQL_CVT_FLOAT, SQL_CVT_DOUBLE, SQL_CVT_BINARY, SQL_CVT_VARBINARY, SQL_CVT_WCHAR, SQL_CVT_LONGVARBINARY, SQL_CVT_TIMESTAMP, SQL_CVT_BIT | no |
| SQL_CONVERT_REAL|SQL_CVT_CHAR, SQL_CVT_VARCHAR, SQL_CVT_LONGVARCHAR, SQL_CVT_WVARCHAR, SQL_CVT_NUMERIC, SQL_CVT_DECIMAL, SQL_CVT_BIT, SQL_CVT_FLOAT, SQL_CVT_SMALLINT, SQL_CVT_REAL, SQL_CVT_INTEGER, SQL_CVT_BIGINT, SQL_CVT_DOUBLE, SQL_CVT_TINYINT, SQL_CVT_WLONGVARCHAR, SQL_CVT_BINARY, SQL_CVT_VARBINARY, SQL_CVT_LONGVARBINARY, SQL_CVT_TIMESTAMP, SQL_CVT_WCHAR | no |
| SQL_CONVERT_INTEGER|SQL_CVT_CHAR, SQL_CVT_VARCHAR, SQL_CVT_LONGVARCHAR, SQL_CVT_WCHAR, SQL_CVT_WLONGVARCHAR, SQL_CVT_WVARCHAR, SQL_CVT_NUMERIC, SQL_CVT_DECIMAL, SQL_CVT_TINYINT, SQL_CVT_SMALLINT, SQL_CVT_BIT, SQL_CVT_INTEGER, SQL_CVT_BIGINT, SQL_CVT_REAL, SQL_CVT_FLOAT, SQL_CVT_DOUBLE, SQL_CVT_BINARY, SQL_CVT_VARBINARY, SQL_CVT_LONGVARBINARY, SQL_CVT_TIMESTAMP | no |
| SQL_CONVERT_NUMERIC|SQL_CVT_CHAR, SQL_CVT_VARCHAR, SQL_CVT_LONGVARCHAR, SQL_CVT_WCHAR, SQL_CVT_WLONGVARCHAR, SQL_CVT_WVARCHAR, SQL_CVT_NUMERIC, SQL_CVT_DECIMAL, SQL_CVT_SMALLINT, SQL_CVT_INTEGER, SQL_CVT_BIGINT, SQL_CVT_REAL, SQL_CVT_BIT, SQL_CVT_TINYINT, SQL_CVT_FLOAT, SQL_CVT_DOUBLE, SQL_CVT_BINARY, SQL_CVT_VARBINARY, SQL_CVT_LONGVARBINARY, SQL_CVT_TIMESTAMP | no |
| SQL_CONVERT_SMALLINT|SQL_CVT_CHAR, SQL_CVT_VARCHAR, SQL_CVT_LONGVARCHAR, SQL_CVT_WCHAR, SQL_CVT_WLONGVARCHAR, SQL_CVT_WVARCHAR, SQL_CVT_NUMERIC, SQL_CVT_DECIMAL, SQL_CVT_VARBINARY, SQL_CVT_BIT, SQL_CVT_TINYINT, SQL_CVT_SMALLINT, SQL_CVT_INTEGER, SQL_CVT_BIGINT, SQL_CVT_REAL, SQL_CVT_FLOAT, SQL_CVT_DOUBLE, SQL_CVT_BINARY, SQL_CVT_LONGVARBINARY, SQL_CVT_TIMESTAMP | no |
| SQL_CONVERT_TINYINT|SQL_CVT_CHAR, SQL_CVT_VARCHAR, SQL_CVT_LONGVARCHAR, SQL_CVT_WCHAR, SQL_CVT_WLONGVARCHAR, SQL_CVT_WVARCHAR, SQL_CVT_NUMERIC, SQL_CVT_DECIMAL, SQL_CVT_TINYINT, SQL_CVT_SMALLINT, SQL_CVT_BIT, SQL_CVT_INTEGER, SQL_CVT_BIGINT, SQL_CVT_REAL, SQL_CVT_FLOAT, SQL_CVT_DOUBLE, SQL_CVT_BINARY, SQL_CVT_VARBINARY, SQL_CVT_LONGVARBINARY, SQL_CVT_TIMESTAMP | no |
| SQL_CONVERT_TIME|SQL_CVT_CHAR, SQL_CVT_VARCHAR, SQL_CVT_LONGVARCHAR, SQL_CVT_WVARCHAR, SQL_CVT_WCHAR, SQL_CVT_WLONGVARCHAR, SQL_CVT_BINARY, SQL_CVT_VARBINARY, SQL_CVT_LONGVARBINARY, SQL_CVT_TIME, SQL_CVT_TIMESTAMP | no |
| SQL_CONVERT_TIMESTAMP|SQL_CVT_CHAR, SQL_CVT_VARCHAR, SQL_CVT_LONGVARCHAR, SQL_CVT_DATE, SQL_CVT_WCHAR, SQL_CVT_WLONGVARCHAR, SQL_CVT_WVARCHAR, SQL_CVT_NUMERIC, SQL_CVT_LONGVARBINARY, SQL_CVT_DECIMAL, SQL_CVT_INTEGER, SQL_CVT_BINARY, SQL_CVT_VARBINARY, SQL_CVT_TIMESTAMP, SQL_CVT_BIGINT, SQL_CVT_TIME | no |
| SQL_CONVERT_VARBINARY|SQL_CVT_CHAR, SQL_CVT_VARCHAR, SQL_CVT_LONGVARCHAR, SQL_CVT_BIT, SQL_CVT_WCHAR, SQL_CVT_WLONGVARCHAR, SQL_CVT_WVARCHAR, SQL_CVT_NUMERIC, SQL_CVT_DECIMAL, SQL_CVT_TINYINT, SQL_CVT_SMALLINT, SQL_CVT_DATE, SQL_CVT_BIGINT, SQL_CVT_REAL, SQL_CVT_FLOAT, SQL_CVT_DOUBLE, SQL_CVT_INTEGER, SQL_CVT_BINARY, SQL_CVT_VARBINARY, SQL_CVT_LONGVARBINARY, SQL_CVT_TIME, SQL_CVT_TIMESTAMP, SQL_CVT_GUID | no |
| SQL_CONVERT_LONGVARBINARY|SQL_CVT_CHAR, SQL_CVT_VARCHAR, SQL_CVT_BIT, SQL_CVT_TINYINT, SQL_CVT_WCHAR, SQL_CVT_WLONGVARCHAR, SQL_CVT_WVARCHAR, SQL_CVT_LONGVARCHAR, SQL_CVT_NUMERIC, SQL_CVT_DECIMAL, SQL_CVT_FLOAT, SQL_CVT_INTEGER, SQL_CVT_BIGINT, SQL_CVT_REAL, SQL_CVT_DATE, SQL_CVT_DOUBLE, SQL_CVT_BINARY, SQL_CVT_VARBINARY, SQL_CVT_LONGVARBINARY, SQL_CVT_TIMESTAMP, SQL_CVT_SMALLINT, SQL_CVT_TIME, SQL_CVT_GUID | no |
| SQL_PARAM_ARRAY_ROW_COUNTS | SQL_PARC_BATCH | no |
| SQL_PARAM_ARRAY_SELECTS | SQL_PAS_NO_SELECT | no |
| SQL_SCROLL_OPTIONS | SQL_SO_FORWARD_ONLY, SQL_SO_STATIC | no |
| SQL_ALTER_DOMAIN | 0 (not supported) | no |
| SQL_ALTER_TABLE | 0 (not supported) | no |
| SQL_CREATE_ASSERTION | 0 (not supported) | no |
| SQL_CREATE_CHARACTER_SET | 0 (not supported) | no |
| SQL_CREATE_COLLATION | 0 (not supported) | no |
| SQL_CREATE_DOMAIN | 0 (not supported) | no |
| SQL_CREATE_SCHEMA | 0 (not supported) | no |
| SQL_CREATE_TABLE | SQL_CT_CREATE_TABLE, SQL_CT_COLUMN_CONSTRAINT | no |
| SQL_CREATE_TRANSLATION | 0 (not supported) | no |
| SQL_CREATE_VIEW | 0 (not supported) | no |
| SQL_CURSOR_COMMIT_BEHAVIOR | SQL_CB_CLOSE (Set on Linux/Apple only) | no |
| SQL_CURSOR_ROLLBACK_BEHAVIOR | SQL_CB_PRESERVE (Set on Linux/Apple only) | no |
| SQL_CURSOR_SENSITIVITY | SQL_INSENSITIVE | no |
| SQL_DDL_INDEX | 0 (not supported) | no |
| SQL_DEFAULT_TXN_ISOLATION | SQL_TXN_REPEATABLE_READ | no |
| SQL_DROP_ASSERTION | 0 (not supported) | no |
| SQL_DROP_CHARACTER_SET | 0 (not supported) | no |
| SQL_DROP_COLLATION | 0 (not supported) | no |
| SQL_DROP_DOMAIN | 0 (not supported) | no |
| SQL_DROP_SCHEMA | 0 (not supported) | no |
| SQL_DROP_TABLE | 0 (not supported) | no |
| SQL_DROP_TRANSLATION | 0 (not supported) | no |
| SQL_DROP_VIEW | 0 (not supported) | no |
| SQL_DYNAMIC_CURSOR_ATTRIBUTES1 | SQL_CA1_NEXT | no |
| SQL_DYNAMIC_CURSOR_ATTRIBUTES2 | SQL_CA2_READ_ONLY_CONCURRENCY | no |
| SQL_FORWARD_ONLY_CURSOR_ATTRIBUTES1 | SQL_CA1_NEXT | no |
| SQL_FORWARD_ONLY_CURSOR_ATTRIBUTES2|SQL_CA2_READ_ONLY_CONCURRENCY | no |
| SQL_INDEX_KEYWORDS | SQL_IK_NONE | no |
| SQL_INFO_SCHEMA_VIEWS | 0 (not supported) | no |
| SQL_INSERT_STATEMENT | 0 (not supported) | no |
| SQL_KEYSET_CURSOR_ATTRIBUTES1 | SQL_CA1_NEXT | no |
| SQL_KEYSET_CURSOR_ATTRIBUTES2 | 0 (not supported) | no |
| SQL_MAX_ASYNC_CONCURRENT_STATEMENTS | 0 (unknown) | no |
| SQL_MAX_BINARY_LITERAL_LEN | 0 (no maximum) | no |
| SQL_MAX_CATALOG_NAME_LEN | 0 (no maximum) | no |
| SQL_MAX_CHAR_LITERAL_LEN | 0 (no maximum) | no |
| SQL_MAX_INDEX_SIZE | 0 (no maximum) | no |
| SQL_MAX_ROW_SIZE | 0 (no maximum) | no |
| SQL_MAX_STATEMENT_LEN | 0 (no maximum) | no |
| SQL_SQL92_FOREIGN_KEY_DELETE_RULE | 0 (not supported) | no |
| SQL_SQL92_FOREIGN_KEY_UPDATE_RULE | 0 (not supported) | no |
| SQL_SQL92_GRANT | 0 (not supported) | no |
| SQL_SQL92_REVOKE | 0 (not supported) | no |
| SQL_SQL92_ROW_VALUE_CONSTRUCTOR|SQL_SRVC_VALUE_EXPRESSION, SQL_SRVC_DEFAULT, SQL_SRVC_NULL, SQL_SRVC_ROW_SUBQUERY | no |
| SQL_STANDARD_CLI_CONFORMANCE | 0 (not supported) | no |
| SQL_SUBQUERIES | 0 (not supported) | no |
| SQL_TXN_ISOLATION_OPTION | SQL_TXN_REPEATABLE_READ | no |
| SQL_UNION | 0 (not supported) | no |
| SQL_FETCH_DIRECTION | SQL_FD_FETCH_NEXT | no |
| SQL_LOCK_TYPES | SQL_LCK_NO_CHANGE | no |
| SQL_ODBC_API_CONFORMANCE | SQL_OAC_LEVEL1 | no |
| SQL_ODBC_SQL_CONFORMANCE | SQL_OSC_CORE | no |
| SQL_POSITIONED_STATEMENTS | 0 (not supported) | no |
| SQL_SCROLL_CONCURRENCY | SQL_SCCO_READ_ONLY | no |
| SQL_STATIC_SENSITIVITY | 0 (not supported) | no |
| SQL_MAX_CONCURRENT_ACTIVITIES | 0 (no limit) | no |
| SQL_TXN_CAPABLE | SQL_TC_NONE | no |
| SQL_QUOTED_IDENTIFIER_CASE | SQL_IC_SENSITIVE | no |
| SQL_ACTIVE_ENVIRONMENTS | 0 (no limit) | no |
| SQL_CONCAT_NULL_BEHAVIOR | SQL_CB_NULL | no |
| SQL_CORRELATION_NAME | SQL_CN_ANY | no |
| SQL_FILE_USAGE | SQL_FILE_NOT_SUPPORTED | no |
| SQL_GROUP_BY | SQL_GB_GROUP_BY_EQUALS_SELECT | no |
| SQL_IDENTIFIER_CASE | SQL_IC_SENSITIVE | no |
| SQL_MAX_COLUMN_NAME_LEN | 0 (no limit) | no |
| SQL_MAX_COLUMNS_IN_GROUP_BY | 0 (no limit) | no |
| SQL_MAX_COLUMNS_IN_INDEX | 0 (no limit) | no |
| SQL_MAX_COLUMNS_IN_ORDER_BY | 0 (no limit) | no |
| SQL_MAX_COLUMNS_IN_SELECT | 0 (no limit) | no |
| SQL_MAX_COLUMNS_IN_TABLE | 0 (no limit) | no |
| SQL_MAX_CURSOR_NAME_LEN | 0 (no limit) | no |
| SQL_MAX_DRIVER_CONNECTIONS | 0 (no limit) | no |
| SQL_MAX_IDENTIFIER_LEN | 0 (no limit) | no |
| SQL_MAX_PROCEDURE_NAME_LEN | 0 (no limit) | no |
| SQL_MAX_SCHEMA_NAME_LEN | 0 (no limit) | no |
| SQL_MAX_TABLE_NAME_LEN | 0 (no limit) | no |
| SQL_MAX_TABLES_IN_SELECT | 0 (no limit) | no |
| SQL_MAX_USER_NAME_LEN | 0 (no limit) | no |
| SQL_NON_NULLABLE_COLUMNS | SQL_NNC_NON_NULL | no |
| SQL_NULL_COLLATION | SQL_NC_LOW | no |

## Supported Connection Attributes
| Connection Information Types | Default | Support Value Change|
|--------|------|-------|
| SQL_ATTR_ANSI_APP | SQL_ERROR | no |
| SQL_ATTR_ASYNC_ENABLE | SQL_ASYNC_ENABLE_OFF | no |
| SQL_ATTR_AUTO_IPD | false | no |
| SQL_ATTR_AUTOCOMMIT | true | yes |
| SQL_ATTR_CONNECTION_DEAD | - | no |
| SQL_ATTR_CONNECTION_TIMEOUT | 0 | no |
| SQL_ATTR_TSLOG_DEBUG | - | yes |
| SQL_ATTR_METADATA_ID | false | yes |

Note: SQL_ATTR_TSLOG_DEBUG is an internal connection attribute. It can be used to change logging level after a connection is established.

## Supported Statements Attributes
Table of statement attributes supported by the Amazon Timestream ODBC driver.\
Related function: `SQLSetStmtAttr`
| Statement attribute | Default | Support Value Change|
|--------|------|-------|
|SQL_ATTR_PARAM_BIND_OFFSET_PTR| - | yes |
|SQL_ATTR_PARAM_BIND_TYPE| - | no |
|SQL_ATTR_PARAM_OPERATION_PTR| - | no |
|SQL_ATTR_PARAM_STATUS_PTR| - | yes |
|SQL_ATTR_PARAMS_PROCESSED_PTR| | no |
|SQL_ATTR_PARAMSET_SIZE| - | yes | 
|SQL_ATTR_ROW_ARRAY_SIZE| 1 | no | 
|SQL_ATTR_ROW_BIND_OFFSET_PTR| - | yes |
|SQL_ATTR_ROW_BIND_TYPE| - | no |
|SQL_ATTR_ROW_OPERATION_PTR| - | no |
|SQL_ATTR_ROW_STATUS_PTR| - | yes |
|SQL_ATTR_ROWS_FETCHED_PTR| - | yes |

## SQLPrepare, SQLExecute and SQLExecDirect

To support BI tools that may use the SQLPrepare interface in auto-generated queries, the driver
supports the use of SQLPrepare. However, the use of parameters in queries (values left as ?) is not supported in SQLPrepare, SQLExecute and SQLExecDirect. 

Timestream does not support SQL queries with ";", so SQLExecDirect does work with SQL queries with ";" at the end. For the types of SQL queries supported by Timestream, visit the official Timestream query [language support page](https://docs.aws.amazon.com/timestream/latest/developerguide/reference.html).

## SQLTables
The driver supports catalog patterns for SQLTables for both ODBC ver 2.0 and ODBC ver 3.0. 
When `SQL_ATTR_METADATA_ID` is set to `false` (default), it means database name and table name need to be treated as case-sensitive search patterns. Parameters passed as nullptr has same meaning as "%" (search pattern that match everything). Read more about search patterns [here](https://learn.microsoft.com/en-us/sql/odbc/reference/develop-app/pattern-value-arguments?view=sql-server-ver16).

When `SQL_ATTR_METADATA_ID` is set to `true`, it means database name and table name need to be treated as case-insensitive identifiers. In this case, if database name/table name are passed as nullptr to the driver, then the driver would give HY009: invalid use of null pointer error. Read more about identifiers [here](https://learn.microsoft.com/en-us/sql/odbc/reference/develop-app/identifier-arguments?view=sql-server-ver16).

|`SQL_ATTR_METADATA_ID` is set to `true`| CatalogName null value allowed? | SchemaName null value allowed? | TableName null value allowed? 
|---------------------------------------|---------------------------------|--------------------------------|------------------------------|
|Driver supports catalog only ([`DATABASE_AS_SCHEMA`](../setup/developer-guide.md/#database-reporting) not set)      | no | yes | no |
|Driver supports schema only ([`DATABASE_AS_SCHEMA`](../setup/developer-guide.md/#database-reporting) set to `TRUE`) | yes | no | no |

## Database Reporting Differences Between Timestream JDBC Driver and ODBC Driver
| --- | [Timestream JDBC Driver](https://github.com/awslabs/amazon-timestream-driver-jdbc) | Timestream ODBC Driver |
|-----|------------------------------------------------------------------------------------|------------------------|
| Database reporting | Databases reported as schemas. This behavior is not configurable. | Databases reported as catalogs by default. This behavior is configurable by setting [`DATABASE_AS_SCHEMA`](docs/markdown/setup/developer-guide.md/#database-reporting) environment variable
| Reasons for the design decisions  | Databases are reported as schemas for driver to work on Tableau, as Tableau will not include database names in auto-generated queries if databases are reported as catalogs, and Timestream server does not work with queries without database names. | Databases are reported as catalogs for driver to work on macOS Excel, as not all databases show properly on macOS Excel when databases are reported as schemas.

## Timestream Data Types
Timestream SQL support scalar types int, bigint, boolean, double, varchar, date, time, timestamp, interval_year_month and interval_day_second. Their values could be fetched based using their corresponding SQL data types or as a string/unicode string.

Besides complex data types are also supported. Complex types include Array, Row and Timeseries. These complex types data could only be fetched as a string using our driver. 

### Timeseries format
Wrapped in "[]". Each item is wrapped in "{}" and separated from each other by ",". 

#### Example
```
"[{time: 2019-12-04 19:00:00.000000000, value: 35.2},{time: 2019-12-04 19:01:00.000000000, value: 38.2},{time: 2019-12-04 19:02:00.000000000, value: 45.3}]";
```

### Array Format
Wrapped in "[]". Each element is separated by ",".

#### Example
```
[1,2,3]
```
### Row Format
Wrapped in "()". Each element is separated by ",".

#### Example
```
(1,2,3)
```

For null it could only be fetched as a string and the result is "-".

Please refer to [Timestream Data Types](https://docs.aws.amazon.com/timestream/latest/developerguide/supported-data-types.html) for more info.