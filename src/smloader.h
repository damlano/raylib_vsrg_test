#pragma once
#include <bits/stdc++.h>

using namespace std;

struct note_custom {
  int time; // in ms
  int lane; // remember lane receptor_x + (128*lane)
  float y;
  bool hit;
  int snap;
};

struct key_lighting {
  float time; // in ms
  int lane; 
};

void get_simfile_data(std::string path, std::vector<note_custom>& notes, string selected_difficulty, int selected_meter, string & music_path, string & artist, string & song_title, string & subtitle);