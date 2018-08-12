// #include <SFML/Audio.hpp>

// int main()
// {
//     sf::SoundBuffer buffer;
//     // load something into the sound buffer:
//     if (!buffer.loadFromFile("sound.wav"))
//         return -1;

//     sf::Sound sound;
//     sound.setBuffer(buffer);
//     sound.play();

//     return 0;
// }

#include <SFML/Audio.hpp>
#include <cmath>
#include <iostream>

int main() {
	const unsigned SAMPLES = 44100;
	const unsigned SAMPLE_RATE = 44100;
	const unsigned AMPLITUDE = 30000;
	
	sf::Int16 raw[SAMPLES];

	const double PI = 3.141592654;
	const double increment = 440./44100;
	const double step = 1.0594630944;
	double f = pow(step,0);
	double x = 0;
	for (unsigned i = 0; i < SAMPLES; i++) {
	  raw[i] = AMPLITUDE * sin(x*f*PI);
		x += increment;
	}
	
	sf::SoundBuffer Buffer;
	if (!Buffer.loadFromSamples(raw, SAMPLES, 1, SAMPLE_RATE)) {
		std::cerr << "Loading failed!" << std::endl;
		return 1;
	}

	sf::Sound Sound;
	Sound.setBuffer(Buffer);
	Sound.setLoop(true);
	Sound.play();
	while (1) {
		sf::sleep(sf::milliseconds(100));
	}
	return 0;
}
