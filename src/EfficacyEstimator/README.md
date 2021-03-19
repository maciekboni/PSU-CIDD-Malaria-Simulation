# Efficacy Estimator

This mini simulation will produce PK/PD profile for a single drug (mono or combination therapy) as well as drug-efficacy after 28 days.


# Options
| Option Name             | Description                                                                                                                                                                         |
|-------------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| -h,--help               | Print this help message and exit                                                                                                                                                    |
| -p,--popsize INT        | Number of individuals used in the simulation (default: 10,000)                                                                                                                      |
| -t,--halflife FLOAT ... | Drug elimintaion half-life in date unit. Ex: `-t 2` or `--halflife 2` for monotherapy, `-t 4.5 28.0` or `--halflife 4.5 28.0` for a combination of two drugs.                       |
| -k,--kmax FLOAT ...     | The maximum fraction of parasites that can be killed per day. Ex: `-k 0.999` or `--kmax 0.999` for monotherapy, `-k 0.999 0.99` or `--kmax= 0.999 0.99` for drug combination.       |
| -e,--EC50 FLOAT ...     | The drug concentration at which the parasite killng reach 50%. Ex: `-e 0.75` or `--EC50 0.75` for monotherapy, `-e 0.75 0.65` or `--EC50 0.75 0.65` for drug combination.           |
| -n,--slope FLOAT ...    | the slope of the concentration-effect curve. Ex: `-n 25` or `--slope 25` for monotherapy, `-n 25 10` or `--slope 25 10` for drug combination. `-h, --help`: Output help intructions |
| -f,--file               | Output to file. Default: false, output to console.                                                                                                                                  |

