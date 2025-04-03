#pragma once

#include <4dm.h>

using namespace fdm;

namespace aui {

	class BarIndicator : public gui::Element
	{
	public: 
		enum fillDirection {
			LEFT_TO_RIGHT = 0,
			BOTTOM_TO_TOP = LEFT_TO_RIGHT,
			CENTER_TO_SIDES = 1,
			RIGHT_TO_LEFT = 2,
			TOP_TO_BOTTOM = RIGHT_TO_LEFT
		};

		bool visible = true;
		bool showFractionLines = false;
		bool isHorizontal = true;
		int width, height;
		int xOffset, yOffset;
		int fillDirection;
		bool clampFill = true;

		float maxFill=1;
		float currentFill=0.5;
		float fillFraction=0.5;
		float fractionLineValue = 1;
		int fractionLineCount = 0;
		glm::vec4 fillColor;
		glm::vec4 outlineColor;
		glm::vec4 fractionLineColor;
		
		gui::Text text{};

		bool showText = false;
		bool showMax = true;
		
		int textAlignment=0;

		void render(gui::Window* w) override
		{
			QuadRenderer* renderer = w->getQuadRenderer();

			if (!visible) return;
			if (!isHorizontal) { // Vectical mode
				switch (fillDirection) {
				case BOTTOM_TO_TOP:
					renderer->setPos(xOffset, height*1.5 - height * fillFraction + yOffset, width, height * fillFraction);
					break;
				case CENTER_TO_SIDES:
					renderer->setPos(xOffset, height / 2 - height * fillFraction / 2 + yOffset, width, height * fillFraction);
					break;
				case TOP_TO_BOTTOM:
					renderer->setPos(xOffset, yOffset, width, height * fillFraction);
					break;
				}
				int textWidth, textHeight;
				text.getSize(w, &textWidth, &textHeight);
				switch (textAlignment) {
				case 0: //down
					text.offsetX(width / 2 - textWidth / 2 + xOffset);
					text.offsetY(height - textHeight / 2 + yOffset);
					break;
				case 1: //center
					text.offsetX(width / 2 - textWidth / 2 + xOffset);
					text.offsetY(height/2 - textHeight / 2 + yOffset);
					break;
				case 2: //up
					text.offsetX(width / 2 - textWidth / 2 + xOffset);
					text.offsetY(yOffset);
					break;
				}
				

				renderer->setQuadRendererMode(GL_TRIANGLES);
				renderer->setColor(fillColor.r, fillColor.g, fillColor.b, fillColor.a);
				renderer->render();

				renderer->setPos(xOffset, height / 2 + yOffset, width, height-1);
				renderer->setQuadRendererMode(GL_LINE_LOOP);
				renderer->setColor(outlineColor.r, outlineColor.g, outlineColor.b, outlineColor.a);
				renderer->render();

				if (showText)
					text.render(w);
			}
			else  // Horizontal mode
			{
				switch (fillDirection) {
				case LEFT_TO_RIGHT:
					renderer->setPos(xOffset ,yOffset, width * fillFraction, height );
					break;
				case CENTER_TO_SIDES:
					renderer->setPos(width/2 + xOffset - width * fillFraction/2,yOffset, width * fillFraction, height);
					break;
				case RIGHT_TO_LEFT:
					renderer->setPos(width + xOffset - width * fillFraction,yOffset, width*fillFraction, height);
					break;
				}

				int textWidth, textHeight;
				text.getSize(w, &textWidth, &textHeight);
				textHeight = text.size * 7;
				switch (textAlignment) {
				case 0: //left
					text.offsetX( xOffset);
					text.offsetY(height/2 - textHeight / 2 + yOffset);
					break;
				case 1: //center
					text.offsetX(width / 2 - textWidth / 2 + xOffset);
					text.offsetY(height / 2 - textHeight / 2 + yOffset);
					break;
				case 2: //right
					text.offsetX(width - textWidth + xOffset);
					text.offsetY(height / 2 - textHeight / 2 + yOffset);
					break;
				}
				
				
				renderer->setQuadRendererMode(GL_TRIANGLES);
				renderer->setColor(fillColor.r, fillColor.g, fillColor.b, fillColor.a);
				renderer->render();
				
				if (showFractionLines) {
					for (int i = 1;i < fractionLineCount;i++) {
						renderer->setPos(xOffset+i*width/fractionLineCount, yOffset, 1, height);
						renderer->setQuadRendererMode(GL_LINE_LOOP);
						renderer->setColor(fractionLineColor.r, fractionLineColor.g, fractionLineColor.b, fractionLineColor.a);
						renderer->render();
					}
				}

				renderer->setPos(xOffset, yOffset, width, height);
				renderer->setQuadRendererMode(GL_LINE_LOOP);
				renderer->setColor(outlineColor.r, outlineColor.g, outlineColor.b, outlineColor.a);
				renderer->render();
				
				if (showText)
					text.render(w);
			}
		}

		inline void updateText() {
			std::string newText;
			if (showMax)
				newText = std::format("{}/{}", (int)std::ceil(currentFill), (int)std::ceil(maxFill));// ceil to avoid "0/max" output - will be "1/max" unitil death
			else
				newText = std::format("{}", (int)std::ceil(currentFill));
			text.setText(newText);
		}
		inline void clampFilling() {
			fillFraction = std::min(1.0f,std::max(fillFraction, 0.0f));
			currentFill = fillFraction * maxFill;
		}

		void setFillFraction(float fill) {
			fillFraction = fill;
			currentFill = fill* maxFill;
			updateText();
			if (clampFill) clampFilling();
		}
		void setFill(float fill) {
			currentFill = fill;
			fillFraction = fill / maxFill;
			updateText();
			if (clampFill) clampFilling();
		}
		void setMaxFill(float fill) {
			maxFill = fill;
			fillFraction = currentFill / maxFill;
			if (clampFill) clampFilling();
			fractionLineValue = pow(5, (int)log10(maxFill-1));
			fractionLineCount = maxFill / fractionLineValue;
			if (fractionLineCount > 10)
			{
				fractionLineValue = pow(10, (int)log10(maxFill - 1));
				fractionLineCount = maxFill / fractionLineValue;
			}
		}
		void setOutlineColor(glm::vec4 color) {
			outlineColor = color;
		}
		void setFillColor(glm::vec4 color) {
			fillColor = color;
		}
		void setFractionLineColor(glm::vec4 color) {
			fractionLineColor = color;
		}

		void offsetX(int offset) override { xOffset = offset; }
		void offsetY(int offset) override { yOffset = offset; }
		void getSize(const gui::Window* w, int* x, int* y) const override { *x = width; *y = height; }
		void getPos(const gui::Window* w, int* x, int* y) const override { *x = xOffset; *y = yOffset; }
		void setSize(int width, int height) { this->width = width;this->height = height; }
	};
}