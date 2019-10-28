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
        //cout << "The " << i << " round" << endl;
        ull a = (distr(eng) % (n - 1)) + 1;

        //cout << a << endl;
        //cout << "M: " << M << " " << "N: " << N << " " << endl;
        RSA b = RSA::Pow(RSA(a), M, N);
        //cout << "b: " << b << endl;
        if (b == One)
            continue;
        for (; tmpk > 0; tmpk--) {
            if (b == N_1)
                break;
            else
                b = RSA::Pow(b, RSA((ull)2), N);
            //cout << "b: " << b << " " << "tmpk: " << tmpk << endl;
        }
        if (tmpk == 0)
            return false;
        tmpk = k;
    }
    //cout << "Finish function.\n";
    return true;
}

void RandomPrime(ull &p, ull &q, ull e) {
    random_device rd;
    mt19937_64 eng(rd());
    uniform_int_distribution<unsigned long long> distr;
    ull x;
    int time = 0;
    do {
        time++;
        x = distr(eng);
        x = x | 1;
        //cout << x << endl;
    } while (MillerRabin(x) == false || x % e == 0);
    p = x;
    do {
        time++;
        x = distr(eng);
        x = x | 1;
        //cout << x << endl;
    } while (MillerRabin(x) == false || x == p || x % e == 0);
    q = x;
    cout << "p: " << p << ",q: " << q << endl;
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
    //p = 97, q = 37;
    RSA n = RSA(p) * RSA(q);
    cout << "n: " << n << endl;
    RSA phi = RSA(p - 1) * RSA(q - 1);
    cout << "phi: " << phi << endl;
    RSA d, y;
    euclid(phi, RSA(e), d, y);
    if (d.flag == true) {
        d.flag = false;
        d = phi - d;
    }
    cout << "d: " << d << endl;
    RSA E = RSA(e);
    RSA c = RSA::Pow(RSA(88888222), E, n);
    RSA m = RSA::Pow(c, d, n);
    cout << "c: " << c << ", m: " << m << endl;
    E.write(n, 0);
    d.write(n, 1);
}

int main() {
    cout << "Welcome to the RSA system.\n";
    RSAGenerate();
    ifstream in("public.txt", ios::in);
    ull pub, n[2];
    in.read((char *)&n, sizeof(n));
    in.read((char *)&pub, sizeof(pub));
    in.close();
    cout << "pub:" << pub << ",n: " << n[1] << " " << n[0] << endl;
    ifstream in1("private.txt", ios::in);
    ull priv[2], n1[2];
    in1.read((char *)&n1, sizeof(n1));
    in1.read((char *)&priv, sizeof(priv));
    in1.close();
    if (priv[1] == 0)
        cout << "priv:" << priv;
    else
        cout << "priv: " << priv[1] << " " << priv[0];
    cout << ",n: " << n1[1] << " " << n1[0] << endl;
}
