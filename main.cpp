#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"

#define DEBUG_FONT_SIZE 20
#define FONT_SIZE 30

typedef enum { DIR_LEFT, DIR_RIGHT, DIR_UP, DIR_DOWN} Direction;

typedef struct {
	Vector2 position;
	Vector2 size;
	bool alive;
	Color color;
} Point;

typedef struct {
	Vector2 position;
	float radius;
	float speed;
	float mouth_angle;
	float h_mouth_angle;
	float l_mouth_angle;
	float angle_change;
	bool mouth_opening;
	Color color;
	Direction direction;
} Sprites;

int random(int low, int high)
{
	return rand() % (high - low) + low;
}

void update_pac(Sprites* pac)
{
	if (pac->mouth_opening) {
		pac->mouth_angle += pac->angle_change;
		if (pac->mouth_angle >= pac->h_mouth_angle) {
			pac->mouth_opening = false;
		}

	}
	else {
		pac->mouth_angle -= pac->angle_change;
		if (pac->mouth_angle <= pac->l_mouth_angle) {
			pac->mouth_opening = true;
		}
	}
}

void update_siren(Music sirens[], Music* current_siren, size_t* siren_i)
{
	StopMusicStream(*current_siren);
	*current_siren = sirens[*siren_i];
	PlayMusicStream(*current_siren);
}

int main()
{
	// config
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	SetTargetFPS(144);

	int factor = 100;
	int screen_width = 16 * factor;
	int screen_height = 9 * factor;
	const char* title = "Pacman";
	InitWindow(screen_width, screen_height, title);
	InitAudioDevice();

	// score
	int score = 0;

	// points
	const int points_size = 15;
	Point points[points_size];
	for (size_t i = 0; i < points_size; ++i) {
		points[i].position = {(float)100 + 100 * i, (float)screen_height/4};
		points[i].size = {(float)10, (float)10};
		points[i].alive = true;
		points[i].color = RAYWHITE;
	}

	// init pac
	Sprites pac;
	pac.position = {(float)screen_width/2, (float)screen_height/2};
	pac.radius = 20;
	pac.speed = 4;
	pac.mouth_angle = 70.0f;
	pac.h_mouth_angle = 70.0f;
	pac.l_mouth_angle = 0.0f;
	pac.angle_change = 5.0f;
	pac.mouth_opening = false;
	pac.color = YELLOW;
	pac.direction = DIR_LEFT;
	
	int start_angle, end_angle;

	// audio part
	// config
	float volume = 1.0f;

	// load audio
	const char* audio_path= "assets/audio";
	char buffer[500];
	sprintf_s(buffer, sizeof(buffer), "%s/start.mp3", audio_path);
	Music start_music = LoadMusicStream(buffer);
	if (start_music.frameCount == 0) {
		printf("ERROR: failed to load %s\n", buffer);
		return 1;
	}
	else {
		printf("SUCCESS: %s loaded successfully!\n", buffer);
	}
	start_music.looping = false;
	PlayMusicStream(start_music);
	SetMusicVolume(start_music, volume);

	const size_t sirens_size = 5;
	Music sirens[sirens_size];
	for (size_t i = 0; i < sirens_size; ++i) {
		sprintf_s(buffer, sizeof(buffer), "%s/siren_%d.wav", audio_path, i+1);
		sirens[i] = LoadMusicStream(buffer);

		if (sirens[i].frameCount == 0) {
			printf("ERROR: Failed to load %s\n", buffer);
			return 1;
		}
		SetMusicVolume(sirens[i], volume);
		sirens[i].looping = true;
	}
	int score_seg = points_size / sirens_size;
	size_t siren_i = 0;
	Music current_siren = sirens[siren_i];
	PlayMusicStream(current_siren);

	// game loop
	while (!WindowShouldClose()) {
		screen_width = GetScreenWidth();
		screen_height = GetScreenHeight();

		BeginDrawing();
		ClearBackground(BLACK);

		// control part
		if (IsKeyDown(KEY_UP)) {
			//printf("DEBUG: ARROW_UP\n");
			pac.direction = DIR_UP;
			if (pac.position.y > 0 - pac.radius) {
				pac.position.y -= pac.speed;
			}
			else {
				pac.position.y = screen_height;
			}
		}
		else if (IsKeyDown(KEY_DOWN)) {
			//printf("DEBUG: ARROW_DOWN\n");
			pac.direction = DIR_DOWN;
			if (pac.position.y < screen_height + pac.radius) {
				pac.position.y += pac.speed;
			}
			else {
				pac.position.y = 0;
			}
		}
		else if (IsKeyDown(KEY_RIGHT)) {
			//printf("DEBUG: ARROW_RIGHT\n");
			pac.direction = DIR_RIGHT;
			if (pac.position.x < screen_width + pac.radius) {
				pac.position.x += pac.speed;
			}
			else {
				pac.position.x = 0;
			}
		}
		else if (IsKeyDown(KEY_LEFT)) {
			//printf("DEBUG: ARROW_LEFT\n");
			pac.direction = DIR_LEFT;
			if (pac.position.x > 0 - pac.radius) {
				pac.position.x -= pac.speed;
			}
			else {
				pac.position.x = screen_width;
			}
		}

		// update
		switch (pac.direction) {
			case DIR_UP:
				start_angle = 180 + pac.mouth_angle;
				end_angle = 540 - pac.mouth_angle;
				break;
			case DIR_DOWN:
				start_angle = pac.mouth_angle;
				end_angle = 360 - pac.mouth_angle;
				break;
			case DIR_RIGHT:
				start_angle = 90 + pac.mouth_angle;
				end_angle = 450 - pac.mouth_angle;
				break;
			case DIR_LEFT:
				start_angle = 270 - pac.mouth_angle;
				end_angle = -90 + pac.mouth_angle;
				break;
		}

		UpdateMusicStream(start_music);
		
		if (!IsMusicStreamPlaying(start_music)) {
			UpdateMusicStream(current_siren);
			
			if (siren_i == 0 && score > score_seg) {
				siren_i = 1;
				update_siren(sirens, &current_siren, &siren_i);
				printf("DEBUG: Switiching to siren_%zu.wav\t score=%d\n", siren_i + 1, score);
			}
			else if (siren_i == 1 && score > 2 * score_seg) {
				siren_i = 2;
				update_siren(sirens, &current_siren, &siren_i);
				printf("DEBUG: Switiching to siren_%zu.wav\t score=%d\n", siren_i + 1, score);

			}
			else if (siren_i == 2 && score > 3 * score_seg) {
				siren_i = 3;
				update_siren(sirens, &current_siren, &siren_i);
				printf("DEBUG: Switiching to siren_%zu.wav\t score=%d\n", siren_i + 1, score);

			}
			else if (siren_i == 3 && score > 4 * score_seg) {
				siren_i = 4;
				update_siren(sirens, &current_siren, &siren_i);
				printf("DEBUG: Switiching to siren_%zu.wav\t score=%d\n", siren_i + 1, score);
			}

		}
		update_pac(&pac);

		// Draw part
		for (size_t i = 0; i < points_size; ++i) {
			if (points[i].alive) {
				DrawRectangleV(points[i].position, points[i].size, points[i].color);
			}
		}
		DrawCircleSector(pac.position, pac.radius, start_angle, end_angle, 60, pac.color);

		for (size_t i = 0; i < points_size; ++i) {
			if (points[i].alive) {
				float dx = pac.position.x - points[i].position.x;
				float dy = pac.position.y - points[i].position.y;
				if (dx * dx + dy * dy <= pac.radius * pac.radius) {
					//printf("[DEBUG]%zu \n", i);
					points[i].alive = false;
					++score;
				}
			}
		}

		// labels
		char buffer[50];
		sprintf_s(buffer, sizeof(buffer), "FPS: %d", GetFPS());
		DrawText(buffer, screen_width - 200, 0, FONT_SIZE, RAYWHITE);

		sprintf_s(buffer, sizeof(buffer), "SCORE: %d", score*10);
		DrawText(buffer, screen_width - 200, FONT_SIZE, FONT_SIZE, RAYWHITE);

		// debug texts
		sprintf_s(buffer, sizeof(buffer), "pac_position: (%.0f, %.0f)", pac.position.x, pac.position.y);
		DrawText(buffer, 0, 0, DEBUG_FONT_SIZE, RAYWHITE);

		sprintf_s(buffer, sizeof(buffer), "speed: %.2f", pac.speed);
		DrawText(buffer, 0, DEBUG_FONT_SIZE, DEBUG_FONT_SIZE, RAYWHITE);

		sprintf_s(buffer, sizeof(buffer), "sound_length: %.2f / %.2f", GetMusicTimePlayed(start_music), GetMusicTimeLength(start_music));
		DrawText(buffer, 0, 2*DEBUG_FONT_SIZE, DEBUG_FONT_SIZE, RAYWHITE);

		EndDrawing();
	}

	for (size_t i = 0; i < sirens_size; ++i) {
		UnloadMusicStream(sirens[i]);
	}
	UnloadMusicStream(start_music);
	CloseAudioDevice();
	CloseWindow();
	
	return 0;
}