#include "menus.h"
#include "../loaders/smloader.h"
#include <raylib.h>
#include <raygui.h>
#include <vector>

extern std::vector<simfile> songs_list;
extern std::vector<simfile> reversed_songs_list;
extern int currentindex_song;
extern int currentindex_diff;

void draw_difficultyselect(int screenwidth,int screenheight, std::string & state, bool & shouldclose){
    int visible_range = 3;
    DrawRectangleGradientEx((Rectangle){0,0,screenwidth, screenheight }, PURPLE, DARKBLUE, DARKPURPLE, DARKBLUE);
    DrawRectangle(screenwidth/2 - screenwidth*0.1f, 0, screenwidth*0.2f, screenheight*0.05f, (Color){0,0,0,100});
    DrawText("difficulty select", screenwidth/2 - screenwidth*0.05f, 0, 35, (Color){255,255,255,100});

    std::vector<std::string> difficulties_to_draw;
    std::vector<int> meter_to_draw;

    int loop_initilization {currentindex_diff};
    simfile current_simfile = songs_list[currentindex_song];
    for (int i{loop_initilization+visible_range}; i > loop_initilization; i--){
        meter_to_draw.push_back(current_simfile.meter[i%current_simfile.meter.size()]);
        difficulties_to_draw.push_back(current_simfile.difficulties[i%current_simfile.difficulties.size()]);
    }

    difficulties_to_draw.push_back(current_simfile.difficulties[currentindex_diff]);
    meter_to_draw.push_back(current_simfile.meter[currentindex_diff]);

    int test = current_simfile.difficulties.size() - currentindex_diff;

    for (int i{test}; i < test + visible_range; i++){
        meter_to_draw.push_back(current_simfile.meter[i%current_simfile.meter.size()]);
        difficulties_to_draw.push_back(current_simfile.difficulties[i%current_simfile.difficulties.size()]);
    }

    int index{0};
    for(auto& i: difficulties_to_draw ){
        GuiSetStyle(DEFAULT, TEXT_SIZE, 50);

        if (i.length() > 30){
            GuiSetStyle(DEFAULT, TEXT_SIZE, 30);
        }
        if (i.length() > 40){
            GuiSetStyle(DEFAULT, TEXT_SIZE, 24);
        }
        string diff_string = format("{} {}", i, meter_to_draw[index]);

        if(i == current_simfile.difficulties[currentindex_diff]){
            GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt((Color) {255,0,0,255}));
            GuiLabel((Rectangle){ screenwidth*0.50f -screenwidth*0.2f, screenheight*(0.1f+0.1f*index), screenwidth*0.4f, screenheight*0.2f }, diff_string.c_str());
            
            } else {
                GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt((Color) {255,255,255,255}));
                GuiLabel((Rectangle){ screenwidth*0.50f -screenwidth*0.2f, screenheight*(0.1f+0.1f*index), screenwidth*0.4f, screenheight*0.2f }, diff_string.c_str()); 
            }
        index++;
    }
}