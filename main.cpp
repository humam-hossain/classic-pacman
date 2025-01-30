#include <stdio.h>
#include "raylib.h"

#define DEBUG_FONT_SIZE 20

typedef enum { DIR_LEFT, DIR_RIGHT, DIR_UP, DIR_DOWN} Direction;

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

int main()
{
	// init
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);

	int factor = 100;
	int screen_width = 16 * factor;
	int screen_height = 9 * factor;
	const char* title = "Pacman";
	InitWindow(screen_width, screen_height, title);
	SetTargetFPS(60);

	// init pac
	Sprites pac;
	pac.position = {(float)screen_width/2, (float)screen_height/2};
	pac.radius = 20;
	pac.speed = 8;
	pac.mouth_angle = 70.0f;
	pac.h_mouth_angle = 70.0f;
	pac.l_mouth_angle = 0.0f;
	pac.angle_change = 5.0f;
	pac.mouth_opening = false;
	pac.color = YELLOW;
	pac.direction = DIR_LEFT;
	
	int start_angle, end_angle;

	while (!WindowShouldClose()) {
		screen_width = GetScreenWidth();
		screen_height = GetScreenHeight();

		BeginDrawing();
		ClearBackground(BLACK);

		// control part
		if (IsKeyDown(KEY_UP)) {
			printf("DEBUG: ARROW_UP\n");
			pac.direction = DIR_UP;
			if (pac.position.y > 0 - pac.radius) {
				pac.position.y -= pac.speed;
			}
			else {
				pac.position.y = screen_height;
			}
		}
		else if (IsKeyDown(KEY_DOWN)) {
			printf("DEBUG: ARROW_DOWN\n");
			pac.direction = DIR_DOWN;
			if (pac.position.y < screen_height + pac.radius) {
				pac.position.y += pac.speed;
			}
			else {
				pac.position.y = 0;
			}
		}
		else if (IsKeyDown(KEY_RIGHT)) {
			printf("DEBUG: ARROW_RIGHT\n");
			pac.direction = DIR_RIGHT;
			if (pac.position.x < screen_width + pac.radius) {
				pac.position.x += pac.speed;
			}
			else {
				pac.position.x = 0;
			}
		}
		else if (IsKeyDown(KEY_LEFT)) {
			printf("DEBUG: ARROW_LEFT\n");
			pac.direction = DIR_LEFT;
			if (pac.position.x > 0 - pac.radius) {
				pac.position.x -= pac.speed;
			}
			else {
				pac.position.x = screen_width;
			}
		}

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

		// update
		update_pac(&pac);

		// Draw part
		DrawCircleSector(pac.position, pac.radius, start_angle, end_angle, 60, pac.color);

		// debug texts
		char buffer[50];
		sprintf_s(buffer, sizeof(buffer), "pac_position: (%.0f, %.0f)", pac.position.x, pac.position.y);
		DrawText(buffer, 0, 0, DEBUG_FONT_SIZE, RAYWHITE);

		sprintf_s(buffer, sizeof(buffer), "speed: %.2f", pac.speed);
		DrawText(buffer, 0, DEBUG_FONT_SIZE, DEBUG_FONT_SIZE, RAYWHITE);
		
		EndDrawing();
	}
	CloseWindow();
	
	return 0;
}