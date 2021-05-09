#include <vector>
#include <random>
#include <algorithm>

#include "Game.hpp"
#include "Tetrimino.hpp"

Game make_game() {
	int board_width = 10;
	int board_height = 20;
	sf::Color wall_color = sf::Color::White;

	Game game;
	game.place_position = sf::Vector2(board_width / 2, 9);
	game.player_tetrimino = make_random(game.place_position);
	fill_bag(game);

	game.font.loadFromFile("resources/Pixeboy-z8XGD.ttf");
	game.text.setFont(game.font);
	game.text.setCharacterSize(64);
	game.text.setFillColor(sf::Color::White);

	// set walls
	for (int i = 1; i < board_height + 10; ++i) {
		game.walls.push_back(
				make_block(wall_color, sf::Vector2i(0, i)));
		game.walls.push_back(
				make_block(wall_color, sf::Vector2i(board_width + 1, i)));
	}

	// set floor
	for (int i = 0; i <= board_width + 1; ++i) {
		game.walls.push_back(
				make_block(wall_color, sf::Vector2i(i, board_height + 10)));
	}

	return game;
}

// collision check, real simple because blocks are
// grid and rotation locked, and the same size
bool check_collision(Game& level) {
	bool is_collision = false;;
	for (auto b : level.player_tetrimino.blocks) {
		for (auto w : level.walls) {
			if (w.position == level.player_tetrimino.position + b.position) {
				is_collision = true;
			}
		}
		for (auto &[_, pb] : level.placed_blocks) {
			for (auto p : pb) {
				if (p.position == 
						level.player_tetrimino.position
						+ b.position) {
					is_collision = true;
				}
			}
		}
	}
	return is_collision;
}

bool move_player_tetrimino(Game& level, int x, int y) {
	level.player_tetrimino.position.x += x;
	level.player_tetrimino.position.y += y;

	bool collision = check_collision(level);
	if (collision) {
		level.player_tetrimino.position.x -= x;
		level.player_tetrimino.position.y -= y;
	}
	return collision;
}

// there's a bug where you can rotate into walls,
// this function should fix that
void rotate_player_tetrimino(Game& level);

void hold_tetrimino(Game& level) {
	if (!level.held_tetrimino.has_value()) {
		level.held_tetrimino = level.next_tetriminos.front();
		level.next_tetriminos.pop();
	}
	std::swap(level.player_tetrimino, *level.held_tetrimino);
}

bool is_game_over(const Game& level) {
	for (auto &[row, blocks] : level.placed_blocks) {
		// inverted y access + an offset for the max height
		if (row < 10 && blocks.size() > 0) { return true; }
	}
	return false;
}

void fill_bag(Game& level) {
	std::vector<Tetrimino> bag;
	bag.push_back(make_tetrimino(sf::Vector2i(-10, -10), 0));
	bag.push_back(make_tetrimino(sf::Vector2i(-10, -10), 1));
	bag.push_back(make_tetrimino(sf::Vector2i(-10, -10), 2));
	bag.push_back(make_tetrimino(sf::Vector2i(-10, -10), 3));
	bag.push_back(make_tetrimino(sf::Vector2i(-10, -10), 4));
	bag.push_back(make_tetrimino(sf::Vector2i(-10, -10), 5));
	bag.push_back(make_tetrimino(sf::Vector2i(-10, -10), 6));

	auto rng = std::default_random_engine();
	std::shuffle(bag.begin(), bag.end(), rng);

	for (auto t : bag) {
		level.next_tetriminos.push(t);
	}
}

// scoring and level up go in here
void clear_rows(Game& level) {
	int lines_cleared = 0;
	for (auto &[_, blocks] : level.placed_blocks) {
		if (blocks.size() >= 10) {
			blocks.clear();
			lines_cleared++;
		}
	}
	if (lines_cleared == 0) return;

	// something buggy here
	for (auto &[row, blocks] : level.placed_blocks) {
		if (!blocks.empty()) continue;

		for (int next_row = row - 1; next_row > 0; next_row--) {
			for (auto &b : level.placed_blocks[next_row]) {
				b.position.y++;
			}

			std::swap(level.placed_blocks[next_row + 1],
					level.placed_blocks[next_row]);
		}

	}


	// scoring and leveling up
	switch (lines_cleared) {
		case 1: level.score += level.level *  800; break;
		case 2: level.score += level.level * 1200; break;
		case 3: level.score += level.level * 1800; break;
		case 4: level.score += level.level * 2000; break;
	}
	level.lines_cleared += lines_cleared;
	while (level.lines_cleared >= level.level * 5) {
		level.level++;
		if (level.fall_speed > 100.0f) level.fall_speed -= 33.3f;
	}
}

void place_tetrimino(Game& level) {
	auto pos = level.player_tetrimino.position;
	for (auto b : level.player_tetrimino.blocks) {
		b.position += pos;
		level.placed_blocks[b.position.y].push_back(b);
	}
	clear_rows(level);

	if (is_game_over(level)) {
		std::cout << "Game Over!\n";
		return;
	}

	level.player_tetrimino = level.next_tetriminos.front();
	level.player_tetrimino.position = level.place_position;
	level.next_tetriminos.pop();
	if (level.next_tetriminos.size() < 7) {
		fill_bag(level);
	}
}

void draw_game(sf::RenderWindow& window, sf::RenderStates& states, Game& level) {
	for (auto b : level.walls) {
		states.transform = sf::Transform::Identity;
		draw_block(window, states, b);
	}

	for (auto &[_, blocks] : level.placed_blocks) {
		for (auto b : blocks) {
			states.transform = sf::Transform::Identity;
			draw_block(window, states, b);
		}
	}

	draw_tetrimino(window, states, level.player_tetrimino);

	level.text.setString("Score: " + std::to_string(level.score));
	level.text.setPosition(sf::Vector2f(420, 64));
	window.draw(level.text);

	level.text.setString("Level: " + std::to_string(level.level));
	level.text.setPosition(sf::Vector2f(420, 128));
	window.draw(level.text);
}

// this'll need to get updated to allow player input and all that
void update_game(Game& level, std::vector<Input> inputs) {
	if (level.timer.getElapsedTime().asMilliseconds() > level.fall_speed) {
		bool collision = move_player_tetrimino(level, 0, 1);
		level.timer.restart();
		if (collision) place_tetrimino(level);
	}

	if (inputs[Command::LEFT].is_just_pressed) {
		move_player_tetrimino(level, -1, 0);
	}
	if (inputs[Command::RIGHT].is_just_pressed) {
		move_player_tetrimino(level, 1, 0);
	}
	if (inputs[Command::ROTATE_CW].is_just_pressed) {
		rotate(level.player_tetrimino);
	}
	if (inputs[Command::HARD_DROP].is_just_pressed) {
		while (!move_player_tetrimino(level, 0, 1)) { }
		place_tetrimino(level);
	}
}
