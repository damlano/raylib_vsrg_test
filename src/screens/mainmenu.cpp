#include <iostream>
#include "resource_dir.h"
#include <raylib.h>
#include <raygui.h>

void draw_mainmenu(int screenwidth,int screenheight, std::string & state, bool & shouldclose){
    DrawRectangleGradientEx((Rectangle){0,0,screenwidth, screenheight }, PURPLE, DARKBLUE, DARKPURPLE, DARKBLUE);

    GuiSetStyle(DEFAULT, TEXT_SIZE, 100);
    GuiLabel((Rectangle){ screenwidth*0.50f -screenwidth*0.2f, screenheight*0.2f, screenwidth*0.4f, screenheight*0.2f }, "Raylib VSRG");
    
    GuiSetStyle(DEFAULT, TEXT_SIZE, 35);
    if (GuiButton((Rectangle){ screenwidth*0.50f -screenwidth*0.1f, screenheight*0.4f, screenwidth*0.2f, screenheight*0.05f }, "PLAY")){state = "packselect";}
    GuiButton((Rectangle){ screenwidth*0.50f -screenwidth*0.1f, screenheight*0.5f, screenwidth*0.2f, screenheight*0.05f }, "Settings");
    if (GuiButton((Rectangle){ screenwidth*0.50f -screenwidth*0.1f, screenheight*0.6f, screenwidth*0.2f, screenheight*0.05f }, "QUIT")) {shouldclose = true;}
}