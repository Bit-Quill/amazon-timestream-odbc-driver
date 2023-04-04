# performance-test-odbc
Performance test framework for testing ODBC driver.

# How to run with executable
1. Setup test database system under the `timestream-iam` DSN, this DSN is hardcoded into the test framework.
2. Build the performance tool: `cd performance && cmake . && make -j 4 && cd ..`.
3. Run executable: `./performance/PTODBCResults/performance_results`.
e.g. output console:
```
[ RUN      ] TestPerformance.Time_Execute
%%__PARSE__SYNC__START__%%
%%__QUERY__%% SELECT * FROM performance.employer limit 1
%%__CASE__%% 1: Rows retrieved 1
%%__MIN__%% 62 ms
%%__MAX__%% 81 ms
%%__MEAN__%% 67 ms
%%__MEDIAN__%% 66 ms
%%__PARSE__SYNC__END__%%
Time dump: 232 ms
[       OK ] TestPerformance.Time_Execute (798 ms)
```

