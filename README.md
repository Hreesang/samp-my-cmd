# MyCMD

[![sampctl](https://img.shields.io/badge/sampctl-samp--my--cmd-2f2f2f.svg?style=for-the-badge)](https://github.com/Hreesang/samp-my-cmd)

MyCMD is a library that allows you to manage your script commands. It enables an option for you to have an access to modify your commands when the script is initializing each of it, so there's no more hooking init callbacks (`OnGameModeInit` or `OnFilterScriptInit`) just to initialize a command.

## Installation

Simply install to your project:

```bash
sampctl install Hreesang/samp-my-cmd
```

Include in your code and begin using the library:

```c
#include <my_cmd>
```

## Usage

### Creating a Command

There are various ways to create a command that's supported by this library:

```c
COMMAND:yourcommand(playerid, const params[])
{
    return 1;
}

CMD:yourcommand(playerid, const params[])
{
    return 1;
}

command(yourcommand, playerid, const params[])
{
    return 1;
}

cmd(yourcommand, playerid, const params[])
{
    return 1;
}
```

When a player types `/yourcommand parameters`, the command's handler will be called. The `playerid` parameter will have the id of the player who used the command and the `params[]` parameter will have the text that was typed excluding the command itself (for the given example, `params[]` will have `parameters`). You must return `1` if the command was executed successfully; otherwise, return `0`. Returning `0` would make server send the "Unknown Command" message unless handled in `OnPlayerCommandPerformed`.

Note that `params[]` will never be empty. If the player did not provide any arguments to the command, `params[0]` will be `'\1'`. Use the `isnull(string[])` macro provided with the include to check for nullity.

### Adding Aliases

**With macro:**
```c
// @alias(aliases) cmdName;
@alias("help", "helpme", "assistme") assistance;
CMD:assistance(playerid, const params[])
{
    return 1;
}
```

**Without macro:**
```c
CMD:assistance(playerid, const params[])
{
    return 1;
}

public OnCommandsInit() // or OnGameModeInit / OnFilterScriptInit
{
    new assistanceID = GetCommandID("assistance");
    AddCommandAlias(assistanceID, "help", "helpme", "assistme");
    return 1;
}

// -
// -- OR --
// -

@commandInit(cmdid) assistance;
{
    new assistanceID = cmdid;
    // or 'new assistanceID = GetCommandID("assistance");'
    AddCommandAlias(assistanceID, "help", "helpme", "assistme");
    return 1;
}

CMD:assistance(playerid, const params[])
{
    return 1;
}

```

## Callbacks

### OnCommandsInit

This callback is called right after all the commands are initialized.

**Parameters:**
- N/A

**Return Values:**
- N/A

```c
public OnCommandsInit()
{
    return 1;
}
```

### OnPlayerCommandReceived

This callback is called everytime a player sends a command and before the command function is executed.

**Parameters:**
- `playerid` the player who used the command
- `cmdtext` text which the player typed (inclusive of the command delimiter `\` and the command)

**Return Values:**
- `1` - execute the command
- `0` - do not execute the command (not triggering unknown message warning)

```c
public OnPlayerCommandReceived(playerid, cmdtext[])
{
	return 1;
}
```

### OnPlayerCommandPerformed
This callback is called after a command has been executed.

**Parameters:**
- `playerid` the player who used the command
- `cmdtext` text which the player typed (inclusive of the command delimiter `\` and the command)
- `success` is what the command function returned (`CMD_SUCCESS` or `CMD_FAILURE`)

**Return Values:**
- `0` - triggers unknown command warning
- `1` - nothing happens

```c
public OnPlayerCommandPerformed(playerid, cmdtext[], success)
{
	return 1;
}
```

If you are not using `OnPlayerCommandPerformed` then the return value of the command function will be always `1`.

## Functions

#### GetTotalCommands
>* **Parameters:**
>* *
>* **Returns:**
>* * total created commands
>* **Remarks:**
>* * Returns the total of created commands in the script.

#### GetCommandID
>* **Parameters:**
>* * `const cmd[]`: command name
>* **Returns:**
>* * command ID of the specified command name
>* **Remarks:**
>* * Returns the ID of specified command name or `INVALID_COMMAND_ID` if not exists

#### IsValidCommand
>* **Parameters:**
>* * `cmdid`: command ID
>* **Returns:**
>* * `true` if command exists otherwise `false`
>* **Remarks:**
>* * 

#### GetCommandName
>* **Parameters:**
>* * `cmdid`: command ID
>* * `output[]`: array where the name will be passed to
>* * `len = sizeof output`: length of the output array
>* **Returns:**
>* * `false` if command not exists otherwise `true`
>* **Remarks:**
>* * Gets specified command ID's name

#### GetCommandFunctionName
>* **Parameters:**
>* * `cmdid`: command ID
>* * `output[]`: array where the function name will be passed to
>* * `len = sizeof output`: length of the output array
>* **Returns:**
>* * `false` if command not exists otherwise `true`
>* **Remarks:**
>* * Gets the specified command ID's function name. It's a callback name that would be executed when a player types the command ID's name.

#### AddCommandAlias
>* **Parameters:**
>* * `cmdid`: command ID
>* * `...`: strings that will be the aliases of the command ID
>* **Returns:**
>* * `false` if command not exists otherwise `true`
>* **Remarks:**
>* * Adds alias(es) for the specified command ID's

```c
public OnCommandsInit()
{
    new cmdid = GetCommandName("engine");
    AddCommandAlias(cmdid, "car_engine", "en", "e");
    // "/car_engine", "/en", "/e" are now aliases for "/engine".
    return 1;
}
```

#### IsCommandAlias
>* **Parameters:**
>* * `cmdid`: command ID
>* **Returns:**
>* * `true` if an alias otherwise `false` if not exists or not an alias
>* **Remarks:**
>* * 

#### GetCommandAlias
>* **Parameters:**
>* * `cmdid`: command ID
>* * `results`: array to store the command ID's alias(es)
>* * `size = sizeof results`: size of the results array
>* **Returns:**
>* * numbers of the aliases
>* **Remarks:**
>* * Gets specified command ID's aliases

```c
public OnCommandsInit()
{
    new cmdid = GetCommandName("engine");
    AddCommandAlias(cmdid, "car_engine", "en", "e");
    // cmdid = 0
    // /car_engine command ID = 1
    // /en command ID = 2
    // /e command ID = 3
    
    new alias[4];
    new totalAlias = GetCommandAlias(cmdid, alias);
    // alias = {1, 2, 3, INVALID_COMMAND_ID}
    // totalAlias = 3
    return 1;
}
```

#### EmulateCommand
>* **Parameters:**
>* * `playerid`: playerid who will execute the command
>* * `const cmd[]`: command name that'll be run if `cmdid` is `INVALID_COMMAND_ID`
>* * `const params[] = ""`: command parameters
>* * `cmdid = INVALID_COMMAND_ID`: command ID that'll be run (more prioritized than `const cmd[]` parameter)
>* **Returns:**
>* * `false` or `true` that depends on how you handle the return for valid and invalid commands on `OnPlayerCommandPerformed`.
>* **Remarks:**
>* * Force-run a command for the specified player

## Testing

To test, simply run the package:

```bash
sampctl run
```

## Credits

- **Hreesang** for making the library.
- **Yashas** for the I-ZCMD snippets and `README.md`.
