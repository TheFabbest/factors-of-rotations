// Computation of the suffix array of read-only general alphabets is described in:
// "Optimal in-place suffix array construction"

#include <algorithm>
#include <iostream>
#include <limits>
using namespace std;

#define BT -1UL
#define BH -2UL
#define E -3UL
#define R1 -4UL
#define R2 -5UL
#define MAX_CHAR -6UL

constexpr unsigned long MSB = 1UL << (std::numeric_limits<unsigned long>::digits - 1);


// ----- auxiliary functions

inline void pause()
{
    // cin.get();
}

/* obtains the last index of substrings from observation 2 from the paper:
    For any index i of T, let j ∈[i +1, n −1] be the smallest index such that T[j] < T[j +1] (So T[j] is S-type).
    Furthermore let k ∈[i + 1, j] be the smallest index such that T[l] = T[j] for any k ≤ l ≤ j. Then T[k] is the first S-type
    character after index i. Moreover, all characters between T[i] and T[k] are L-type, and characters between T[k] and T[j] are
    S-type.

    also see https://pdf.sciencedirectassets.com/272975/1-s2.0-S1570866705X00090/1-s2.0-S1570866704000498/main.pdf
*/
unsigned long getS_SubstringEnd(const unsigned long *const input, const unsigned long start, const unsigned long length)
{
    unsigned long j = start + 1;

    // find j
    while (j < length - 1 && input[j] >= input[j + 1])
    {
        ++j;
    }
    if (j == length - 1)
        return length; // sentinel

    // find k
    unsigned long k = j;

    while (k > start && input[k] == input[j])
    {
        --k;
    }
    ++k;
    return k;
}

// assumed
unsigned long getL_SubstringEnd(const unsigned long *const input, const unsigned long start, const unsigned long length)
{
    unsigned long j = start + 1;

    // find j
    while (j < length - 1 && input[j] <= input[j + 1])
    {
        ++j;
    }

    // find k
    unsigned long k = j - 1;

    while (k > start && input[k] == input[j])
    {
        --k;
    }

    ++k;
    return k;
}

int compare_substrings(const unsigned long *const input, unsigned long offsetA, unsigned long offsetB, const unsigned long length, const bool usingLType)
{
    unsigned long endA, endB;
    if (usingLType)
    {
        endA = getL_SubstringEnd(input, offsetA, length);
        endB = getL_SubstringEnd(input, offsetB, length);
    }
    else
    {
        endA = getS_SubstringEnd(input, offsetA, length);
        endB = getS_SubstringEnd(input, offsetB, length);
    }


    while (offsetA < endA && offsetB < endB)
    {
        if (input[offsetA] != input[offsetB])
        {
            if (input[offsetA] < input[offsetB])
            {
                return -1;
            }
            else
            {
                return 1;
            }
        }
        ++offsetA;
        ++offsetB;
    }

    if (offsetA == length && offsetB == length)
    {
        return 0;
    }
    else if (offsetA == length) {
        return -1;
    }
    else if (offsetB == length) {
        return 1;
    }
    else {
        if (input[offsetA] < input[offsetB]) {
            return -1;
        }
        else if (input[offsetA] > input[offsetB]) {
            return 1;
        }
        else
        {
            if (offsetA == endA && offsetB == endB) {
                return 0;
            }
            else if (offsetA == endA) {
                if (usingLType) {
                    return -1;
                }
                else {
                    return 1;
                }
            }
            else {
                if (usingLType) {
                    return 1;
                }
                else {
                    return -1;
                }
            }
        }
    }
}

void Heapify(unsigned long *const SA, unsigned long *const array, const unsigned long length, unsigned long index)
{
    while (true)
    {
        unsigned long biggest = index;
        const unsigned long left = 2 * index + 1;
        const unsigned long right = 2 * index + 2;
        if (left < length && array[left] > array[biggest])
        {
            biggest = left;
        }
        if (right < length && array[right] > array[biggest])
        {
            biggest = right;
        }
        if (biggest != index)
        {
            swap(array[index], array[biggest]);
            swap(SA[index], SA[biggest]);
            index = biggest;
        }
        else
        {
            break; // heap property is satisfied
        }
    }
}

void BuildHeap(unsigned long *const SA, const unsigned long length, const unsigned long nS)
{
    for (unsigned long i = nS / 2; i > 0; --i)
    {
        const unsigned long index = i - 1;
        Heapify(SA, SA + length - nS, nS, index);
    }
}

// modified binary search that returns the first occurrence of the value
unsigned long custom_binary_search(const unsigned long *const input, const unsigned long * const array, const unsigned long length, const unsigned long value, const bool usingLType)
{
    unsigned long left = 0;
    unsigned long right = length - 1;

    auto is_valid = [array, length, usingLType](unsigned long index)
    { return array[index] < length && (usingLType || index == 0 || array[index - 1] != BH) && (!usingLType || index == length - 1 || array[index + 1] != BH); };

    while (right > left)
    {
        const unsigned long mid = left + ((right - left) / 2);
        unsigned long checking = mid;
        while (checking > left && !is_valid(checking))
        {
            --checking;
        }

        if (is_valid(checking) && input[array[checking]] >= value)
        {
            right = checking;
        }
        else
        {
            left = mid + 1;
        }
    }
    if (input[array[left]] != value)
        return length;
    return left;
}

unsigned long custom_binary_search_last(const unsigned long *const input, const unsigned long *const array, const unsigned long length, const unsigned long value)
{
    unsigned long left = 0;
    unsigned long right = length - 1;

    auto is_valid = [array, length](const unsigned long index)
    { return (array[index] < length) && (index == length - 1 || array[index + 1] != BH); };

    while (right > left)
    {
        const unsigned long mid = left + (right - left) / 2 + ((right - left) % 2);
        unsigned long checking = mid;
        while (checking < right && !is_valid(checking))
        {
            ++checking;
        }
        if (is_valid(checking) && input[array[checking]] <= value)
        {
            left = checking;
        }
        else
        {
            right = mid - 1;
        }
    }
    if (input[array[right]] != value)
        return length;
    return right;
}
// end of auxiliary functions -----

void optimalSuffixArray(const unsigned long *const input, unsigned long *const SA, const unsigned long length);

unsigned long countS_Type(const unsigned long *const input, const unsigned long length)
{
    bool nextIsL = true;
    unsigned long count = 0;
    unsigned long last_inserted_index = length;
    for (unsigned long i = 0; i < length - 1; ++i)
    {
        const unsigned long index = length - i - 2;
        const bool currentIsL = input[index] > input[index + 1] || (nextIsL && input[index] == input[index + 1]);
        if (!currentIsL)
        {
            ++count;
        }
        nextIsL = currentIsL;
    }
    return count;
}

// section 5.2 - step 1
// find S suffixes and put indexes in SA[length-nS, length-1]
// returns the number of S-type suffixes found (nS)
void placeIndicesOf_Type(const unsigned long *const input, const unsigned long length, unsigned long * const SA, const bool usingLType)
{
    bool nextIsL = false; // the sentinel at the end is S_TYPE
    unsigned long last_inserted_index = length;
    for (unsigned long i = 0; i < length; ++i)
    {
        const unsigned long index = length - i - 1;
        const bool currentIsL = (index == length - 1) || (input[index] > input[index + 1] || (nextIsL && input[index] == input[index + 1]));
        if ((currentIsL && usingLType) || (!currentIsL && !usingLType))
        {
            SA[--last_inserted_index] = index;
        }
        nextIsL = currentIsL;
    }
}

// section 5.2 - step 2
// sorts the S-type suffixes in SA[length-nS, length-1] using merge sort
// the comparison function compares suffixes by their lexicographic order
// according to this ordering, a word is always bigger than its prefixes
void mergeSort_Substrings(const unsigned long *const input, const unsigned long length, unsigned long *const SA, const unsigned long nS, const bool usingLType)
{
    // merge sort SA[nS, length-1] using SA[0, nS-1] as the auxiliary array, thus O(1) space

    // stable_sort(SA+length-nS, SA+length, [input, length, nS, usingLType](const unsigned long &a, const unsigned long &b) {
    //                        return compare_substrings(input, a, b, length, usingLType) < 0;
    //                    });

    unsigned long * const auxiliary = SA;
    unsigned long * const array = SA + length - nS;

    unsigned long step = 1;
    while (step < length)
    {
        for (unsigned long i = length - nS; i < length - step; i += step * 2)
        {
            const unsigned long mid = i + step;
            const unsigned long end = min(i + step * 2, length);
            merge(SA + i, SA + mid, SA + mid, SA + end, SA,
                  [input, length, usingLType](const unsigned long &a, const unsigned long &b)
                  {
                      return compare_substrings(input, a, b, length, usingLType) < 0;
                  });
            move(SA, SA + end - i, SA + i);
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
void constructReducedProblem(const unsigned long *const input, const unsigned long length, unsigned long * const SA, const unsigned long nS, const bool usingLType)
{
    unsigned long currentChar = 0;
    SA[0] = currentChar;

    for (unsigned long i = length - nS + 1; i < length; ++i)
    {
        if (compare_substrings(input, SA[i - 1], SA[i], length, usingLType) != 0)
        {
            ++currentChar;
        }
        SA[i - length + nS] = currentChar;
    }
}

// section 5.3 - step 2
// sorts the reduced problem using heap sort
// the reduced problem is stored in SA[length-nS, length-1]
// when swapping elements, we also swap the corresponding indices in SA[0, nS-1]
void heapSortReducedProblem(unsigned long *const SA, const unsigned long length, const unsigned long nS)
{
    BuildHeap(SA, length, nS);
    unsigned long * const array = SA + length - nS;
    for (unsigned long i = nS - 1; i > 0; --i)
    {
        std::swap(array[0], array[i]);
        std::swap(SA[0], SA[i]);
        Heapify(SA, array, i, 0);
    }
}

// section 5.4
void RestoreFromRecursion(const unsigned long *const input, const unsigned long length, unsigned long *const SA, const unsigned long nS, const bool usingLType)
{
    // restore
    bool nextIsL = false;
    unsigned long sum = 0;
    for (unsigned long i = 0; i < length; ++i)
    {
        const unsigned long index = length - i - 1;
        const bool currentIsL = (index == length - 1) || (input[index] > input[index + 1] || (nextIsL && input[index] == input[index + 1]));
        if ((!currentIsL && !usingLType) || (currentIsL && usingLType))
        {
            ++sum;
            SA[nS - sum] = index;
        }
        nextIsL = currentIsL;
    }

    for (unsigned long i = length - nS; i < length; ++i)
    {
        SA[i] = SA[SA[i]];
    }
}

// section 5.5 - preprocessing
// at this point, the S-suffixes are already relatively sorted in SA[length-nS, length-1]
// we are now putting the L-suffixes in SA[0, length-nS-1]
// then we sort the whole SA using mergesort, sorting key for SA[i] is T[SA[i]]
void preprocess(const unsigned long *const input, unsigned long * const SA, const unsigned long length, const unsigned long nS, const bool usingLType)
{
    bool nextIsL = false;
    unsigned long last_inserted_index = length - nS;
    for (unsigned long i = 0; i < length; ++i)
    {
        const unsigned long index = length - i - 1;
        const bool currentIsL = (index == length - 1) || (input[index] > input[index + 1] || (nextIsL && input[index] == input[index + 1]));

        if ((currentIsL && !usingLType) || (!currentIsL && usingLType))
        {
            SA[--last_inserted_index] = index;
        }
        nextIsL = currentIsL;
    }

    // Then, we sort SA[0 ... n − 1] (the sorting key of SA[i] is T [SA[i]] i.e., the ﬁrst character of suf(SA[i]))
    //  using the mergesort, with the merging step implemented by the stable, in-place, linear time merging algorithm

    // stable_sort(SA, SA + length,
    //     [input](const unsigned long a, const unsigned long b) {
    //         return input[a] < input[b];
    //     });

    unsigned long step = 1;
    while (step < length)
    {
        for (unsigned long i = 0; i < length - step; i += step * 2)
        {
            const unsigned long mid = i + step;
            const unsigned long end = min(i + step * 2, length);
            inplace_merge(SA + i, SA + mid, SA + end,
                          [input](const unsigned long a, const unsigned long b)
                          {
                              return input[a] < input[b];
                          });
        }
        step *= 2;
    }
}

// section 5.5 - step 1
// this initializes the suffix array with special symbols that inform us on the number
// of L-type suffixes in each bucket.
void initializeSA(const unsigned long *const input, unsigned long * const SA, const unsigned long length, const bool usingLType)
{
    // we scan T from right to left
    bool nextIsL = false;
    unsigned long met = 0;
    for (unsigned long i = 0; i < length; ++i)
    {
        unsigned long index = length - i - 1;
        const bool currentIsL = (index == length - 1) || (input[index] > input[index + 1] || (nextIsL && input[index] == input[index + 1]));
        // for each scanning character T[i] which is L_TYPE, if bucket T[i] has not been initialized yet, we initialize it

        if ((currentIsL && !usingLType) || (!currentIsL && usingLType))
        {
            // Let l denote the head of bucket T [i ] in SA (i.e. l is the smallest index in SA such that T [SA[l]] = T [i])
            //  We can find l by searching T [i ] in SA (the search key for SA[i ] is T [SA[i ]]) using binary search.
            unsigned long l;
            unsigned long first = custom_binary_search(input, SA, length, input[index], usingLType);

            if (usingLType)
            {
                l = custom_binary_search_last(input, SA, length, input[index]);
            }
            else
            {
                l = first;
            }
            if (usingLType && l == 0 || (!usingLType && l == length - 1))
            {
                // obviously this is the only suffix in this bucket
                continue;
            }
            const unsigned long position = usingLType ? l - 1 : l + 1;
            const unsigned long further_position = usingLType ? l - 2 : l + 2;

            if (SA[position] >= length)
            {
                // if the bucket is already initialized, we skip it
                continue;
            }

            // we let rL denote the tail of L-suffixes in this bucket (i.e., rL is the largest index in SA such
            // that T[SA[rL]] = T[i] and T[SA[rL]] is L-type).
            unsigned long rL = l;

            while (rL < length && SA[rL] != index)
            {
                if (usingLType)
                    --rL;
                else
                    ++rL;
            }

            // Note that nL = rL - l + 1. Hence, it suffices to compute l and rL.
            const unsigned long nL = ((usingLType) ? rL - first + 1 : rL - l + 1);
            if (usingLType)
            {
                // s-types (left) and l-types (right) must be swapped
                rotate(SA + first, SA + first + nL, SA + l + 1);
            }

            if (nL == 2)
            {
                SA[position] = BT;
            }
            else if (nL == 3)
            {
                SA[position] = BH;
                SA[further_position] = BT;
            }
            else if (nL > 3)
            {
                SA[position] = BH;
                SA[further_position] = E;
                if (usingLType)
                {
                    SA[l - nL + 1] = BT;
                }
                else
                {
                    SA[l + nL - 1] = BT;
                }
            }
        }
        nextIsL = currentIsL;
    }
}

void case4(const unsigned long *const input, unsigned long *const SA, const unsigned long l, const unsigned long j, const unsigned long length)
{
    unsigned long rL = l + 1;
    while (rL < length && SA[rL] != R1 && (SA[rL] < length && input[SA[rL]] == input[j]))
    {
        ++rL;
    }
    // if R1 is found (if not, it's either case nL=1 or current was not L-type)
    if (rL != length && (SA[rL] == R1 || (SA[rL] < length && input[SA[rL]] == input[j])))
    {
        SA[rL] = j;
    }
}

void case4_S(const unsigned long *const input, unsigned long *const SA, const unsigned long l, const unsigned long j, const unsigned long length)
{
    unsigned long rL = l + 1;
    while (rL > 0 && SA[rL - 1] != R1 && input[SA[rL - 1]] == input[j])
    {
        --rL;
    }
    // if R1 is found (if not, it's either case nS=1 or current was not S-type)
    if (rL != 0 && SA[rL - 1] == R1)
    {
        SA[rL - 1] = j;
    }
}

void sortS_body(const unsigned long *const input, unsigned long *const SA, const unsigned long length, const unsigned long j)
{
    const unsigned long l = custom_binary_search_last(input, SA, length, input[j]); // modified to look for the last occurrence

    if (l == 0)
    {
        // skipping as edge case, clearly the only suffix in the bucket
        return;
    }

    // skip L type suffixes, this does not cover case 4 (but is always true for case 4)
    if (j == length - 1 || input[j] > input[j + 1])
    {
        return;
    }
    else if (input[j] == input[j + 1])
    {
        if (!(SA[l - 1] == BH || SA[l - 1] == R2 || SA[l - 1] == R1))
        {
            // skip if bucket is filled (j is not L-type)
            if (SA[l - 1] != BT && SA[l - 1] != R1 && SA[l - 1] != BH && SA[l - 1] != R2)
            {
                case4_S(input, SA, l, j, length);
            }
            return;
        }
    }
    const unsigned long prev = SA[l - 1];

    // nL = 2?
    if (prev == BT)
    {
        // nL = 2, first to put in
        SA[l] = j;
        SA[l - 1] = R1; // not in the paper, but how I implemented it
    }
    else if (prev == R1)
    {
        // nL = 2, second to put in
        SA[l - 1] = j;
    }
    else if (prev == BH)
    {
        if (SA[l - 2] == E)
        {
            // case 1
            SA[l] = j;
            SA[l - 2] = 1; // counter for the number of L-type suffixes put so far
        }
        else if (SA[l - 2] == BT)
        { // not in the paper, nL=3, first to put in
            SA[l] = j;
            SA[l - 2] = R2; // not in the paper, but how I implemented it
        }
        else if (SA[l - 2] == R2)
        { // not in the paper, nL=3, second to put in
            SA[l - 1] = j;
            SA[l - 2] = R1; // not in the paper, but how I implemented it
        }
        else if (SA[l - 2] == R1)
        {
            // nL = 3, third to put in
            SA[l - 2] = j;
        }
        else
        {
            const unsigned long c = SA[l - 2];
            if (SA[l - c - 2] != BT)
            {
                // case 2 (1)
                SA[l - c - 2] = j;
                SA[l - 2] = c + 1;
            }
            else
            {
                // case 2 (2)
                const unsigned long rL = l - 2 - c;              // TODO see
                std::move(SA + rL + 1, SA + l - 2, SA + rL + 2); // see
                SA[rL + 1] = j;
                SA[l - 1] = R2;
            }
        }
    }
    else if (prev == R2)
    {
        // case 3
        unsigned long rL = l - 2;
        while (SA[rL] != BT)
        {
            --rL;
        }
        move(SA + rL + 1, SA + l - 1, SA + rL + 2);
        SA[rL + 1] = j;
        SA[rL] = R1;
    }
    else
    {
        case4_S(input, SA, l, j, length);
    }
}

void sortL_body(const unsigned long *const input, unsigned long *const SA, const unsigned long length, const unsigned long j)
{
    const unsigned long l = custom_binary_search(input, SA, length, input[j], false);
    if (l == length - 1)
    {
        // skipping as edge case, clearly the only suffix in the bucket
        return;
    }

    // skip S type suffixes, this does not cover case 4 (but is always true for case 4)
    if (j != length - 1 && input[j] < input[j + 1])
    {
        return;
    }
    else if (j != length - 1 && input[j] == input[j + 1])
    {
        if (!(SA[l + 1] == BH || SA[l + 1] == R2 || SA[l + 1] == R1))
        {
            // skip if bucket is filled (j is not L-type)
            if (SA[l + 1] != BT && SA[l + 1] != R1 && SA[l + 1] != BH && SA[l + 1] != R2)
            {
                case4(input, SA, l, j, length);
            }
            return;
        }
    }

    if (SA[l + 1] == BT)
    {
        // nL = 2, first to put in
        SA[l] = j;
        SA[l + 1] = R1; // not in the paper, but how I implemented it
    }
    else if (SA[l + 1] == R1)
    {
        // nL = 2, second to put in
        SA[l + 1] = j;
    }
    else if (SA[l + 1] == BH)
    {
        if (SA[l + 2] == E)
        {
            // case 1
            SA[l] = j;
            SA[l + 2] = 1; // counter for the number of L-type suffixes put so far
        }
        else if (SA[l + 2] == BT)
        { // not in the paper, nL=3, first to put in
            // nL = 3, first to put in
            SA[l] = j;
            SA[l + 2] = R2; // not in the paper, but how I implemented it
        }
        else if (SA[l + 2] == R2)
        { // not in the paper, nL=3, second to put in
            // nL = 3, second to put in
            SA[l + 1] = j;
            SA[l + 2] = R1; // not in the paper, but how I implemented it
        }
        else if (SA[l + 2] == R1)
        {
            // nL = 3, third to put in
            SA[l + 2] = j;
        }
        else
        {
            const unsigned long c = SA[l + 2];
            if (SA[l + c + 2] != BT)
            {
                // case 2 (1)
                SA[l + c + 2] = j;
                SA[l + 2] = c + 1;
            }
            else
            {
                // case 2 (2)
                const unsigned long rL = l + 2 + c;
                move(SA + l + 3, SA + rL, SA + l + 2);
                SA[rL - 1] = j;
                SA[l + 1] = R2;
            }
        }
    }
    else if (SA[l + 1] == R2)
    {
        // case 3
        unsigned long rL = l + 2;
        while (rL != length - 1 && SA[rL] != BT)
        {
            ++rL;
        }
        move(SA + l + 2, SA + rL, SA + l + 1);
        SA[rL - 1] = j;
        SA[rL] = R1;
    }
    else
    {
        case4(input, SA, l, j, length);
    }
}

void sortS(const unsigned long *const input, unsigned long * const SA, const unsigned long length)
{
    for (unsigned long iter = 1; iter <= length; ++iter)
    {
        const unsigned long i = length - iter;
        if (SA[i] == 0)
            continue;

        // does this ever happen? Should this happen?
        if (SA[i] >= length)
        {
            if (SA[i] == BH)
                ++iter; // skips counter
            cout << "skipping index " << i << " because SA[i] is a special symbol." << endl;
            pause();
            continue;
        }

        unsigned long j = SA[i] - 1;
        sortS_body(input, SA, length, j);

        // does this ever happen? Should this happen?
        while (SA[i] - 1 != j && SA[i] < length && SA[i] != 0) // changed current, repeat.
        {
            cout << "REPEATING" << endl;
            pause();
            j = SA[i] - 1;
            sortS_body(input, SA, length, j);
        }
    }
}

void sortL(const unsigned long *const input, unsigned long * const SA, const unsigned long length)
{
    sortL_body(input, SA, length, length - 1);

    for (unsigned long i = 0; i < length; ++i)
    {
        if (SA[i] == 0)
            continue;

        // does this ever happen? Should this happen?
        if (SA[i] >= length)
        {
            if (SA[i] == BH)
                ++i; // skips counter
            cout << "skipping index " << i << " because SA[i] is a special symbol." << endl;
            pause();
            continue;
        }

        unsigned long j = SA[i] - 1;
        sortL_body(input, SA, length, j);

        // does this ever happen? Should this happen?
        while (SA[i] - 1 != j && SA[i] < length && SA[i] != 0) // changed current, repeat.
        {
            cout << "REPEATING" << endl;
            pause();
            j = SA[i] - 1;
            sortL_body(input, SA, length, j);
        }
    }
}

// section 5.5 - step 2
// for usingLType=true, go from right to left and keep the "rightmost free" instead of leftmost free
void inducedSorting(const unsigned long *const input, unsigned long * const SA, const unsigned long length, const bool usingLType)
{
    if (usingLType)
        sortS(input, SA, length);
    else
        sortL(input, SA, length);
}

bool optimalSuffixArray_first(const unsigned long *const input, unsigned long *const SA, const unsigned long length, unsigned long &nS)
{
    if (length < 2)
    {
        if (length == 1)
        {
            SA[0] = 0;
        }
        return false;
    }

    // check if nS <= nL
    nS = countS_Type(input, length);
    if (nS == 0)
    {
        for (unsigned long i = 0; i < length; ++i)
        {
            SA[i] = length - i - 1;
        }
        return false;
    }

    const bool usingLType = (length - nS) < nS; // if there are more L-type suffixes, we swap the roles of S and L
    if (usingLType)
    {
        nS = length - nS;
    }

    // step 1
    placeIndicesOf_Type(input, length, SA, usingLType);

    // step 2
    mergeSort_Substrings(input, length, SA, nS, usingLType);

    // step 3
    constructReducedProblem(input, length, SA, nS, usingLType);

    // step 4 (no need to adapt the heap sort for L-type suffixes, it works as is)
    heapSortReducedProblem(SA, length, nS);

    return true;
}

void optimalSuffixArray_second(const unsigned long *const input, unsigned long *const SA, const unsigned long length) {
    unsigned long nS = countS_Type(input, length);

    const bool usingLType = (length - nS) < nS; // if there are more L-type suffixes, we swap the roles of S and L
    if (usingLType)
    {
        nS = length - nS;
    }
    
    // step 5
    RestoreFromRecursion(input, length, SA, nS, usingLType);

    // step 6
    preprocess(input, SA, length, nS, usingLType);

    // step 7
    initializeSA(input, SA, length, usingLType);

    // step 8
    inducedSorting(input, SA, length, usingLType);
}

void optimalSuffixArray(unsigned long *const input, unsigned long *const SA, const unsigned long length)
{
    unsigned long prev_nS;
    bool needsRecursion = optimalSuffixArray_first(input, SA, length, prev_nS);
    if (!needsRecursion)
    {
        return;
    }

    unsigned long *old_output = SA;
    unsigned long old_length = length;
    unsigned long new_output_index = 0;
    unsigned long old_old_length = old_length;
    unsigned long recursionDepth = 0;
    unsigned long *new_input;
    while (needsRecursion) {
        new_input = old_output;
        unsigned long new_length = prev_nS;
        unsigned long *new_output = old_output + old_length - new_length;
        
        // cleaning up before recursion
        bool prev_had_space = new_input[0] & MSB;
        if (prev_had_space) {
            new_input[0] &= ~MSB;
        }

        // recursion
        needsRecursion = optimalSuffixArray_first(new_input, new_output, new_length, prev_nS);
        
        // restoring after recursion
        if (prev_had_space) {
            new_input[0] |= MSB;
        }

        // get ready for next recursive call
        if (needsRecursion) {
            new_output_index += old_length - new_length;
            ++recursionDepth;

            // make sure I store information to go back later
            if (new_length*2 != old_length) {
                new_output[0] |= MSB;
                new_output[-1] = old_length;
            }

            old_output = new_output;
        }

        old_old_length = old_length;
        old_length = new_length;
    }
    
    unsigned long *old_input = new_input;

    while (recursionDepth-- > 0) {

        old_length = old_old_length;
        const bool had_space = old_input[0] & MSB;
        if (had_space) {
            old_input[0] &= ~MSB;
            old_old_length = old_input[-1];
        }
        else {
            old_old_length *= 2;
        }
        old_output = old_input;

        new_output_index += old_length - old_old_length;
        old_input = &SA[new_output_index];

        const bool current_have_space = old_input[0] & MSB;
        if (current_have_space) {
            old_input[0] &= ~MSB;
        }

        optimalSuffixArray_second(old_input, old_output, old_length);

        if (current_have_space) {
            old_input[0] |= MSB;
        }
    }
    optimalSuffixArray_second(input, SA, length);
}

// Recursive and simpler version of optimalSuffixArray, needs log(N) memory workload
void optimalSuffixArray_recursive(const unsigned long *const input, unsigned long *const SA, const unsigned long length)
{    
    if (length < 2)
    {
        if (length == 1)
        {
            SA[0] = 0;
        }
        return;
    }

    // check if nS <= nL
    unsigned long nS = countS_Type(input, length);
    if (nS == 0)
    {
        for (unsigned long i = 0; i < length; ++i)
        {
            SA[i] = length - i - 1;
        }
        return;
    }

    const bool usingLType = (length - nS) < nS; // if there are more L-type suffixes, we swap the roles of S and L
    if (usingLType)
    {
        nS = length - nS;
    }

    // step 1
    placeIndicesOf_Type(input, length, SA, usingLType);

    // step 2
    mergeSort_Substrings(input, length, SA, nS, usingLType);

    // step 3
    constructReducedProblem(input, length, SA, nS, usingLType);

    // step 4 (no need to adapt the heap sort for L-type suffixes, it works as is)
    heapSortReducedProblem(SA, length, nS);

    // step 5
    RestoreFromRecursion(input, length, SA, nS, usingLType);

    // step 6
    preprocess(input, SA, length, nS, usingLType);

    // step 7
    initializeSA(input, SA, length, usingLType);

    // step 8
    inducedSorting(input, SA, length, usingLType);
}