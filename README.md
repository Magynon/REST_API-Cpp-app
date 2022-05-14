# Tema-3-PCom

I chose to use the *nlohmann* JSON library to ease myself of the tedious task of parsing the texts myself.

The main flux of the homework is in the *client.c* source file. Basically it is a never ending while loop which awaits input from the command line. I used the cin.getline() function to parse the input (to get rid of the '\n' at the end of every command).

Each command has its own function that first checks the integrity of the input data before sending it to the server. All the functions work somewhat the same - parse input as JSON, build HTML request, send it to the server, parse the server's response and notify the user accordingly (nothing too fancy). The logout function makes sure to reset the login cookie and JWT token locally, to make sure that the client doesn't have access to the library anymore, eventhough the server might still permit it.