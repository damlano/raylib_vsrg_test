#pragma GCC optimize("Ofast")

#include <sstream>
#include <vector>
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

struct simfile {
    string base_path;
    string ARTIST;
    string CREDIT; // usually the stepper
    string TITLE;
    string MUSIC; // path to music
    string GENRE;
    string BANNER;
    string BACKGROUND;
    string CDTITLE;

    map<int, float> bpms;
    float DISPLAYBPM;
    float SAMPLESTART; // seconds
    float SAMPLELENGTH; // seconds
    float offset; // seconds

    std::vector<std::string> difficulties;
    std::vector<int> meter;
    std::vector<std::streampos> note_line; // line at which the note section starts
};

bool comp(note_custom a, note_custom b) { return a.time < b.time; }

simfile get_simfile_data(std::string path){
    simfile current_file;
    current_file.base_path = std::format("{}", path);
    int notes_section{-1};
    ifstream f(path);

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
            current_file.MUSIC = s;
        }

        if (s.starts_with("#ARTIST:")){
            s.erase(s.begin(),s.begin()+8);
            s.erase(std::remove(s.begin(), s.end(), ';'), s.end());
            current_file.ARTIST = s;
        }

        if (s.starts_with("#TITLE:")){
            s.erase(s.begin(),s.begin()+7);
            s.erase(std::remove(s.begin(), s.end(), ';'), s.end());
            current_file.TITLE = s;
        }

        if (s.starts_with("#CREDIT:")){
            s.erase(s.begin(),s.begin()+8);
            s.erase(std::remove(s.begin(), s.end(), ';'), s.end());
            current_file.CREDIT = s;
        }

        if (s.starts_with("#BANNER:")){
            s.erase(s.begin(),s.begin()+8);
            s.erase(std::remove(s.begin(), s.end(), ';'), s.end());
            current_file.BANNER = s;
        }

        if (s.starts_with("#BACKGROUND:")){
            s.erase(s.begin(),s.begin()+12);
            s.erase(std::remove(s.begin(), s.end(), ';'), s.end());
            current_file.BACKGROUND = s;
        }

        if (s.starts_with("#CDTITLE:")){
            s.erase(s.begin(),s.begin()+9);
            s.erase(std::remove(s.begin(), s.end(), ';'), s.end());
            current_file.CDTITLE = s;
        }
        if (s.starts_with("#DISPLAYBPM:")){
            s.erase(s.begin(),s.begin()+12);
            s.erase(std::remove(s.begin(), s.end(), ';'), s.end());
            if (s == "") {
            current_file.DISPLAYBPM = stof(s);
            } else {
                current_file.DISPLAYBPM = -100;
            }
        }
        if (s.starts_with("#OFFSET:")){
            s.erase(s.begin(),s.begin()+8);
            s.erase(std::remove(s.begin(), s.end(), ';'), s.end());
            current_file.offset = stof(s);
        }
        if (s.starts_with("#SAMPLESTART:")){
            s.erase(s.begin(),s.begin()+13);
            s.erase(std::remove(s.begin(), s.end(), ';'), s.end());
            current_file.SAMPLESTART = stof(s);
        }

        if (s.starts_with("#SAMPLELENGTH:")){
            s.erase(s.begin(),s.begin()+14);
            s.erase(std::remove(s.begin(), s.end(), ';'), s.end());
            current_file.SAMPLELENGTH = stof(s);
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
                    current_file.bpms[atoi(key.c_str())] = stof(value.c_str());
                }
            }
        }
        if (notes_section >=0 && notes_section <= 5) { 
            s.erase(std::remove(s.begin(), s.end(), ':'), s.end());
            s.erase(std::remove(s.begin(), s.end(), ' '), s.end());

            if (notes_section == 2){
                current_file.difficulties.push_back(s);
            }

            if (notes_section == 3){
                current_file.meter.push_back(stoi(s));
            }

            if (notes_section == 4){ // line where the note objects start
                current_file.note_line.push_back(f.tellg());
            }
            notes_section += 1; 
        }

        if (s.starts_with("#NOTES")){ 
            notes_section=0;
        }
    }
    f.close();
    return current_file;
}

std::vector<note_custom> get_note_data(simfile & desired_simfile, int currentindex_diff){
    std::filesystem::path simfile_path;
    simfile_path = desired_simfile.base_path;
    std::string line;
    std::vector<std::string> snap_lines;
    std::vector<note_custom> notes;

    int beatcount = 0;
    float current_bpm = 0;
    int total_snaps = 0;
    float current_offset {desired_simfile.offset*1000};

    ifstream f(simfile_path);
    f.seekg(desired_simfile.note_line[currentindex_diff]);

    while (getline(f, line)){
        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
        line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());

        if (line.starts_with("#NOTES")){ 
            sort(notes.begin(), notes.end(), comp);
            return notes;
        }

        if (line.starts_with(',')){ // beat seperator is ,
            auto it = desired_simfile.bpms.find(beatcount);
            if (it != desired_simfile.bpms.end()){
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

        } else {
            total_snaps += 1;
            snap_lines.push_back(line);
        }


    }
    sort(notes.begin(), notes.end(), comp);
    return notes; // either we return whjen #NOTES is found as thats the next difficulty OR at EOF
}