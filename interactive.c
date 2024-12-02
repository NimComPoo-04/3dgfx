#if defined(INTERACTIVE)

#ifdef __WIN32__
#else

void (*keyPressed)(void *state, int which);

#include <raylib.h>

void interactive_window(void *state, void(*refresh)(void *), int width, int height, void *buffer)
{
	InitWindow(width, height, "...");

	Image img = {
		buffer, width, height, 1,
		PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
	};

	SetTargetFPS(30);

	Texture tex = LoadTextureFromImage(img);

	Vector2 vec = {0};
	Rectangle src = {0, 0, width, -height};

	while(!WindowShouldClose())
	{
		refresh(state);

		int c = GetKeyPressed();
		if(c)
			keyPressed(state, c);

		BeginDrawing();

		ClearBackground(BLACK);

		UpdateTexture(tex, buffer);
		DrawTextureRec(tex, src, vec, WHITE);
		DrawFPS(5, 5);
		
		EndDrawing();
	}

	UnloadTexture(tex);

	CloseWindow();
}

#endif /* UNIX */

#endif /* defined(INTERACTIVE) */
