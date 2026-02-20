#include "raylib.h"
#include "resource_dir.h"
#include <algorithm>
#include <iostream>
#include <ostream>
#include <vector>
#include "smloader.h"

using namespace std;

// big to do: call the clock less bcuz this is just eating performance probably LMFAO
// fixed the typo lmfao

#define NUM_TEXTURES 4
#define scrollspeed 1
#define receptor_y 950
#define receptor_x 700
#define SNAP_ARROWS 9
#define useautoplay true

Music song;

float GetCmodY(note_custom& current_note){ // this is stolen from etterna (i jsut changed func name and some variables alright LOL)
		float m_fScrollBPM {1255};
    float m_fTimeSpacing {1};
		float ARROW_SPACING {64};
		float g_fVisualDelaySeconds {0};
    float fYOffset{0};
		float musicrate {1};
    //float scale{1}; // scale of the receptors

		float fPositionSeconds = GetMusicTimePlayed(song);
		float m_fMusicSecondsVisible = fPositionSeconds - (g_fVisualDelaySeconds * musicrate);

		float fSongSeconds = m_fMusicSecondsVisible;
    float fNoteSeconds = current_note.time/1000.f;
		float fSecondsUntilStep = fNoteSeconds - fSongSeconds;
		float fBPM = m_fScrollBPM;
		float fBPS = fBPM / 60.f / musicrate;
		float fYOffsetTimeSpacing = fSecondsUntilStep * fBPS;

		fYOffset += fYOffsetTimeSpacing * m_fTimeSpacing;
		fYOffset *= ARROW_SPACING;

		if (fYOffset < 0) {
			return ((fYOffset *= scrollspeed)- receptor_y) *-1;
		}

		return ((fYOffset *= scrollspeed)- receptor_y) *-1;
}


/*
todo:
make hit lightning multiple keys at once not only 1 key it looks cursed!
make the *-1 a variable to allow both up and downscroll
*/ 

vector<note_custom> notes;
int future_note{ 0 };
int last_judge_time{ 0 };
int last_hit_type{0};
int last_key_hit_time {0};
int last_key_hit_lane {0};
Texture2D textures_arrows_lit[NUM_TEXTURES] = {0};

Texture2D judgement_texture[5] = { 0 };
int timings[5] {17, 200, 300, 400,600};
int cur_judgements[5] {0,0,0,0,0};
double accuracy {0};
int combo;
// from left to right: marv/perf/great/boo/miss

bool comp(note_custom a, note_custom b) { return a.time < b.time; }

void set_hit(note_custom& current_note, int hit_type){
    cur_judgements[hit_type] += 1;
    current_note.hit = true;
    future_note++;
    last_judge_time = GetMusicTimePlayed(song)*1000;
    last_hit_type = hit_type;
}

void hit(int lane){
  for (int i{future_note}; i < notes.size(); i++) {
      note_custom &current_note = notes[i];
      double timediffrence = (current_note.time - (GetMusicTimePlayed(song) * 1000)) * -1;
      last_key_hit_time = GetMusicTimePlayed(song)*1000;
      last_key_hit_lane = lane;
      if (current_note.lane == lane){
        if (timediffrence <= timings[0] && timediffrence >= timings[0]*-1){
            set_hit(current_note, 0);
            combo += 1;
            break;
        }
        else if (timediffrence <= timings[1] && timediffrence >= timings[1]*-1){
            set_hit(current_note, 1);
            combo += 1;
            break;
        }
        else if (timediffrence <= timings[2] && timediffrence >= timings[2]*-1){
            set_hit(current_note, 2);
            combo += 1;
            break;
        }
        else if (timediffrence <= timings[3] && timediffrence >= timings[3]*-1){
            set_hit(current_note, 3);
            combo = 0;
            break;
        }
        else if (timediffrence <= timings[4] && timediffrence >= timings[4]*-1){
            set_hit(current_note, 4);
            combo = 0;
            break;
        }
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
  SetTargetFPS(250);
  InitAudioDevice();

  vector<int> rotations;
  rotations = vector<int>{90, 0, 180, -90};
  Texture2D textures[NUM_TEXTURES] = {0};
  Texture2D textures_arrow[SNAP_ARROWS][NUM_TEXTURES] = {0};

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
    // yay edit the snapped arrow loading logic ffs
    Image receptor_unlit = LoadImage("receptor_unlit.png");
    Image receptor_lit = LoadImage("receptor_lit.png");
    ImageRotate(&receptor_unlit, rotations[i]);
    ImageRotate(&receptor_lit, rotations[i]);
    textures[i] = LoadTextureFromImage(receptor_unlit);
    textures_arrows_lit[i] = LoadTextureFromImage(receptor_lit);
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
  song = LoadMusicStream(music_path.c_str());
  cout << IsMusicValid(song) << std::endl;
  PlayMusicStream(song);

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);

    for (int i { 0 }; i < NUM_TEXTURES; i++) {
      DrawTexture(textures[i], receptor_x + (128*i), receptor_y, GRAY);
    }

    DrawFPS(10, 10);

    double currentTime = GetMusicTimePlayed(song)*1000;    

    for (int i{future_note}; i < notes.size(); i++) {
      currentTime = GetMusicTimePlayed(song)*1000;
      note_custom &current_note = notes[i];
      double noteDuration = current_note.time - currentTime;
      double timediffrence = (current_note.time - currentTime) * -1;

      if (current_note.hit == false) {
        current_note.y = GetCmodY(current_note);
        if(current_note.y > -100 && current_note.y < 1080) {
          DrawTexture(textures_arrow[current_note.snap % SNAP_ARROWS][current_note.lane],
                    receptor_x + (128 * current_note.lane), current_note.y, WHITE);
          }
      }

      if (useautoplay == true){
        if(timediffrence <= timings[0] && timediffrence >= timings[0]*-1){
          hit(current_note.lane);
        }
      }

      if (timediffrence >= timings[4] && current_note.hit == false) {
        current_note.hit = true;
        future_note++;
        cur_judgements[4] += 1;
        last_judge_time = currentTime;
        last_hit_type = 4;
        timediffrence = 0;
      }

      if (current_note.time <= currentTime){
        timediffrence = 0;
      }
    }

    if (useautoplay == true){
      DrawText("Autoplay enabled", 1400, 500, 24, RED);
    }
 
    string step_info_text = format("Artist: {} Song: {} Stepper: {}",artist, song_title, subtitle);
    DrawText(step_info_text.c_str(), 600, 100, 28, BLUE);

    string combo_text = format("{}",combo);
    DrawText(combo_text.c_str(), 950, 500, 28, RED);

    string test2 = format("Time remaining: {} / {}", GetMusicTimeLength(song), GetMusicTimePlayed(song));
    DrawText(test2.c_str(), 1300, 750, 24, WHITE);


    if (last_judge_time != 0 && (last_judge_time +100) >= GetMusicTimePlayed(song)*1000){
      DrawTexture(judgement_texture[last_hit_type], receptor_x, 300, WHITE);
    }
    
    if (last_key_hit_time != 0 && (last_key_hit_time +100) >= GetMusicTimePlayed(song)*1000){
        DrawTexture(textures_arrows_lit[last_key_hit_lane], receptor_x + (128*last_key_hit_lane), receptor_y, WHITE);
    }
    UpdateMusicStream(song);
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
      float total_hits = cur_judgements[0]+cur_judgements[1]+cur_judgements[2]+cur_judgements[3]+cur_judgements[4];
      if (total_hits > 0){
        accuracy = (305*cur_judgements[0] + 300*cur_judgements[1] + 200*cur_judgements[2] + 50*cur_judgements[3]) / (305*total_hits) *100;
      } else {
        accuracy= 100.0; 
      }
      DrawText(TextFormat("%.2f", accuracy), receptor_x, 10, 18, RED);
  }

  for (int i = 0; i < NUM_TEXTURES; i++)
    UnloadTexture(textures[i]);

  for (int i = 0; i < SNAP_ARROWS; i++){
    for (int i2 = 0; i2 < NUM_TEXTURES; i2++)
      UnloadTexture(textures_arrow[i2][i]);
  }
  
  for (int i = 0; i < NUM_TEXTURES; i++)
    UnloadTexture(textures_arrows_lit[i]);
  for (int i = 0; i < NUM_TEXTURES; i++)
    UnloadTexture(judgement_texture[i]);

  UnloadMusicStream(song);
  CloseAudioDevice();

  CloseWindow();
  return 0;
}
