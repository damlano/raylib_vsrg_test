#include "../loaders/smloader.h"
#include <raylib.h>
#include <raygui.h>
#include <vector>

extern std::vector<simfile> songs_list;
extern std::vector<simfile> reversed_songs_list;
extern int currentindex_song;

void draw_songselect(int screenwidth,int screenheight, std::string & state, bool & shouldclose){
    int visible_range = 3;
    DrawRectangleGradientEx((Rectangle){0,0,screenwidth, screenheight }, PURPLE, DARKBLUE, DARKPURPLE, DARKBLUE);
    DrawRectangle(screenwidth/2 - screenwidth*0.1f, 0, screenwidth*0.2f, screenheight*0.05f, (Color){0,0,0,100});
    DrawText("song select", screenwidth/2 - screenwidth*0.05f, 0, 35, (Color){255,255,255,100});

    std::vector<simfile> songs_to_draw;

    int loop_initilization {currentindex_song};
    for (int i{loop_initilization+visible_range}; i > loop_initilization; i--){
        songs_to_draw.push_back(songs_list[i%songs_list.size()]);
    }

    songs_to_draw.push_back(songs_list[currentindex_song]);

    int test = songs_list.size() - currentindex_song;

    for (int i{test}; i < test + visible_range; i++){
        songs_to_draw.push_back(reversed_songs_list[i%songs_list.size()]);
    }

    int index{0};

    

    for(auto& i: songs_to_draw ){
        GuiSetStyle(DEFAULT, TEXT_SIZE, 50);

        if (i.TITLE.length() > 30){
            GuiSetStyle(DEFAULT, TEXT_SIZE, 30);
        }
        if (i.TITLE.length() > 40){
            GuiSetStyle(DEFAULT, TEXT_SIZE, 24);
        }

        if(i.base_path == songs_list[currentindex_song].base_path){
            GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt((Color) {255,0,0,255}));
            GuiLabel((Rectangle){ screenwidth*0.50f -screenwidth*0.2f, screenheight*(0.1f+0.1f*index), screenwidth*0.4f, screenheight*0.2f }, i.TITLE.c_str());
            
            } else {
                GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt((Color) {255,255,255,255}));
                GuiLabel((Rectangle){ screenwidth*0.50f -screenwidth*0.2f, screenheight*(0.1f+0.1f*index), screenwidth*0.4f, screenheight*0.2f }, i.TITLE.c_str()); 
            }
        index++;
    }
}
