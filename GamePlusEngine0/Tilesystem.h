 #pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>

#include "Texture2D.h"
#include "TextureLoader.h"
#include "FileIO.h"

#include <array>
#include "Renderer.h"
#include "SpritsheetLoader.h"
#include "Defines.h"

namespace IceEngine {
	
	struct Tile_Info {
		int ID;
		int x_offset;
		int y_offset;
		int image_tile_width; // in pixels
		int image_tile_height; // in pixels
		std::string name;
	};

	class Tileset {
	public:
		explicit Tileset(const std::string filePath, const std::string textFilePath) {
			// Load Sprite Sheet
			m_spritesheet = TextureLoader::LoadTexture(filePath);

			// Load Tiles Info from File
			auto text = LoadTextFile(textFilePath.c_str());
			if (!text) {
				Logger::Instance().Log("Error opening tiles text file", LogLevel::ERROR);
				return;
			}

			std::string line = "";
			int indexStr = 1;
			int IdIcr = 0;

			for (int i = 0; text[i] != '\0'; i++) {
				if (text[i] != '\n') line += text[i];
				else {
					line += '\n';
					std::array<std::string, 5> lineInfo;
					// 0 1 2 3 4      i++ % 4
					int inc = 0;
					int structIndx = 0;
					std::string subLineStr = "";
					while (true) {
						if (line[inc] == ' ' || line[inc] == '\n') {
							switch (structIndx) {
							case 0: lineInfo[0] = subLineStr; break;
							case 1: lineInfo[1] = subLineStr; break;
							case 2: lineInfo[2] = subLineStr; break;
							case 3: lineInfo[3] = subLineStr; break;
							case 4: {
								lineInfo[4] = subLineStr;

								Tile_Info tile_info;
								tile_info.name = lineInfo[0];
								tile_info.x_offset = std::stoi(lineInfo[1]);
								tile_info.y_offset = std::stoi(lineInfo[2]);
								tile_info.image_tile_width = std::stoi(lineInfo[3]);
								tile_info.image_tile_height = std::stoi(lineInfo[4]);
								tile_info.ID = IdIcr++;
								m_tiles.push_back(tile_info);
								lineInfo[0] = lineInfo[1] = lineInfo[2] = lineInfo[3] = lineInfo[4] = "";
							} break;
							}
							structIndx = structIndx + 1 % 4;
							subLineStr = "";
							if (line[inc] == '\n') break;
						}
						else {
							subLineStr += line[inc];
						}
						inc++;
					}
					line = "";
				}
			}
		}

		Tile_Info GetTile(int ID) const {
			return m_tiles[ID];
		}

		Texture2D GetTexture() const {
			return m_spritesheet;
		}

	private:
		Texture2D m_spritesheet;
		std::vector<Tile_Info> m_tiles;
	};


	struct Tile_Draw {
		int ID;
		glm::vec2 world_position; // x, y
		glm::vec2 world_size;     // width, height
		std::vector<glm::vec2> texture_coords;
	};


	struct Layer {
		int m_priority; // 0 draws first 1 on top and so on...
		std::vector<Tile_Draw> m_tiles;
	};

	class Tilemap {
	public:
		Tilemap() = default;

		void AddLayerFromCSV(const std::string filePath) {
			
			if (!m_tileset) {
				Logger::Instance().Log("Tileset is not yet set", LogLevel::ERROR);
				return;
			}

			// Open Map File CSV and Parse it
			auto text = LoadTextFile(filePath.c_str());
			if (!text) {
				Logger::Instance().Log("Error opening map csv file as a text file", LogLevel::ERROR);
				return;
			}

			Layer new_layer = {};
			new_layer.m_priority = index++;
			m_tileSize = 16;
			int x = 0;
			int y = 0;
			
			// Parse CSV and add to layer Tile_Draws vector
			for (int i = 0; text[i] != '\0'; i++) {
				if (text[i] == ',') continue;
				if (text[i] == '\n') {
					x = 0;
					y += m_tileSize;
					continue;
				}

				// Handle -1 case
				if (text[i] == '-') {
					new_layer.m_tiles.push_back({ -1, {x,y}, {0,0}, {} });
					x += m_tileSize;

					// Skip two positions - and 1
					i += 1; 
					continue;
				}
				else {
					std::string advance = "";
					int k = i;
					while (text[k] != ',') {
						advance += text[k];
						k++;
					}

					int ID = std::stoi(advance);
					auto tile = m_tileset->GetTile(ID);
					auto textureCoords = GetTextureCoords(m_tileset->GetTexture(), tile.y_offset, tile.x_offset, tile.image_tile_width, tile.image_tile_height);

					new_layer.m_tiles.push_back({ ID , {x,y}, {tile.image_tile_width, tile.image_tile_height}, textureCoords });
					x += m_tileSize;
					i += static_cast<int>(advance.length());
				}
			}
		
			// Add to layers
			m_layers.push_back(new_layer);
			
			m_width = x * m_tileSize;
			m_height = y * m_tileSize;
		}

		void SetTileset(Tileset *tileset) {
			m_tileset = tileset;
		}

		void Draw() {
			// Draw tile map
		 
			for (auto &layer : m_layers) {
				for (auto &tile : layer.m_tiles) {
					
					if (tile.ID == -1) continue;

					// Adjust the offset as needed
					glm::vec2 offset(0,0);

					// Calculate the drawing position with size consideration
					glm::vec2 position = offset + glm::vec2(tile.world_position.x, tile.world_position.y);
					float tileRotation = 0;
					glm::vec2 tileSize = tile.world_size;

					// Translation matrix without the pivot adjustment
					glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(position.x, position.y, 0.0f));

					// Rotation matrix (assuming rotation is in radians)
					glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(tileRotation), glm::vec3(0.0f, 0.0f, 1.0f));

					// Scale matrix
					glm::mat4 scaling = glm::scale(glm::mat4(1.0f), glm::vec3(tileSize.x, -tileSize.y, 1.0f));

					// Combine the transformations
					glm::mat4 modelMatrix = translation * rotationMatrix * scaling;

					// Adjust the size for drawing
					tileSize = { tileSize.x / SCREEN_WIDTH, tileSize.y / SCREEN_HEIGHT };

					// Draw the tile
					auto texture_id = m_tileset->GetTexture().id;
					Renderer::DrawQuad(glm::vec2(0, 0), glm::vec2(1.0f / SCREEN_WIDTH, 1.0f / SCREEN_HEIGHT), modelMatrix, texture_id, tile.texture_coords);
				}
			}
		}
		
	private:
		std::vector<Layer> m_layers;
		int m_tileSize;
		int m_width;
		int m_height;
		Tileset *m_tileset;
		int index = 0;
	};
}
