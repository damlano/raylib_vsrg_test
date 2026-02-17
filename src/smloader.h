#pragma once
#include <bits/stdc++.h>

struct note_custom {
  int time; // in ms
  int lane; // remember lane 500 + (128*lane)w
  float y;
  bool hit;
  int snap;
};

void get_simfile_data(std::string path, std::vector<note_custom>& notes);