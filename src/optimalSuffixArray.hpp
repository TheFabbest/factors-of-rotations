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

/* obtains the last index of substrings from observation 2 from the paper:
    For any index i of T, let j ∈[i +1, n −1] be the smallest index such that T[j] < T[j +1] (So T[j] is S-type). 
    Furthermore let k ∈[i + 1, j] be the smallest index such that T[l] = T[j] for any k ≤ l ≤ j. Then T[k] is the first S-type 
    character after index i. Moreover, all characters between T[i] and T[k] are L-type, and characters between T[k] and T[j] are 
    S-type.
*/
unsigned long getS_SubstringEnd(const unsigned long * const input, const unsigned long start, const unsigned long length) {
    unsigned long j = start+1;

    // find j
    while (j < length-1 && input[j] >= input[j + 1]) {
        ++j;
    }
    if (j == length-1) return length; // sentinel

    // find k
    unsigned long k = j;

    while (k > start && input[k] == input[j]) {
        --k;
    }
    ++k;
    return k;
}

// assumed
unsigned long getL_SubstringEnd(const unsigned long * const input, const unsigned long start, const unsigned long length) {
    unsigned long j = start+1;

    // find j
    while (j < length-1 && input[j] <= input[j + 1]) {
        ++j;
    }

    // find k
    unsigned long k = j-1;

    while (k > start && input[k] == input[j]) {
        --k;
    }

    ++k;
    return k;
}


// una stringa qualsiasi e piu grande di ogni suo prefisso (si appende carattere speciale)
int compare_substrings(const unsigned long * const input, unsigned long offsetA, unsigned long offsetB, const unsigned long length, const bool usingLType) {
    unsigned long endA, endB;
    if (usingLType) {
        // TODO see, assume the last L-substring only contains one character, which is the last one.
        endA = getL_SubstringEnd(input, offsetA, length);
        endB = getL_SubstringEnd(input, offsetB, length);
    } else {
        endA = getS_SubstringEnd(input, offsetA, length);
        endB = getS_SubstringEnd(input, offsetB, length);
    }

    while (offsetA < endA && offsetB < endB) {
        if (input[offsetA] != input[offsetB]) {
            if (input[offsetA] < input[offsetB]) {
                return -1;
            }
            else {
                return 1;
            }
        }
        ++offsetA;
        ++offsetB;
    }
    
    if (offsetA == length && offsetB == length) return 0;
    else if (offsetA == length) return -1;
    else if (offsetB == length) return 1;
    else {
        if (input[offsetA] < input[offsetB]) {
            return -1;
        }
        else if (input[offsetA] > input[offsetB]){
            return 1;
        }
        else {
            return 0;
        }
    }
}

void Heapify(unsigned long *SA, unsigned long *array, unsigned long length, unsigned long index) {
    while (true) {
        unsigned long biggest = index;
        const unsigned long left = 2 * index + 1;
        const unsigned long right = 2 * index + 2;
        if (left < length && array[left] > array[biggest]) {
            biggest = left;
        }
        if (right < length && array[right] > array[biggest]) {
            biggest = right;
        }
        if (biggest != index) {
            std::swap(array[index], array[biggest]);
            std::swap(SA[index], SA[biggest]);
            index = biggest;
        } else {
            break; // heap property is satisfied
        }
    }
}

void BuildHeap(unsigned long *SA, const unsigned long length, const unsigned long nS) {
    for (unsigned long i = nS/2; i > 0; --i) {
        const unsigned long index = i - 1;
        Heapify(SA, SA+length-nS, nS, index);
    }
}

// modified binary search that returns the first occurrence of the value
unsigned long custom_binary_search(const unsigned long * const input, const unsigned long *array, const unsigned long length, const unsigned long value, const bool usingLType) {
    unsigned long left = 0;
    unsigned long right = length - 1;

    auto is_valid = [array, length, usingLType](unsigned long index) {return array[index] < length && (usingLType || index == 0 || array[index-1] != BH) && (!usingLType || index == length-1 || array[index+1] != BH);};
    
    while (right > left)
    {
        const unsigned long mid = left + ((right - left) / 2);
        unsigned long checking = mid;
        while (checking > left && !is_valid(checking)) {
            --checking;
        }
        const unsigned long current = input[array[checking]];
        if (is_valid(checking) && current >= value)
        {
            right = checking;
        }
        else {
            left = mid+1;
        }
    }
    if (input[array[left]] != value) return length;
    return left;
}

// TODO only used with usingLType = true.
unsigned long custom_binary_search_last(const unsigned long * const input, const unsigned long *array, const unsigned long length, const unsigned long value, const bool usingLType) {
    unsigned long left = 0;
    unsigned long right = length - 1;

    auto is_valid = [array, length, usingLType](unsigned long index) {return array[index] < length && (usingLType || index == 0 || array[index-1] != BH) && (!usingLType || index == length-1 || array[index+1] != BH);};
    
    while (right > left)
    {
        const unsigned long mid = left + (right - left) / 2 + ((right-left)%2);
        unsigned long checking = mid;
        while (checking < right && !is_valid(checking)) {
            ++checking;
        }
        cout << left << " to " << right << "; mid = " << mid << "; checking = " << checking << endl;
        const unsigned long current = input[array[checking]];
        if (is_valid(checking) && current <= value)
        {
            left = checking;
        }
        else {
            right = mid-1;
        }
    }
    if (input[array[right]] != value) return length;
    return right;
}
// end of auxiliary functions -----

void optimalSuffixArray(const unsigned long * const input, unsigned long *SA, const unsigned long length);

unsigned long countS_Type(const unsigned long * const input, const unsigned long length) {
    bool nextIsL = true;
    unsigned long count = 0;
    unsigned long last_inserted_index = length;
    for (unsigned long i = 0; i < length-1; ++i) {
        const unsigned long index = length - i - 2;
        const bool currentIsL = input[index] > input[index+1] || (nextIsL && input[index] == input[index+1]);
        if (!currentIsL) {
            ++count;
        }
        nextIsL = currentIsL;
    }
    return count;
}

// section 5.2 - step 1
// find S suffixes and put indexes in SA[length-nS, length-1]
// returns the number of S-type suffixes found (nS)
// correct for test AABBABAB
// updated to work on L-type suffixes as well
void placeIndicesOf_Type(const unsigned long * const input, const unsigned long length, unsigned long *SA, const bool usingLType) {
    bool nextIsL = false; // the sentinel at the end is S_TYPE
    unsigned long last_inserted_index = length;
    for (unsigned long i = 0; i < length; ++i) {
        const unsigned long index = length - i - 1;
        const bool currentIsL = (index == length-1) || (input[index] > input[index+1] || (nextIsL && input[index] == input[index+1]));
        if ((currentIsL && usingLType) || (!currentIsL && !usingLType)) {
            if (!usingLType) cout << "Inserting S-type suffix of index " << index << " in " << last_inserted_index-1 << endl;
            else cout << "Inserting L-type suffix of index " << index << " in " << last_inserted_index-1 << endl;
            SA[--last_inserted_index] = index;
        }
        nextIsL = currentIsL;
    }
}

// section 5.2 - step 2
// sorts the S-type suffixes in SA[length-nS, length-1] using merge sort
// the comparison function compares suffixes by their lexicographic order
// according to this ordering, a word is always bigger than its prefixes
// correct for test AABBABAB
// updated to work on L-type suffixes as well
void mergeSort_Substrings(const unsigned long * const input, const unsigned long length, unsigned long *SA, const unsigned long nS, const bool usingLType) {
    // merge sort SA[nS, length-1] using SA[0, nS-1] as the auxiliary array, thus O(1) space
    stable_sort(SA+length-nS, SA+length, [input, length, nS, usingLType](const unsigned long &a, const unsigned long &b) {
                           return compare_substrings(input, a, b, length, usingLType) < 0;
                       });
    return;
    unsigned long *auxiliary = SA;
    unsigned long *array = SA + length - nS;

    unsigned long step = 2;
    while (step < nS) {
        for (unsigned long i = 0; i < nS; i += step) {
            unsigned long mid = i + step / 2;
            unsigned long end = std::min(i + step, nS);
            cout << "mid: " << mid << " end: " << end << endl;
            std::merge(array + i, array + mid, array + mid, array + end, auxiliary, 
                       [input, length, usingLType](const unsigned long &a, const unsigned long &b) {
                           return compare_substrings(input, a, b, length, usingLType);
                       });
            std::copy(auxiliary, auxiliary + (end - i), array + i);
        }
        step *= 2;
    }
    unsigned long mid = step / 2;
    unsigned long end = nS;
    cout << "mid: " << mid << " end: " << end << endl;
    merge(array, array + mid, array + mid, array + end, auxiliary, 
                    [input, length, usingLType](const unsigned long &a, const unsigned long &b) {
                        return compare_substrings(input, a, b, length, usingLType);
                    });
    copy(auxiliary, auxiliary + end, array);
}

// section 5.3 - step 1
// this renames the s-substrings by their ranks.
// since the s-substrings are sorted, if they are equal, they have the same rank,
//  otherwise, the one on the right comes immediately after (rank_2 = rank_1 + 1).
// maybe this could be done together with the merge sort?
// note: could process the input from the end, starting with the highest representable character.
// works for test AABBABAB
// updated to work on L-type suffixes as well
void constructReducedProblem(const unsigned long * const input, const unsigned long length, unsigned long *SA, const unsigned long nS, const bool usingLType) {
    unsigned long currentChar = 0;
    // if (usingLType) {
    //     currentChar = -6UL;
    // }
    SA[0] = currentChar;

    for (unsigned long i = length - nS + 1; i < length; ++i) {
        if (compare_substrings(input, SA[i-1], SA[i], length, usingLType) != 0) {
            // if (!usingLType) {
            //     ++currentChar;
            // }
            // else { // TODO see, made sense to me
            //     --currentChar;
            // }
            ++currentChar;
        }
        SA[i - length + nS] = currentChar;
    }
}


// section 5.3 - step 2
// sorts the reduced problem using heap sort
// the reduced problem is stored in SA[length-nS, length-1]
// when swapping elements, we also swap the corresponding indices in SA[0, nS-1]
// correct for test AABBABAB
void heapSortReducedProblem(unsigned long *SA, const unsigned long length, const unsigned long nS) {
    BuildHeap(SA, length, nS);
    unsigned long *array = SA + length - nS;
    for (unsigned long i = nS - 1; i > 0; --i) {
        std::swap(array[0], array[i]);
        std::swap(SA[0], SA[i]);
        Heapify(SA, array, i, 0);
    }
}

// section 5.4
void RestoreFromRecursion(const unsigned long * const input, const unsigned long length, unsigned long *SA, const unsigned long nS, const bool usingLType) {
    // recursion (TODO in O(1) space)
    optimalSuffixArray(SA, SA+length-nS, nS);

    // restore
    bool nextIsL = false;
    unsigned long sum = 0;
    for (unsigned long i = 0; i < length; ++i) {
        const unsigned long index = length - i - 1;
        const bool currentIsL = (index == length-1) || (input[index] > input[index+1] || (nextIsL && input[index] == input[index+1]));
        if ((!currentIsL && !usingLType) || (currentIsL && usingLType)) {
            ++sum;
            SA[nS - sum] = index;
        }
        nextIsL = currentIsL;
    }

    for (unsigned long i = length-nS; i < length; ++i) {
        SA[i] = SA[SA[i]];
    }
}

// section 5.5 - preprocessing
// at this point, the S-suffixes are already relatively sorted in SA[length-nS, length-1]
// we are now putting the L-suffixes in SA[0, length-nS-1]
// then we sort the whole SA using mergesort, sorting key for SA[i] is T[SA[i]]
void preprocess(const unsigned long * const input, unsigned long *SA, const unsigned long length, const unsigned long nS, const bool usingLType) {
    bool nextIsL = false;
    unsigned long last_inserted_index = length-nS;
    for (unsigned long i = 0; i < length; ++i) {
        const unsigned long index = length - i - 1;
        const bool currentIsL = (index == length-1) || (input[index] > input[index+1] || (nextIsL && input[index] == input[index+1]));

        if ((currentIsL && !usingLType) || (!currentIsL && usingLType)) {
            SA[--last_inserted_index] = index;
        }
        nextIsL = currentIsL;
    }

    // TODO
    //Then, we sort SA[0 ... n − 1] (the sorting key of SA[i] is T [SA[i]] i.e., the ﬁrst character of suf(SA[i]))
    // using the mergesort, with the merging step implemented by the stable, in-place, linear time merging algorithm
    
    // implemented like this for testing purposes, but should be implemented as commented below
    stable_sort(SA, SA + length,
        [input](const unsigned long a, const unsigned long b) {
            return input[a] < input[b];
        });

    /* incorrect, fix
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
    */
}

// section 5.5 - step 1
// this initializes the suffix array with special symbols that inform us on the number
// of L-type suffixes in each bucket.
void initializeSA(const unsigned long * const input, unsigned long *SA, const unsigned long length, const bool usingLType) {
    cout << "initializing SA" << endl;
    for (unsigned long i = 0; i < length; ++i) {
        cout << SA[i] << " " ;
    }
    cout << endl;
    for (unsigned long i = 0; i < length; ++i) {
        cout << input[i] << " ";
    }
    cout << endl;
    // we scan T from right to left
    bool nextIsL = false;
    unsigned long met = 0;
    for (unsigned long i = 0; i < length; ++i) {
        unsigned long index = length - i - 1;
        cout << "index: " << index << endl;
        const bool currentIsL = (index == length-1) || (input[index] > input[index+1] || (nextIsL && input[index] == input[index+1]));
        // for each scanning character T[i] which is L_TYPE, if bucket T[i] has not been initialized yet, we initialize it
        
        if ( (currentIsL && !usingLType) || (!currentIsL && usingLType) ) {
            //Let l denote the head of bucket T [i ] in SA (i.e. l is the smallest index in SA such that T [SA[l]] = T [i])
            // We can find l by searching T [i ] in SA (the search key for SA[i ] is T [SA[i ]]) using binary search.
            unsigned long l;
            unsigned long first = custom_binary_search(input, SA, length, input[index], usingLType);
            if (usingLType) {
                l = custom_binary_search_last(input, SA, length, input[index], true);
            } 
            else {
                l = first;
            }
            cout << "the smallest/largest (usingS/usingL) index l in SA such that T[SA[l]] = T[index] is " << l << endl;
            if (usingLType && l == 0 || (!usingLType && l == length-1))
            {
                cout << "skipping because obviously this is the only suffix in this bucket and this is seen as an edge case." << endl;
                continue;
            }
            const unsigned long position = usingLType ? l-1 : l+1;
            const unsigned long further_position = usingLType ? l-2 : l+2;
            if (SA[position] >= length) {
                // if the bucket is already initialized, we skip it
                cout << "bucket initialized, skipping" << endl;
                continue;
            }
            
            // we let rL denote the tail of L-suffixes in this bucket (i.e., rL is the largest index in SA such
            // that T[SA[rL]] = T[i] and T[SA[rL]] is L-type).
            unsigned long rL = l;
            
            cout << "looking for rL, index=" << index << endl;
            while (rL < length && SA[rL] != index) {
                cout << "SA[" << rL << "] = " << SA[rL] << endl;
                if (usingLType) --rL;
                else ++rL;
            }
            cout << "RL: " << rL;

            // Note that nL = rL - l + 1. Hence, it suffices to compute l and rL.
            const unsigned long nL = ((usingLType) ? rL - first + 1 : rL-l+1);

            if (usingLType) {
                // s-types (left) and l-types (right) must be swapped
                rotate(SA+first, SA+first+nL, SA+l+1);
            }

            cout << "initializing bucket T[" << index << "] with nL = " << nL << endl;
            if (nL == 2) {
                SA[position] = BT;
            }
            else if (nL == 3) {
                SA[position] = BH;
                SA[further_position] = BT;
            }
            else if (nL > 3) {
                SA[position] = BH;
                SA[further_position] = E;
                if (usingLType) {
                    SA[l - nL + 1] = BT; // TODO check
                } 
                else {
                    SA[l + nL - 1] = BT;
                }
            }
        }
        nextIsL = currentIsL;
    }
}

void case4(const unsigned long * const input, unsigned long *SA, unsigned long l, unsigned long j, unsigned long length) {
    unsigned long rL = l+1;
    cout << "looking for R1, beginning at " << rL << endl;
    while (rL < length && SA[rL] != R1 && (SA[rL] < length && input[SA[rL]] == input[j])) {
        ++rL;
        cout << "at " << rL << endl;
    }
    cout << "out with rL = " << rL;
    if (rL != length && (SA[rL] == R1 || (SA[rL] < length && input[SA[rL]] == input[j]))) {
        SA[rL] = j;
        printf("case 4 (L), initialized SA[%lu] = %lu\n", rL, j);
    }
    else {
        printf("case 4 (L), no R1 found, skipping (either case nL=1 or current was not L-type).\n");
    }
}

void case4_S(const unsigned long * const input, unsigned long *SA, unsigned long l, unsigned long j, unsigned long length) {
    unsigned long rL = l+1;
    cout << "j was " << j << endl;
    cout << "current rL = " << rL << endl;
    while (rL > 0 && SA[rL-1] != R1 && input[SA[rL-1]] == input[j]) {
        cout << SA[rL-1] << " is not R1, going down" << endl;
        --rL;
    }
    cout << "rL is " << rL << endl;
    if (rL != 0 && SA[rL-1] == R1) {
        SA[rL-1] = j;
        printf("case 4 (S), initialized SA[%lu] = %lu\n", rL, j);
    }
    else {
        printf("case 4 (S), no R1 found, skipping (either case nS=1 or current was not S-type).\n");
    }
}

void sortS_body(const unsigned long * const input, unsigned long *SA, const unsigned long length, unsigned long j){
    unsigned long l = custom_binary_search_last(input, SA, length, input[j], true); // modified to look for the last occurrence
    printf("l = %lu, j = %lu\n", l, j);
    
    if (l == 0) {
        cout << "skipping as edge case (clearly the only suffix in the bucket)" << endl;
        return;
    }
    
    // skip L type suffixes, this does not cover case 4 (but is always true for case 4)
    // TODO see this condition
    
    if (j == length-1 || input[j] > input[j+1]) {
        cout << "Skipping suffix with j=" << j << " because it is not S-type." << endl;
        return;
    }
    else if (input[j] == input[j+1]) {
        if (! (SA[l-1] == BH || SA[l-1] == R2 || SA[l-1] == R1) ) {
            cout << "Skipping suffix with j=" << j << " because it is not S-type (I have already filled this bucket)." << endl;
            if (SA[l-1] != BT && SA[l-1] != R1 && SA[l-1] != BH && SA[l-1] != R2) case4_S(input, SA, l, j, length);
            return;
        }
    }
    const unsigned long prev = SA[l-1]; // was l+1 for sortL_body
    
    // nL = 2?
    if (prev == BT) {
        printf("nL = 2, first to put in.\n");
        SA[l] = j;
        SA[l-1] = R1; // not in the paper, but how I implemented it
    }
    else if (prev == R1) {
        SA[l-1] = j;
    }
    else if (prev == BH) {
        if (SA[l-2] == E) {
            SA[l] = j;
            SA[l-2] = 1; // counter for the number of L-type suffixes put so far
            
            printf("case 1, initialized SA[%lu] = %lu, SA[%lu] = 1\n", l, j, l - 2);
        }
        else if (SA[l-2] == BT) { // not in the paper, nL=3, first to put in
            SA[l] = j;
            SA[l - 2] = R2; // not in the paper, but how I implemented it
            printf("nL = 3, first to put in, initialized SA[%lu] = %lu, SA[%lu] = R2\n", l, j, l - 2);
        }
        else if (SA[l-2] == R2) { // not in the paper, nL=3, second to put in
            SA[l - 1] = j;
            SA[l - 2] = R1; // not in the paper, but how I implemented it
            printf("nL = 3, second to put in, initialized SA[%lu] = %lu\n", l - 1, j);
        }
        else if (SA[l-2] == R1) {
            SA[l-2] = j;
            printf("nL = 3, third to put in, initialized SA[%lu] = %lu\n", l - 2, j);
        }
        else {
            const unsigned long c = SA[l-2];
            if (SA[l-c-2] != BT) {
                SA[l - c - 2] = j;
                SA[l - 2] = c + 1;
                printf("case 2 (1), initialized SA[%lu] = %lu, SA[%lu] = %lu\n", l - c - 2, j, l - 2, c + 1);
            }
            else {
                const unsigned long rL = l - 2 - c; // TODO see
                std::move(SA + rL + 1, SA + l - 2, SA + rL + 2); // see
                SA[rL + 1] = j;
                SA[l - 1] = R2;
                printf("case 2 (2), moved [SA[%lu], SA[%lu]) to SA[%lu], initialized SA[%lu] = %lu, SA[%lu] = R2\n", 
                    rL + 1, l - 2, rL + 2, rL + 1, j, l - 1);
            }
        }
    }
    else if (prev == R2) {
        unsigned long rL = l - 2;
        while (SA[rL] != BT) {
            --rL;
        }
        cout << "rL = " << rL << endl;
        std::move(SA + rL + 1, SA + l - 1, SA + rL + 2);
        SA[rL + 1] = j;
        SA[rL] = R1;
        printf("case 3, moved SA[%lu] to SA[%lu], initialized SA[%lu] = %lu, SA[%lu] = R1\n", 
            rL + 1, rL + 2, rL + 1, j, rL);
    }
    else {
        case4_S(input, SA, l, j, length);
    }
}


void sortL_body(const unsigned long * const input, unsigned long *SA, const unsigned long length, unsigned long j){
    unsigned long l = custom_binary_search(input, SA, length, input[j], false); // TODO check if input[i] or input[j]
    printf("l = %lu, j = %lu\n", l, j);
    if (l == length-1) {
        cout << "skipping as edge case, clearly the only suffix in the bucket" << endl;
    }
    
    // skip S type suffixes, this does not cover case 4 (but is always true for case 4)
    // TODO see this condition
    if (j != length-1 && input[j] < input[j+1]) {
        cout << "Skipping suffix with j=" << j << " because it is not L-type." << endl;
        return;
    }
    else if (j != length-1 && input[j] == input[j+1]) {
        if (! (SA[l+1] == BH || SA[l+1] == R2 || SA[l+1] == R1) ) {
            if (SA[l+1] != BT && SA[l+1] != R1 && SA[l+1] != BH && SA[l+1] != R2) 
            {
                case4(input, SA, l, j, length);
            }
            else {
                cout << "Skipping suffix with j=" << j << " because it is not L-type (I have already filled this bucket)." << endl;
            }
            return;
        }
    }
    
    // nL = 2?
    if (SA[l+1] == BT) {
        printf("nL = 2, first to put in.\n");
        SA[l] = j;
        SA[l+1] = R1; // not in the paper, but how I implemented it
    }
    else if (SA[l+1] == R1) {
        SA[l+1] = j;
    }
    else if (SA[l+1] == BH) {
        if (SA[l+2] == E) {
            SA[l] = j;
            SA[l+2] = 1; // counter for the number of L-type suffixes put so far
            
            printf("case 1, initialized SA[%lu] = %lu, SA[%lu] = 1\n", l, j, l + 2);
        }
        else if (SA[l+2] == BT) { // not in the paper, nL=3, first to put in
            SA[l] = j;
            SA[l + 2] = R2; // not in the paper, but how I implemented it
            printf("nL = 3, first to put in, initialized SA[%lu] = %lu, SA[%lu] = R2\n", l, j, l + 2);
        }
        else if (SA[l+2] == R2) { // not in the paper, nL=3, second to put in
            SA[l + 1] = j;
            SA[l + 2] = R1; // not in the paper, but how I implemented it
            printf("nL = 3, second to put in, initialized SA[%lu] = %lu\n", l + 1, j);
        }
        else if (SA[l+2] == R1) {
            SA[l+2] = j;
            printf("nL = 3, third to put in, initialized SA[%lu] = %lu\n", l + 2, j);
        }
        else {
            const unsigned long c = SA[l+2];
            if (SA[l+c+2] != BT) {
                SA[l + c + 2] = j;
                SA[l + 2] = c + 1;
                printf("case 2 (1), initialized SA[%lu] = %lu, SA[%lu] = %lu\n", l + c + 2, j, l + 2, c + 1);
            }
            else {
                const unsigned long rL = l + 2 + c;
                std::move(SA + l + 3, SA + rL, SA + l + 2);
                SA[rL - 1] = j;
                SA[l + 1] = R2;
                printf("case 2 (2), moved SA[%lu] to SA[%lu], initialized SA[%lu] = %lu, SA[%lu] = R2\n", 
                    l + 3, l + 2, rL - 1, j, l + 1);
            }
        }
    }
    else if (SA[l+1] == R2) {
        unsigned long rL = l + 2;
        while (rL != length-1 && SA[rL] != BT) {
            ++rL;
        }
        std::move(SA + l + 2, SA + rL, SA + l + 1);
        SA[rL - 1] = j;
        SA[rL] = R1;
        printf("case 3, moved SA[%lu] to SA[%lu], initialized SA[%lu] = %lu, SA[%lu] = %lu\n", 
            l + 2, l + 1, rL - 1, j, rL, R1);
    }
    else {
        case4(input, SA, l, j, length);
    }
}

void sortS(const unsigned long * const input, unsigned long *SA, const unsigned long length) {
    for (unsigned long iter = 1; iter <= length; ++iter) {
        const unsigned long i = length - iter;
        if (SA[i] == 0) continue;

        cout << "i= " << i<<endl;

        // does this ever happen? Should this happen?
        if (SA[i] >= length) {
            if (SA[i] == BH) ++iter; //skips counter
            cout << "skipping index " << i << " because SA[i] is a special symbol." << endl;
            cin.get();
            continue;
        }

        unsigned long j = SA[i]-1;
        sortS_body(input, SA, length, j);

        // does this ever happen? Should this happen?
        while (SA[i]-1 != j && SA[i] < length && SA[i] != 0) // changed current, repeat.
        {
            cout << "REPEATING" << endl;
            cin.get();
            j = SA[i]-1;
            sortS_body(input, SA, length, j);
        }

        for (unsigned long m = 0; m < length; ++m){
            cout << SA[m] << " ";
        }
        cout << endl;
    }
}


void sortL(const unsigned long * const input, unsigned long *SA, const unsigned long length) {
    sortL_body(input, SA, length, length-1);
    
    for (unsigned long i = 0; i < length; ++i) {
        if (SA[i] == 0) continue;

        cout << "i= " << i<<endl;
        cout << "input[i] = " << input[i] << endl;

        // does this ever happen? Should this happen?
        if (SA[i] >= length) {
            if (SA[i] == BH) ++i; //skips counter
            cout << "skipping index " << i << " because SA[i] is a special symbol." << endl;
            cin.get();
            continue;
        }

        // TODO all this method is wrong because the whole algorithm is meant to use the character at the end,
        // first thing, j should not be SA[i]-1 but SA[i].
        // second thing, the first j is always length-1 (because SA[0] is the character at the end, so length).
        unsigned long j = SA[i]-1; // TODO see SA[i] = 0 means the sentinel, so we take the last index
        sortL_body(input, SA, length, j);

        // does this ever happen? Should this happen?
        while (SA[i]-1 != j && SA[i] < length && SA[i] != 0) // changed current, repeat.
        {
            cout << "REPEATING" << endl;
            cin.get();
            j = SA[i]-1;
            sortL_body(input, SA, length, j);
        }
    }
}

// section 5.5 - step 2
// TODO for usingLType=true, go from right to left and keep the "rightmost free" instead of leftmost free
void inducedSorting(const unsigned long * const input, unsigned long *SA, const unsigned long length, const bool usingLType) {
    printf("Sorting suffixes of the other type, length = %lu\n", length);
    printf("BT = %lu, BH = %lu, E = %lu, R1 = %lu, R2 = %lu\n", BT, BH, E, R1, R2);
    if (usingLType) sortS(input, SA, length);
    else sortL(input, SA, length);
}

void optimalSuffixArray(const unsigned long * const input, unsigned long *SA, const unsigned long length) {
    printf("Optimal Suffix Array for input of length: %lu\n", length);
    if (length < 2) {
        printf("base case.\n");
        if (length == 1) {
            SA[0] = 0;
        }
        return;
    }

    // check if nS <= nL
    unsigned long nS = countS_Type(input, length);
    if (nS == length) {
        for (unsigned long i = 0; i < length; ++i) {
            SA[i] = i;
        }
        printf("All suffixes are S-type, SA is initialized as identity.\n");
    }
    else if (nS == 0) {
        for (unsigned long i = 0; i < length; ++i) {
            SA[i] = length - i - 1;
        }
        printf("All suffixes are L-type, SA is reversed.\n");
        return;
    }
    
    const bool usingLType = (length-nS) < nS; // if there are more L-type suffixes, we swap the roles of S and L
    if (usingLType) {
        printf("Using L-type suffixes.\n");
        printf("behavior is the same as in the paper, but with L-type and S-type inverted.\n");
        nS = length - nS;
        printf("nL = %lu\n", nS);
    } else {
        printf("Using S-type suffixes.\n");
        printf("behavior is the same as in the paper.\n");
        printf("nS = %lu\n", nS);
    }

    // step 1
    placeIndicesOf_Type(input, length, SA, usingLType);

    // step 2
    mergeSort_Substrings(input, length, SA, nS, usingLType);
    if (usingLType) printf("Sorted L-substrings: ");
    else printf("Sorted S-substrings: ");
    for (unsigned long i = length-nS; i < length; ++i) {
        printf("%lu ", SA[i]);
    }
    printf("\n");

    // step 3
    constructReducedProblem(input, length, SA, nS, usingLType);
    printf("Reduced problem: ");
    for (unsigned long i = 0; i < nS; ++i) {
        printf("%lu ", SA[i]);
    }
    printf("\n");


    // step 4 (no need to adapt the heap sort for L-type suffixes, it works as is)
    heapSortReducedProblem(SA, length, nS);
    printf("SA after sorting reduced problem: ");
    for (unsigned long i = 0; i < length; ++i) {
        printf("%lu ", SA[i]);
    }
    printf("\n");


    // step 5
    RestoreFromRecursion(input, length, SA, nS, usingLType);
    printf("Restored SA: ");
    for (unsigned long i = 0; i < length; ++i) {
        printf("%lu ", SA[i]);
    }
    printf("\n");

    // step 6
    preprocess(input, SA, length, nS, usingLType);
    printf("Preprocessed SA: ");
    for (unsigned long i = 0; i < length; ++i) {
        printf("%lu ", SA[i]);
    }
    printf("\n");

    // step 7
    initializeSA(input, SA, length, usingLType);
    printf("Initialized SA: ");
    for (unsigned long i = 0; i < length; ++i) {
        printf("%lu ", SA[i]);
    }
    printf("\n");

    // step 8
    inducedSorting(input, SA, length, usingLType);
    printf("Sorted L-type suffixes.\n");
    for (unsigned long i = 0; i < length; ++i) {
        printf("%lu ", SA[i]);
    }

    printf("\n");
}