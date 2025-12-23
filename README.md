# Lyndon Factors of Cyclic Rotations

An efficient O(n) implementation of the algorithm from [Bijective BWT based Compression Schemes](https://doi.org/10.48550/arXiv.2406.16475) for computing all Lyndon factors of all cyclic rotations (conjugates) of a string.

This program was developed by Fabrizio Apuzzo as part of his Bachelor's Thesis at University of Naples Federico II.

## What are Lyndon Factors?

A Lyndon word is a string that is strictly smaller in lexicographic order than all of its non-trivial rotations. The Lyndon factorization of a string decomposes it into a non-increasing sequence of Lyndon words.

This tool computes these factorizations efficiently for all cyclic rotations of an input string.

## Features

- **Conjugate Factors**: Compute Lyndon factors for all cyclic rotations of a word
- **Lyndon Trees**: Visualize right and left Lyndon trees for string analysis
- **Suffix Array**: Build suffix arrays from strings or files, with configurable byte-per-character encoding and more
- **Built-in Tests**: Verify implementation correctness

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
./factors_of_rotations conjugatefactors banana
```

#### 2. Show Lyndon Trees
Display right and left Lyndon trees:
```bash
./factors_of_rotations showtrees "example"
```

#### 3. Suffix Array
Build a suffix array from a string:
```bash
./factors_of_rotations suffixarray "mississippi"
```

Build a suffix array from a binary file:
```bash
./factors_of_rotations suffixarray myfile.bin --input-file
```

With custom encoding (e.g., 2 bytes per character):
```bash
./factors_of_rotations suffixarray myfile.bin --input-file --bytes-per-char 2
```

Limit characters read from file:
```bash
./factors_of_rotations suffixarray myfile.bin --input-file --max-chars 10000
```

#### 4. Run Tests
Execute built-in test suite:
```bash
./factors_of_rotations test
```

## Command Reference

| Command | Description | Options |
|---------|-------------|---------|
| `conjugatefactors <word>` | Find Lyndon factors of all conjugates |
| `showtrees <word>` | Show right and left Lyndon trees |
| `suffixarray <input>` | Build suffix array | `--input-file`, `--bytes-per-char`, `--max-chars` |
| `test` | Run test suite | none |

### Global Flags
- `-q, --quiet`: Disable verbose output for detailed information, generally recommended.

## Algorithm Complexity

The implementation achieves **O(n)** time complexity for computing Lyndon factors of all n cyclic rotations, as described in the referenced paper.

## Implementation Philosophy

This implementation prioritizes:
- **Clarity**: Readable and understandable code structure
- **Simplicity**: Straightforward implementation without unnecessary complexity  
- **Fidelity**: Accurate adherence to the algorithm in the paper

Performance optimizations are secondary to these goals, though the optimized build provides improved speed for production use.

## Requirements

- C++ compiler with C++11 support (g++ recommended)
- Make build system
- CLI11 header library (included in project)

## Project Structure

```
.
├── src/
│   ├── main.cpp       # Main program entry point
│   ├── utils.hpp      # Utility functions
│   ├── tests.hpp      # Test suite
│   ├── cli.hpp        # CLI interface functions
│   └── CLI/
│       └── CLI11.hpp  # Command-line parsing library
├── Makefile
└── README.md
```

## References

This implementation is based on the algorithm presented in:

**Bijective BWT based Compression Schemes**  
arXiv:2406.16475  
https://doi.org/10.48550/arXiv.2406.16475

## License

[Add your license information here]

## Author

Fabrizio Apuzzo  
Bachelor's Thesis Project  
University of Naples Federico II

## Contributing

[Add contribution guidelines if accepting contributions]

## Citation

If you use this implementation in your research, please cite both this repository and the original paper.