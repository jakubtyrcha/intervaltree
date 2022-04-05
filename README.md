# Short intervals
| benchmark name   | samples     | iterations   | estimated  |
|------------------|-------------|--------------|------------|
| mean             | low mean    | high mean    |            |
| std dev          | low std dev | high std dev |            |
|                  |             |              |            |
| brute-force list | 100         | 1            | 8.51603 s  |
| 85.5308 ms       | 85.3521 ms  | 85.7892 ms   |            |
| 1.08734 ms       | 829.181 us  | 1.53159 ms   |            |
| interval tree    | 100         | 1            | 267.661 ms |
| 2.70882 ms       | 2.69812 ms  | 2.73358 ms   |            |
| 78.6848 us       | 32.4244 us  | 144.889 us   |            |


# Long intervals
| benchmark name   | samples     | iterations   | estimated  |
|------------------|-------------|--------------|------------|
| mean             | low mean    | high mean    |            |
| std dev          | low std dev | high std dev |            |
|                  |             |              |            |
| brute-force list | 100         | 1            | 8.77848 s  |
| 87.9095 ms       | 87.8159 ms  | 88.0266 ms   |            |
| 532.849 us       | 442.839 us  | 690.95 us    |            |
| interval tree    | 100         | 1            | 929.991 ms |
| 9.19718 ms       | 9.18038 ms  | 9.22372 ms   |            |
| 105.871 us       | 74.9969 us  | 154.995 us   |            |
