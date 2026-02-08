#include <iostream>
#include <string>
#include <vector>
#include <string>
#include <bits/stdc++.h>
using namespace std;

struct note_custom {
  int time; // in ms
  int lane; // remember lane 500 + (128*lane)w
  float y;
  bool hit;
};

void get_simfile_data(string path, std::vector<note_custom[]>* notes){
    ifstream f(path);

    if (!f.is_open()) {
        cerr << "Error opening the file!";
    }
    string s;

    while (getline(f, s))
        cout << s << endl;

    f.close();
}
