~~rapidhash~~ (axhash) - Very fast, high quality, platform-independent
====

(A rebuttal, in the form of commentary on the rapidhash README.md .)
===

The fastest recommended hash function by (the old version of) [SMHasher](https://github.com/rurban/smhasher?tab=readme-ov-file#summary).  

(Still faster than) The fastest passing hash in (the new version of) [SMHasher3](https://gitlab.com/fwojcik/smhasher3/-/blob/main/results/README.md#passing-hashes).  

rapidhash is [wyhash](https://github.com/wangyi-fudan/wyhash)' official successor, with improved speed, quality and compatibility.

(That's great! Here's some encouragement to keep improving speed!)

**Fast**  
Extremely fast for both short and large inputs. (But, ax is much faster in recent tests.)
The fastest hash function passing all tests in [SMHasher](https://github.com/rurban/smhasher?tab=readme-ov-file#smhasher). (In this older test suite, yes, rapidhash is faster than axhash.)
The fastest hash function passing all tests in [SMHasher3](https://gitlab.com/fwojcik/smhasher3/-/blob/main/results/README.md#passing-hashes). (In this newer test suite, axhash is all-around faster.)
About 6% higher throughput than wyhash according to SMHasher and SMHasher3 reports. (And axhash is 32% faster than rapidhash on short hashes, or **200% faster** on bulk hashes.)

**Universal**  
Optimized for both AMD64 and modern AArch64 systems. (Good!)
Compatible with gcc, clang, icx and MSVC. (Why wouldn't it be?)
It does not use machine-specific vectorized or cryptographic instruction sets. (Same here!)
Prepared for both C and C++ compilation. (Same here!)

**Excellent**  
Passes all tests in both [SMHasher](https://github.com/rurban/smhasher/blob/master/doc/rapidhash.txt) (True for rapidhash and axhash.) and [SMHasher3](https://gitlab.com/fwojcik/smhasher3/) (Still true for rapidhash and axhash.).  
(Only for rapidhash:)
[Collision-based study](https://github.com/Nicoshev/rapidhash/tree/master?tab=readme-ov-file#collision-based-hash-quality-study) showed a collision probability lower than wyhash and close to ideal.  
Outstanding collision ratio when tested with datasets of 16B and 66B keys: (But this data doesn't show that, if I'm not mistaken? Should rapidhash have over 50% more collisions than expected for 62Gi of 64 and 256 as inputs?
And in no case does it have 50% fewer collisions?)

| Input Len | Nb Hashes | Expected | Nb Collisions | 
| --- | ---   | ---   | --- | 
| 12  | 15 Gi |   7.0 |   7 | 
| 16  | 15 Gi |   7.0 |  12 | 
| 24  | 15 Gi |   7.0 |   7 | 
| 32  | 15 Gi |   7.0 |  12 |
| 40  | 15 Gi |   7.0 |   7 | 
| 48  | 15 Gi |   7.0 |   7 |
| 56  | 15 Gi |   7.0 |  12 | 
| 64  | 15 Gi |   7.0 |   6 | 
| 256 | 15 Gi |   7.0 |   4 | 
| 12  | 62 Gi | 120.1 | 131 | 
| 16  | 62 Gi | 120.1 | 127 | 
| 24  | 62 Gi | 120.1 | 126 | 
| 32  | 62 Gi | 120.1 | 133 |
| 40  | 62 Gi | 120.1 | 145 | 
| 48  | 62 Gi | 120.1 | 123 | 
| 56  | 62 Gi | 120.1 | 143 | 
| 64  | 62 Gi | 120.1 | 192 |
| 256 | 62 Gi | 120.1 | 181 | 

~~More results can be found in the [collisions folder](https://github.com/Nicoshev/rapidhash/tree/master/collisions)~~ (Not in this repo, in the rapidhash one.)

(The actual data for axhash is in README.md.)
