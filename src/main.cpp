#include <iostream>

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

//#include "Game.hpp"
#include "Tetrimino.hpp"
#include "Block.hpp"

int main()
{
    // Create the main window
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Tetris");

    // Load a sprite to display
//    sf::Texture texture;
//    if (!texture.loadFromFile("./resources/block.png"))
//        return EXIT_FAILURE;
//    sf::Sprite sprite(texture);

    // Load a music to play
//    sf::Music music;
//    if (!music.openFromFile("resources/Shapeforms/Dystopia – Ambience and Drone Preview/AUDIO/AMBIENCE_SIGNAL_LOOP.wav"))
//        return EXIT_FAILURE;
//    music.play();

	std::vector<Tetrimino> tetri;
	tetri.push_back(make_tetrimino(sf::Vector2<int>(400, 300), 0));
	tetri.push_back(make_tetrimino(sf::Vector2<int>(800, 300), 1));
	tetri.push_back(make_tetrimino(sf::Vector2<int>(1200, 300), 2));
	tetri.push_back(make_tetrimino(sf::Vector2<int>(300, 600), 3));
	tetri.push_back(make_tetrimino(sf::Vector2<int>(600, 600), 4));
	tetri.push_back(make_tetrimino(sf::Vector2<int>(900, 600), 5));
	tetri.push_back(make_tetrimino(sf::Vector2<int>(1200, 600), 6));

	sf::Clock timer;

//	Game game = make_game();

	sf::RenderStates states = sf::RenderStates::Default;
	Block b1 = make_block(sf::Color::Cyan);
	Block b2 = make_block(sf::Color::Green, sf::Vector2i(1, 1));
	Block b3 = make_block(sf::Color::Magenta, sf::Vector2i(2, 2));
	Block b4 = make_block(sf::Color::Red, sf::Vector2i(3, 3));

	Tetrimino t = make_tetrimino(sf::Vector2i(10, 4), 0);

    // Start the game loop
    while (window.isOpen())
    {
        // Process events
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Close window: exit
            if (event.type == sf::Event::Closed)
                window.close();
        }
		
		// rotate blocks every 3/4th second
		if (timer.getElapsedTime().asSeconds() > 0.75f) {
			for (auto &t : tetri) {
				rotate(t);
			}
			rotate(t);
			timer.restart();
		}

//		run(game);

        // Clear screen
        window.clear();

        // Draw the sprite
		//for (auto t : tetri) { draw_tetrimino(t, window); }
		//for (auto b : game.placed_blocks) { window.draw(b); }
		//draw_tetrimino(game.player_tetrimino, window);
		states.transform = sf::Transform::Identity;
		draw_block(window, states, b1);
		states.transform = sf::Transform::Identity;
		draw_block(window, states, b2);
		states.transform = sf::Transform::Identity;
		draw_block(window, states, b3);
		states.transform = sf::Transform::Identity;
		draw_block(window, states, b4);

		draw_tetrimino(window, states, t);

        // Update the window
        window.display();
    }
    return EXIT_SUCCESS;
}
