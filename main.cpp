#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <queue>
#include <set>
#include <fstream>
#include "BitReader.h"
#include "BitWriter.h"
using namespace std;

ifstream fin("/Users/elizavetamanzula/Desktop/C++/праки/Хаффман/Генератор/input.txt");
ifstream kin("/Users/elizavetamanzula/Desktop/C++/праки/Хаффман/Генератор/input2.txt");

ofstream fout("/Users/elizavetamanzula/Desktop/C++/праки/Хаффман/Генератор/output.txt");
ofstream kout("/Users/elizavetamanzula/Desktop/C++/праки/Хаффман/Генератор/output2.txt");

bool isNumber(char c) {
    return c >= '0' && c <= '9';
}

map<char, int> tr;
set<char> A;

struct node {
    
    node () {
        key = '$';
        freq = 0;
        point0 = NULL;
        point1 = NULL;
        par = NULL;
    }
    
    int freq;
    char key;
    node* point0;
    node* point1;
    node* par;
    
};

node* merge(node* ft, node* sc) {
    node* ans = new node;
    
    ans->freq = ft->freq + sc->freq;
    ans->point0 = ft;
    ans->point1 = sc;
    ft->par = ans;
    sc->par = ans;
    
    return ans;
}

node* base(char c) {
    node* ans = new node;
    
    ans->key = c;
    ans->freq = tr[c];
    ans->point0 = NULL;
    ans->point1 = NULL;
    ans->par = NULL;
    
    return ans;
}

node* basefreq(int f) {
    node* ans = new node;
    
    ans->key = '$';
    ans->freq = f;
    ans->point0 = NULL;
    ans->point1 = NULL;
    ans->par = NULL;
    
    return ans;
}

node* basekeyfreq(char c, int f) {
    node* ans = new node;
    
    ans->key = c;
    ans->freq = f;
    ans->point0 = NULL;
    ans->point1 = NULL;
    ans->par = NULL;
    
    return ans;
}

node* build() {
    
    auto cmp = [](node* a, node* b) { return a->freq > b->freq; };
    
    priority_queue<node*, vector<node*>, decltype(cmp)> srt(cmp);
    
    for (char c : A) {
        srt.push(base(c));
    }
    
    if (srt.empty()) return NULL;
    
    while (srt.size() > 1) {
        node* ft = srt.top();
        srt.pop();
        node* sc = srt.top();
        srt.pop();
        
        srt.push(merge(ft, sc));
    }

    return srt.top();
}

void print(node* root) {
    if (root == NULL) return;
    if (root->par != NULL) cout << root->par->key << root->par->freq << " ";
    cout << root->key << root->freq << endl;
    print(root->point0);
    print(root->point1);
}

map<char, string> code;

void fill_code(node* root, node* t, string cur) {
    if (t == NULL) return;
    
    if (t->point0 == NULL && t->point1 == NULL) {
        if (t == root) cur = "0";
        code[t->key] = cur;
        return;
    }
    
    if (t->point0 != NULL) {
        fill_code(root, t->point0, cur + '0');
    }
    if (t->point1 != NULL) {
        fill_code(root, t->point1, cur + '1');
    }
}

void encrypt(string text, map<char, string> cd) {
    fout.close();
    BitWriter writer("/Users/elizavetamanzula/Desktop/C++/праки/Хаффман/Генератор/output.txt");
    for (auto c : text) {
        for (auto t : cd[c]) {
            writer << (t == '1');
        }
    }
    return;
}

string decrypt(node* root) {
    string ans = "";
    node* cur = root;
    kin.close();
    BitReader reader("/Users/elizavetamanzula/Desktop/C++/праки/Хаффман/Генератор/input2.txt");
    while(!reader.empty()) {
        bool b;
        reader >> b;
        if (root->point0 != NULL || root->point1 != NULL) {
            if (!b) {
                cur = cur->point0;
            }
            if (b) {
                cur = cur->point1;
            }
        }
        if (cur->point0 == NULL && cur->point1 == NULL) {
            ans += cur->key;
            fout << (unsigned char)cur->key;
            cout << (int)cur->key<< " ";
            cur = root;
        }
    }
    return ans;
}

node* root;

void out(node* roo) {
    if (roo == NULL) {
        fout << "N ";
        return;
    }
    fout << roo->freq << " ";
    if (roo->point0 == NULL && roo->point1 == NULL) {
        fout << "-1 " << roo->key << " ";
        return;
    }
    out(roo->point0);
    out(roo->point1);
}

int i = 0;
node* dfs(string s, node* par) {
    if (s[0] == '-') return NULL;
    int cur = 0;
    while (i < s.size() && s[i] == ' ') ++i;
    while (isNumber(s[i])) {
        cur = cur * 10 + (s[i++] - '0');
    }
    ++i;
    if (s[i] == '-') {
        i += 3;
        char c = s[i];
        ++i;
        node* ans = basekeyfreq(c, cur);
        ans->par = par;
        return ans;
    }
    node* ans = basefreq(cur);
    ans->par = par;
    ans->point0 = dfs(s, ans);
    ans->point1 = dfs(s, ans);
    return ans;
}

void CODE() {

    string s;
    getline(fin, s, '\0');
    
    for (int i = 0; i < s.size(); ++i) {
        tr[s[i]]++;
        A.insert(s[i]);
    }
    
    root = build();
    if (root != NULL) out(root);
    else fout << -1 << endl;
    
    fill_code(root, root, "");
    for(char c : A) {
        kout << c << " " << code[c] << endl;
    }
}

void ENCR() {

    char ch;
    while(fin.get(ch)){
        string s;
        fin >> s;
        code[ch] = s;
        cout << int(ch) << " " << s << '\n';
        fin.ignore();
    }
    
    string text;
    getline(kin, text, '\0');
    
    encrypt(text, code);
}

void DECR() {
    
    string s_tree;
    getline(fin, s_tree, '\0');
    root = dfs(s_tree, NULL);
    cout << endl;
    decrypt(root);
}

int main() {
    
    //CODE();
    //ENCR();
    DECR();
    
    return 0;
}
