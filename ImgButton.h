#pragma once

#include <4dm.h>

using namespace fdm;
using namespace fdm::gui;

namespace aui {

	class ImgButton : public Button
	{
	public:
		TexRenderer tex;

		int edgeW = 1;
		int edgeH = 1;

		ImgButton() {}
		ImgButton(const std::string& imagePath, int width = 0, int height = 0, int edgeW = 1, int edgeH = 1) : edgeW(edgeW), edgeH(edgeH) {
			tex.texture = ResourceManager::get(imagePath);
			tex.shader = ShaderManager::get("tex2DShader");
			tex.init();
			tex.setClip(0, 0, tex.texture->width, tex.texture->height);
			tex.setColor(1, 1, 1, 1);

			this->width = width + edgeW * 2;
			this->height = height + edgeH * 2;
		}

		void render(Window* w) override {
			Button::render(w);

			int x, y;
			getPos(w, &x, &y);

			tex.setPos(x + edgeW, y + edgeH, width - edgeW * 2, height - edgeW * 2);
			tex.render();
		}
	};

}