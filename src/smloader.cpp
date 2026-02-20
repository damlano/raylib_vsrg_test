#pragma GCC optimize("Ofast")

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
  int snap;
};

void get_simfile_data(std::string path, std::vector<note_custom>& notes, string selected_difficulty, int selected_meter, string & music_path, string & artist, string & song_title, string & subtitle){
    map<int, float> bpms;
    int beatcount = 0;
    int notes_section = -1; // -1 is not yet reached notes section 0 is its on and 6 means its done with the entire notes section
    float current_bpm = 0;
    int total_snaps = 0;
    float current_offset = 0;
    std::vector<std::string> snap_lines;
    ifstream f(path);
    bool should_read_notedata {false};
    bool correct_difficulty {false};
    bool correct_meter {false};

    if (!f.is_open()) {
        cerr << "Error opening the file!";
    }
    string s;
    string t;

    while (getline(f, s)){
        s.erase(std::remove(s.begin(), s.end(), '\r'), s.end());
        s.erase(std::remove(s.begin(), s.end(), '\n'), s.end());
        if (s.starts_with("#MUSIC:")){
            s.erase(s.begin(),s.begin()+7);
            s.erase(std::remove(s.begin(), s.end(), ';'), s.end());
            music_path = s;
        }

        if (s.starts_with("#ARTIST:")){
            s.erase(s.begin(),s.begin()+8);
            s.erase(std::remove(s.begin(), s.end(), ';'), s.end());
            artist = s;
        }

        if (s.starts_with("#TITLE:")){
            s.erase(s.begin(),s.begin()+7);
            s.erase(std::remove(s.begin(), s.end(), ';'), s.end());
            song_title = s;
        }

        if (s.starts_with("#SUBTITLE:")){
            s.erase(s.begin(),s.begin()+10);
            s.erase(std::remove(s.begin(), s.end(), ';'), s.end());
            subtitle = s;
        }

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
                    bpms[atoi(key.c_str())] = stof(value.c_str());
                }
            }
        }
        if (notes_section >=0 && notes_section <= 5) { 
            s.erase(std::remove(s.begin(), s.end(), ':'), s.end());
            s.erase(std::remove(s.begin(), s.end(), ' '), s.end());
            if (notes_section == 1){
                // to do: add it so that only dance-single cna be selected
            }

            if (notes_section == 2){
                if (selected_difficulty == s){
                    correct_difficulty = true;
                }
            }

            if (notes_section == 3){
                if (std::to_string(selected_meter) == s){
                    correct_meter = true;
                }
            }
            
            notes_section += 1; 

            if (correct_difficulty == true && correct_meter == true){
                should_read_notedata = true;
            }
        }

        if (s.starts_with("#NOTES")){ 
            notes_section=0;
            correct_difficulty = false;
            correct_meter = false;
        }

        if (should_read_notedata == true && notes_section == 6 && !s.starts_with(",")){
            total_snaps += 1;
            snap_lines.push_back(s);
        }

        if (should_read_notedata == true && notes_section == 6 && s.starts_with(",")|| should_read_notedata == true && notes_section == 6 && s.starts_with(";")){
            auto it = bpms.find(beatcount);
            if (it != bpms.end()){
                current_bpm = it->second;
            }

            float ms_per_beat = 60000.0f / current_bpm;
            float ms_per_measure = ms_per_beat * 4; // measure = 4 beats
            float ms_per_snap = ms_per_measure/total_snaps;
            int snapcount = 0;

            for(const string& i : snap_lines){
                for (size_t i2 = 0; i2 < i.size(); i2 += 1){
                    std::string test = i.substr(i2, 1);
                    if(test == "1"){
                        note_custom new_note;
                        new_note.time = current_offset;
                        new_note.lane = i2;
                        new_note.y = 0;
                        new_note.hit = false;
                        new_note.snap = snapcount;
                        notes.push_back(new_note);
                    }
                }
                snapcount++;
                current_offset += ms_per_snap;
            }
            beatcount += 1;
            total_snaps = 0;
            snap_lines.clear();
        }   
    }
    f.close();
}
