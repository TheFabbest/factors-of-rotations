#ifndef LEAST_ROTATION_HPP
#define LEAST_ROTATION_HPP

// booth's algorithm, see more efficient and O(1) space "Shiloach's Fast Canonization Algorithm"
unsigned long least_rotation(const char* const word, const unsigned long length){
    long *f = new long [2*length] {-1};
    unsigned long k = 0;
    for (unsigned long j = 1; j < 2*length; ++j)
    {
        long i = f[j - k - 1];
        while (i != -1 && word[j % length] != word[(k + i + 1) % length]){
            if (word[j % length] < word[(k + i + 1) % length]){
                k = j - i - 1;
            }
            i = f[i];
        }
            
        if (i == -1 && word[j % length] != word[(k + i + 1) % length])
        {
            if (word[j % length] < word[(k + i + 1) % length]){
                k = j;
            }
            f[j - k] = -1;
        }
        else {
            f[j - k] = i + 1;
        }
    }
    
    delete[] f;
    return k;
}

#endif