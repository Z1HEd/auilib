#pragma once

#include <4dm.h>

using namespace fdm;

namespace aui {
	// Actually kinda usable already but still gotta review this in the future
	class VBoxContainer : public gui::Element, public gui::ElemContainer
	{
	public:
		int currentCursorType = GLFW_ARROW_CURSOR;
		gui::Element* selectedElem;
		std::vector<gui::Element*> elements{};
		QuadRenderer& background = Item::qr;
		uint32_t width = 0;
		uint32_t height = 0;

		//  If >1 will try to arrange to multiple columns if possible
		unsigned int maxColumns = 1;

		// "Actual" topleft position of the box
		int xPos = 0;
		int yPos = 0;

		// Additional offset set by parent
		int xOffset = 0;
		int yOffset = 0;

		// Offsets of the first element
		int elementXOffset = 0;
		int elementYOffset = 0;

		// Additional size on top of what is automatically determined
		int xMargin = 10;
		int yMargin = 10;

		// Distance between "Actual" border of the box and its drawn background
		int xPadding = 8;
		int yPadding = 8;

		// Space between children
		int xSpacing = 0;
		int ySpacing = 0;

		gui::AlignmentX xAlign = gui::ALIGN_LEFT;
		gui::AlignmentY yAlign = gui::ALIGN_TOP;

		gui::AlignmentX elementXAlign = gui::ALIGN_LEFT;

		bool renderBackground = false;

		void render(gui::Window* w) override
		{
			if (elements.size() < 1) return;
			int Height = 0;
			int Width = 0;
			width = 0;
			height = 0;
			//Get size of the box based on child elements
			for (int i = 0;i < elements.size();i++) {
				elements[i]->getSize(w, &Width, &Height);

				//All characters in the font are 1 pixel higher, so have to manually calculate height of a text
				//Hope 0.3 font system will fix that 
				if (auto* text = dynamic_cast<gui::Text*>(elements[i]))
					Height = text->size * 7;

				if (Width > width) width = Width;
				height += Height + ySpacing;
			}

			w->getSize(&Width, &Height);
			width += xMargin*2;
			height += yMargin * 2;

			int columns = std::min(Width / (width+xSpacing), maxColumns);
			if (columns == 0) return;
			int rows = (elements.size() + columns - 1) / columns;
			// set own position
			switch (xAlign) {
			case gui::ALIGN_CENTER_X:
				xPos=(Width / 2 - width / 2);
				break;
			case gui::ALIGN_LEFT:
				xPos = 0;
				break;
			case gui::ALIGN_RIGHT:
				xPos = (Width - width-xPadding);
				break;
			};

			switch (yAlign) {
			case gui::ALIGN_CENTER_Y:
				yPos = (Height / 2 - height / 2);
				break;
			case gui::ALIGN_TOP:
				yPos = 0;
				break;
			case gui::ALIGN_BOTTOM:
				yPos = (Height - height-yPadding);
				break;
			};


			//Arrange child elements
			int posX = xPos + elementXOffset + xMargin-(columns-1)*(width/2+xSpacing) + xOffset;
			for (int j = 0;j < columns;j++) {
				int posY = yPos + elementYOffset + yMargin + yOffset;
				for (int i = 0;i < rows;i++) {
					int index = i * columns + j;
					if (index >= elements.size()) break;
					elements[index]->getSize(w, &Width, &Height);

					if (auto* text = dynamic_cast<gui::Text*>(elements[index]))
						Height = text->size * 7;

					elements[index]->offsetY(posY );

					switch (elementXAlign)
					{
					case gui::ALIGN_LEFT:
						elements[index]->offsetX(posX); // Level of identation: 7
						break;
					case gui::ALIGN_CENTER_X:
						elements[index]->offsetX(posX + width / 2 - Width / 2); // With every other level of nesting...
						break;
					case gui::ALIGN_RIGHT:
						elements[index]->offsetX(posX + width  - Width); // we stray further from god.
						break;
					}
					posY += Height + ySpacing;
				}
				posX += width+xSpacing;
			}
			
			//Render stuff
			if (renderBackground) {
				background.setPos(xPos+xOffset + xPadding,yPos+ yOffset + yPadding, width - xPadding * 2, height - yPadding * 2);
				background.setQuadRendererMode(GL_TRIANGLES);
				background.setColor(0, 0, 0, 0.5);
				background.render();

				background.setQuadRendererMode(GL_LINE_LOOP);
				background.setColor(1, 1, 1, 0.5);
				background.render();
			}
			
			for (int i = 0;i < elements.size();i++) {

				elements[i]->render(w);
			}
			
		}

		bool hasElement(gui::Element* element) {
			return std::find(elements.begin(), elements.end(), element) != elements.end() ;
		}

		void alignX(gui::AlignmentX alignment) override { xAlign = alignment; }
		void alignY(gui::AlignmentY alignment) override { yAlign = alignment; }
		void offsetX(int offset) override { xOffset = offset; }
		void offsetY(int offset) override { yOffset = offset; }
		void getSize(const gui::Window* w, int* x, int* y) const override { *x = width; *y = height; }
		void getPos(const gui::Window* w, int* x, int* y) const override { *x = xPos; *y = yPos; }

		void addElement(gui::Element* e) override
		{
			elements.push_back(e);
		}
		void addElement(gui::Element* e, int pos)
		{
			elements.insert(elements.begin()+pos,e);
		}
		bool removeElement(gui::Element* e) override
		{
			return elements.erase(std::remove(elements.begin(), elements.end(), e), elements.end()) != elements.end();
		}
		void clear() override
		{
			elements.clear();
		}
		bool empty() override
		{
			return elements.empty();
		}

		//Succesfully stolen from ContentBox.h


		bool mouseInput(const gui::Window* w, double xpos, double ypos) override
		{
			if (elements.empty()) return false;
			
			for (auto& el : elements)
			{
				bool r = el->mouseInput(w, xpos, ypos);
				if (r)
				{
					this->currentCursorType = el->getCursorType();
					return true;
				}
			}

			return false;
		}
		bool mouseButtonInput(const gui::Window* w, int button, int action, int mods) override
		{
			if (elements.empty()) return false;

			for (auto& el : elements)
			{
				bool r = el->mouseButtonInput(w, button, action, mods);
				if (r)
				{
					this->currentCursorType = el->getCursorType();
					selectedElem = el;
					return true;
				}
			}
			
		}
		void select() override 
		{ 
			if (elements.empty()) return;
			if (selectedElem == nullptr) return;
			selectedElem->select();
		}
		void deselect() override 
		{ 
			if (elements.empty()) return;
			if (selectedElem == nullptr) return;
			selectedElem->deselect();
			selectedElem = nullptr;
		}
	};
}