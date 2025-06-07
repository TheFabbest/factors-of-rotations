// Computation of the suffix array of read-only general alphabets is described in:
// "Optimal in-place suffix array construction"

#include <algorithm>

// auxiliary functions
int compareSuffixes(const char * const input, const unsigned long length, const unsigned long offsetA, const unsigned long offsetB) {
    unsigned long i = 0;
    while (i < length) {
        if (input[offsetA + i] != input[offsetB + i]) {
            return input[offsetA + i] - input[offsetB + i];
        }
        ++i;
    }
    return 0; // equal suffixes
}

// section 5.2 - step 1
unsigned long placeIndicesOfS_Type(const char * const input, const unsigned long length, unsigned long *SA) {
    bool nextIsL = true; // last character is always L_TYPE because of the sentinel at the end (which is S_TYPE)
    unsigned long last_inserted_index = length;
    for (unsigned long i = 0; i < length-1; ++i) {
        const unsigned long index = length - i - 2;
        const bool currentIsL = input[index] > input[index+1] || (nextIsL && input[index] == input[index+1]);
        if (!currentIsL) {
            SA[--last_inserted_index] = index;
        }
        nextIsL = currentIsL;
    }

    return last_inserted_index;
}

// section 5.2 - step 2
void mergeSortS_Substrings(const char * const input, const unsigned long length, unsigned long *SA, const unsigned long nS) {
    // merge sort SA[nS, length-1] using SA[0, nS-1] as the auxiliary array, thus O(1) space
    unsigned long *auxiliary = SA;
    unsigned long *array = SA + nS;

    unsigned long step = 2;
    while (step < length - nS) {
        for (unsigned long i = nS; i < length; i += step) {
            unsigned long mid = i + step / 2;
            std::merge(array + i, array + mid, array + mid, array + i + step, auxiliary, 
                       [input, length](unsigned long a, unsigned long b) {
                           return compareSuffixes(input, length, a, b) < 0;
                       });
            std::copy(auxiliary, auxiliary + (i + step - nS), array + i);
        }
    }
}

// section 5.3 - step 1
void constructReducedProblem(const char * const input, const unsigned long length, unsigned long *SA, const unsigned long nS) {
    char currentChar = 0;
    SA[0] = currentChar;

    for (unsigned long i = length - nS + 1; i < length; ++i) {
        // TODO inefficient ?
        for (unsigned long iter = 0; iter < SA[i] - SA[i-1]; ++iter) {
            if (input[SA[i] + iter] != input[SA[i-1] + iter]) {
                ++currentChar;
                break;
            }
        }
        SA[i - length - nS] = currentChar;
    }
}

// section 5.4
void RestoreFromRecursion(const char * const input, const unsigned long length, unsigned long *SA, const unsigned long nS) {
    bool nextIsL = true;
    unsigned long sum = 0;
    for (unsigned long i = 0; i < length-1; ++i) {
        const unsigned long index = length - i - 2;
        const bool currentIsL = input[index] > input[index+1] || (nextIsL && input[index] == input[index+1]);
        if (!currentIsL) {
            ++sum;
            SA[nS - sum] = index;
        }
        nextIsL = currentIsL;
    }

    for (unsigned long i = length-nS-1; i < length; ++i) {
        SA[i] = SA[SA[i]];
    }

}