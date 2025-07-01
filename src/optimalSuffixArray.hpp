// Computation of the suffix array of read-only general alphabets is described in:
// "Optimal in-place suffix array construction"

#include <algorithm>
#include <iostream>
using namespace std;

#define BT (unsigned long) -1
#define BH (unsigned long) -2
#define E (unsigned long) -3
#define R1 (unsigned long) -4
#define R2 (unsigned long) -5


// ----- auxiliary functions

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

void Heapify(unsigned long *array, unsigned long n, unsigned long index) {
    while (true) {
        unsigned long smallest = index;
        const unsigned long left = 2 * index + 1;
        const unsigned long right = 2 * index + 2;
        if (left < n && array[left] < array[smallest]) {
            smallest = left;
        }
        if (right < n && array[right] < array[smallest]) {
            smallest = right;
        }
        if (smallest != index) {
            std::swap(array[index], array[smallest]);
            index = smallest;
        } else {
            break; // heap property is satisfied
        }
    }
}

void BuildHeap(unsigned long *array, unsigned long n){
    for (unsigned long i = n/2+1; i > 0; --i) {
        const unsigned long index = i - 1;
        Heapify(array, n, index);
    }
}

// modified binary search that returns the first occurrence of the value
unsigned long binary_search(const char * const input, const unsigned long *array, const unsigned long length, const unsigned long value) {
    unsigned long left = 0;
    unsigned long right = length - 1;
    while (right > left)
    {
        const unsigned long mid = left + ((right-left) / 2);
        if (input[array[mid]] >= value)
        {
            right = mid;
        }
        else
        {
            left = mid + 1;
        }
    }
    return right;
}

// end of auxiliary functions -----

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

    return length-last_inserted_index;
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
        step *= 2;
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


// section 5.3 - step 2
void heapSortReducedProblem(unsigned long *SA, unsigned long length, const unsigned long nS) {
    BuildHeap(SA + nS, length - nS);
    for (unsigned long i = length; i > nS; --i) {
        std::swap(SA[nS], SA[i]);
        Heapify(SA + nS, i, 0);
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

// section 5.5 - preprocessing
void preprocess(const char * const input, unsigned long *SA, const unsigned long length, const unsigned long nS) {
    bool nextIsL = true; // last character is always L_TYPE because of the sentinel at the end (which is S_TYPE)
    unsigned long last_inserted_index = length-nS;
    for (unsigned long i = 0; i < length-1; ++i) {
        const unsigned long index = length - i - 2;
        const bool currentIsL = input[index] > input[index+1] || (nextIsL && input[index] == input[index+1]);
        if (currentIsL) {
            SA[--last_inserted_index] = index;
        }
        nextIsL = currentIsL;
    }

    for (unsigned long i = 0; i < length; ++i) {
        printf("SA[%lu] = %lu\n", i, SA[i]);
        printf("input[%lu] = %c\n", i, input[SA[i]]);
    }

    // TODO
    //Then, we sort SA[0 ... n − 1] (the sorting key of SA[i] is T [SA[i]] i.e., the ﬁrst character of suf(SA[i]))
    // using the mergesort, with the merging step implemented by the stable, in-place, linear time merging algorithm
    unsigned long step = 2;
    while (step < length) {
        for (unsigned long i = 0; i < length; i += step) {
            unsigned long mid = std::min(i + step / 2, length);
            unsigned long end = std::min(i + step, length);
            if (mid > end) mid = end; // ensure mid is not past end
            if (i < mid && mid < end) { // only merge if there is something to merge
                std::inplace_merge(SA + i, SA + mid, SA + end, 
                    [input](const unsigned long a, const unsigned long b) {
                        return input[a] < input[b];
                    });
            }
        }
        step *= 2;
    }
}

// section 5.5 - step 1
void initializeSA(const char * const input, unsigned long *SA, const unsigned long length) {
    printf("here with l %lu\n",length);

    // we scan T from right to left
    bool nextIsL = true;
    for (unsigned long i = 0; i < length-1; ++i) {
        printf("i = %lu, nextIsL = %d\n", i, nextIsL);
        const unsigned long index = length - i - 2;
        const bool currentIsL = input[index] > input[index+1] || (nextIsL && input[index] == input[index+1]);
        
        // for each scanning character T[i] which is L_TYPE, if bucket T[i] has not been initialized yet, we initialize it
        if (currentIsL) {
            
            //Let l denote the head of bucket T [i ] in SA (i.e. l is the smallest index in SA such that T [SA[l]] = T [i])
            // We can ﬁnd l by searching T [i ] in SA (the search key for SA[i ] is T [SA[i ]]) using binary search.

            const unsigned long l = binary_search(input, SA, length, input[index]);

            if (SA[l+1] == BH || SA[l+1] == BT) {
                // if the bucket is already initialized, we skip it
                continue;
            }

            // we let rL denote the tail of L-suﬃxes in this bucket (i.e., rL is the largest index in SA such
            // that T[SA[rL]] = T[i] and T[SA[rL]] is L-type).
            unsigned long rL = l;
            while (SA[rL] != index) {
                ++rL;
            }
            
            
            // Note that nL = rL - l + 1. Hence, it suﬃces to compute l and rL.

            const unsigned long nL = rL - l + 1;
            if (nL == 2) {
                SA[l+1] = BT;
            }
            else if (nL == 3) {
                SA[l+1] = BH;
                SA[l+2] = BT;
            }
            else if (nL > 3) {
                SA[l + 1] = BH;
                SA[l + 2] = E;
                SA[l + nL - 1] = BT;
            }
        }
        nextIsL = currentIsL;
    }
}

// section 5.5 - step 2
void sortL(const char * const input, unsigned long *SA, const unsigned long length) {
    for (unsigned long l = 0; l < length; ++l) {
        const unsigned long j = SA[l] - 1;

        // skip S type suffixes, this does not cover case 4 (but is always true for case 4)
        if (input[j] < input[j+1] || (input[j] == input[j+1] && (SA[l+2] == E || SA[l+2] == 0)))
        {
            continue;
        }

        if (SA[l+1] == BH) {
            if (SA[l+2] == E) {
                SA[l] = j;
                SA[l+2] = 1; // counter for the number of L-type suffixes put so far
            }
            else {
                const unsigned long c = SA[l+2];
                if (SA[l+c+2] == j != BT) {
                    SA[l + c + 2] = j;
                    SA[l + 2] = c + 1;
                }
                else {
                    const unsigned long rL = l + 3 + c;
                    std::move(SA + l + 3, SA + rL - 1, SA + l + 2);
                    SA[rL - 1] = j;
                    SA[l + 1] = R2;
                }
            }
        }
        else if (SA[l+1] == R2) {
            unsigned long rL = l + 2;
            while (SA[rL] != BT) {
                ++rL;
            }
            std::move(SA + l + 2, SA + rL - 1, SA + l + 1);
            SA[rL - 1] = j;
            SA[rL] = R1;
        }
        else {
            unsigned long rL = l + 2;

            // this check should be O(nL), how?
            // once R1 is implemented for good it could be better than this, maybe even O(1):
            // I'd like the special symbols to be implemented as five variables which value is
            // the index of the (only) occurrence of the symbol in SA. Which is MAYBE better than
            // what is done in the paper.
            while (rL < length && SA[rL] != R1) {
                ++rL;
            }
            if (rL != length) {
                SA[rL] = j;
            }
        }
    }
}

void optimalSuffixArray(const char * const input, unsigned long *SA, const unsigned long length) {
    printf("Optimal Suffix Array for input: %s\n", input);
    // TODO assumes nS < nL.
    // step 1
    const unsigned long nS = placeIndicesOfS_Type(input, length, SA);
    printf("nS = %lu\n", nS);
    
    // step 2
    mergeSortS_Substrings(input, length, SA, nS);
    printf("Sorted S-substrings: ");
    for (unsigned long i = 0; i < length; ++i) {
        printf("%lu ", SA[i]);
    }
    printf("\n");

    // step 3
    constructReducedProblem(input, length, SA, nS);
    printf("Reduced problem: ");
    for (unsigned long i = 0; i < length; ++i) {
        printf("%lu ", SA[i]);
    }
    printf("\n");


    // step 4
    heapSortReducedProblem(SA, length, nS);
    printf("Sorted reduced problem: ");
    for (unsigned long i = 0; i < length; ++i) {
        printf("%lu ", SA[i]);
    }
    printf("\n");


    // step 5
    RestoreFromRecursion(input, length, SA, nS);
    printf("Restored SA: ");
    for (unsigned long i = 0; i < length; ++i) {
        printf("%lu ", SA[i]);
    }
    printf("\n");

    // step 6
    preprocess(input, SA, length, nS);
    printf("Preprocessed SA: ");
    for (unsigned long i = 0; i < length; ++i) {
        printf("%lu ", SA[i]);
    }
    printf("\n");


    // step 7
    initializeSA(input, SA, length);
    printf("Initialized SA: ");
    for (unsigned long i = 0; i < length; ++i) {
        printf("%lu ", SA[i]);
    }
    printf("\n");


    // step 8
    sortL(input, SA, length);
    printf("Sorted L-type suffixes.");
}