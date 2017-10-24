#pragma once

class BufferData;
class Texture;
class SpriteBatch {
private:
    float * buffer;
    int bufferSize;
    int bufferCount;
    Texture * texture;
    BufferData * bufferData;
public:
    SpriteBatch(Texture * texture,int bufferSize);
    bool addElement(float * element, int size);
    void reset();
    void draw();
};
