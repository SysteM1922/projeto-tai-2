#include <iostream>

#include "progress_bar.hpp"

using namespace std;

void progress_bar(int progress, int total)
{
    float percent = static_cast<float>(progress) / total * 100;

    int barWidth = 50;
    int numChars = static_cast<int>(percent / 100 * barWidth);

    cout << "[";
    for (int i = 0; i < barWidth; ++i)
    {
        if (i < numChars)
            cout << "=";
        else
            cout << " ";
    }
    cout << "] " << static_cast<int>(percent) << "%\r";
    cout.flush();
}