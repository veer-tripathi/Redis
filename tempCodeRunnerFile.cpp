#include <iostream>
#include <cmath>
#include <cstring>
#include <unistd.h> // for usleep()

int main() {
    float A = 0, B = 0;
    float i, j;
    int k;

    float z[1760];
    char b[1760];

    std::cout << "\x1b[2J"; // clear screen

    while (true) {
        memset(b, ' ', 1760);
        memset(z, 0, 1760 * sizeof(float));

        for (j = 0; j < 6.28; j += 0.07) {
            for (i = 0; i < 6.28; i += 0.02) {

                float c = sin(i);
                float d = cos(j);
                float e = sin(A);
                float f = sin(j);
                float g = cos(A);
                float h = d + 2;
                float D = 1 / (c * h * e + f * g + 5);
                float l = cos(i);
                float m = cos(B);
                float n = sin(B);
                float t = c * h * g - f * e;

                int x = 40 + 30 * D * (l * h * m - t * n);
                int y = 12 + 15 * D * (l * h * n + t * m);
                int o = x + 80 * y;
                int N = 8 * ((f * e - c * d * g) * m - c * d * e - f * g - l * d * n);

                if (y < 22 && y >= 0 && x >= 0 && x < 80 && D > z[o]) {
                    z[o] = D;
                    b[o] = ".,-~:;=!*#$@"[N > 0 ? N : 0];
                }
            }
        }

        std::cout << "\x1b[H"; // reset cursor

        for (k = 0; k < 1760; k++) {
            std::cout << (k % 80 ? b[k] : '\n');
        }

        A += 0.04;
        B += 0.02;

        usleep(30000); // control speed
    }

    return 0;
}