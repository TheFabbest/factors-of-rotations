#ifndef COMPUTE_STRUCTURES_HPP
#define COMPUTE_STRUCTURES_HPP

// left-lyndon-tree.pdf
// algorithm LyndonSuffixT
void LyndonSuffixTable(const char* const y, const unsigned long n, unsigned long* const LynS) {
    LynS[0] = 1;
    unsigned long per = 1;
    unsigned long i = 0;
    for (unsigned long j = 1; j < n; ++j) {
        if (y[j] != y[i]) {
            LynS[j] = j+1;
            per = j+1;
            i = 0;
        }
        else {
            LynS[j] = LynS[i];
            i = (i+1) % per;
        }
    }
}

void LongestLyndon(const char* const word, const unsigned long length, const unsigned long * const rank, unsigned long * const Lyn) {
    for (unsigned long i = length; i > 0; --i)
    {
        Lyn[i-1] = 1;
        unsigned long j = i;
        while (j < length && rank[i-1] < rank[j]) {
            Lyn[i-1] += Lyn[j];
            j += Lyn[j];
        }
    }
}

void ComputeNNS(const char* const word, const unsigned long length, unsigned long * const NNS)
{
    NNS[length-1] = length;
    for (unsigned long i = length-1; i > 0; --i) {
        unsigned long j = i;
        while (word[i-1] < word[j] && j < length) {
            j = NNS[j];
        }
        NNS[i-1] = j;
    }
}

void rankArrayFromSA(const unsigned long * SA, const unsigned long n, unsigned long * const rank) {
    for (unsigned long i = 0; i < n; ++i){
        rank[SA[i]] = i;
    }
}

#endif