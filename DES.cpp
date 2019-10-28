#include<iostream>
#include<stdio.h>
#include<bitset>
#include<string>
#include<fstream>
#include<assert.h>
#include<malloc.h>
#include<memory.h>
#include<stdlib.h>
#include<time.h>
using namespace std;
#define ENCRYPT 0
#define DECRYPT 1

bitset<64> Key;
bitset<48> Keys[16];

int IP[] = {57,	49,	41,	33,	25,	17,	 9,	 1,	59,	51,	43,	35,	27,	19,	11,	 3,
			61,	53,	45,	37,	29,	21,	13,	 5,	63,	55,	47,	39,	31,	23,	15,	 7,
			56,	48,	40,	32,	24,	16,	 8,	 0,	58,	50,	42,	34,	26,	18,	10,	 2,
			60,	52,	44,	36,	28,	20,	12,	 4,	62,	54,	46,	38,	30,	22,	14,	 6 };
// 64->56
int IPC[] = {56, 48, 40, 32, 24, 16,  8,  0, 57, 49, 41, 33, 25, 17,  9,  1,
			 58, 50, 42, 34, 26, 18, 10,  2, 59, 51, 43, 35, 62, 54, 46, 38,
			 30, 22, 14,  6, 61, 53, 45, 37, 29, 21, 13,  5, 60, 52, 44, 36,
			 28, 20, 12,  4, 27, 19, 11,  3 };
// 56->48
int PC[] = { 13, 16, 10, 23,  0,  4,  2, 27, 14,  5, 20,  9, 22, 18, 11,  3,
			 25,  7, 15,  6, 26, 19, 12,  1, 40, 51, 30, 36, 46, 54, 29, 39,
			 50, 44, 32, 47, 43, 48, 38, 55, 33, 52, 45, 41, 49, 35, 28, 31 };
// 32->48
int E[] = { 31,  0,  1,  2,  3,  4,  3,  4,  5,  6,  7,  8,  7,  8,  9, 10,
			11, 12, 11, 12, 13, 14, 15, 16, 15, 16, 17, 18, 19, 20, 19, 20,
			21,	22,	23,	24,	23,	24,	25,	26,	27,	28,	27,	28,	29,	30,	31,  0 };
// 6->4
int S_BOX[][4][16] = {
	{  
		{14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7},  
		{0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8},  
		{4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0}, 
		{15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13} 
	},
	{  
		{15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10},  
		{3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5}, 
		{0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15},  
		{13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9}  
	}, 
	{  
		{10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8},  
		{13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1},  
		{13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7},  
		{1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12}  
	}, 
	{  
		{7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15},  
		{13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9},  
		{10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4},  
		{3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14}  
	},
	{  
		{2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9},  
		{14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6},  
		{4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14},  
		{11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3}  
	},
	{  
		{12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11},  
		{10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8},  
		{9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6},  
		{4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13}  
	}, 
	{  
		{4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1},  
		{13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6},  
		{1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2},  
		{6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12}  
	}, 
	{  
		{13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7},  
		{1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2},  
		{7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8},  
		{2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11}  
	} 
};

int P[] = { 15,	 6,	19,	20,	28,	11,	27,	16,	 0,	14,	22,	25,	 4,	17,	30,	 9,
 			 1,	 7,	23,	13,	31,	26,	 2,	 8,	18,	12,	29,	 5,	21,	10,	 3,	24 };

int IP_1[] = { 39,  7, 47, 15, 55, 23, 63, 31, 38,  6, 46, 14, 54, 22, 62, 30,
			   37,	5, 45, 13, 53, 21, 61, 29, 36,  4, 44, 12, 52, 20, 60, 28,
			   35,	3, 43, 11, 51, 19, 59, 27, 34,  2, 42, 10, 50, 18, 58, 26,
			   33,	1, 41,	9, 49, 17, 57, 25, 32,  0, 40,  8, 48, 16, 56, 24 };
// left
int shiftBits[] = {1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1};

bitset<32> F(bitset<32> R, bitset<48> Key) {
	bitset<48> expandR;
	for(int i = 0; i < 48; i++)
		expandR[i] = R[E[i]];
	
	expandR = expandR ^ Key;
	
	bitset<32> output;
	int count = 0;
	//(x0, x1, ..., x5) (x0*2+x5, x1*8+...+x4)
	for(int i = 0; i < 48; i += 6) {
		int row = expandR[i]*2 + expandR[i+5];
		int col = expandR[i+1]*8 + expandR[i+2]*4 + expandR[i+3]*2 + expandR[i+4];
		int num = S_BOX[i/6][row][col];
		bitset<4> tmp(num);
		output[count] = tmp[0];
		output[count+1] = tmp[1];
		output[count+2] = tmp[2];
		output[count+3] = tmp[3];
		count += 4;
	}
	
	bitset<32> tmp = output;
	for(int i = 0; i < 32; i++)
		output[i] = tmp[P[i]];
	return output;
}

bitset<28> leftshift(bitset<28> Key, int shift) {
	bitset<28> output;
	for(int i = 0; i < 28; i++)
		output[(28 - shift + i) % 28] = Key[i];
	return output;
}

void KeyGenerate() {
	bitset<56> tmpKey;
	bitset<28> left, right;
	bitset<48> CompressKey;
	for (int i = 0; i < 56; i++)
		tmpKey[i] = Key[IPC[i]];

	for (int i = 0; i < 16; i++) {
		for(int j = 0; j < 28; j++) {
			left[j] = tmpKey[j];
			right[j] = tmpKey[j + 28];
		}
		left = leftshift(left, shiftBits[i]);
		right = leftshift(right, shiftBits[i]);
		for (int j = 0; j < 28; j++) {
			tmpKey[j] = left[j];
			tmpKey[j + 28] = right[j];
		}
		for (int j = 0; j < 48; j++)
			CompressKey[j] = tmpKey[PC[j]];
		Keys[i] = CompressKey;
	}
}

void char2Bit(const char* s, bitset<64>& bits) {
	for(int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			bits[8*i+j] = ((s[i] >> j) & 1);
}

void bit2Char(const bitset<64>& bits, char* s) {
	for(int i = 0; i < 8; i++)
		for(int j = 0; j < 8; j++)
			s[i] |= bits[8*i+j] << j;
}

void crypt(bitset<64>& input, bitset<64>& output, bool flag) {
	bitset<32> left, right, newleft;
	bitset<64> tmp;
	for(int i = 0; i < 64; i++)
		tmp[i] = input[IP[i]];
	for(int i = 0; i < 32; i++) {
		left[i] = tmp[i];
		right[i] = tmp[i + 32];
	}
	// flag == 0 encrypt / == 1 decrypt
	if(flag == 0)
		for(int i = 0; i < 16; i++) {
			newleft = right;
			right = left ^ F(right, Keys[i]);
			left = newleft;
		}
	else
		for(int i = 0; i < 16; i++) {
			newleft = right;
			right = left ^ F(right, Keys[15 - i]);
			left = newleft;
		}
	
	// R L
	for(int i = 0; i < 32; i++) {
		output[i + 32] = left[i];
		output[i] = right[i];
	}
	tmp = output;
	for(int i = 0; i < 64; i++)
		output[i] = tmp[IP_1[i]];
}

long long getrand() {
    long long R;
    for(int i = 0; i < 4; i++) {
        R = R << 16;
        R += (long long)rand();
    }
    return R;
}

int main() {
	int num, choice;
	string k;
	ofstream out;
	string filename;

	cout << "Welcome to the DES-cryption system!\n";
	cout << "Input 0 indicates encryption, while 1 means decryption.\n";
	cin >> num;
	cout << "Input 0 indicates input the Keys while 1 means reading from the given file.\n";
	cin >> choice;
	if (choice == 0) {
		cout << "Input the Keys.\n";
		cin >> k;
		char2Bit(k.c_str(), Key);
	} else {
		cout << "Input the URL to read keys";
		string url;
		cin >> url;
		ifstream RSA(url, ios::binary);
		RSA.read((char*)&Key, sizeof(char)*8);
	}
	KeyGenerate();
	cout << "Which file?\n";
	cin >> filename;
	ifstream in(filename, ios::binary);
	if(num == 0)
		out.open("cipher.txt", ios::binary);
	else
		out.open("reverse.txt", ios::binary);

	char outputtext[9];
	bitset<64> plain;
	bitset<8> tmp;
	int count = 0;
    int fill = 0;
    srand((unsigned)time(NULL));
    bitset<64> cipher(getrand());
    bitset<64> IV = cipher;
    bitset<64> tofill(getrand());
    in.seekg(0, in.end);
	int sum = in.tellg();
	in.seekg(0, in.beg);
	bitset<64> temp;
	
	if(num == 0) {
		crypt(IV, temp, num);
		memset(outputtext, 0, 9);
		bit2Char(temp, outputtext);
		out.write((char*)&outputtext, sizeof(char) * 8);
	} else {
		in.read((char*)&temp, sizeof(temp));
		crypt(temp, IV, num);
	}
	while(in.read((char *)&tmp, sizeof(char))) {
		//cout << "Hello in while" << endl;
        for(int i = 0; i < 8; i++)
            plain[(count % 8)*8+i] = tmp[i];
        count++;
        if(count % 8 == 0) {
			if(num == 0) {
				plain = plain ^ cipher;
				crypt(plain, cipher, num);
				memset(outputtext, 0, 9);
				bit2Char(cipher, outputtext);
				out.write((char *)&outputtext, sizeof(char) * 8);
				//cout << "write" << endl;
				plain.reset();
			} else {
				crypt(plain, cipher, num);
				cipher = cipher ^ IV;
				IV = plain;
				memset(outputtext, 0, 9);
				bit2Char(cipher, outputtext);
				out.write((char *)&outputtext, sizeof(char) * 8);
				//cout << "write" << endl;
				plain.reset();
			}
        } else if(count == sum) {
            fill = 8 - count % 8;
			//cout << fill << endl;
			//cout << plain << endl << count << endl;;
            for(int i = count % 8; i < 7; i++)
                for(int j = 0; j < 8; j++)
                    plain[i*8+j] = tofill[i*8+j];
			//cout << plain << endl;
            bitset<8> mark(fill*8);
            for(int i = 0; i < 8; i++)
                plain[56+i] = mark[i];
			plain = plain ^ cipher;
			//cout << plain << endl;;
			crypt(plain, cipher, num);
			memset(outputtext, 0, 9);
			bit2Char(cipher, outputtext);
			out.write((char *)&outputtext, sizeof(char) * 8);
			plain.reset();
        }
    }
	in.close();
	out.close();
	/*while(in.read((char *)&plain, sizeof(plain))) {
		cout << sizeof(plain) << endl;
		cout << plain << " " << "plain[0]: " << plain[0] << " " << "plain[7]: " << plain[7] << endl;
		memset(outputtext, 0, 9);
		//char2Bit(input.c_str(), plain);
		crypt(plain, cipher, num);
		bit2Char(cipher, outputtext);
		//cout << outputtext;

		out.write((char *)&outputtext, sizeof(char) * 8);
		plain.reset();
	}
	in.close();
	out.close();*/
}

/*int main() {
	string s = "romantic";
	string k = "12345678";
	string output = "";
	string tmp = "";
	char tmptext[9], outputtext[9];
	bitset<64> plain, cipher, newplain;
	memset(tmptext, 0, 9);
	memset(outputtext, 0, 9);

	char2Bit(s.c_str(), plain);
	//bit2Char(plain, tmptext);
	//cout << "tmptext: " << tmptext << endl;
	//tmp += tmptext;
	//cout << "Hello before reverse\n";
	//cout << "reverse " << tmp << "\n";
	//cout << "Hello after reverse\n";
	char2Bit(k.c_str(), Key);
	//cout << "Hello after Key\n";
	generateKey();
	//cout << "Hello after generate\n";
	crypt(plain, cipher, ENCRYPT);
	//cout << "Hello after encrypt\n";
	//bit2Char(cipher, tmptext);
	//cout << "cipher: " << tmp << "\n";
	crypt(cipher, newplain, DECRYPT);
	bit2Char(newplain, outputtext);
	cout << "newplain: " << outputtext << "\n";
	//assert(plain == newplain);
	return 0;
}*/
