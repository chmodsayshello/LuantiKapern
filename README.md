# LuantiKapern

[https://www.duden.de/rechtschreibung/kapern](https://www.duden.de/rechtschreibung/kapern "https://www.duden.de/rechtschreibung/kapern")  "Kapern" basically is the German verb for "hijack".

# Purpose

This software let's you pretend to be a Luanti Server to take over sessions of players on a server of your choosing.

It was quickly hacked together in a little over an hour or two, so do not expect a code beauty.

**It is only intended to be used for demonstration purposes, use against unknowing players is not endorsed.**

# Building

LuantiLapern only builds on UNIX systems, and has only been tested on GNU/Linux. As it uses no libaries other than libc and system headers, you can simply feed all .c files into the C compiler of your choice (tested with gcc).

# Usage



As this software is indended to be a proof of concept, currently, you have to know the username of your "victim". This limitation can be easily overcome, yet I currently do not plan on adding that myself.

 1. Download minetest's source code and alter the constant `GAME_CONNECTION_TIMEOUT`, set it to a much higher value such as 3600 (technically optional)
	 - Compile the modified minetest
 2. Start the program with the following command `./binary -l <victim port> -b <your port> -a <server address> -p <server_port>`
 3. Connect to localhost with the attacker port using the modified version of minetest we've just created. Choose your victim's username. It is important that you type some random password in before connecting.
 4. Make your victim connect to localhost:victim_port

