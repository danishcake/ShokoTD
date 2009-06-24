#pragma once
#include "vmath.h"
#include <string>
#include "TextAlignment.h"

struct SDL_Surface;

class BlittableRect
{
private:
	static unsigned int surface_flags_;
	static int depth_;
	static unsigned int bytes_used;
	Vector2i size_;
	SDL_Surface* surface_;
	bool error_occurred_;
	bool dont_free_;

public:
	BlittableRect(SDL_Surface* _surface, bool _dont_free_surface);
	BlittableRect(Vector2i _size);
	BlittableRect(std::string _filename);
	BlittableRect(std::string _filename, bool _dont_append_animations);
	BlittableRect(std::string _filename, Vector2i _size);
	~BlittableRect(void);

	void Blit(Vector2i _position, BlittableRect* _dest);
	void RawBlit(Vector2i _position, BlittableRect* _dest);
	void Fade(float _degree, unsigned char r, unsigned char g, unsigned char b);
	void SetAlpha(unsigned char a);
	void BlitText(std::string _string, TextAlignment::Enum _alignment);
	void Fill(unsigned char a, unsigned char r, unsigned char g, unsigned char b);
	void Save(std::string _filename);
	
	bool GetError(){return error_occurred_;}
	
	Vector2i GetSize(){return size_;}
	BlittableRect* Resize(Vector2i _new_size);


	static void SetSurfaceFlags(unsigned int flags);
};
