#ifndef SAIS_HPP
#define SAIS_HPP

#define S_TYPE true
#define L_TYPE false

using namespace std;

unsigned long* SAIS(const char* input, const unsigned long length, const unsigned long alphabet_size);

bool isLMS(unsigned long index, const bool *typeArray) {
    if (index == 0) {
        return false;
    }
    return typeArray[index] == S_TYPE && typeArray[index-1] == L_TYPE;
}

void computeBucketSizes(const char* input, const unsigned long length, unsigned long * const bucket_sizes, const unsigned long alphabet_size, 
                        const function<char (char)> mapping) {
    fill_n(bucket_sizes, alphabet_size, 0);
    for (unsigned long i = 0; i < length; ++i) {
        ++bucket_sizes[mapping(input[i])];
    }
}

void computeBucketTails(unsigned long *bucket_tails, const unsigned long *bucket_sizes, unsigned long alphabet_size) {
    unsigned long offset = 1;
    for (unsigned long i = 0; i < alphabet_size; ++i) {
        offset += bucket_sizes[i];
        bucket_tails[i] = offset-1; 
    }
}

void computeBucketHeads(unsigned long *bucket_heads, const unsigned long *bucket_sizes, unsigned long alphabet_size) {
    unsigned long offset = 1;
    for (unsigned long i = 0; i < alphabet_size; ++i) {
        bucket_heads[i] = offset;
        offset += bucket_sizes[i];
    }
}

void buildTypeArray(const char * const input, const unsigned long length, bool *typeArray) {
    typeArray[length] = S_TYPE;

    if (length == 0) return;

    typeArray[length-1] = L_TYPE;
    
    for (unsigned long i = 0; i < length-1; ++i) {
        unsigned long index = length - i - 2;
        if (input[index] > input[index+1] || (input[index] == input[index+1] && typeArray[index+1] == L_TYPE)) {
            typeArray[index] = L_TYPE;
        }
        else {
            typeArray[index] = S_TYPE;
        }
    }
}

bool areLMS_SubstringsEqual(string input, const bool *typeArray, const bool *isLMS, const unsigned long offsetA, const unsigned long offsetB) {
    const unsigned long length = input.length();
    
    if (offsetA >= length || offsetB >= length) {
        return false;
    }

    unsigned long i = 0;
    while (true) {
        bool aIsLMS = isLMS[offsetA];
        bool bIsLMS = isLMS[offsetB];

        if (i > 0 && aIsLMS && bIsLMS) {
            return true;
        }
        else if (aIsLMS != bIsLMS) {
            return false;
        }
        else if (input[i+offsetA] != input[i+offsetB]){
            return false;
        }

        ++i;
    }
}

// without precomputed isLMS array
bool areLMS_SubstringsEqual(string input, const bool *typeArray, const unsigned long offsetA, const unsigned long offsetB) {
    const unsigned long length = input.length();
    
    if (offsetA >= length || offsetB >= length) {
        return false;
    }

    unsigned long i = 0;
    while (true) {
        bool aIsLMS = isLMS(offsetA+i, typeArray);
        bool bIsLMS = isLMS(offsetB+i, typeArray);

        if (i > 0 && aIsLMS && bIsLMS) {
            return true;
        }
        else if (aIsLMS != bIsLMS) {
            return false;
        }
        else if (input[i+offsetA] != input[i+offsetB]){
            return false;
        }

        ++i;
    }
}



void guessLMS_Sort(const char* input, const unsigned long length, unsigned long *guessed_SA, unsigned long *bucket_sizes,
                    bool* typeArray, unsigned long alphabet_size, function<char (char)> mapping) {
    
    unsigned long *bucket_tails = new unsigned long[alphabet_size];
    computeBucketTails(bucket_tails, bucket_sizes, alphabet_size);

    // Initialize SA with -1
    fill_n(guessed_SA, length+1, (unsigned long)-1);

    // Sort LMS characters
    for (unsigned long i = 0; i < length; ++i) {
        if (isLMS(i, typeArray)) {
            char bucket = mapping(input[i]);
            guessed_SA[bucket_tails[bucket]] = i;
            --bucket_tails[bucket];
        }
    }
    guessed_SA[0] = length;

    delete[] bucket_tails;
}

void induceSortLType(string input, unsigned long *SA, unsigned long *bucket_sizes, bool *typeArray, unsigned long alphabet_size,
                    function<char (char)> mapping) {
    const unsigned long length = input.length();
    unsigned long *bucket_heads = new unsigned long[alphabet_size];
    computeBucketHeads(bucket_heads, bucket_sizes, alphabet_size);
    for (unsigned long i = 0; i < length; ++i) {
        if (SA[i] != (unsigned long)-1 && SA[i] != 0) {
            unsigned long prev = SA[i] - 1;
            if (typeArray[prev] == L_TYPE) {
                char bucket = mapping(input[prev]);
                SA[bucket_heads[bucket]] = prev;
                ++bucket_heads[bucket];
            }
        }
    }
    delete[] bucket_heads;
}

void induceSortSType(string input, unsigned long *SA, unsigned long *bucket_sizes, bool *typeArray, unsigned long alphabet_size,
                    function<char (char)> mapping) {
    const unsigned long length = input.length();
    
    unsigned long *bucket_tails = new unsigned long[alphabet_size];
    computeBucketTails(bucket_tails, bucket_sizes, alphabet_size);

    for (unsigned long i = length+1; i > 0; --i) {
        if (SA[i-1] != (unsigned long)-1 && SA[i-1] != 0) {
            unsigned long prev = SA[i-1] - 1;
            if (typeArray[prev] == S_TYPE) {
                char bucket = mapping(input[prev]);
                SA[bucket_tails[bucket]] = prev;
                --bucket_tails[bucket];
            }
        }
    }
    delete[] bucket_tails;
}

unsigned long* accurateLMS_Sort(string input, unsigned long *bucket_sizes, unsigned long* summarySuffixArray, unsigned long* summarySuffixOffsets, unsigned long alphabet_size,
                    unsigned long summaryLength, function<char (char)> mapping) {
    const unsigned long length = input.length();
    
    unsigned long *suffixOffsets = new unsigned long[length+1];

    fill_n(suffixOffsets, length+1, (unsigned long)-1);

    unsigned long *bucket_tails = new unsigned long[alphabet_size];
    computeBucketTails(bucket_tails, bucket_sizes, alphabet_size);

    if (summaryLength != 0) {
        for (unsigned long i = summaryLength; i > 1; --i){
            unsigned long stringIndex = summarySuffixOffsets[summarySuffixArray[i]];
            unsigned long bucket = mapping(input[stringIndex]);
            suffixOffsets[bucket_tails[bucket]] = stringIndex;
            --bucket_tails[bucket];
        }
    }
    

    suffixOffsets[0] = length;

    delete[] bucket_tails;
    return suffixOffsets;
}

void summariseSuffixArray(string input, unsigned long* guessed_SA, unsigned long &summary_alphabet_size, bool* typeArray, unsigned long **summarySuffixOffset, string &summaryString, char min_char, function<char (char)> mapping) {
    const unsigned long length = input.length();
    unsigned long* lmsNames = new unsigned long[length+1];
    *summarySuffixOffset = new unsigned long[length];
    fill_n(lmsNames, length+1, (unsigned long)-1);
    unsigned long currentName = min_char;
    unsigned long lastLMS_SuffixOffset = guessed_SA[0];
    lmsNames[lastLMS_SuffixOffset] = currentName;

    for (unsigned long i = 1; i <= length; ++i) {
        unsigned long suffixOffset = guessed_SA[i];
        if (isLMS(suffixOffset, typeArray)) {
            if (!areLMS_SubstringsEqual(input, typeArray, lastLMS_SuffixOffset, suffixOffset)) {
                ++currentName;
            }
            lastLMS_SuffixOffset = suffixOffset;
            lmsNames[suffixOffset] = currentName;
        }
    }

    unsigned long summary_length = 0;
    for (unsigned long i = 0; i <= length; ++i) {
        if (lmsNames[i] != (unsigned long)-1) {
            (*summarySuffixOffset)[summary_length] = i;
            summaryString += (char)lmsNames[i];
            ++summary_length;
        }
    }
    
    unsigned long *newSummarySuffixOffset = new unsigned long[summary_length];
    copy(*summarySuffixOffset, (*summarySuffixOffset) + summary_length, newSummarySuffixOffset);
    delete [] *summarySuffixOffset;
    delete [] lmsNames;
    *summarySuffixOffset = newSummarySuffixOffset;

    summary_alphabet_size = currentName - min_char + 1;
}

unsigned long* makeSummarySuffixArray(string summary_string, unsigned long summary_alphabet_size, function<char (char)> mapping) {
    if (summary_string.length() <= summary_alphabet_size) {
        unsigned long *summary_SA = new unsigned long[summary_string.length() + 1];
        fill_n(summary_SA, summary_string.length()+1, (unsigned long)-1);
        summary_SA[0] = summary_string.length();
        for (unsigned long i = 0; i < summary_string.length(); ++i) {
            summary_SA[mapping(summary_string[i])+1] = i;
        }
        return summary_SA;
    }
    else {
        return SAIS(summary_string.c_str(), summary_string.length(), summary_alphabet_size);
    }
}

// WARNING: the returned array includes the empty suffix at the beginning
unsigned long* SAIS(const char* input, const unsigned long length, const unsigned long alphabet_size) {
    
    // get smallest character in input
    char min_char = input[0];
    for (unsigned long i = 1; i < length; ++i) {
        if (input[i] < min_char) {
            min_char = input[i];
        }
    }

    // map characters to 0-based alphabet
    const function<char (char)> mapping = [min_char](char c) { return c - min_char; };

    // Initialize type array
    bool *typeArray = new bool[length+1];
    buildTypeArray(input, length, typeArray);

    // Initialize bucket sizes
    unsigned long *bucket_sizes = new unsigned long[alphabet_size];
    computeBucketSizes(input, length, bucket_sizes, alphabet_size, mapping);

    // guess LMS sort and induce
    unsigned long *guessed_SA = new unsigned long[length+1];
    guessLMS_Sort(input, length, guessed_SA, bucket_sizes, typeArray, alphabet_size, mapping);
    induceSortLType(input, guessed_SA, bucket_sizes, typeArray, alphabet_size, mapping);
    induceSortSType(input, guessed_SA, bucket_sizes, typeArray, alphabet_size, mapping);
    
    // summarise
    string summaryString;
    unsigned long new_alphabet_size;
    unsigned long *summarySuffixOffset;
    summariseSuffixArray(input, guessed_SA, new_alphabet_size, typeArray, &summarySuffixOffset, summaryString, min_char, mapping);
    unsigned long *summary_SA = makeSummarySuffixArray(summaryString, new_alphabet_size, mapping);
    
    // accurate sort based on summary
    unsigned long *suffixOffset = accurateLMS_Sort(input, bucket_sizes, summary_SA, summarySuffixOffset, alphabet_size, summaryString.length(), mapping);
    induceSortLType(input, suffixOffset, bucket_sizes, typeArray, alphabet_size, mapping);
    induceSortSType(input, suffixOffset, bucket_sizes, typeArray, alphabet_size, mapping);
    
    // free memory
    delete [] typeArray;
    delete [] bucket_sizes;
    delete [] guessed_SA;
    delete [] summarySuffixOffset;
    delete [] summary_SA;

    return suffixOffset;
}

#endif