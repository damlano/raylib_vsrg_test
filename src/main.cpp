#include "raylib.h"
#include "resource_dir.h"
#include <algorithm>
#include <iostream>
#include <vector>
#include <cstdlib>

#define NUM_TEXTURES 4
#define scrollspeed 1.5 // how much pixels per second
#define receptor_y 950

struct note_custom{
	int time; // in ms
	int lane; // remember lane 500 + (128*lane)w
	float y;
	bool enabled;
	bool hit;
};

bool comp(note_custom a, note_custom b)
{
    return a.time < b.time;
}

int main ()
{
	InitWindow(1920, 1080, "Hello Raylib");
	SearchAndSetResourceDir("resources");
	ToggleFullscreen();
	SetTargetFPS(250);

	std::vector<note_custom> notes;
	int future_note { 0 }; // public bcuz if a note is passed no need to draaw it anymore

	Image arrow = LoadImage("snapped_arrows/00.png");
	Image arrow_rotated_90 = LoadImage("snapped_arrows/00.png");
	Image arrow_rotated_180 = LoadImage("snapped_arrows/00.png");
	Image arrow_rotated_negative_90 = LoadImage("snapped_arrows/00.png");

	Image receptor_unlit = LoadImage("receptor_unlit.png");
	Image receptor_unlit1 = LoadImage("receptor_unlit.png");
	Image receptor_unlit2 = LoadImage("receptor_unlit.png");
	Image receptor_unlit3 = LoadImage("receptor_unlit.png");

	Image receptor_lit = LoadImage("receptor_lit.png");
	Image receptor_lit1 = LoadImage("receptor_lit.png");
	Image receptor_lit2 = LoadImage("receptor_lit.png");
	Image receptor_lit3 = LoadImage("receptor_lit.png");

	ImageRotate(&receptor_unlit1, 90);
	ImageRotate(&receptor_unlit2, -90);
	ImageRotate(&receptor_unlit3, 180);

	ImageRotate(&receptor_lit1, 90);
	ImageRotate(&receptor_lit2, -90);
	ImageRotate(&receptor_lit3, 180);

	ImageRotate(&arrow_rotated_90, 90);
	ImageRotate(&arrow_rotated_negative_90, -90);
	ImageRotate(&arrow_rotated_180, 180);

	Texture2D textures[NUM_TEXTURES] = { 0 };
	Texture2D textures_lit[NUM_TEXTURES] = { 0 };
	Texture2D textures_arrow[NUM_TEXTURES] = { 0 };

    textures[0] = LoadTextureFromImage(receptor_unlit1); 
    textures[1] = LoadTextureFromImage(receptor_unlit);  
    textures[2] = LoadTextureFromImage(receptor_unlit3); 
	textures[3] = LoadTextureFromImage(receptor_unlit2);

    textures_arrow[0] = LoadTextureFromImage(arrow_rotated_90); 
    textures_arrow[1] = LoadTextureFromImage(arrow);  
    textures_arrow[2] = LoadTextureFromImage(arrow_rotated_180); 
	textures_arrow[3] = LoadTextureFromImage(arrow_rotated_negative_90); 

	// each texture is 128px high and wide

	for (int i { 0 }; i <= 100; i++){
		note_custom new_note;
		new_note.time = rand() % 3000 + 3000;
		new_note.lane = rand() % 4;
		new_note.y = 0;
		new_note.enabled = false;
		new_note.hit = false;
		notes.push_back(new_note);
	}

	std::sort(notes.begin(), notes.end(), comp);

	while (!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(BLACK);

        DrawTexture(textures[0], 500, receptor_y, GRAY);
		DrawTexture(textures[1], 500+128, receptor_y, GRAY);
		DrawTexture(textures[2], 500+128+128, receptor_y, GRAY);
		DrawTexture(textures[3], 500+128+128+128, receptor_y, GRAY);

		DrawFPS(10, 10);

		for (int i { future_note }; i < notes.size(); i++) {
			note_custom& current_note = notes[i]; 
			double test = GetTime() *1000;
			double timediffrence = (current_note.time - (GetTime() * 1000)) *-1;

			if (current_note.hit == false){
				DrawTexture(textures_arrow[current_note.lane], 500+(128*current_note.lane), current_note.y, WHITE);
				current_note.y = (timediffrence + 950) *scrollspeed;
			}

			if (timediffrence >= 100){
				current_note.hit = true;
				future_note++;
				timediffrence = 0;
			}

			if (IsKeyPressed(KEY_D)) {std::cout << "Key pressed: D" << std::endl;}
        	if (IsKeyPressed(KEY_F)) {std::cout << "Key pressed: F" << std::endl;}
        	if (IsKeyPressed(KEY_J)) {std::cout << "Key pressed: J" << std::endl;}
        	if (IsKeyPressed(KEY_K)) {std::cout << "Key pressed: K" << std::endl;}
		}
		
		EndDrawing();
	}

	for (int i = 0; i < NUM_TEXTURES; i++) UnloadTexture(textures[i]);
	for (int i = 0; i < NUM_TEXTURES; i++) UnloadTexture(textures_arrow[i]);
	for (int i = 0; i < NUM_TEXTURES; i++) UnloadTexture(textures_lit[i]);
	//UnloadTexture(receptor_lit);

	CloseWindow();
	return 0;
}
