#include "raylib.h"
#include "resource_dir.h"
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <vector>

#define NUM_TEXTURES 4
#define scrollspeed 1.5 // how much pixels per second
#define receptor_y 950
#define receptor_x 500

struct note_custom {
  int time; // in ms
  int lane; // remember lane 500 + (128*lane)w
  float y;
  bool enabled;
  bool hit;
};

std::vector<note_custom> notes;
int future_note{ 0 };
int last_hit_time{ 0 };
int last_key_hit_time {0};
int last_key_hit_lane {0};
Texture2D textures_lit[NUM_TEXTURES] = {0};
Texture judgement = { 0 };

bool comp(note_custom a, note_custom b) { return a.time < b.time; }

void hit(int lane){
  for (int i{future_note}; i < notes.size(); i++) {
      note_custom &current_note = notes[i];
      double timediffrence = (current_note.time - (GetTime() * 1000)) * -1;
      last_key_hit_time = GetTime()*1000;
      last_key_hit_lane = lane;
      if (current_note.lane == lane){
        if (timediffrence <= 500 && timediffrence >= -500){
          current_note.hit = true;
          future_note++;
          last_hit_time = GetTime()*1000;
          break;
        }
        break;
      }
  }
}


int main() {
  InitWindow(1920, 1080, "Hello Raylib");
  SearchAndSetResourceDir("resources");
  ToggleFullscreen();
  SetTargetFPS(250);

  std::vector<int> rotations;
  rotations = std::vector<int>{90, 0, 180, -90};
  Texture2D textures[NUM_TEXTURES] = {0};
  Texture2D textures_arrow[NUM_TEXTURES] = {0};
  Image arrow = LoadImage("snapped_arrows/00.png");
  Image receptor_unlit = LoadImage("receptor_unlit.png");
  Image receptor_lit = LoadImage("receptor_lit.png");

  for (int i { 0 }; i < NUM_TEXTURES; i++) {
    ImageRotate(&receptor_unlit, rotations[i]);
    ImageRotate(&receptor_lit, rotations[i]);
    ImageRotate(&arrow, rotations[i]);
    textures[i] = LoadTextureFromImage(receptor_unlit);
    textures_arrow[i] = LoadTextureFromImage(arrow);
    textures_lit[i] = LoadTextureFromImage(receptor_lit);
  }

  UnloadImage(arrow);
  UnloadImage(receptor_lit);
  UnloadImage(receptor_unlit);

  Image judgement_img = LoadImage("judgements/00.png");
  judgement = LoadTextureFromImage(judgement_img);

  for (int i{0}; i <= 100; i++) {
    note_custom new_note;
    new_note.time = rand() % 3000 + 3000;
    new_note.lane = rand() % 4;
    new_note.y = 0;
    new_note.enabled = false;
    new_note.hit = false;
    notes.push_back(new_note);
  }

  std::sort(notes.begin(), notes.end(), comp);

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);

    for (int i { 0 }; i < NUM_TEXTURES; i++) {
      DrawTexture(textures[i], receptor_x + (128*i), receptor_y, GRAY);
    }

    DrawFPS(10, 10);

    for (int i{future_note}; i < notes.size(); i++) {
      note_custom &current_note = notes[i];
      double test = GetTime() * 1000;
      double timediffrence = (current_note.time - (GetTime() * 1000)) * -1;

      if (current_note.hit == false) {
        DrawTexture(textures_arrow[current_note.lane],
                    500 + (128 * current_note.lane), current_note.y, WHITE);
        current_note.y = (timediffrence + 950) * scrollspeed;
      }

      if (timediffrence >= 120) {
        current_note.hit = true;
        future_note++;
        timediffrence = 0;
      }
    }

    if ((last_hit_time +100) >= GetTime()*1000){
      DrawTexture(judgement, receptor_x, 300, WHITE);
    }
    
    if ((last_key_hit_time +100) >= GetTime()*1000){
        DrawTexture(textures_lit[last_key_hit_lane], receptor_x + (128*last_key_hit_lane), receptor_y, WHITE);
    }

    EndDrawing();

    if (IsKeyPressed(KEY_D)) {
        
        hit(0);
      }
      if (IsKeyPressed(KEY_F)) {
        hit(1);
      }
      if (IsKeyPressed(KEY_J)) {
        hit(2);
      }
      if (IsKeyPressed(KEY_K)) {
        hit(3);
      }
  }

  for (int i = 0; i < NUM_TEXTURES; i++)
    UnloadTexture(textures[i]);
  for (int i = 0; i < NUM_TEXTURES; i++)
    UnloadTexture(textures_arrow[i]);
  for (int i = 0; i < NUM_TEXTURES; i++)
    UnloadTexture(textures_lit[i]);

  CloseWindow();
  return 0;
}
