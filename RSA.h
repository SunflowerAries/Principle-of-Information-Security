#ifndef RSA_H
#define RSA_H
#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<fstream>
#include<random>
#include<limits>
#include<memory.h>
#include<assert.h>
using namespace std;
typedef unsigned long long ull;
class RSA {
    ull value[10];
    int len;
    
    static const ull base = (ull)(0xffffffff);
    public:
        bool flag;
        RSA() {
            memset(value, 0, sizeof(value));
            len = 0;
            flag = false;
        }

        RSA(ull x) {
            memset(value, 0, sizeof(value));
            flag = false;
            int i;
            for (i = 0; x > 0; x >>= 32, i++)
                value[i] = x & base;
            len = i;
        }

        RSA abs(const RSA &rhs) const {
            RSA res = rhs;
            res.flag = false;
            return res;
        }

        RSA operator * (const RSA &rhs) const {
            RSA res;
            res.flag = this->flag ^ rhs.flag;
            
            for (int i = 0; i < this->len; i++)
                for (int j = 0; j < rhs.len; j++) {
                    res.value[i + j] += this->value[i] * rhs.value[j];
                    res.value[i + j + 1] += res.value[i + j] >> 32;
                    res.value[i + j] &= base;
                }
            res.len = this->len + rhs.len;
            if (res.value[res.len] != 0)
                res.len++;
            else if (res.value[res.len - 1] == 0)
                res.len--;
            return res;
        }

        RSA operator - (const RSA &rhs) const {
            RSA res;
            if (!this->flag && rhs.flag) {
                res = *this;
                return res += abs(rhs);
            } else if (this->flag && rhs.flag) {
                return abs(rhs) - abs(*this);
            } else if (this->flag && !rhs.flag) {
                res = abs(*this);
                res += rhs;
                res.flag = true;
                return res;
            }
            const RSA *a = this, *b = &rhs;
            if (rhs <= *this) 
                res.flag = false;
            else {
                res.flag = true;
                std::swap(a, b);
            }
            //cout << "a: " << *a << "b: " << *b << endl;
            for (int i = a->len - 1; i >= 0; i--) {
                res.value[i] = res.value[i] + a->value[i] - b->value[i];
                int less = 0;
                for (int j = i - 1; j >= 0; j--) {
                    if (a->value[j] < b->value[j]) {
                        less = 1;
                        break;
                    } else if (a->value[j] > b->value[j]) {
                        less = 2;
                        break;
                    }
                }
                if (less == 1) {
                    res.value[i] -= 1;
                    res.value[i - 1] = (ull)1 << 32;
                }
            }
            res.len = a->len;
            //std::cout << "In subsection res: " << res << " " << res.len << ".\n";
            while (res.len > 0)
                if (res.value[res.len - 1] == 0)
                    res.len--;
                else break;
            return res;
        }

        RSA operator -= (const RSA &rhs) {
            //std::cout << "rhs: " << rhs << ".\n";
            (*this) = (*this) - rhs;
            //std::cout << "In -= section this: " << *this << ".\n";
            return *this;
        }

        RSA operator += (const RSA &rhs) {
            RSA res;
            if (!this->flag && rhs.flag) {
                res = *this;
                return res -= abs(rhs);
            } else if (this->flag && rhs.flag) {
                res = abs(*this);
                res += abs(rhs);
                res.flag = true; 
                return res;
            } else if (this->flag && !rhs.flag) {
                return rhs - abs(*this);
            }
            res.flag = this->flag;
            res.len = (this->len > rhs.len) ? this->len : rhs.len;
            for (int i = 0; i < res.len; i++) {
                res.value[i] += (this->value[i] + rhs.value[i]) & base;
                res.value[i + 1] += (this->value[i] + rhs.value[i]) / (base + 1);
            }
            if (res.value[res.len] != 0)
                res.len++;
            *this = res;
            return *this;
        }

        RSA operator << (ull k) const {
            RSA res;
            res.len = this->len;
            res.flag = this->flag;
            if (k > 32) {
                res = *this;
                while (k > 0) {
                    for (int i = res.len - 1; i >= 0; i--) {
                        ull tmp;
                        tmp = res.value[i] << 32;
                        res.value[i] = tmp & base;
                        res.value[i + 1] += tmp / (base + 1);
                    }
                    k -= 32;
                    if (res.value[res.len] != 0)
                        res.len++;
                }
            }
            else {
                for (int i = this->len - 1; i >= 0; i--) {
                        ull tmp;
                        tmp = this->value[i] << k;
                        res.value[i] = tmp & base;
                        res.value[i + 1] += tmp / (base + 1);
                    }
                if (res.value[res.len] != 0)
                    res.len++;
            }
            return res;
        }

        RSA operator / (const RSA &rhs) const {
            RSA res;
            if ((*this) < rhs)
                return RSA(0);
            RSA tmp = (*this);
            //cout << tmp.value[1] << " " << tmp.value[0] << endl;
            while (rhs <= tmp) {
                //cout << "tmp: " << tmp << " " << ", rhs: " << rhs << endl;
                //cout << "tmp: " << tmp.value[tmp.len - 1] << ", rhs: " << rhs.value[rhs.len - 1] << endl;
                if (tmp.value[tmp.len - 1] >= rhs.value[rhs.len - 1]) {
                    RSA Rqt = RSA(tmp.value[tmp.len - 1] / rhs.value[rhs.len - 1]);
                    //cout << tmp.len - rhs.len << " " << tmp.value[tmp.len - 1] / rhs.value[rhs.len - 1] << endl;
                    RSA temp;
                    RSA shift = RSA((ull)1) << ((tmp.len - rhs.len) * 32);
                    do {
                        if (!(temp == RSA(0)))
                            Rqt -= RSA(1);
                        //cout << "shift: " << shift << endl;
                        //cout << "lRqt: " << Rqt * shift << endl;
                        RSA lRqt = Rqt * rhs;
                        /*cout << "lRqt: ";
                        for (int i = lRqt.len - 1; i >= 0; i--)
                            cout << lRqt.value[i] << " ";
                        cout << endl;*/
                        temp = lRqt * shift;
                        //cout << "tmp: " << tmp << endl;
                        //cout << "temp: " << temp << endl;
                        /*cout << "tmp: ";
                        for (int i = tmp.len - 1; i >= 0; i--)
                            cout << tmp.value[i] << " ";
                        cout << endl;
                        cout << "temp: ";
                        for (int i = temp.len - 1; i >= 0; i--)
                            cout << temp.value[i] << " ";
                        cout << endl;*/
                        if (tmp.len < temp.len) {
                            //cout << temp.value[temp.len - 1] << " " << temp.value[temp.len - 2] << endl;
                            temp.value[temp.len - 2] += (((ull)1 << 32) * temp.value[temp.len - 1]);
                            //cout << temp.value[temp.len - 2] << endl;
                            temp.value[temp.len - 1] = 0;
                            temp.len--;
                        }
                        //cout << "temp: ";
                        //cout << temp << endl;
                        /*for (int i = temp.len - 1; i >= 0; i--)
                            cout << temp.value[i] << " ";
                        cout << endl;*/
                        //cout << "temp: " << temp << endl;
                    } while (tmp < temp);
                    //cout << temp << endl;
                    tmp -= temp;
                    //cout << "tmp: " << tmp << endl;
                    res += (Rqt * shift);
                } else {
                    tmp.value[tmp.len - 2] += (((ull)1 << 32) * tmp.value[tmp.len - 1]);
                    tmp.value[tmp.len - 1] = 0;
                    tmp.len--;
                }
            }
            return res;
        }

        RSA operator % (const RSA &rhs) const {
            RSA res;
            //std::cout << "this: " << (*this) << " " << "rhs: " << rhs << ".\n";
            if ((*this) < rhs)
                return *this;
            
            //cout << "div: " << *this / rhs << endl;
            res = *this - (((*this) / rhs) * rhs);
            //std::cout << "res: " << res << ".\n";
            return res;
        }

        bool operator < (const RSA &rhs) const {
            if (this->flag && !rhs.flag)
                return true;
            else if (!this->flag && rhs.flag)
                return false;
            
            if (this->len < rhs.len)
                return true;
            else if (this->len > rhs.len)
                return false;
            else {
                for (int i = this->len - 1; i >= 0; i--)
                    if (this->value[i] < rhs.value[i])
                        return true;
                    else if (this->value[i] > rhs.value[i])
                        return false;
                return false;
            }
        }

        bool operator <= (const RSA &rhs) const {
            return *this < rhs || *this == rhs;
        }

        bool operator == (const RSA &rhs) const {
            if (this->flag != rhs.flag)
                return false;
            else if (this->len != rhs.len)
                return false;
            else {
                for (int i = 0; i < this->len; i++)
                    if (this->value[i] != rhs.value[i])
                        return false;
                return true;
            }
        }

        static RSA Pow(const RSA &a, const RSA &order, const RSA &mod) {
            RSA b = a, ans = RSA(1);
            //std::cout << "b: " << b << " " << "order: " << order << " " << "mod: " << mod << ".\n";
            for (int i = 0; i < order.len; i++) {
                ull tmp = order.value[i];
                for (int j = 0; j < 32; tmp >>= 1, b = (b * b) % mod, j++) {
                    if (tmp & 1) {
                        ans = (ans * b) % mod;
                        //cout << "ans: " << ans << endl;
                        //cout << "b: " << b << endl;
                    }
                    //cout << j << endl;
                    //cout << "b: " << b << ", b^2: " << b*b << ", mod: " << (b * b) % mod << endl;
                }
            }
            //std::cout << "ans: " << ans << " " << "b: " << b << ".\n";
            return ans;
        }

        void write(const RSA &n, int num) {
            ofstream File;
            ull tmp[2] = {0};
            ull temp[2] = {0};
            if (num == 0)
                File.open("public.txt", ios::out);
            else
                File.open("private.txt", ios::out);
            //assert(this->len <= 2);
            
            for (int i = 3; i >= 0; i--) {
                temp[i / 2] <<= 32;
                temp[i / 2] += n.value[i];
            }

            for (int i = this->len - 1; i >= 0; i--) {
                tmp[i / 2] <<= 32;
                //cout << tmp[i / 2] << endl;
                tmp[i / 2] += this->value[i];
                //cout << tmp[i / 2] << endl;
            }

            File.write((char *)&temp, sizeof(ull) * 2);
            if (this->len > 2)
                File.write((char *)&tmp, sizeof(ull) * 2);
            else
                File.write((char *)&tmp, sizeof(ull));
            File.close();
        }

        friend std::ostream& operator << (std::ostream &os, const RSA &r);
};

std::ostream& operator << (std::ostream &os, const RSA &r) {
        int i;
        if (r.flag == true)
            os << "-";
        for (i = r.len - 1; i >= 0; i--) {
            os << r.value[i] << " ";
        }
        return os;
    }

bool MillerRabin(ull n);
void RandomPrime(ull &p, ull &q, ull e);
void euclid(const RSA &phi, const RSA &e, RSA &d, RSA &y);
void RSAGenerate();
#endif