#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <string>
#include <bits/stdc++.h>
#include <string>
#include <iostream>
#include <algorithm>

using namespace std;

struct note_custom {
  int time; // in ms
  int lane; // remember lane 500 + (128*lane)w
  float y;
  bool hit;
};

void get_simfile_data(string path, std::vector<note_custom[]>* notes){
    map<int, float> bpms;
    int beatcount = 0;
    int snapcount = 0;
    int notes_section = -1; // -1 is not yet reached notes section 0 is its on and 6 means its done with the entire notes section
    // float ms_per_beat = 60000.0f / beats
    ifstream f(path);

    if (!f.is_open()) {
        cerr << "Error opening the file!";
    }
    string s;
    string t;

    while (getline(f, s)){
        cout << s << endl;
        if (s.starts_with("#BPMS")){
            char chars[] = "#BPMS:;";

            for (unsigned int i = 0; i < strlen(chars); ++i)
            {
                s.erase (std::remove(s.begin(), s.end(), chars[i]), s.end());
            }

            std::stringstream ss(s);
            std::string token;

            while (std::getline(ss, token, ',')) {
                std::stringstream ss2(token);
                std::string key;
                std::string value;

                if (std::getline(ss2, key, '=') && std::getline(ss2, value)) {
                    bpms[atoi(key.c_str())] = atoi(value.c_str());
                }
            }
        }
        if (notes_section >=0 && notes_section <=5) { notes_section += 1; }
        if (s.starts_with("#NOTES")){ notes_section=0; }

        if (notes_section == 6){
            std::cout << "its workingg";
        }
        
    }
    f.close();



}
