// -
// Include Guards
// -

#if defined _INC_my_cmd
	#undef _INC_my_cmd
#endif

#if defined _pp_mycmd_included
	#endinput
#endif
#define _pp_mycmd_included


// -
// External Packages
// -

#tryinclude <amx_header>
#if !defined AMX_HEADER_INC
	#error You need amx_assembly to use this library (https://github.com/Zeex/amx_assembly).
#endif

#tryinclude <md-sort>
#if !defined _INC_md_sort
	#error You need md-sort to use this library (https://github.com/oscar-broman/md-sort).
#endif

#tryinclude <sscanf2>
#if !defined _INC_SSCANF
	#error You need sscanf to use this library (https://github.com/Y-Less/sscanf).
#endif


// -
// Pre-Hooks
// -

#if !defined CHAIN_ORDER
	#define CHAIN_ORDER() 0
#endif

#define CHAIN_HOOK(%0) forward @CO_%0();public @CO_%0(){return CHAIN_ORDER()+1;}
#define CHAIN_NEXT(%0) @CO_%0

#define CHAIN_FORWARD:%0_%2(%1)=%3; \
	forward %0_%2(%1); \
	public %0_%2(%1) <_ALS : _ALS_x0, _ALS : _ALS_x1> { return (%3); } \
	public %0_%2(%1) <> { return (%3); }

#define CHAIN_PUBLIC:%0(%1) %0(%1) <_ALS : _ALS_go>

CHAIN_HOOK(MYC)
#undef CHAIN_ORDER
#define CHAIN_ORDER CHAIN_NEXT(MYC)

static stock _MYC_IncludeStates() <_ALS : _ALS_x0, _ALS : _ALS_x1, _ALS : _ALS_x2, _ALS : _ALS_x3> {}
static stock _MYC_IncludeStates() <_ALS : _ALS_go> {}


// -
// Declarations
// -

#if !defined MAX_COMMANDS
	#define MAX_COMMANDS				128
#endif

#if !defined INVALID_COMMAND_ID
	#define INVALID_COMMAND_ID			-1
#endif

#if !defined MAX_COMMAND_FUNC_NAME
	#define MAX_COMMAND_FUNC_NAME 		32
#endif

#if !defined MAX_COMMAND_NAME
	#define MAX_COMMAND_NAME 			28
#endif


// Macros
#define COMMAND:%1(%2) \
			forward cmd_%1(%2); \
			public cmd_%1(%2)
			
#define CMD:%1(%2) \
			COMMAND:%1(%2)

#define command(%1,%2,%3) \
			COMMAND:%1(%2, %3)

#define cmd(%1,%2,%3) \
			COMMAND:%1(%2, %3)

#define cint_%0\32; cint_
#define @commandInit(%2)%1; \
			forward cint_%1(%2); \
			public cint_%1(%2)

#define @cmdInit(%2)%1; \
			@commandInit(%2)%1;

#define altc_%0\32; altc_
#define @alias(%2)%1; \
			forward altc_%1(); \
			public altc_%1() { return AddCommandAlias(GetCommandID(#%1), %2), 1; }

#if !defined isnull
	#define isnull(%1) \
				((!(%1[0])) || (((%1[0]) == '\1') && (!(%1[1]))))
#endif


static enum E_CMD_INFO {
	E_CMD_NAME[MAX_COMMAND_NAME],
	E_CMD_FUNC_NAME[MAX_COMMAND_FUNC_NAME],
	E_CMD_POINTING_ID,
}

static stock gs_commandsInfo[MAX_COMMANDS][E_CMD_INFO];
static stock gs_totalCommands;


// -
// API
// -

forward OnCommandsInit();
forward OnPlayerCommandReceived(playerid, cmdtext[]); 
forward OnPlayerCommandPerformed(playerid, cmdtext[], success); 

stock GetTotalCommands()
{
	return gs_totalCommands;
}

stock GetCommandID(const cmd[])
{
	new cmdid = INVALID_COMMAND_ID;
	for (new i = 0; i < gs_totalCommands; i++) {
		if (strcmp(gs_commandsInfo[i][E_CMD_NAME], cmd) == 0) {
			cmdid = i;
			break;
		}
	}
	return cmdid;
}

stock bool:IsValidCommand(cmdid)
{
	return (0 <= cmdid < gs_totalCommands);
}

stock bool:GetCommandName(cmdid, output[], len = sizeof output)
{
	if (!IsValidCommand(cmdid)) {
		return false;
	}

	output[0] = EOS;
	strcat(output, gs_commandsInfo[cmdid][E_CMD_NAME], len);

	return true;
}

stock bool:GetCommandFunctionName(cmdid, output[], len = sizeof output)
{
	if (!IsValidCommand(cmdid)) {
		return false;
	}

	output[0] = EOS;
	strcat(output, gs_commandsInfo[cmdid][E_CMD_FUNC_NAME], len);
	
	return true;
}

stock bool:AddCommandAlias(cmdid, ...)
{
	if (!IsValidCommand(cmdid)) {
		return false;
	}

	new args = numargs();
	for (new arg = 1; arg < args; arg++) {
		if (gs_totalCommands >= MAX_COMMANDS) {
			break;
		}

		new aliasName[MAX_COMMAND_NAME];
		new pos;
		new c;

		while ((c = getarg(arg, pos)) != '\0' && pos < sizeof aliasName) {
			aliasName[pos] = c;
			pos++;
		}

		new idx = gs_totalCommands++;
		strcat(gs_commandsInfo[idx][E_CMD_NAME], aliasName, MAX_COMMAND_NAME);
		strcat(gs_commandsInfo[idx][E_CMD_FUNC_NAME], gs_commandsInfo[cmdid][E_CMD_FUNC_NAME], MAX_COMMAND_FUNC_NAME);
		gs_commandsInfo[idx][E_CMD_POINTING_ID] = cmdid;
	}

	return true;
}

stock bool:IsCommandAlias(cmdid)
{
	if (!IsValidCommand(cmdid)) {
		return false;
	}

	return gs_commandsInfo[cmdid][E_CMD_POINTING_ID] != INVALID_COMMAND_ID;
}

stock GetCommandAlias(cmdid, results[], size = sizeof results)
{
	if (!IsValidCommand(cmdid)) {
		return 0;
	}

	new totalAlias;
	for (new i = 0; i < gs_totalCommands; i++) {
		if (gs_commandsInfo[i][E_CMD_POINTING_ID] != cmdid) {
			continue;
		}
		
		new count = totalAlias++;
		results[count] = i;

		if (totalAlias >= size) {
			break;
		}
	}

	return totalAlias;
}

stock bool:EmulateCommand(playerid, const cmd[], const params[] = "", cmdid = INVALID_COMMAND_ID)
{
	if (cmdid == INVALID_COMMAND_ID) {
		cmdid = GetCommandID(cmd);
	}

	new cmdtext[MAX_COMMAND_NAME + (128-MAX_COMMAND_NAME)] = "/";
	strcat(cmdtext, cmd);
	if (!isnull(params)) {
		strcat(cmdtext, " ");
		strcat(cmdtext, params);
	}

	if (!IsValidCommand(cmdid)) {
		return bool:OnPlayerCommandPerformed(playerid, cmdtext, 0);
	}

	new cmdret = CallLocalFunction(gs_commandsInfo[cmdid][E_CMD_FUNC_NAME], "is", playerid, isnull(params) ? "\1" : params);
	return bool:OnPlayerCommandPerformed(playerid, cmdtext, cmdret);
}


// -
// Internals
// -

public OnGameModeInit()
{
    state _ALS : _ALS_go;
	MYC_InitializeCommands();

    return MYC_OnGameModeInit();
}

public OnFilterScriptInit()
{
    state _ALS : _ALS_go;
	MYC_InitializeCommands();

    return MYC_OnFilterScriptInit();
}

static MYC_InitializeCommands()
{
	new hdr[AMX_HDR];
	GetAmxHeader(hdr);
	new totalPublics = GetNumPublics(hdr);

	new const cmdPrefix[] = "cmd_";
	new const cmdPrefixLen = sizeof cmdPrefix;

	// Initialize commands...
	for (new i = 0; i < totalPublics; i++) {
		new publicName[MAX_COMMAND_FUNC_NAME];
		GetPublicNameFromIndex(i, publicName);

		if (strfind(publicName, cmdPrefix) != 0) {
			continue;
		}

		new cmdName[MAX_COMMAND_NAME];
		strmid(cmdName, publicName, cmdPrefixLen - 1, strlen(publicName));

		new idx = gs_totalCommands++;
		strcat(gs_commandsInfo[idx][E_CMD_NAME], cmdName, MAX_COMMAND_NAME);
		strcat(gs_commandsInfo[idx][E_CMD_FUNC_NAME], publicName, MAX_COMMAND_FUNC_NAME);
		gs_commandsInfo[idx][E_CMD_POINTING_ID] = INVALID_COMMAND_ID;
	}

	new const aliasFuncPrefix[] = "altc_";
	new const initFuncPrefix[] = "cint_";
	
	for (new i = 0; i < gs_totalCommands; i++) {
		new funcName[MAX_COMMAND_FUNC_NAME];
		
		// Initialize command aliases...
		strcat(funcName, aliasFuncPrefix);
		strcat(funcName, gs_commandsInfo[i][E_CMD_NAME]);
		CallLocalFunction(funcName, "");

		// Call command init...
		funcName[0] = EOS;
		strcat(funcName, initFuncPrefix);
		strcat(funcName, gs_commandsInfo[i][E_CMD_NAME]);
		CallLocalFunction(funcName, "i", i);
	}

	MYC_SortCommands();
	CallLocalFunction("OnCommandsInit", "");
}

static MYC_SortCommands()
{
	SortArrayUsingComparator(gs_commandsInfo, MYC_CommandComparator);
}

Comparator:MYC_CommandComparator(left[E_CMD_INFO], right[E_CMD_INFO])
{
	new bool:isLeftValid = (!isnull(left[E_CMD_NAME]));
	new bool:isRightValid = (!isnull(right[E_CMD_NAME]));

	if (!isLeftValid && !isRightValid) {
		return 0;
	} else if (!isLeftValid && isRightValid) {
		return 1;
	} else if (isLeftValid && !isRightValid) {
		return 0;
	}

	new bool:isLeftAlias = (left[E_CMD_POINTING_ID] != INVALID_COMMAND_ID);
	new bool:isRightAlias = (right[E_CMD_POINTING_ID] != INVALID_COMMAND_ID);
	
	if (isLeftAlias && !isRightAlias) {
		return 1;
	}

	for (new i = 0; i < MAX_COMMAND_NAME; i++) {
		new leftChar = left[E_CMD_NAME][i];
		new rightChar = right[E_CMD_NAME][i];

		if (leftChar == '\0' || rightChar == '\0') {
			break;
		}

		if (leftChar > rightChar) {
			return 1;
		} else if (leftChar < rightChar) {
			return 0;
		}
	}

	if (strlen(left[E_CMD_NAME]) > strlen(right[E_CMD_NAME])) {
		return 1;
	}
	
	return 0;
}

public OnPlayerCommandText(playerid, cmdtext[])
{
	if (!MYC_OnPlayerCommandText(playerid, cmdtext)) {
		return 0;
	}

	if (!OnPlayerCommandReceived(playerid, cmdtext)) {
		return 0;
	}

	new cmd[MAX_COMMAND_NAME];
	new params[128];

	sscanf(cmdtext[1], "s["#MAX_COMMAND_NAME"]S()[128]", cmd, params);
	return EmulateCommand(playerid, cmd, params);
}

public OnPlayerCommandReceived(playerid, cmdtext[])
{
	return MYC_OnPlayerCommandReceived(playerid, cmdtext);
}

public OnPlayerCommandPerformed(playerid, cmdtext[], success)
{
	return MYC_OnPlayerCommandPerformed(playerid, cmdtext, success);
}


// -
// Hooks
// -

#if defined _ALS_OnGameModeInit
    #undef OnGameModeInit
#else
    #define _ALS_OnGameModeInit
#endif
#define OnGameModeInit(%0) CHAIN_PUBLIC:MYC_OnGameModeInit(%0)
CHAIN_FORWARD:MYC_OnGameModeInit() = 1;

#if defined _ALS_OnFilterScriptInit
    #undef OnFilterScriptInit
#else
    #define _ALS_OnFilterScriptInit
#endif
#define OnFilterScriptInit(%0) CHAIN_PUBLIC:MYC_OnFilterScriptInit(%0)
CHAIN_FORWARD:MYC_OnFilterScriptInit() = 1;

#if defined _ALS_OnPlayerCommandText
	#undef OnPlayerCommandText
#else
	#define _ALS_OnPlayerCommandText
#endif
#define OnPlayerCommandText(%0) CHAIN_PUBLIC:MYC_OnPlayerCommandText(%0)
CHAIN_FORWARD:MYC_OnPlayerCommandText(playerid, cmdtext[]) = 1;

#if defined _ALS_OnPlayerCommandReceived
	#undef OnPlayerCommandReceived
#else
	#define _ALS_OnPlayerCommandReceived
#endif
#define OnPlayerCommandReceived(%0) CHAIN_PUBLIC:MYC_OnPlayerCommandReceived(%0)
CHAIN_FORWARD:MYC_OnPlayerCommandReceived(playerid, cmdtext[]) = 1;

#if defined _ALS_OnPlayerCommandPerformed
	#undef OnPlayerCommandPerformed
#else
	#define _ALS_OnPlayerCommandPerformed
#endif
#define OnPlayerCommandPerformed(%0) CHAIN_PUBLIC:MYC_OnPlayerCommandPerformed(%0)
CHAIN_FORWARD:MYC_OnPlayerCommandPerformed(playerid, cmdtext[], success) = 0;
