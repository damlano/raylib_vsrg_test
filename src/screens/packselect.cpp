#include <iostream>
#include "resource_dir.h"
#include <ostream>
#include <raylib.h>
#include <raygui.h>
#include <vector>


int visible_range = 3; 

extern std::vector<std::string> reversed_packs;
extern std::vector<std::string> packs;
extern int currentindex;

void draw_packselect(int screenwidth,int screenheight, std::string & state, bool & shouldclose){
    DrawRectangleGradientEx((Rectangle){0,0,screenwidth, screenheight }, PURPLE, DARKBLUE, DARKPURPLE, DARKBLUE);
    DrawRectangle(screenwidth/2 - screenwidth*0.1f, 0, screenwidth*0.2f, screenheight*0.05f, (Color){0,0,0,100});
    DrawText("pack select", screenwidth/2 - screenwidth*0.05f, 0, 35, (Color){255,255,255,100});

    std::vector<std::string> packs_to_draw;

    int loop_initilization {currentindex};
    for (int i{loop_initilization+visible_range}; i > loop_initilization; i--){
        packs_to_draw.push_back(packs[i%packs.size()]);
    }

    packs_to_draw.push_back(packs[currentindex]);

    int test = packs.size() - currentindex;

    for (int i{test}; i < test + visible_range; i++){
        packs_to_draw.push_back(reversed_packs[i%packs.size()]);
    }

    int index{0};
    for(auto i: packs_to_draw ){
        GuiSetStyle(DEFAULT, TEXT_SIZE, 50);

        if (i.length() > 30){
            GuiSetStyle(DEFAULT, TEXT_SIZE, 30);
        }
        if (i.length() > 40){
            GuiSetStyle(DEFAULT, TEXT_SIZE, 24);
        }

        if(i == packs[currentindex]){
            GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt((Color) {255,0,0,255}));
            GuiLabel((Rectangle){ screenwidth*0.50f -screenwidth*0.2f, screenheight*(0.1f+0.1f*index), screenwidth*0.4f, screenheight*0.2f }, i.c_str());
            
            } else {
                GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt((Color) {255,255,255,255}));
                GuiLabel((Rectangle){ screenwidth*0.50f -screenwidth*0.2f, screenheight*(0.1f+0.1f*index), screenwidth*0.4f, screenheight*0.2f }, i.c_str()); 
            }
        index++;
    }
}