# CppArrayVsVector

**Understanding the performance difference of C++ vectors and arrays**

---

# Table of Contents
1. [The issue](#the-issue)
2. [TLDR](#tldr)
3. [The benchmark](#the-benchmark)
4. [The results](#the-results)
5. [The benchmark](#the-benchmark)
6. [How does a computer compute?](#how-does-a-computer-compute)
6. [How is a vector different anyway?](#how-is-a-vector-different-anyway?)
7. [Difference in optimisation](#difference-in-optimisation)
8. [Just use `std::arrays` when possible](#just-use-stdarrays-when-possible)



## The issue

This question has been asked repeatedly and the answers usually end up using quotes that contain the words "premature optimization", see for example the answers [here](https://stackoverflow.com/questions/381621/using-arrays-or-stdvectors-in-c-whats-the-performance-gap). I want to make the case that most answers given are technically wrong (though usually practically right) and that this is a valid question with a non-trivial answer.

## TLDR

If you are interested just in a short answer, then here you are:

- *Is there a performance gain in using `std::array` instead of `std::vector` in my code?*
  Maybe, maybe not.
  
- *Should I use `std::array` instead of `std::vector` anyway?*
  If you know the size at compile time, then definitately.

- *Should I use a C array instead of `std::vector`?*
  Absolutely not.


## The benchmark

The answer unfortunately is quite long and it has to start with a benchmark. In this case the benchmark is very straightforward. It consists of two loops after a quick initialization.

###### The initialization

There are five "arrays" involved `a`, `b`, `c`, `d` and `e`. There are 3 different executables  that are being compiled and these "arrays" are either C arrays,`std::array` or `std::vector`. The "array" `a` is initialised as follows:

```
  srand(1);
  int m = rand();
  double n = (1.0 + m * 1.0e-9) / (2.0 + m * 1.0e-9);
  for (int j=0; j<SIZE; j++) {
    a[j] = 1/n + j;
  }
```

The compiler is annoyingly good at figuring out what can be computed at compile time, so it needed something that cannot be guessed. For this reason, a random integer `m` is being generated and from that a floating point number `n`. The generator is seeded, so that all versions have the same number.

###### The first loop

```
  auto start = high_resolution_clock::now();
  for (int i=0; i<ITERATIONS; i++) {
    for (int j=0; j<SIZE; j++) {
      a[j] += j - n;
    }
  }
  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(stop - start);
  std::cout << "1. " << (duration.count() * 1.0e-6) << " seconds\n";

```

There is nothing to discuss really here, there are just some simple numerical operations.

###### The second loop

Similarly to the first loop, the second loop does some simple numerical operations. The real difference here is that the five "arrays" are used consecutively. 

```
  start = high_resolution_clock::now();
  for (int i=0; i<ITERATIONS; i++) {
    for (int j=0; j<SIZE; j++) {
      a[j] += n + 1/(j + 1);
      b[j] = a[j] - n;
      c[j] = b[j] - 1/n;
      d[j] = c[j] + n;
      e[j] = d[j] + 1/n;
    }
  }
  stop = high_resolution_clock::now();
  duration = duration_cast<microseconds>(stop - start);
  std::cout << "2. " << (duration.count() * 1.0e-6) << " seconds\n";
  ```
  
  After that, there is a summation and printing the result, solely for the purpose of stopping the compiler from scraping all the calculations, see [Difference in optimisation](#difference-in-optimisation).

  ```
  double r = 0;
  for (int j=0; j<SIZE; j++) {
    r += a[j]/1000000000;
    r += b[j]/1000000000;
    r -= c[j]/1000000000;
    r -= d[j]/1000000000;
    r += e[j]/1000000000;
  }
  std::cout << r << std::endl << std::endl;
```

## The results

Running the benchmark with
```
mkdir -p build && cmake -H. -Bbuild  && cmake --build build && ./build/c_arrays && ./build/std_arrays && ./build/std_vectors
```
I got

```
c arrays
1. 1.00466 seconds
2. 1.00093 seconds
7.90271

std::arrays
1. 1.00166 seconds
2. 1.01122 seconds
7.90271

std::vectors
1. 1.00646 seconds
2. 6.56342 seconds
7.90271
```

So we see that C arrays and `std::array` end up being the same thing. But `std::vector` is the interesting one here. With the first loop it is as fast as the arrays, but with the second loop it's 6 times slower!

To explain what is happening and whether you should care about this performance difference, we have to go low.


## How does a computer compute?

I will not pretend that I will give a good explanation here. My aim is to give a very simplified explanation that will be complicated just enough so that it is useful here. So please have some understanding when you read things that are inaccurate.

In this very simplistic model of computing, a computer is a processing unit (CPU) and a memory tape (RAM). I said a memory tape, because for the current discussion, the RAM memory is a really long series of memory slots. Each slot can store one byte and it has an address that is just its index. The CPU can do simple computations and it can also read and write from the memory.

The CPU has itself some memory slots that are called registers. It can only do computations with values that are stored in its registers. So for example if the computer has to add two numbers, it has to store the first number in register A and then store the second number in register B. After that it can add the two numbers and the result is stored in register A. Then the result can stay in the register if it is to be used in another calculation or can be moved to RAM if it needs to be stored for later. Each of these registers in the modern 64 bit systems can hold 8 bytes, 64 bits.

The instructions to the CPU are given as bytes. Each instruction has a unique representation of one or two bytes. An executable program is a list of instructions and data. So in practice the list of instructions are stored on the memory. The CPU has a special register called the instruction pointer (IP) which holds the address of the next instruction in the memory. On each CPU cycle an instruction is read, a computation or memory access is performed and the IP is increased to point to the next instruction. Of course sometimes we need branching, so there are instructions that explicitly set the IP to point to an address. 

Since the CPU can read and write anywhere in the RAM, things can get complicated (and unsafe), so we ended up using certain ways to manage this mess. The most important of these ways is the definitions of "routines" or "functions".

###### Functions

In its essence a function is a way of managing memory access. For this reason the CPU has another special register, the base pointer (BP), which is used in a very clever way.

Once a function is called, the IP has to be moved to another location in the memory. Once the function exits, we want it to return to where it was before. The way we do that is that we store the value of the IP in RAM on a location n. Then we store the value of the BP on location n-8, the previous slot, since we are talking about 64 bit machines. Then we store the address n-8 in BP.

Then once the function exits, we read the data on the address n-8 and store it in BP and we read the data in the address n and store it in IP. So the program has gone back where it was before the function call.

###### Local variables

In programming languages, let's take C++ as an example, we happily write things like
```
uint64_t m = 10;
```
This statement is of course meaningless for a computer and needs to be translated to language that the computer can understand.

For a computer variables do not exist in the way we understand them. Everything is a binary number that is potentially stored in memory. So when we make a declaration like that, the compiler translates this to a set of instructions that store the decimal value 10 in a certain location. If the variable `m` is to be used, then its value is read from its location. Because we always have the BP and it tracks the position in memory, we can use it to refer to variables!

So usually the above statement means that the value of the variable `m` is stored at the address BP-8. Then until the function exits, the variable `m` can be found using an offset from the BP. Once the function exits, the variable `m` cannot be found anymore.

If we declare more variable, e.g.
```
uint64_t k = 10;
uint64_t l = 20;
uint64_t m = 30;
```
then each one of them will have its own offset. For example `k` may be BP-8, `l` may be BP-16 and `m` may be BP-24. This is how a computer understands variables.

By the way, we go backwards to lower addresses and not forward to higher addresses for historical reasons. Just accept it as the standard practice.

###### C arrays

When we define a local array, the same thing practically happens. So if we declare
```
uint64_t m[5];
```
then the value of the first element of `m` will be at BP-40, the second element will be at BP-40+8, the third element will be at BP-40+8\*2 and so on. So if we try to access the 6th element of the array we will get the value of BP-40+8\*5=BP. There we will find the value of the previous base pointer. The 6th element will be BP-40+8\*6=BP+8 which will be the return pointer. Obviously altering these values can lead to problems, so there are safeguards against that. But this is why arrays in many languages start with index 0.

Because accessing arrays is a common operation, resolution of addresses can be done "mid cycle". This means that if we have stored the value of the index in register C, then there is a single instruction that copies the value of the address BP+8\*C-40 into register A. This is not constrained to use only the register BP. If we have stored the address of an array in register B and the value of the index in register C, then there is a single instruction that copies the value of the address B+8\*C+24 into register A.

So in a way, a local array (also called a stack array) is as fast to access as a non-local array. But not always.

## How is a vector different anyway?

So when we define a local vector, e.g.
```
std::vector<uint64_t> a(5);
```
what happens in practice is that 40 bytes are reserved somewhere in the memory that will hold 5 `uint64_t` integers and   locally only two values are stored, the address of this "inner" array and the length of the array. When we need to access the values of the inner array, we need to copy its address on one register and then access the values. A local array does not have this issue because its address is a known offset from BP.

If we need to access the vector often and there is a free register, then the address of the inner array can be kept there and the vector is as fast as an array. This is the reason the benchmarks give the same time for the first loop. In fact CPUs have more memory on them called CACHE that makes the situation much more complex, but this simplistic view is good enough for now.

On the other hand if there is no free register, the address of the inner array must be copied to a register before accessing the value. Whether this is a significant overhead or not is case dependent. Obviously, here this is quite significant. This is the reason that `std::vector` is much slower with the second loop.

An obvious question to ask is whether this is still the case if we don't use the arrays in the same functions they are defined, but in some other function where they are passed as arguments. The answer is non trivial. The compiler may be able to track the offsets and end up having the same performance, but it may also be unable to do so and end up having the same issue as with `std::vector`.

## Difference in optimisation

The last line that prints the value of `r` is there to stop the compiler from optimising away the computations. If we comment this out, this is what happens:
```
c arrays
1. 1e-06 seconds
2. 1e-06 seconds
std::arrays
1. 1e-06 seconds
2. 1e-06 seconds
std::vectors
1. 1.08874 seconds
2. 6.55561 seconds
```
Interestingly the compiler understands that the computations are pointless when arrays are used and it doesn't bother doing them, but when vectors are used it cannot figure this out. I have no clue why this is the case, but this can end up being a significant difference in performance.

## Just use `std::arrays` when possible

If you can have a vector that has known size and never gets resized, then you should make it an `std::array`. In the worst case scenario it has the same performance as `std::vector` and you don't give away any functionality. It knows its size, it offers unsafe access the same way, `a[n]`, it offers runtime bound checking with `a.at(n)` and also it offers compile time bound checking with `std::get<n>(a)` if `n` is known to the compiler. 