#pragma once

class Texture 
{
public:
    unsigned int SHADOW_WIDTH = 1600;
    unsigned int SHADOW_HEIGHT = 1200;
	void setupTexture(const char* texturePath);
    void setupDepthTexture();
	void bind(unsigned int slot) const;
    void bindFramebuffer();
	void unbind() const;
    void unbindFramebuffer();

private:
	unsigned int ID = 0;
    unsigned int DEPTH_FBO = 0;
	int Width = 0, Height = 0, BPP = 0;
};
