/***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 *                                                                         *
 *   Copyright (C) 2009 by Mattias Runge <mattias@slacker.se>              *
 *                                                                         *
 *   File: spotify_cmd.cpp                                                 *
 *                                                                         *
 ***************************************************************************/
/*
 *  Inspiration and definitions from:
 *  http://code.google.com/p/pytify/
 *
 *  This project has no affiliation to spotify.com whatsoever!
 *
 *  Intended use is to control Spotify running under Linux with Wine.
 *
 *  Compile: wineg++ spotify_cmd.cpp -o spotify_cmd.exe
 *  Run example: ./spotify_cmd.exe status
 */

#include <windows.h>
#include <iostream>

#define VERSION "0.5"

#define APPCOMMAND 0x0319

#define CMD_PLAYPAUSE 917504
#define CMD_MUTE 524288
#define CMD_VOLUMEDOWN 589824
#define CMD_VOLUMEUP 655360
#define CMD_STOP 851968
#define CMD_PREVIOUS 786432
#define CMD_NEXT 720896

int main(int argc, char **argv)
{
	if (argc == 1)
	{
	    std::cout << "spotify_cmd version " << VERSION << ", copyright by Mattias Runge 2009" << std::endl;
	    std::cout << "Usage:" << " ./spotify_cmd [playpause|prev|next|stop|mute|volup|voldown|status]" << std::endl;
            return 0;
        }

	HWND window_handle = FindWindow("SpotifyMainWindow", NULL);

	if (window_handle == NULL)
	{
		std::cout << "ERROR" << std::endl;
		std::cout << "Can not find spotify, is it running?" << std::endl;
		return 1;
	}

	char buffer[512] = "";
	std::string artistName = "";
	std::string songName = "";

	if (GetWindowText(window_handle, buffer, sizeof(buffer)) > 0)
	{
		std::string title = buffer;
		std::string::size_type pos1 = title.find('-') + 2;
		std::string::size_type pos2 = title.find(static_cast<char>(-106));
		artistName = title.substr(pos1, pos2 - pos1 - 1);
		songName = title.substr(pos2 + 2);
	}

	std::string command = argv[1];

	if (command == "playpause")
	{
		SendMessage(window_handle, APPCOMMAND, 0, CMD_PLAYPAUSE);
		return 0;
	}
	else if (command == "next")
	{
		SendMessage(window_handle, APPCOMMAND, 0, CMD_NEXT);
		return 0;
	}
	else if (command == "prev")
	{
		SendMessage(window_handle, APPCOMMAND, 0, CMD_PREVIOUS);
		return 0;
	}
	else if (command == "stop")
	{
		SendMessage(window_handle, APPCOMMAND, 0, CMD_STOP);
		return 0;
	}
	else if (command == "mute")
	{
		SendMessage(window_handle, APPCOMMAND, 0, CMD_MUTE);
		return 0;
	}
	else if (command == "volup")
	{
		SendMessage(window_handle, APPCOMMAND, 0, CMD_VOLUMEUP);
		return 0;
	}
	else if (command == "voldown")
	{
		SendMessage(window_handle, APPCOMMAND, 0, CMD_VOLUMEDOWN);
		return 0;
	}
	else if (command == "status")
	{
		if (artistName == "" && songName == "")
		{
			std::cout << "WARN" << std::endl;
			std::cout << "Playback paused" << std::endl;
		}
		else
		{
			std::cout << artistName << std::endl;
			std::cout << songName << std::endl;
		}
		return 0;
	}

	return 0;
}
