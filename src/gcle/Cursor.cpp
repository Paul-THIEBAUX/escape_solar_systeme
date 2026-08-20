#include "Cursor.h"

void Cursor::OnInitialize()
{
	AddActiveScene("MainScene");
	AddActiveScene("CorridorScene");
	AddActiveScene("ReactorScene");
	AddActiveScene("LaboScene");
	AddActiveScene("CuisineScene");
	AddActiveScene("StorageScene");
	AddActiveScene("CabineScene");
}
