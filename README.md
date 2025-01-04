# axhash
Chop down tall trees? Chop down big keys.

Much fast. Very hashing. Such independence. Wow.

## What is it?
It's a non-cryptographic hashing algorithm! Axhash is based loosely on [MX3.v3 by Jon Maiga](https://github.com/jonmaiga/mx3).
The mix_stream method should be the same, though everything else is different. Unlike stock MX3.v3, axhash passes all tests in
[SMHasher 3](https://gitlab.com/fwojcik/smhasher3). It also just happens to be the fastest passing hash on small keys, while
also having 3x the throughput of the previous #1 hash, [rapidhash](https://github.com/Nicoshev/rapidhash), on large keys
(bulk, 262144-bytes).

## Test results

```
----------------------------------------------------------------------------------------------
-log2(p-value) summary:

          0     1     2     3     4     5     6     7     8     9    10    11    12
        ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----
         4390  1297   587   301   153    96    24    18    13     3     0     0     0

         13    14    15    16    17    18    19    20    21    22    23    24    25+
        ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----
            0     1     0     0     0     0     0     0     0     0     0     0     0

----------------------------------------------------------------------------------------------
Summary for: ax
Overall result: pass            ( 188 / 188 passed)

----------------------------------------------------------------------------------------------
Verification value is 0x00000001 - Testing took 384.370490 seconds
```

### Speed results

These were run on a "workstation laptop" running Windows 11 with a 12th Gen Intel(R)
Core(TM) i7-12800H 2.40 GHz processor. It has 6 performance cores and 8 low-power cores.
The performance cores were probably running at 3.5 GHz for both tests, though it
really is hard to tell with modern processors. Background load was comparable,
though non-zero, for either test.

For axhash:

```
[[[ Speed Tests ]]]

Small key speed test - [1, 31]-byte keys
   1-byte keys -    12.03 cycles/hash
   2-byte keys -    14.76 cycles/hash
   3-byte keys -    15.91 cycles/hash
   4-byte keys -    12.27 cycles/hash
   5-byte keys -    12.38 cycles/hash
   6-byte keys -    12.38 cycles/hash
   7-byte keys -    12.34 cycles/hash
   8-byte keys -    23.72 cycles/hash
   9-byte keys -    25.83 cycles/hash
  10-byte keys -    25.85 cycles/hash
  11-byte keys -    25.70 cycles/hash
  12-byte keys -    25.83 cycles/hash
  13-byte keys -    25.83 cycles/hash
  14-byte keys -    25.80 cycles/hash
  15-byte keys -    25.22 cycles/hash
  16-byte keys -    25.86 cycles/hash
  17-byte keys -    28.21 cycles/hash
  18-byte keys -    28.00 cycles/hash
  19-byte keys -    28.15 cycles/hash
  20-byte keys -    28.21 cycles/hash
  21-byte keys -    28.14 cycles/hash
  22-byte keys -    28.15 cycles/hash
  23-byte keys -    28.17 cycles/hash
  24-byte keys -    28.16 cycles/hash
  25-byte keys -    30.46 cycles/hash
  26-byte keys -    30.46 cycles/hash
  27-byte keys -    29.94 cycles/hash
  28-byte keys -    30.46 cycles/hash
  29-byte keys -    29.91 cycles/hash
  30-byte keys -    29.90 cycles/hash
  31-byte keys -    30.46 cycles/hash
Average        -    24.47 cycles/hash
 rnd-byte keys -    24.47 cycles/hash

Bulk speed test - 262144-byte keys
Alignment   7 - 12.15 bytes/cycle - 39.61 GiB/sec @ 3.5 ghz
Alignment   6 - 12.15 bytes/cycle - 39.62 GiB/sec @ 3.5 ghz
Alignment   5 - 12.15 bytes/cycle - 39.61 GiB/sec @ 3.5 ghz
Alignment   4 - 12.15 bytes/cycle - 39.61 GiB/sec @ 3.5 ghz
Alignment   3 - 12.15 bytes/cycle - 39.62 GiB/sec @ 3.5 ghz
Alignment   2 - 12.15 bytes/cycle - 39.62 GiB/sec @ 3.5 ghz
Alignment   1 - 12.15 bytes/cycle - 39.61 GiB/sec @ 3.5 ghz
Alignment   0 - 12.16 bytes/cycle - 39.62 GiB/sec @ 3.5 ghz
Average       - 12.15 bytes/cycle - 39.62 GiB/sec @ 3.5 ghz
Alignment rnd - 12.15 bytes/cycle - 39.61 GiB/sec @ 3.5 ghz

Bulk speed test - [262017, 262144]-byte keys
Alignment   7 - 12.13 bytes/cycle - 39.53 GiB/sec @ 3.5 ghz
Alignment   6 - 12.13 bytes/cycle - 39.53 GiB/sec @ 3.5 ghz
Alignment   5 - 12.13 bytes/cycle - 39.53 GiB/sec @ 3.5 ghz
Alignment   4 - 12.13 bytes/cycle - 39.53 GiB/sec @ 3.5 ghz
Alignment   3 - 12.13 bytes/cycle - 39.52 GiB/sec @ 3.5 ghz
Alignment   2 - 12.12 bytes/cycle - 39.52 GiB/sec @ 3.5 ghz
Alignment   1 - 12.13 bytes/cycle - 39.53 GiB/sec @ 3.5 ghz
Alignment   0 - 12.13 bytes/cycle - 39.54 GiB/sec @ 3.5 ghz
Average       - 12.13 bytes/cycle - 39.53 GiB/sec @ 3.5 ghz
Alignment rnd - 12.04 bytes/cycle - 39.23 GiB/sec @ 3.5 ghz

[[[ 'Hashmap' Speed Tests ]]]

std::unordered_map
Init std HashMapTest:     441.465 cycles/op (792291 inserts, 1% deletions)
Running std HashMapTest:  192.585 cycles/op (6.7 stdv)

greg7mdp/parallel-hashmap
Init fast HashMapTest:    106.661 cycles/op (792291 inserts, 1% deletions)
Running fast HashMapTest: 156.758 cycles/op (6.2 stdv)
```

Compare to rapidhash results on the same machine:

```
[[[ Speed Tests ]]]

Small key speed test - [1, 31]-byte keys
   1-byte keys -    23.64 cycles/hash
   2-byte keys -    23.63 cycles/hash
   3-byte keys -    23.69 cycles/hash
   4-byte keys -    23.55 cycles/hash
   5-byte keys -    31.11 cycles/hash
   6-byte keys -    31.08 cycles/hash
   7-byte keys -    31.08 cycles/hash
   8-byte keys -    20.67 cycles/hash
   9-byte keys -    30.89 cycles/hash
  10-byte keys -    30.89 cycles/hash
  11-byte keys -    30.90 cycles/hash
  12-byte keys -    20.35 cycles/hash
  13-byte keys -    20.35 cycles/hash
  14-byte keys -    20.36 cycles/hash
  15-byte keys -    20.33 cycles/hash
  16-byte keys -    20.37 cycles/hash
  17-byte keys -    40.22 cycles/hash
  18-byte keys -    40.20 cycles/hash
  19-byte keys -    40.20 cycles/hash
  20-byte keys -    40.20 cycles/hash
  21-byte keys -    40.19 cycles/hash
  22-byte keys -    40.22 cycles/hash
  23-byte keys -    40.20 cycles/hash
  24-byte keys -    40.19 cycles/hash
  25-byte keys -    40.18 cycles/hash
  26-byte keys -    40.20 cycles/hash
  27-byte keys -    40.18 cycles/hash
  28-byte keys -    40.18 cycles/hash
  29-byte keys -    40.21 cycles/hash
  30-byte keys -    40.19 cycles/hash
  31-byte keys -    40.21 cycles/hash
Average        -    32.45 cycles/hash
 rnd-byte keys -    32.50 cycles/hash

Bulk speed test - 262144-byte keys
Alignment   7 -  4.04 bytes/cycle - 13.16 GiB/sec @ 3.5 ghz
Alignment   6 -  4.03 bytes/cycle - 13.15 GiB/sec @ 3.5 ghz
Alignment   5 -  4.03 bytes/cycle - 13.15 GiB/sec @ 3.5 ghz
Alignment   4 -  4.04 bytes/cycle - 13.16 GiB/sec @ 3.5 ghz
Alignment   3 -  4.04 bytes/cycle - 13.16 GiB/sec @ 3.5 ghz
Alignment   2 -  4.04 bytes/cycle - 13.16 GiB/sec @ 3.5 ghz
Alignment   1 -  4.04 bytes/cycle - 13.16 GiB/sec @ 3.5 ghz
Alignment   0 -  4.04 bytes/cycle - 13.16 GiB/sec @ 3.5 ghz
Average       -  4.04 bytes/cycle - 13.16 GiB/sec @ 3.5 ghz
Alignment rnd -  4.03 bytes/cycle - 13.13 GiB/sec @ 3.5 ghz

Bulk speed test - [262017, 262144]-byte keys
Alignment   7 -  4.02 bytes/cycle - 13.09 GiB/sec @ 3.5 ghz
Alignment   6 -  4.02 bytes/cycle - 13.11 GiB/sec @ 3.5 ghz
Alignment   5 -  4.02 bytes/cycle - 13.11 GiB/sec @ 3.5 ghz
Alignment   4 -  4.02 bytes/cycle - 13.10 GiB/sec @ 3.5 ghz
Alignment   3 -  4.02 bytes/cycle - 13.10 GiB/sec @ 3.5 ghz
Alignment   2 -  4.02 bytes/cycle - 13.10 GiB/sec @ 3.5 ghz
Alignment   1 -  4.02 bytes/cycle - 13.10 GiB/sec @ 3.5 ghz
Alignment   0 -  4.02 bytes/cycle - 13.10 GiB/sec @ 3.5 ghz
Average       -  4.02 bytes/cycle - 13.10 GiB/sec @ 3.5 ghz
Alignment rnd -  3.95 bytes/cycle - 12.88 GiB/sec @ 3.5 ghz
```

### License
[CC0](LICENSE). You can freely use this for any purpose, even commercial closed-source apps.
Because it is public domain under CC0, I am not liable if it doesn't do exactly what you want;
after all, public domain means it belongs to you as much as it does to me.

Post an issue or send a PR if you want to! PRs must also be CC0 if they are to be accepted here.

This does currently use several boilerplate macros and defines from rapidhash to account for
missing cross-platform/multiple-compiler functionality in C. Rapidhash is licensed under the
BSD 2-Clause license. If a legal body rules that makes this algorithm also licensed under the
BSD 2-Clause license, that won't be a problem because CC0 allows freely relicensing code, and
BSD 2-Clause is similarly lenient (it also allows usage in commercial closed-source apps).

## Snark

For continuous sass, see my [REBUTTAL.md](REBUTTAL.md) to the rapidhash README.md .
In all honesty, rapidhash is really cool; it didn't look like wyhash would be easy to make
significant improvements upon, yet they did it.
