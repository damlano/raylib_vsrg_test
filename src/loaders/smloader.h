#pragma once
#include <bits/stdc++.h>

using namespace std;


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

struct note_custom {
  int time; // in ms
  int lane; // remember lane receptor_x + (128*lane)
  float y;
  bool hit;
  int snap;
};

// splitted this function into get note data and this one for general data
simfile get_simfile_data(std::string path);
std::vector<note_custom> get_note_data(simfile & desired_simfile, int currentindex_diff); // to do chnage this to note_custom