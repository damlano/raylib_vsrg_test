#pragma GCC optimize("Ofast")

#include "raylib.h"
#include <vector>
#include "loaders/smloader.h"
#include "tracy/Tracy.hpp"
#include "screens/menus.h"

using namespace std;

// local public variables

constexpr int g_fVisualDelaySeconds {0};
constexpr int musicrate {1};
constexpr int ARROW_SPACING {64};
constexpr int m_fScrollBPM {2000};
constexpr int m_fTimeSpacing {1};

// from left to right: marv/perf/great/boo/miss i think in ms
int timings[5] {17, 100, 300, 400,600};

struct key_lighting {
  float time; // in ms
  int lane; 
};

vector<key_lighting> key_down_lighting;

// extern variables
extern Music song_music;
extern vector<note_custom> notes;
extern Texture2D textures_arrows_lit[NUM_TEXTURES];
extern Texture2D judgement_texture[5];
extern Texture2D textures[NUM_TEXTURES];
extern Texture2D textures_arrow[SNAP_ARROWS][NUM_TEXTURES];
extern int real_y;
extern int song_length;
extern double currentTime;

extern int future_note;
extern int last_judge_time;
extern int last_judge_type;
extern int cur_judgements[5];
extern float accuracy;
extern int combo;
extern int receptor_y;
extern int receptor_x;
// helper functions

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

void draw_gameplay(int screenwidth,int screenheight, std::string & state, bool & shouldclose){
    ClearBackground(BLACK);
    DrawFPS(screenheight*0.01, screenheight*0.01);

    for (int i { 0 }; i < NUM_TEXTURES; i++) {
      DrawTexture(textures[i], receptor_x + (128*i), real_y, GRAY);
    }

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

    if (useautoplay == true)[[unlikely]]{
      DrawText("Autoplay enabled", screenwidth*0.73, screenheight*0.45, 24, RED);
    }

    string step_info_text = format("Artist: {} Song: {} Stepper: {}",songs_list[currentindex_song].ARTIST, songs_list[currentindex_song].TITLE, songs_list[currentindex_song].CREDIT);
    string combo_text = format("{}",combo);
    string test2 = format("Time remaining: {} / {:.0f}", song_length, currentTime/1000);

    DrawText(combo_text.c_str(), screenwidth*0.5, screenheight*0.45, 28, RED);
    DrawText(step_info_text.c_str(), screenwidth*0.25, screenheight*0.01, 28, BLUE);
    DrawText(test2.c_str(), screenwidth*0.68, screenheight*0.7, 24, WHITE);

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
    DrawText(TextFormat("%.2f", accuracy), screenwidth*0.5, screenheight*0.2, 18, RED);

}
