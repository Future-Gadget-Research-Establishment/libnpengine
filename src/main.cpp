/* 
 * libnpengine: Nitroplus script interpreter
 * Copyright (C) 2013 Mislav Blažević <krofnica996@gmail.com>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * */
#include "game.hpp"

#ifdef _WIN32
#include <iostream>
#include <fstream>
#endif

extern "C"
{
int
#ifdef _WIN32
__declspec(dllexport)
#endif
NitroplusMain()
{
#ifdef _WIN32
    using namespace std;
    ofstream log("log.txt");
    cout.rdbuf(log.rdbuf());
#endif
    // Note that scripts call .nss files, NOT nsb. This is a hack
    Game* pGame = new Game({"cg.npa", "nss.npa", "voice.npa", "sound.npa"}, "nss/sg00_01.nsb");
    pGame->Run();
    delete pGame;
    return 0;
}
}