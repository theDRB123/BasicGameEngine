#include <iostream>
#include <Windows.h>
#include <chrono>

using namespace std;

int nScreenWidth = 120;
int nScreenHeight = 40;

// the X , Y positions , and the Angle the player begins with
float fPlayerX = 2.0f;
float fPlayerY = 2.0f;
float fPlayerA = 0.0f;

float fFOV = 3.14 / 4.0f;

// the size of the map
int nMapHeight = 25;
int nMapWidth = 25; 

int speedPan = 400;
int speedTranslate = 800;

//we want a consistent frame speed , so we use two time points to find an absolute speed
auto tp1 = chrono::system_clock::now();
auto tp2 = chrono::system_clock::now();

int main()
{
	tp2 = chrono::system_clock::now();
	chrono::duration<float> elapsedTime = tp2 - tp1;
	tp1 = tp2;
	float fElapsedTime = elapsedTime.count();
	//now we can just multiply all of our speed with this elapsed time , so if the rate is slow then the speed increases to cover it up


	// Create Screen Buffer
	wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];
	// here we are defining the screen size for our buffer
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	// we are creating a new handle . createConsoleSearchBuffer , with generic read and write privilages
    SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0; // the number of characters to be written

	wstring map;
	// visually creating a map
	map += L"#########################";
	map += L"#.......................#";
	map += L"#.......................#" ;
	map += L"#...............#...#...#";
	map += L"#...............#...#...#";
	map += L"#...#############...#...#";
	map += L"#...#...........#...#...#";
	map += L"#...#...........#...#...#";
	map += L"#...#...........#...#...#";
	map += L"#...#...........#...#...#";
	map += L"#####...#########...#####";
	map += L"#.......................#";
	map += L"#####...#############...#";
	map += L"#...#...##..........#...#";
	map += L"#...#...............#...#";
	map += L"#...#...............#...#";
	map += L"#...#...##..........#...#";
	map += L"#...#...#...........#...#";
	map += L"#...#...#...........#...#";
	map += L"#...#...#############...#";
	map += L"#...#...................#";
	map += L"#...#...................#";
	map += L"#...#...#############...#";
	map += L"#...#...#...........#...#";
	map += L"#########################";

	while (1)
	{
		
		//controls
		//Handle CCW Rotation
		if (GetAsyncKeyState((unsigned short)'A') & 0x8000) {
			fPlayerA -= (speedPan * fElapsedTime);
		}
		if (GetAsyncKeyState((unsigned short)'D') & 0x8000) {
			fPlayerA += (speedPan * fElapsedTime);
		}

		//unit vector for motion
		float nUnitX = cosf(fPlayerA);
		float nUnitY = sinf(fPlayerA);

		if ((GetAsyncKeyState((unsigned short)'W') & 0x8000)) {
			fPlayerX += nUnitX * speedTranslate * fElapsedTime;
			fPlayerY += nUnitY * speedTranslate * fElapsedTime;
			if (map[((int)fPlayerY * nMapWidth + (int)fPlayerX)] == '#') {
				fPlayerX -= nUnitX * speedTranslate * fElapsedTime;
				fPlayerY -= nUnitY * speedTranslate * fElapsedTime;
			}
		}if (GetAsyncKeyState((unsigned short)'S') & 0x8000) {
			fPlayerX -= nUnitX * speedTranslate * fElapsedTime;
			fPlayerY -= nUnitY * speedTranslate * fElapsedTime;
			if (map[((int)fPlayerY * nMapWidth + (int)fPlayerX)] == '#') {
				fPlayerX += nUnitX * speedTranslate * fElapsedTime;
				fPlayerY += nUnitY * speedTranslate * fElapsedTime;
			}
		}

		for (int x = 0; x < nScreenWidth; x++)
		{
			float fRayAngle = (fPlayerA - fFOV / 2.0f) + ((float)x / (float)nScreenWidth) * fFOV; // this is the angle for which we are calculating the current ray

			// creating a unit vector
			float nEyeX = cosf(fRayAngle);
			float nEyeY = sinf(fRayAngle);

			float fDepth = 25.00f;
			float fDistanceToWall = 0.00f;

			bool bHitWall = false;
			bool bHitCorner = false;

			while (!bHitWall && fDistanceToWall < fDepth)
			{
				fDistanceToWall += 0.1f;

				int nTestX = (int)(fPlayerX + nEyeX * fDistanceToWall);
				int nTestY = (int)(fPlayerY + nEyeY * fDistanceToWall);

				// if the ray goes out of bounds
				if ((nTestX < 0 || nTestX > nMapWidth) || (nTestY < 0 || nTestY > nMapHeight))
				{
					bHitWall = true;
					fDistanceToWall = fDepth;
				}
				else
				{
					if (map[nTestY * nMapWidth + nTestX] == '#')
					{
						bHitWall = true;
					}
				}
			}


			float var = (fPlayerX + nEyeX * fDistanceToWall) + (fPlayerY + nEyeY * fDistanceToWall);

			int var2 = (fPlayerX + nEyeX * fDistanceToWall) + (fPlayerY + nEyeY * fDistanceToWall);
			double range = 1.00f;

			if ( var - var2 <= 0.1)
			{
				bHitCorner = true;
			}

			// now comes the display part
			// we will be calculating how much we have to make the column height
			// to do so we will find the distance to the column start (ceiling ) and distance to the column end (floor)
			int nCeiling = (float)(nScreenHeight / 2.0) - (nScreenHeight / ((float)fDistanceToWall) * 1.5);
			// thus as the distance to wall increases the ceiling comes closer and closer to the midpoint (i.e the column is shrinking)
			int nFloor = nScreenHeight - nCeiling;


			//we want to change the shading depending on the distance to the wall
			short nShade = ' ';

			if (fDistanceToWall <= fDepth / 4.0f)				nShade = 0x2588;
			else if (fDistanceToWall <= fDepth / 3.0f)			nShade = 0x2593;
			else if (fDistanceToWall <= fDepth / 2.0f)			nShade = 0x2592;
			else if (fDistanceToWall <= fDepth)					nShade = 0x2591;
			else												nShade = 0x2591;

			// drawing
			for (int y = 0; y < nScreenHeight; y++)
			{
				if (y <= nCeiling)
				{
					screen[y * nScreenWidth + x] = ' ';
				}
				else if ((y > nCeiling) && (y < nFloor))
				{
					if (bHitCorner)
					{
						screen[y * nScreenWidth + x] = ' ';
					}else 
					screen[y * nScreenWidth + x] = nShade;
				}
				else if(y >= nFloor)
				{
					char nFShade;
					int b = (nScreenHeight - y);

					if (b < 2)			nFShade = '#';
					else if (b < 4)      nFShade = 'x';
					else if (b < 5)      nFShade = '=';
					else if (b < 10)       nFShade = '_';
					else					nFShade = '.';

					screen[y * nScreenWidth + x] = nFShade;
				}
			}
		}
		
		//drawing the map
		swprintf(screen, 40, L"X=%3.2f , Y=%3.2f , A=%3.2f , FPS=%3.2f", fPlayerX, fPlayerY, fPlayerA, 1.0f / fElapsedTime);

		//printing the map
		for (int x = 0; x < 25; x++) {
			for (int y = 0; y < 25; y++)
			{
				screen[y * nScreenWidth + x+95] = map[y*25 + x ];
			}
		}

		//printing the player position

		screen[(int)fPlayerX + 95 + (int)fPlayerY * nScreenWidth] = 'P';

		screen[nScreenWidth * nScreenHeight - 1] = '\0';
		// here we are defining the last character of this array to be the escape char so that the below function knows where to stop
		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0, 0 }, &dwBytesWritten);
		
	}

	return 0;
}
