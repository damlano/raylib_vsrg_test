#pragma once

#include <string>
#include <vector>
#include "../loaders/smloader.h"
#include "raylib.h"

#define scrollspeed 1
#define useautoplay false
#define scrolldirection -1
#define SNAP_ARROWS 9
#define NUM_TEXTURES 4

extern std::vector<std::string> packs;
extern std::vector<std::string> reversed_packs;
extern int currentindex;
extern int currentindex_song;
extern std::vector<simfile> songs_list;
extern std::vector<simfile> reversed_songs_list;
extern int currentindex_diff;

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

void draw_mainmenu(int screenwidth,int screenheight, std::string & state, bool & shouldclose);
void draw_packselect(int screenwidth,int screenheight, std::string & state, bool & shouldclose);
void draw_songselect(int screenwidth,int screenheight, std::string & state, bool & shouldclose);
void draw_difficultyselect(int screenwidth,int screenheight, std::string & state, bool & shouldclose);
void draw_gameplay(int screenwidth,int screenheight, std::string & state, bool & shouldclose);
void hit(int lane, int currenttime);