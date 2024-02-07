#ifndef EFFECT_NONE_H
#define EFFECT_NONE_H

#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/GL/Texture.h>

using namespace Magnum;

class NoneEffect: public GL::AbstractShaderProgram {
  public:
    NoneEffect() {};

    explicit NoneEffect(GL::Texture2D& texture) {
      this->_outputTexture = &texture;
    };

    GL::Texture2D& getTexture() {
      return *_outputTexture;
    };

  private:
    GL::Texture2D* _outputTexture;
};

#endif