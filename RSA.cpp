#include"RSA.h"
bool MillerRabin(ull n) {
    random_device rd;
    mt19937_64 eng(rd());
    uniform_int_distribution<unsigned long long> distr;
    if (n == 2) return true;
    if (n & 1 == 0) return false;
    ull m = n - 1;
    int k = 0;
    RSA One = RSA(1);
    while(!(m & 1)) {
        k++;
        m >>= 1;
    }
    RSA M = RSA(m);
    RSA N = RSA(n);
    RSA N_1 = RSA(n - 1);
    int tmpk = k;
    for (int i = 0; i < 50; i++) {
        ull a = (distr(eng) % (n - 1)) + 1;
        RSA b = RSA::Pow(RSA(a), M, N);
        if (b == One)
            continue;
        for (; tmpk > 0; tmpk--)
            if (b == N_1)
                break;
            else
                b = RSA::Pow(b, RSA((ull)2), N);
        if (tmpk == 0)
            return false;
        tmpk = k;
    }
    return true;
}

void RandomPrime(ull &p, ull &q, ull e) {
    random_device rd;
    mt19937_64 eng(rd());
    uniform_int_distribution<unsigned long long> distr;
    ull x;
    //int time = 0;
    do {
        //time++;
        x = distr(eng);
        x = x | 1;
    } while (MillerRabin(x) == false || x % e == 0);
    p = x;
    do {
        //time++;
        x = distr(eng);
        x = x | 1;
    } while (MillerRabin(x) == false || x == p || x % e == 0);
    q = x;
    //cout << "p: " << p << ",q: " << q << endl;
    //cout << time << endl;
}

void euclid(const RSA &phi, const RSA &e, RSA &d, RSA &y) {
    if (phi == 0) {
        d = 1, y = 0;
        return;
    }
    //cout << "phi: " << phi << "e: " << e << endl;
    euclid(e % phi, phi, d, y);
    RSA tmp = d - (e / phi) * y;
    d = y;
    y = tmp;
}

void RSAGenerate() {
    ull e = 65537;
    ull p, q;
    RandomPrime(p, q, e);
    RSA n = RSA(p) * RSA(q);
    //cout << "n: " << n << endl;
    RSA phi = RSA(p - 1) * RSA(q - 1);
    //cout << "phi: " << phi << endl;
    RSA d, y;
    euclid(phi, RSA(e), d, y);
    if (d.flag == true) {
        d.flag = false;
        d = phi - d;
    }
    //cout << "d: " << d << endl;
    RSA E = RSA(e);
    E.write("public.txt", n);
    d.write("private.txt", n);
}

void read(const char *url, RSA *P, RSA *N) {
    ull k[2], n[2];
    ifstream File(url, ios::in);
    File.seekg(0, File.end);
    int sum = File.tellg();
    File.seekg(0, File.beg);
    File.read((char*)&n, sizeof(n));
    if (sum == 32)
        File.read((char*)&k, sizeof(k));
    else
        File.read((char*)&k, sizeof(ull));
    *P = RSA::toRSA(k);
    *N = RSA::toRSA(n);
}

void read(const char *url, RSA *P) {
    ull k[2];
    ifstream File(url, ios::in);
    File.seekg(0, File.end);
    int sum = File.tellg();
    File.seekg(0, File.beg);
    if (sum == 16)
        File.read((char*)&k, sizeof(k));
    else
        File.read((char*)&k, sizeof(ull));
    *P = RSA::toRSA(k);
}
