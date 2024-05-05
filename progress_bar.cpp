#include <iostream>

#include "progress_bar.h"

using namespace std;

void progressBar(int progress, int total) {
    // Calcula a porcentagem de progresso
    float percent = static_cast<float>(progress) / total * 100;

    // Número de caracteres '#' a serem exibidos na barra de progresso
    int barWidth = 50;
    int numChars = static_cast<int>(percent / 100 * barWidth);

    // Imprime a barra de progresso
    cout << "[";
    for (int i = 0; i < barWidth; ++i) {
        if (i < numChars)
            cout << "=";
        else
            cout << " ";
    }
    cout << "] " << static_cast<int>(percent) << "%\r";
    cout.flush();
}