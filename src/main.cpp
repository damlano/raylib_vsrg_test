#pragma GCC optimize("Ofast")

#include "raylib.h"
#include "resource_dir.h"
#include <algorithm>
#include <vector>
#include "smloader.h"
#include "tracy/Tracy.hpp"

using namespace std;

#define NUM_TEXTURES 4
#define scrollspeed 1
#define receptor_y 950
#define receptor_x 700
#define SNAP_ARROWS 9
#define useautoplay false
#define scrolldirection -1
Music song;

// these can be floats
constexpr int g_fVisualDelaySeconds {0};
constexpr int musicrate {1};
constexpr int ARROW_SPACING {64};
constexpr int m_fScrollBPM {2000};
constexpr int m_fTimeSpacing {1};

int real_y;

float GetCmodY(note_custom& current_note, float current_time){ // this is stolen from etterna (i jsut changed func name and some variables alright LOL)
    float fYOffset{0};
    //float scale{1}; // scale of the receptors

		//float fPositionSeconds = current_time;
		float m_fMusicSecondsVisible = current_time - (g_fVisualDelaySeconds * musicrate);

		float fSongSeconds = m_fMusicSecondsVisible;
    float fNoteSeconds = current_note.time/1000.f;
		float fSecondsUntilStep = fNoteSeconds - fSongSeconds;
		float fBPM = m_fScrollBPM;
		float fBPS = fBPM / 60.f / musicrate;
		float fYOffsetTimeSpacing = fSecondsUntilStep * fBPS;

		fYOffset += fYOffsetTimeSpacing * m_fTimeSpacing;
		fYOffset *= ARROW_SPACING;

    if (scrolldirection == -1){
      return ((fYOffset *= scrollspeed)- real_y) * scrolldirection;
    }

		return ((fYOffset *= scrollspeed)+ real_y) *scrolldirection; // + receptor Y if upscroll - if downscroll
}

vector<note_custom> notes;
int future_note{ 0 };
int last_judge_time{ 0 };
int last_judge_type{0};
vector<key_lighting> key_down_lighting;
Texture2D textures_arrows_lit[NUM_TEXTURES] = {0};

Texture2D judgement_texture[5] = { 0 };
int timings[5] {17, 200, 300, 400,600};
int cur_judgements[5] {0,0,0,0,0};
// from left to right: marv/perf/great/boo/miss
float accuracy {0};
int combo;

bool comp(note_custom a, note_custom b) { return a.time < b.time; }

void set_hit(note_custom& current_note, int hit_type, int current_time){
    ZoneScoped;
    cur_judgements[hit_type] += 1;
    current_note.hit = true;
    future_note++;
    last_judge_time = current_time;
    last_judge_type = hit_type;
}

void hit(int lane, int currenttime){
  ZoneScoped;
  key_lighting current_keydown_light;
  current_keydown_light.lane = lane;
  current_keydown_light.time = currenttime;
  key_down_lighting.push_back(current_keydown_light);

for (int i{future_note}; i < notes.size(); i++) {
    note_custom &current_note = notes[i];
    if (current_note.lane != lane)[[likely]]{
      continue;
    }
    double timediffrence = (current_note.time - (currenttime)) * -1;
    if (timediffrence <= timings[0] && timediffrence >= timings[0]*-1){
        set_hit(current_note, 0, currenttime);
        combo += 1;
        break;
    }
    else if (timediffrence <= timings[1] && timediffrence >= timings[1]*-1){
        set_hit(current_note, 1, currenttime);
        combo += 1;
        break;
    }
    else if (timediffrence <= timings[2] && timediffrence >= timings[2]*-1){
        set_hit(current_note, 2, currenttime);
        combo += 1;
        break;
    }
    else if (timediffrence <= timings[3] && timediffrence >= timings[3]*-1){
        set_hit(current_note, 3, currenttime);
        combo = 0;
        break;
    }
    else if (timediffrence <= timings[4] && timediffrence >= timings[4]*-1){
        set_hit(current_note, 4, currenttime);
        combo = 0;
        break;
    }
  }
}


int main() {
  // read the file before launching the game its just better
  string music_path;
  string song_title;
  string subtitle;
  string artist;
  get_simfile_data("resources/10 Aim Burst.sm", notes,"Challenge" ,10,music_path, artist, song_title, subtitle);
  sort(notes.begin(), notes.end(), comp);

  InitWindow(1920, 1080, "raylib vsrg test"); // my dumbass didnt change the title OOPSIE
  SearchAndSetResourceDir("resources");
  ToggleFullscreen();
  SetTargetFPS(1000);
  InitAudioDevice();

  if (scrolldirection == -1){
    real_y = receptor_y;
  } else {
    real_y = GetScreenHeight() -receptor_y;
  }

  vector<int> rotations;
  rotations = vector<int>{90, 0, 180, -90};
  Texture2D textures[NUM_TEXTURES] = {0};
  Texture2D textures_arrow[SNAP_ARROWS][NUM_TEXTURES] = {0};
  Texture2D judgement_images[5];
  string step_info_text = format("Artist: {} Song: {} Stepper: {}",artist, song_title, subtitle);

  for (int i { 0 }; i < SNAP_ARROWS; i++){
    string filename = format("snapped_arrows/0{}.png", i);
    for (int i2 { 0 }; i2 < NUM_TEXTURES; i2++){
      Image arrow = LoadImage(filename.c_str());
      ImageRotate(&arrow, rotations[i2]);
      textures_arrow[i][i2] = LoadTextureFromImage(arrow);
      UnloadImage(arrow);
    }
  }
  
  for (int i { 0 }; i < NUM_TEXTURES; i++) {
    Image receptor_unlit = LoadImage("receptor_unlit.png");
    Image receptor_lit = LoadImage("receptor_lit.png");
    ImageRotate(&receptor_unlit, rotations[i]);
    ImageRotate(&receptor_lit, rotations[i]);
    textures[i] = LoadTextureFromImage(receptor_unlit);
    textures_arrows_lit[i] = LoadTextureFromImage(receptor_lit);
    UnloadImage(receptor_lit);
    UnloadImage(receptor_unlit);
  }

  for (int i { 0 }; i < 5; i++){
    string filename = format("judgements/0{}.png", i);
    Image judgement = LoadImage(filename.c_str());
    judgement_texture[i] = LoadTextureFromImage(judgement);
    UnloadImage(judgement);
  }
  song = LoadMusicStream(music_path.c_str());
  song.looping = false;
  PlayMusicStream(song);
  int song_length = GetMusicTimeLength(song);
  
  while (!WindowShouldClose()) {
    FrameMark;
    BeginDrawing();
    ClearBackground(BLACK);

    for (int i { 0 }; i < NUM_TEXTURES; i++) {
      DrawTexture(textures[i], receptor_x + (128*i), real_y, GRAY);
    }

    DrawFPS(10, 10);

    double currentTime = GetMusicTimePlayed(song)*1000;    

    for (int i{future_note}; i < notes.size(); i++) {
      note_custom &current_note = notes[i];
      double timediffrence = (current_note.time - currentTime) * -1;

      if (current_note.hit == false) [[likely]]{
        current_note.y = GetCmodY(current_note, currentTime/1000);
        DrawTexture(textures_arrow[current_note.snap % SNAP_ARROWS][current_note.lane],receptor_x + (128 * current_note.lane), current_note.y, WHITE);
      }

      if (scrolldirection == 1) {
          if (current_note.y >= GetScreenHeight() + 100) {
            break; 
        }
      } else { 
          if (current_note.y <= -100) {
            break;
          }
      }

      if (useautoplay == true) [[likely]]{
        if(timediffrence <= timings[0] && timediffrence >= timings[0]*-1) [[likely]]{
          hit(current_note.lane, currentTime);
        }
      }

      if (timediffrence >= timings[4] && current_note.hit == false)[[unlikely]] {
        current_note.hit = true;
        future_note++;
        cur_judgements[4] += 1;
        last_judge_time = currentTime;
        last_judge_type = 4;
        combo = 0;
      }  
    }

    if (useautoplay == true)[[likely]]{
      DrawText("Autoplay enabled", 1400, 500, 24, RED);
    }
    string combo_text = format("{}",combo);
    string test2 = format("Time remaining: {} / {:.0f}", song_length, currentTime/1000);

    DrawText(combo_text.c_str(), 950, 500, 28, RED);
    DrawText(step_info_text.c_str(), 600, 100, 28, BLUE);
    DrawText(test2.c_str(), 1300, 750, 24, WHITE);

    if (last_judge_time != 0 && (last_judge_time +250) >= currentTime) [[likely]]{
      DrawTexture(judgement_texture[last_judge_type], receptor_x, 300, WHITE);
    }

    for (auto it = key_down_lighting.begin(); it != key_down_lighting.end(); )
    {
        if ((it->time + 100) >= currentTime) [[likely]]{
            DrawTexture( textures_arrows_lit[it->lane], receptor_x + (128 * it->lane), real_y, WHITE);
            ++it;
        } else {
            it = key_down_lighting.erase(it);
        }
    }

    float total_hits = cur_judgements[0]+cur_judgements[1]+cur_judgements[2]+cur_judgements[3]+cur_judgements[4];
      if (total_hits > 0) [[likely]]{
        accuracy = (305*cur_judgements[0] + 300*cur_judgements[1] + 200*cur_judgements[2] + 50*cur_judgements[3]) / (305*total_hits) *100;
      } else {
        accuracy= 100.0; 
      }
      DrawText(TextFormat("%.2f", accuracy), receptor_x, 10, 18, RED);
    

    UpdateMusicStream(song);
    EndDrawing();

      if (IsKeyPressed(KEY_D)) {
        hit(0, currentTime);
      }
      if (IsKeyPressed(KEY_F)) {
        hit(1, currentTime);
      }
      if (IsKeyPressed(KEY_J)) {
        hit(2, currentTime);
      }
      if (IsKeyPressed(KEY_K)) {
        hit(3, currentTime);
      }
  }

  for (int i = 0; i < NUM_TEXTURES; i++){
    UnloadTexture(textures[i]);
  }
  for (int i = 0; i < SNAP_ARROWS; i++){
      for (int i2 = 0; i2 < NUM_TEXTURES; i2++)
          UnloadTexture(textures_arrow[i][i2]);
  }
  for (int i = 0; i < NUM_TEXTURES; i++){
      UnloadTexture(textures_arrows_lit[i]);
  }
  for (int i = 0; i < 5; i++){
      UnloadTexture(judgement_texture[i]);
  }
  UnloadMusicStream(song);
  CloseAudioDevice();

  CloseWindow();
  return 0;
}