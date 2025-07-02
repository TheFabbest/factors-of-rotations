// Computation of the suffix array of read-only general alphabets is described in:
// "Optimal in-place suffix array construction"

#include <algorithm>
#include <iostream>
using namespace std;

#define BT -1UL
#define BH -2UL
#define E -3UL
#define R1 -4UL
#define R2 -5UL
#define MAX_CHAR -6UL


// ----- auxiliary functions

/* obtains the length of substrings from observation 2 from the paper:
    For any index i of T, let j ∈[i +1, n −1] be the smallest index such that T[j] < T[j +1] (So T[j] is S-type). 
    Furthermore let k ∈[i + 1, j] be the smallest index such that T[l] = T[j] for any k ≤ l ≤ j. Then T[k] is the first S-type 
    character after index i. Moreover, all characters between T[i] and T[k] are L-type, and characters between T[k] and T[j] are 
    S-type.
*/
unsigned long getS_SubstringEnd(const char * const input, const unsigned long start, const unsigned long length) {
    unsigned long j = start+1;

    // find j
    while (j < length-1 && input[j] >= input[j + 1]) {
        ++j;
    }

    // find k
    unsigned long k = j-1;

    while (k > start && input[k] == input[j]) {
        --k;
    }

    return k;
}

int compareS_substrings(const char * const input, unsigned long offsetA, unsigned long offsetB, const unsigned long length) {
    const unsigned long endA = getS_SubstringEnd(input, offsetA, length);
    const unsigned long endB = getS_SubstringEnd(input, offsetB, length);

    while (offsetA != endA && offsetB != endB) {
        if (input[offsetA] != input[offsetB]) {
            return input[offsetA] - input[offsetB];
        }
        ++offsetA;
        ++offsetB;
    }
    if (offsetA == endA && offsetB == endB) {
        return 0; // equal
    }
    else if (offsetA == endA) {
        return -1; // A is a prefix of B
    }
    else {
        return 1; // B is a prefix of A
    }
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

void BuildHeap(unsigned long *array, const unsigned long n){
    for (unsigned long i = n/2; i > 0; --i) {
        const unsigned long index = i - 1;
        Heapify(array, n, index);
    }
}

void HeapSort(unsigned long *array, const unsigned long n) {
    BuildHeap(array, n);
    for (unsigned long i = n - 1; i > 0; --i) {
        std::swap(array[0], array[i]);
        Heapify(array, i, 0);
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
// find S suffixes and put indexes in SA[length-nS, length-1]
// returns the number of S-type suffixes found (nS)
// correct for test AABBABAB
unsigned long placeIndicesOfS_Type(const char * const input, const unsigned long length, unsigned long *SA) {
    bool nextIsL = true; // last character is always L_TYPE because of the sentinel at the end (which is S_TYPE)
    unsigned long last_inserted_index = length;
    for (unsigned long i = 0; i < length-1; ++i) {
        const unsigned long index = length - i - 2;
        const bool currentIsL = input[index] > input[index+1] || (nextIsL && input[index] == input[index+1]);
        if (!currentIsL) {
            cout << "Inserting S-type suffix of index " << index << " in " << last_inserted_index-1 << endl;
            SA[--last_inserted_index] = index;
        }
        nextIsL = currentIsL;
    }

    return length-last_inserted_index;
}

// section 5.2 - step 2
// sorts the S-type suffixes in SA[length-nS, length-1] using merge sort
// the comparison function compares suffixes by their lexicographic order
// according to this ordering, a word is always bigger than its prefixes
// correct for test AABBABAB
void mergeSortS_Substrings(const char * const input, const unsigned long length, unsigned long *SA, const unsigned long nS) {
    // merge sort SA[nS, length-1] using SA[0, nS-1] as the auxiliary array, thus O(1) space
    unsigned long *auxiliary = SA;
    unsigned long *array = SA + length - nS;

    unsigned long step = 2;
    while (step <= nS) {
        for (unsigned long i = 0; i < nS; i += step) {
            unsigned long mid = i + step / 2;
            unsigned long end = std::min(i + step, nS);
            std::merge(array + i, array + mid, array + mid, array + end, auxiliary, 
                       [input, length](const unsigned long &a, const unsigned long &b) {
                           return compareS_substrings(input, a, b, length) < 0;
                       });
            std::copy(auxiliary, auxiliary + (end - i), array + i);
        }
        step *= 2;
    }
}

// section 5.3 - step 1
// this renames the s-substrings by their ranks.
// since the s-substrings are sorted, if they are equal, they have the same rank,
//  otherwise, the one on the right comes immediately after (rank_2 = rank_1 + 1).
// maybe this could be done together with the merge sort?
// note: could process the input from the end, starting with the highest representable character.
// works for test AABBABAB
void constructReducedProblem(const char * const input, const unsigned long length, unsigned long *SA, const unsigned long nS) {
    char currentChar = 0;
    SA[0] = currentChar;

    for (unsigned long i = length - nS + 1; i < length; ++i) {
        if (compareS_substrings(input, SA[i-1], SA[i], length) != 0) {
            ++currentChar;
        }
        SA[i - length + nS] = currentChar;
    }
}


// section 5.3 - step 2
void heapSortReducedProblem(unsigned long *SA, unsigned long length, const unsigned long nS) {
    HeapSort(SA + length - nS, nS);
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
        printf("l = %lu, j = %lu, SA[l] = %lu\n", l, j, SA[l]);
        // skip S type suffixes, this does not cover case 4 (but is always true for case 4)
        if (j != BT && j != BH && j != E && j != R1 && j != R2) {
            if (input[j] < input[j+1] || (input[j] == input[j+1] && (SA[l+2] == E || SA[l+2] == 0)))
            {
                cout << "Skipping L-type suffix at index " << l << " because it is not L-type." << endl;
                continue;
            }
        }

        if (SA[l+1] == BH) {
            if (SA[l+2] == E) {
                SA[l] = j;
                SA[l+2] = 1; // counter for the number of L-type suffixes put so far
                
                printf("case 1, initialized SA[%lu] = %lu, SA[%lu%] = 1\n", l, j, l + 2);
            }
            else {
                const unsigned long c = SA[l+2];
                if (SA[l+c+2] == j != BT) {
                    SA[l + c + 2] = j;
                    SA[l + 2] = c + 1;
                    printf("case 2 (1), initialized SA[%lu] = %lu, SA[%lu] = %lu\n", l + c + 2, j, l + 2, c + 1);
                }
                else {
                    const unsigned long rL = l + 3 + c;
                    std::move(SA + l + 3, SA + rL - 1, SA + l + 2);
                    SA[rL - 1] = j;
                    SA[l + 1] = R2;
                    printf("case 2 (2), moved SA[%lu] to SA[%lu], initialized SA[%lu] = %lu, SA[%lu] = %lu\n", 
                           l + 3, l + 2, rL - 1, j, l + 1, R2);
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
            printf("case 3, moved SA[%lu] to SA[%lu], initialized SA[%lu] = %lu, SA[%lu] = %lu\n", 
                   l + 2, l + 1, rL - 1, j, rL, R1);
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
                printf("case 4, initialized SA[%lu] = %lu\n", rL, j);
            }
            else {
                printf("case 4, no R1 found, skipping.\n");
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
    printf("Sorted L-type suffixes.\n");
}