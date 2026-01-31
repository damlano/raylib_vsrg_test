#include "raylib.h"
#include "resource_dir.h"
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <vector>

#define NUM_TEXTURES 4
#define scrollspeed 1
#define receptor_y 950
#define receptor_x 500

struct note_custom {
  int time; // in ms
  int lane; // remember lane 500 + (128*lane)w
  float y;
  bool hit;
};


/*
todo:
make hit lightning multiple keys at once not only 1 key it looks cursed!
fix scrollspeed (idfk how to do this mannnn)
*/ 

std::vector<note_custom> notes;
int future_note{ 0 };
int last_judge_time{ 0 };
int last_hit_type{0};
int last_key_hit_time {0};
int last_key_hit_lane {0};
Texture2D textures_arrows_lit[NUM_TEXTURES] = {0};

Texture2D judgement_texture[5] = { 0 };
int timings[5] {100, 200, 300, 400,600};
int cur_judgements[5] {0,0,0,0,0};
double accuracy {0};
// from left to right: marv/perf/great/boo/miss

bool comp(note_custom a, note_custom b) { return a.time < b.time; }

void set_hit(note_custom current_note, int hit_type){
    cur_judgements[hit_type] += 1;
    current_note.hit = true;
    future_note++;
    last_judge_time = GetTime()*1000;
    last_hit_type = hit_type;
}

void hit(int lane){
  for (int i{future_note}; i < notes.size(); i++) {
      note_custom &current_note = notes[i];
      double timediffrence = (current_note.time - (GetTime() * 1000)) * -1;
      last_key_hit_time = GetTime()*1000;
      last_key_hit_lane = lane;
      if (current_note.lane == lane){
        std::cout << timediffrence << std::endl;
        if (timediffrence <= timings[0] && timediffrence >= timings[0]*-1){
            set_hit(current_note, 0);
            break;
        }
        else if (timediffrence <= timings[1] && timediffrence >= timings[1]*-1){
            set_hit(current_note, 1);
            break;
        }
        else if (timediffrence <= timings[2] && timediffrence >= timings[2]*-1){
            set_hit(current_note, 2);
            break;
        }
        else if (timediffrence <= timings[3] && timediffrence >= timings[3]*-1){
            set_hit(current_note, 3);
            break;
        }
        else if (timediffrence <= timings[4] && timediffrence >= timings[4]*-1){
            set_hit(current_note, 4);
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
  
  for (int i { 0 }; i < NUM_TEXTURES; i++) {
    Image arrow = LoadImage("snapped_arrows/00.png");
    Image receptor_unlit = LoadImage("receptor_unlit.png");
    Image receptor_lit = LoadImage("receptor_lit.png");
    ImageRotate(&receptor_unlit, rotations[i]);
    ImageRotate(&receptor_lit, rotations[i]);
    ImageRotate(&arrow, rotations[i]);
    textures[i] = LoadTextureFromImage(receptor_unlit);
    textures_arrow[i] = LoadTextureFromImage(arrow);
    textures_arrows_lit[i] = LoadTextureFromImage(receptor_lit);
    UnloadImage(arrow);
    UnloadImage(receptor_lit);
    UnloadImage(receptor_unlit);
  }

  // i should clean this entire judgement image spam up and use string formatting i know.... i wasnt aware of this before alright!
  Image judgement_img = LoadImage("judgements/00.png");
  Image judgement_img1 = LoadImage("judgements/01.png");
  Image judgement_img2 = LoadImage("judgements/02.png");
  Image judgement_img3 = LoadImage("judgements/04.png");
  Image judgement_img4 = LoadImage("judgements/05.png");

  Image judgement_images[5]{judgement_img, judgement_img1,judgement_img2, judgement_img3,judgement_img4};

  for (int i { 0 }; i < 5; i++) {
    judgement_texture[i] = LoadTextureFromImage(judgement_images[i]);
    UnloadImage(judgement_images[i]);
  }

  for (int i{0}; i <= 10; i++) {
    note_custom new_note;
    new_note.time = rand() % 3000 + 3000;
    new_note.lane = rand() % 4;
    new_note.y = 0;
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
        current_note.y = (timediffrence + receptor_y);
      }

      if (timediffrence >= timings[4]) {
        current_note.hit = true;
        future_note++;
        cur_judgements[4] += 1;
        last_judge_time = GetTime()*1000;
        last_hit_type = 4;
        timediffrence = 0;
      }

      if (current_note.time <= GetTime()*1000){
        timediffrence = 0;
      }
    }

    if ((last_judge_time +100) >= GetTime()*1000){
      DrawTexture(judgement_texture[last_hit_type], receptor_x, 300, WHITE);
    }
    
    if ((last_key_hit_time +100) >= GetTime()*1000){
        DrawTexture(textures_arrows_lit[last_key_hit_lane], receptor_x + (128*last_key_hit_lane), receptor_y, WHITE);
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
      int total_hits = cur_judgements[0]+cur_judgements[1]+cur_judgements[2]+cur_judgements[3]+cur_judgements[4];
      if (total_hits > 0){
        accuracy = (305*cur_judgements[0] + 300*cur_judgements[1] + 200*cur_judgements[2] + 50*cur_judgements[3]) / (305*total_hits) *100;
      } else {
        accuracy= 100.0; 
      }
      DrawText(TextFormat("%.2f", accuracy), receptor_x, 10, 18, RED);
  }

  for (int i = 0; i < NUM_TEXTURES; i++)
    UnloadTexture(textures[i]);
  for (int i = 0; i < NUM_TEXTURES; i++)
    UnloadTexture(textures_arrow[i]);
  for (int i = 0; i < NUM_TEXTURES; i++)
    UnloadTexture(textures_arrows_lit[i]);
  for (int i = 0; i < NUM_TEXTURES; i++)
    UnloadTexture(judgement_texture[i]);

  CloseWindow();
  return 0;
}
