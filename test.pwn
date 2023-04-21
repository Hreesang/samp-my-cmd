// -
// External Packages
// -

#include <crashdetect>
#include <test-boilerplate>


// -
// Internal Packages
// -

#include "my_cmd"

#include <YSI_Coding\y_va>
#include <YSI_Coding\y_hooks>
#include <YSI_Server\y_colors>


// -
// Internals
// -

main() {}

static gs_processTick;

hook OnPlayerCommandText(playerid, cmdtext[])
{
	gs_processTick = GetTickCount();
	return 1;
}

hook OnPlayerCommandPerformed(playerid, cmdtext[], success)
{
	new string[144];
	format(
		string, 
		sizeof string, 
		"[Debug] "#LIGHTGREY"Command \"%s\" is executed, success: %s (time taken: %ims).", 
		cmdtext, 
		success ? "true" : "false",
		GetTickCount()-gs_processTick
	);
	SendClientMessage(playerid, X11_GOLD, string);

	if (!success) {
		SendClientMessage(playerid, X11_TOMATO, "[Error] "#GREY"That command doesn't exist!");
	}

	return 1;
}

@commandInit(cmdid) wow;
{
	printf("Command \"wow\" (ID: %i) is initialized.", cmdid);
	return 1;
}

@alias("w", "wo") wow;
CMD:wow(playerid, const params[])
{
	SendClientMessage(playerid, X11_GOLD, "Hello, my brother. You're wow-ing it, aren't you?");
	return 1;
}

@commandInit(cmdid) commands;
{
	printf("Command \"hello\" (ID: %i) is initialized.", cmdid);
	return 1;
}

@alias("help", "cmds", "cmd") commands;
CMD:commands(playerid, const params[])
{
	new totalCommands = GetTotalCommands();
	va_SendClientMessage(playerid, X11_LIMEGREEN, "This server has %i commands:", totalCommands);

	for (new i = 0; i < totalCommands; i++) {
		new alias[MAX_COMMANDS];
		new totalAlias = GetCommandAlias(i, alias);

		new cmdName[MAX_COMMAND_NAME];
		GetCommandName(i, cmdName);

		if (totalAlias) {
			new string[128];
			for (new d = 0; d < totalAlias; d++) {
				new aliasCmdid = alias[d];
				new aliasName[MAX_COMMAND_NAME];
				
				GetCommandName(aliasCmdid, aliasName);

				if (d == 0) {
					format(string, sizeof string, "(/%s", aliasName);
				} else {
					format(string, sizeof string, "%s, /%s", string, aliasName);
				}
			}
			strcat(string, ")");

			va_SendClientMessage(playerid, X11_LIMEGREEN, "Command ID %i - /%s %s.", i, cmdName, string);
		} else {
			va_SendClientMessage(playerid, X11_LIMEGREEN, "Command ID %i - /%s.", i, cmdName);
		}
	}

	return 1;
}
