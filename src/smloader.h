#pragma once
#include <bits/stdc++.h>

using namespace std;

struct note_custom {
  int time; // in ms
  int lane; // remember lane 500 + (128*lane)w
  float y;
  bool hit;
  int snap;
};

void get_simfile_data(std::string path, std::vector<note_custom>& notes, string selected_difficulty, int selected_meter, string & music_path, string & artist, string & song_title, string & subtitle);