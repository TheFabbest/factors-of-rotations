# Lyndon Factors of Cyclic Rotations

A simple implementation of the O(n) algorithm from [Bijective BWT based Compression Schemes](https://doi.org/10.48550/arXiv.2406.16475) for computing all Lyndon factors of all cyclic rotations (conjugates) of a string.
This implementation is designed for educational purposes, prioritizing clarity and simplicity over performance.

This program was developed by Fabrizio Apuzzo (TheFabbest) as part of his Bachelor's Thesis at University of Naples Federico II.
See my thesis at https://github.com/TheFabbest/Lyndon-Factorization-of-Conjugates.

## What are Lyndon Factors?

A Lyndon word is a string that is strictly smaller in lexicographic order than all of its non-trivial rotations. The Lyndon factorization of a string decomposes it into a non-increasing sequence of Lyndon words.
This tool's main subcommand `conjugatefactors` computes the factors appearing in all cyclic rotations of an input string (assuming quiet flag is set). It does not compute the factorizations themselves, though it can (and does by default) display the factors of each prefix and suffix of the smallest conjugate of the input string. The latter is not linear in time complexity, since the output is itself quadratic in size.

## Features

- **Conjugate Factors**: Compute Lyndon factors for all cyclic rotations of a word
- **Lyndon Trees**: Visualize right and left Lyndon trees for string analysis
- **Suffix Array**: Build suffix arrays from strings or files, with configurable byte-per-character encoding and more
- **Built-in Tests**: Verify implementation correctness
- **CLI**: Command-line interface for easy usage and integration into scripts, with options for quiet mode (easier to parse) and verbose (default, for understanding) output.

## Building

### Standard Build
```bash
make
```

### Optimized Build
For better performance, recommended for testing:
```bash
make build_optimized
```

### Clean
```bash
make clean
```

## Usage

### View Help
```bash
./factors_of_rotations --help
```

### Commands

#### 1. Conjugate Factors
Compute Lyndon factors for all cyclic rotations of a word:
```bash
$./factors_of_rotations conjugatefactors banana
Working on smallest conjugate at index 5: abanan
-----------------------------------
All factors that appear in prefixes, from LynS: 
a, ab, a, aban, a
Exact factorization of each prefix, from LynS: 
Prefix a: a
Prefix ab: ab
Prefix aba: a, ab
Prefix aban: aban
Prefix abana: a, aban

-----------------------------------
All factors that appear in suffixes, from Lyn: 
n, an, n, an, b
Exact factorization of each suffix, from Lyn: 
Suffix banan: b, an, an
Suffix anan: an, an
Suffix nan: n, an
Suffix an: an
Suffix n: n
```
This command first finds the smallest conjugate of the input word, then computes and displays all factors that appear in prefixes of that conjugate and all factors that appear in suffixes of the original word, separately.
If you're not interested in this verbose output, you can use the `--quiet` flag to only display the factors that appear in all conjugates:
```bash
$./factors_of_rotations -q conjugatefactors banana
a, ab, a, aban, a
n, an, n, an, b
```
This displays all factors that appear in prefixes of the smallest conjugate and all factors that appear in suffixes of the smallest conjugate, respectively on the first and second lines.

#### 2. Show Lyndon Trees
Display right and left Lyndon trees:
```bash
$./factors_of_rotations showtrees example
Working on smallest conjugate: ampleex
Left Lyndon Tree: 
(node (node (node (node (node (node (a) (m)) (p)) (l)) (e)) (e)) (x))
-----------------------------------
Right Lyndon Tree: 
(node (node (node (a) (node (m) (p))) (l)) (node (e) (node (e) (x))))
```

The `--quiet` flag can also be used here to suppress just the first line of output, which is the smallest conjugate of the input word:
```bash
$./factors_of_rotations -q showtrees example
Left Lyndon Tree: 
(node (node (node (node (node (node (a) (m)) (p)) (l)) (e)) (e)) (x))
-----------------------------------
Right Lyndon Tree: 
(node (node (node (a) (node (m) (p))) (l)) (node (e) (node (e) (x))))
```

#### 3. Suffix Array
Build a suffix array from a string:
```bash
$./factors_of_rotations suffixarray banana
Computing suffix array for the word: banana
5, 3, 1, 0, 4, 2
```
The `--quiet` flag can also be used here to suppress the first line of output, which is the input word:
```bash
$./factors_of_rotations -q suffixarray banana
5, 3, 1, 0, 4, 2
```

You can also build a suffix array from a binary file (output assuming myfile.bin contains the string "bbaannaannaa"):
```bash
$./factors_of_rotations suffixarray myfile.bin --input-file
Input array: 98, 98, 97, 97, 110, 110, 97, 97, 110, 110, 97, 97
Suffix Array: 11, 10, 6, 2, 7, 3, 1, 0, 9, 5, 8, 4
```
This treats your input file as a binary file, reading each byte as a character. You can also specify the number of bytes per character (e.g., 2 bytes per character for UTF-16 encoded files):

```bash
$./factors_of_rotations suffixarray myfile.bin --input-file --bytes-per-char 2
Input array: 25186, 24929, 28270, 24929, 28270, 24929
Suffix Array: 5, 3, 1, 0, 4, 2
```

You can also limit the number of characters read from file:
```bash
$./factors_of_rotations suffixarray myfile.bin --input-file --max-chars 3
Input array: 98, 98, 97
Suffix Array: 2, 1, 0
```

#### 4. Run Tests
Execute built-in test suite:
```bash
./factors_of_rotations test
```
This does not use frameworks, but rather a simple set of naive implementations to verify correctness of the main algorithms. It will print a message for each test, signaling a fail in case of a mismatch between our implementations and naive alternatives or expected results.
This may take a while, so make sure you compile the optimized build for better performance.

## Command Reference

| Command | Description | Options |
|---------|-------------|---------|
| `conjugatefactors <word>` | Find Lyndon factors of all conjugates |
| `showtrees <word>` | Show right and left Lyndon trees |
| `suffixarray <input>` | Build suffix array | `--input-file`, `--bytes-per-char`, `--max-chars` |
| `test` | Run test suite |

### Global Flags
- `-q, --quiet`: Disable verbose output for detailed information, generally recommended for production use. We chose to keep the default behavior verbose for educational purposes, making the output more informative for those learning the algorithm.
- `--help`: Display help message and exit. Can be used standalone or applied to any subcommand.

## Algorithm Complexity

The implementation achieves **O(n)** time complexity for computing Lyndon factors of all n cyclic rotations, as described in the referenced paper. The factorization of each prefix and suffix of the smallest conjugate is **O(n^2)** in time complexity, since the output itself always is quadratic in size.
The suffix array construction is done in **O(n log(n))** time complexity and **O(1)** auxiliary space complexity. A more complex linear-time alternative exists, but we chose to implement the simpler **O(n log(n))** version (which is optimal for general alphabets) for simplicity.

## Implementation Philosophy

This implementation prioritizes:
- **Clarity**: Readable and understandable code structure
- **Simplicity**: Straightforward implementation without unnecessary complexity
- **Fidelity**: Accurate adherence to the algorithm in the literature

Performance optimizations are secondary to these goals, though the optimized build provides improved speed for production use.

## Requirements

- C++ compiler with C++11 support (g++ recommended)
- Make build system
- CLI11 header library (included in project)

## References

This implementation is based on the algorithm presented in:

**Bijective BWT based Compression Schemes**  
arXiv:2406.16475  
https://doi.org/10.48550/arXiv.2406.16475

My related thesis document can be found here:
https://github.com/TheFabbest/Lyndon-Factorization-of-Conjugates

All algorithms and data structures used in this implementation are described in the above work, which is the primary reference for understanding the underlying theory and methodology.

## Author

Fabrizio Apuzzo
Bachelor's Thesis Project
University of Naples Federico II

## Contributing

Any suggestion, improvement, bug report, feature request, contribution, question, comment, or update is welcome.

## Citation

If you use this implementation in your research or projects, please cite both this repository and my thesis.
