#pragma once


class Game // ca devrait pas etre un struct ? (tout en public)
{
	float timer  = 60.f;
	bool chamber = true;
	bool coridor = false;
	bool kichen  = false; //Masterclass d'orthographe
	bool control = false;
	bool moteur  = false;
	bool labo	 = false;
	bool stokage = false;
};