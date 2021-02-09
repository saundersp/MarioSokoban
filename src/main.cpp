#include "libraries.hpp"
#include "loader.hpp"
#include "main_menu.hpp"

#ifdef __DEBUG__
void* operator new(size_t size) noexcept {
	CONSOLE_LOG("Allocating %d bytes", size);
	return malloc(size);
}

void operator delete[](void* ptr) noexcept {
	CONSOLE_LOG("Freeing[] undefined bytes");
	free(ptr);
}

void operator delete[](void* ptr, size_t size) noexcept {
	CONSOLE_LOG("Freeing[] %d bytes", size);
	free(ptr);
}

void operator delete(void* ptr) noexcept {
	CONSOLE_LOG("Freeing undefined bytes");
	free(ptr);
}

void operator delete(void* ptr, size_t size) noexcept {
	CONSOLE_LOG("Freeing %d bytes", size);
	free(ptr);
}
#endif

int main(int argc, char** argv) {
#ifndef __DEBUG__
	freopen("stdout.txt", "a+", stdout);
	freopen("stderr.txt", "a+", stderr);
#else
	CONSOLE_LOG("Mario Sokoban Remake V%s", GAME_VERSION);
	for (int i = 0; i < argc; i++)
		CONSOLE_LOG("argv[%d] = %s", i, argv[i]);
	SDL_version v;
	SDL_VERSION(&v);
	CONSOLE_LOG("SDL (compiled) MAJOR=%d MINOR=%d PATCH=%d", v.major, v.minor, v.patch);
	SDL_GetVersion(&v);
	CONSOLE_LOG("SDL (linked) MAJOR=%d MINOR=%d PATCH=%d", v.major, v.minor, v.patch);
	SDL_TTF_VERSION(&v);
	CONSOLE_LOG("SDL_TTF (compiled) MAJOR=%d MINOR=%d PATCH=%d", v.major, v.minor, v.patch);
	v = *TTF_Linked_Version();
	CONSOLE_LOG("SDL_TTF (linked) MAJOR=%d MINOR=%d PATCH=%d", v.major, v.minor, v.patch);
	SDL_IMAGE_VERSION(&v);
	CONSOLE_LOG("SDL_IMAGE (compiled) MAJOR=%d MINOR=%d PATCH=%d", v.major, v.minor, v.patch);
	v = *IMG_Linked_Version();
	CONSOLE_LOG("SDL_IMAGE (linked) MAJOR=%d MINOR=%d PATCH=%d", v.major, v.minor, v.patch);
#endif
	using namespace loader;

	Screen s = { initialize_engine("Mario Sokoban Remake " GAME_VERSION), nullptr };

	if (s.window == nullptr)
		return quit_engine(EXIT_FAILURE);

	CONSOLE_LOG("Getting window surface");
	s.surface = SDL_GetWindowSurface(s.window);
	if (s.surface == nullptr) {
		CONSOLE_ERROR("Error fetching window surface: (%s)", SDL_GetError());
		return quit_engine(s.window, EXIT_FAILURE);
	}

	Assets assets = Assets();
	if (assets.load_error != 0) {
		CONSOLE_LOG("Error loading one or more assets : %d", assets.load_error);
		return quit_engine(assets, s, EXIT_FAILURE);
	}

	SDL_SetWindowIcon(s.window, assets.window_icon);
	SDL_ShowCursor(SDL_FALSE);
	main_menu::loop(s, assets);

	return quit_engine(assets, s, EXIT_SUCCESS);
}