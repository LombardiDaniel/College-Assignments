#include <bits/stdc++.h>
#include <ctime>
using namespace std;

int main() {

    clock_t begin = clock();

    cout << "Test elapsed time: " << endl;

    clock_t end = clock();
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;

    cout << "Elapsed Time (s): " << elapsed_secs << endl;
    return 0;
}