#include <cstddef>
#include <iostream>
#include "resource_dir.h"
#include <raylib.h>
#include "screens/menus.h"
#include "loaders/smloader.h"
#include <string>
#include <vector>
#include <filesystem> 
//#include "tracy/Tracy.hpp"

#define SNAP_ARROWS 9
#define NUM_TEXTURES 4

#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

namespace fs = std::filesystem;
std::vector<std::string> packs;
std::vector<std::string> reversed_packs;
std::vector<simfile> songs_list;
std::vector<simfile> reversed_songs_list;
int currentindex{0};
int currentindex_song{0};
int currentindex_diff{0};
Music song_music;
int real_y;
int song_length;
double currentTime;
int future_note;
int last_judge_time;
int last_judge_type;
int cur_judgements[5];
float accuracy;
int combo;
int receptor_y;
int receptor_x;

vector<note_custom> notes;
Texture2D textures_arrows_lit[NUM_TEXTURES];
Texture2D judgement_texture[5];
Texture2D textures[NUM_TEXTURES];
Texture2D textures_arrow[SNAP_ARROWS][NUM_TEXTURES];

int main()
{
    const fs::path target_path{"/home/demi/windows_data/Etterna/Etterna-0.74.4-Linux/Etterna/Songs"};
    try {
        for (auto const& dir_entry : fs::directory_iterator{target_path})
        {
            if (fs::is_directory(dir_entry.path()))
            {
                packs.push_back(dir_entry.path().filename());
            }
        }
    }
    catch (fs::filesystem_error const& ex)
    {
        std::cout << "Error occurred during file operations!\n" << ex.what() << std::endl;
        return 1;
    }
    reversed_packs.resize(packs.size());
    reverse_copy(packs.begin(), packs.end(), reversed_packs.begin());

    vector<int> rotations = {90, 0, 180, -90};

    SearchAndSetResourceDir("resources");
    InitWindow(400, 200, "test");
    ToggleFullscreen();
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);
    InitAudioDevice();

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

    int screenheight = GetScreenHeight();
    int screenwidth = GetScreenWidth(); 
    std::string state = "mainmenu"; 
    bool shouldclose = false;

    receptor_y = GetScreenHeight() * 0.85;
    receptor_x = GetScreenWidth() * 0.35;

    if (scrolldirection == -1){
        real_y = receptor_y;
    } else {
        real_y = screenheight -receptor_y;
    }

    GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
    Font gamefont = LoadFontEx("test.ttf",100,NULL,0);
    GuiSetFont(gamefont);

    GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt((Color) {255,255,255,155}));
    GuiSetStyle(BUTTON, TEXT_COLOR_FOCUSED, ColorToInt(WHITE));
    GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, ColorToInt((Color){ 255,255,255, 100 }));
    GuiSetStyle(BUTTON, TEXT_COLOR_PRESSED, ColorToInt(PURPLE));
    GuiSetStyle(BUTTON, BASE_COLOR_PRESSED, ColorToInt(WHITE));
    GuiSetStyle(BUTTON, BORDER_WIDTH,0);
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, 0);  

    while (!WindowShouldClose() && !shouldclose)
    {
        BeginDrawing();
            //FrameMark;
            if (state == "gameplay"){
                currentTime = GetMusicTimePlayed(song_music)*1000;
                draw_gameplay(screenwidth, screenheight, state,shouldclose);
                
                if (IsMusicValid(song_music)){
                    UpdateMusicStream(song_music);
                }
            } else if (state == "mainmenu"){
                draw_mainmenu(screenwidth, screenheight, state,shouldclose);
            } else if (state == "packselect"){
                draw_packselect(screenwidth, screenheight, state,shouldclose);
            } else if (state == "songselect"){
                draw_songselect(screenwidth, screenheight, state,shouldclose);
            } else if (state == "difficultyselect"){
                draw_difficultyselect(screenwidth, screenheight, state,shouldclose);
            }
            EndDrawing();

            if (state == "gameplay"){
                if (IsKeyPressed(KEY_ESCAPE)){
                    SetTargetFPS(60);
                    state = "difficultyselect";
                }

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
            
            
        } else if (state == "packselect"){
            if (IsKeyPressed(KEY_UP)) {
                if (currentindex < packs.size()-1){
                    currentindex += 1;
                } else  {
                    currentindex = 0;
                }
            }

            if (IsKeyPressed(KEY_DOWN)) {
                if (currentindex > 0){
                    currentindex -= 1;
                } else  {
                    currentindex = packs.size()-1;
                }
            }

            if (IsKeyPressed(KEY_ESCAPE)) {
                state = "mainmenu";
            }

            if (IsKeyPressed(KEY_ENTER)){
                SetTargetFPS(60);
                state = "songselect";
                songs_list.clear();
                reversed_songs_list.clear();
                std::filesystem::path songpath = target_path / packs[currentindex];
                for (const fs::directory_entry& dir_entry : fs::recursive_directory_iterator(songpath))
                    {
                        if (dir_entry.is_regular_file()){
                            if (dir_entry.path().extension() == ".sm"){
                                songs_list.push_back(get_simfile_data(dir_entry.path()));
                            }
                        }
                        reversed_songs_list.resize(songs_list.size());
                        reverse_copy(songs_list.begin(), songs_list.end(), reversed_songs_list.begin());
                    }
                }

        } else if (state == "mainmenu"){
            if (IsKeyPressed(KEY_ESCAPE)){
                shouldclose = true;
            }
        } else if (state == "songselect"){
            if (IsKeyPressed(KEY_UP)) {
                if (currentindex_song < songs_list.size()-1){
                    currentindex_song += 1;
                } else  {
                    currentindex_song = 0;
                }
            }

            if (IsKeyPressed(KEY_DOWN)) {
                if (currentindex_song > 0){
                    currentindex_song -= 1;
                } else  {
                    currentindex_song = songs_list.size()-1;
                }
            }

            if (IsKeyPressed(KEY_ESCAPE)){
                state = "packselect";
            }

            if (IsKeyPressed(KEY_ENTER)){
                SetTargetFPS(60);
                state = "difficultyselect";
            }
        } else if (state == "difficultyselect"){
            if (IsKeyPressed(KEY_UP)) {
                if (currentindex_diff < songs_list[currentindex_song].difficulties.size()-1){
                    currentindex_diff += 1;
                } else  {
                    currentindex_diff = 0;
                }
            }

            if (IsKeyPressed(KEY_DOWN)) {
                if (currentindex_diff > 0){
                    currentindex_diff -= 1;
                } else  {
                    currentindex_diff = songs_list[currentindex_song].difficulties.size()-1;
                }
            }

            if (IsKeyPressed(KEY_ESCAPE)){
                state = "songselect";
            }

            if (IsKeyPressed(KEY_ENTER)){
                future_note = 0;
                last_judge_time = 0;
                last_judge_type = 0 ;
                for (int i = 0; i < 5; i++) {
                    cur_judgements[i] = 0;
                }
                accuracy= 0;
                combo = 0;
                SetTargetFPS(1000);
                state = "gameplay";
                notes = get_note_data(songs_list[currentindex_song], currentindex_diff);
                std::filesystem::path music_path = std::filesystem::path(songs_list[currentindex_song].base_path).parent_path() / songs_list[currentindex_song].MUSIC;
                song_music = LoadMusicStream(music_path.c_str());
                song_length = GetMusicTimeLength(song_music);
                song_music.looping = false;
                PlayMusicStream(song_music);
            }
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

    UnloadMusicStream(song_music);
    CloseAudioDevice();
    UnloadFont(gamefont);
    CloseWindow();
    return 0;
}