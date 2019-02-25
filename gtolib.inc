/*

	About: grand theft online standart library
	Author: Iain Gilbert
	Co-author: ziggi

*/

#if !defined _samp_included
	#error "Please include a_samp or a_npc before gtolib"
#endif

#if defined _gtolib_included
	#endinput
#endif

#define _gtolib_included

/*

	Defines

*/

#if !defined MAX_STRING
	#define MAX_STRING 128
#endif

#if !defined MAX_NAME
	#define MAX_NAME 40
#endif

#if !defined MAX_LANG_VALUE_STRING
	#define MAX_LANG_VALUE_STRING 144
#endif

#define MAX_CELLVALUE_SIZE 10 // 2147483647

#define MODE_INFO_VERSION 1
#define MODE_INFO_NAME    2

// additional mxINI-like functions
#define ini_fileRemove	fremove
#define ini_fileExist	fexist

// useful states for vehicle windows
#define VEHICLE_WINDOW_CLOSED 1
#define VEHICLE_WINDOW_OPENED 0
#define VEHICLE_WINDOW_UNSET -1

// foreach check
#if !defined foreach
	#error You need to install foreach: https://github.com/Open-GTO/foreach
#endif

#if !FOREACH_I_PlayerPlayersStream
	#error You need to define FOREACH_I_PlayerPlayersStream before foreach
#endif

#if !FOREACH_I_VehiclePlayersStream
	#error You need to define FOREACH_I_VehiclePlayersStream before foreach
#endif

/*

	Key functions

*/

#define PRESSED(%0) \
	(((newkeys & (%0)) == (%0)) && ((oldkeys & (%0)) != (%0)))

#define HOLDING(%0) \
	((newkeys & (%0)) == (%0))

#define RELEASED(%0) \
	(((newkeys & (%0)) != (%0)) && ((oldkeys & (%0)) == (%0)))

/*

	Forwards

*/

forward Float:frandom(Float:max);
forward Float:mathfrandom(Float:min, Float:max);

/*

	Time functions

*/

stock timestamp_to_format_date(unix_timestamp)
{
	new year, month, day, hour, minute, second;
	gmtime(unix_timestamp, year, month, day, hour, minute, second);

	new string[20];
	format(string, sizeof(string), "%04d-%02d-%02d %02d:%02d:%02d", year, month, day, hour, minute, second);
	return string;
}

stock GetTimeStringFromSeconds(playerid, input_seconds, string[], const size = sizeof(string))
{
	new
		str_days[MAX_LANG_VALUE_STRING],
		str_hours[MAX_LANG_VALUE_STRING],
		str_minutes[MAX_LANG_VALUE_STRING],
		str_seconds[MAX_LANG_VALUE_STRING],

		days = input_seconds / 60 / 60 / 24,
		hours = (input_seconds / 60 / 60) % 24,
		minutes = (input_seconds / 60) % 60,
		seconds = input_seconds % 60;

	string[0] = '\0';

	if (days != 0) {
		Declension_GetDays(playerid, days, str_days);
		format(string, size, "%s%d %s, ", string, days, str_days);
	}

	if (hours != 0 || days != 0) {
		Declension_GetHours(playerid, hours, str_hours);
		format(string, size, "%s%d %s, ", string, hours, str_hours);
	}

	if (minutes != 0 || hours != 0 || days != 0) {
		Declension_GetMinutes(playerid, minutes, str_minutes);
		format(string, size, "%s%d %s, ", string, minutes, str_minutes);
	}

	Declension_GetSeconds(playerid, seconds, str_seconds);
	format(string, size, "%s%d %s", string, seconds, str_seconds);
}

stock IsDateExpired(timestamp)
{
	if (gettime() > timestamp) {
		return 1;
	}

	return 0;
}

stock GetTickDiff(newtick, oldtick)
{
	if (oldtick < 0 && newtick >= 0) {
		return newtick - oldtick;
	} else if (oldtick >= 0 && newtick < 0 || oldtick > newtick) {
		return (cellmax - oldtick + 1) - (cellmin - newtick);
	}
	return newtick - oldtick;
}

/*

	Player functions

*/

stock PlayerPlaySoundOnPlayer(playerid, soundid)
{
	return PlayerPlaySound(playerid, soundid, 0.0, 0.0, 0.0);
}

stock PlaySoundForPlayersInRange(soundid, Float:range, Float:x, Float:y, Float:z)
{
	foreach (new i : Player) {
		if (IsPlayerInRangeOfPoint(i, range, x, y, z)) {
			PlayerPlaySound(i, soundid, x, y, z);
		}
	}
}

stock GetPlayersCount()
{
	return Iter_Count(Player);
}

stock SetPlayerPosEx(playerid, Float:x, Float:y, Float:z, Float:angle, interior = 0, world = 0)
{
	SetPlayerInterior(playerid, interior);
	SetPlayerFacingAngle(playerid, angle);
	SetPlayerVirtualWorld(playerid, world);
	SetPlayerPos(playerid, x, y, z);
	SetCameraBehindPlayer(playerid);
}

stock GetPlayerPosEx(playerid, &Float:x, &Float:y, &Float:z, &Float:angle, &interior = 0, &world = 0)
{
	GetPlayerPos(playerid, x, y, z);
	GetPlayerFacingAngle(playerid, angle);
	interior = GetPlayerInterior(playerid);
	world = GetPlayerVirtualWorld(playerid);
}

stock TeleportVehicleToPos(vehicleid, Float:x, Float:y, Float:z, Float:angle, interior = 0, world = 0, bool:with_players = true)
{
	if (!IsValidVehicle(vehicleid)) {
		return 0;
	}

	SetVehiclePos(vehicleid, x, y, z);
	SetVehicleZAngle(vehicleid, angle);

	LinkVehicleToInterior(vehicleid, interior);
	SetVehicleVirtualWorld(vehicleid, world);

	new trailerid = GetVehicleTrailer(vehicleid);
	if (trailerid != 0) {
		SetVehiclePos(trailerid, x, y, z);
		SetVehicleZAngle(trailerid, angle);

		LinkVehicleToInterior(trailerid, interior);
		SetVehicleVirtualWorld(trailerid, world);

		AttachTrailerToVehicle(trailerid, vehicleid);
	}

	if (with_players) {
		foreach (new playerid : Player) {
			if (!IsPlayerInVehicle(playerid, vehicleid)) {
				continue;
			}

			SetPlayerVirtualWorld(playerid, world);
			SetPlayerInterior(playerid, interior);
		}
	}

	return 1;
}

stock TeleportPlayerToPos(playerid, Float:x, Float:y, Float:z, Float:angle, interior = 0, world = 0, bool:with_vehicle = true)
{
	new vehicleid = GetPlayerVehicleID(playerid);
	if (vehicleid != 0 && with_vehicle) {
		TeleportVehicleToPos(vehicleid, x, y, z, angle, interior, world, .with_players = true);
		return 1;
	}

	SetPlayerPos(playerid, x, y, z);
	SetPlayerFacingAngle(playerid, angle);

	SetPlayerInterior(playerid, interior);
	SetPlayerVirtualWorld(playerid, world);
	return 1;
}

stock TeleportPlayerToPlayer(playerid, to_playerid, bool:with_vehicle = true)
{
	new
		Float:x,
		Float:y,
		Float:z,
		Float:angle,
		interior,
		world;

	// get info
	GetPlayerPosEx(to_playerid, x, y, z, angle, interior, world);

	x += mathfrandom(-2.0, 2.0);
	y += mathfrandom(-2.0, 2.0);

	// teleport
	TeleportPlayerToPos(playerid, x, y, z, angle, interior, world, with_vehicle);
}

stock SendMessageToNearPlayers(const message[], Float:range, Float:x, Float:y, Float:z)
{
	foreach (new playerid : Player) {
		if (IsPlayerInRangeOfPoint(playerid, range, x, y, z)) {
			SendClientMessage(playerid, -1, message);
		}
	}
}

stock SendMessageToNearVehiclePlayers(const message[], Float:range, vehicleid)
{
	new
		Float:x,
		Float:y,
		Float:z;

	GetVehiclePos(vehicleid, x, y, z);

	foreach (new playerid : VehiclePlayersStream[vehicleid]) {
		if (IsPlayerInRangeOfPoint(playerid, range, x, y, z)) {
			SendClientMessage(playerid, -1, message);
		}
	}
}

stock SendMessageToNearPlayerPlayers(const message[], Float:range, targetid)
{
	new
		Float:x,
		Float:y,
		Float:z;

	GetPlayerPos(targetid, x, y, z);

	foreach (new playerid : PlayerPlayersStream[targetid]) {
		if (IsPlayerInRangeOfPoint(playerid, range, x, y, z)) {
			SendClientMessage(playerid, -1, message);
		}
	}
}

stock GetNearPlayers(Float:x, Float:y, Float:z, Float:range, players[], const size = sizeof(players), const exclude_playerid = INVALID_PLAYER_ID)
{
	new
		id,
		i;

	foreach (id : Player) {
		if (id != exclude_playerid && IsPlayerInRangeOfPoint(id, range, x, y, z)) {
			players[i] = id;

			if (i >= size - 1) {
				break;
			} else {
				i++;
			}
		}
	}

	players[i] = INVALID_PLAYER_ID;
	return i;
}

stock GetPlayerNearPlayers(playerid, Float:range, players[], const size = sizeof(players), const exclude_playerid = INVALID_PLAYER_ID)
{
	new
		Float:pos_x,
		Float:pos_y,
		Float:pos_z;

	GetPlayerPos(playerid, pos_x, pos_y, pos_z);
	GetNearPlayers(pos_x, pos_y, pos_z, range, players, size, exclude_playerid);
	return 0;
}

stock GetVehicleNearPlayers(vehicleid, Float:range, players[], const size = sizeof(players), const exclude_playerid = INVALID_PLAYER_ID)
{
	new
		Float:pos_x,
		Float:pos_y,
		Float:pos_z;

	GetVehiclePos(vehicleid, pos_x, pos_y, pos_z);
	GetNearPlayers(pos_x, pos_y, pos_z, range, players, size, exclude_playerid);
	return 0;
}

stock GetPlayersInVehicle(vehicleid, players[], const size = sizeof(players))
{
	new
		id,
		i;

	foreach (id : VehiclePlayersStream[vehicleid]) {
		if (vehicleid == GetPlayerVehicleID(id)) {
			players[i] = id;

			if (i >= size - 1) {
				break;
			} else {
				i++;
			}
		}
	}

	players[i] = INVALID_PLAYER_ID;
	return i;
}

stock GetPlayersAndSeatsInVehicle(vehicleid, player_ids[], player_seats[], const size = sizeof(player_ids))
{
	new
		playerid,
		i;

	foreach (playerid : VehiclePlayersStream[vehicleid]) {
		if (vehicleid == GetPlayerVehicleID(playerid)) {
			player_ids[i] = playerid;
			player_seats[i] = GetPlayerVehicleSeat(playerid);

			if (i >= size - 1) {
				break;
			} else {
				i++;
			}
		}
	}

	player_ids[i] = INVALID_PLAYER_ID;
	player_seats[i] = -1;
	return i;
}

stock PlayAudioStreamForPlayers(const players[], const url[], Float:posX = 0.0, Float:posY = 0.0, Float:posZ = 0.0, Float:distance = 50.0, usepos = 0)
{
	for (new idx = 0; players[idx] != INVALID_PLAYER_ID; idx++) {
		PlayAudioStreamForPlayer(players[idx], url, posX, posY, posZ, distance, usepos);
	}
	return 1;
}

stock StopAudioStreamForPlayers(const players[])
{
	for (new idx = 0; players[idx] != INVALID_PLAYER_ID; idx++) {
		StopAudioStreamForPlayer(players[idx]);
	}
	return 1;
}

/*

	Vehicle functions

*/

stock UpdateVehicleNumberPlate(vehicleid, const numberplate[])
{
	const MAX_PLAYERS_IN_VEHICLE = 8;
	const MAX_COMPONENTS_IN_VEHICLE = 13;

	new
		Float:pos_x, Float:pos_y, Float:pos_z, Float:z_angle,
		Float:health,
		panels, doors, lights, tires,
		player_ids[MAX_PLAYERS_IN_VEHICLE],
		player_seats[MAX_PLAYERS_IN_VEHICLE],
		components[MAX_COMPONENTS_IN_VEHICLE];

	// get data
	if (!GetVehiclePos(vehicleid, pos_x, pos_y, pos_z)) {
		return 0;
	}

	GetVehicleZAngle(vehicleid, z_angle);
	GetVehicleHealth(vehicleid, health);
	GetVehicleDamageStatus(vehicleid, panels, doors, lights, tires);
	GetPlayersAndSeatsInVehicle(vehicleid, player_ids, player_seats);

	for (new i = 0; i < sizeof(components); i++) {
		components[i] = GetVehicleComponentInSlot(vehicleid, i);
	}

#if defined GetVehicleColor && defined GetVehiclePaintjob
	new color1, color2;
	GetVehicleColor(vehicleid, color1, color2);

	new paintjob = GetVehiclePaintjob(vehicleid);
#endif

	// update number
	SetVehicleNumberPlate(vehicleid, numberplate);

	SetVehicleToRespawn(vehicleid);

	// set previous data
	SetVehiclePos(vehicleid, pos_x, pos_y, pos_z);
	SetVehicleZAngle(vehicleid, z_angle);
	SetVehicleHealth(vehicleid, health);
	UpdateVehicleDamageStatus(vehicleid, panels, doors, lights, tires);

	for (new i = 0; i < sizeof(components); i++) {
		if (1000 <= components[i] <= 1193) {
			AddVehicleComponent(vehicleid, components[i]);
		}
	}

#if defined GetVehicleColor && defined GetVehiclePaintjob
	ChangeVehicleColor(vehicleid, color1, color2);

	if (paintjob != INVALID_VEHICLE_PAINTJOB_ID) {
		ChangeVehiclePaintjob(vehicleid, paintjob);
	}
#endif

	for (new i = 0; i < MAX_PLAYERS_IN_VEHICLE; i++) {
		if (player_ids[i] == INVALID_PLAYER_ID) {
			continue;
		}

		PutPlayerInVehicle(player_ids[i], vehicleid, player_seats[i]);
	}

	return 1;
}

/*

	Chat functions

*/

stock Chat_Clear(playerid)
{
	for (new i = 0; i < 20; i++) {
		SendClientMessage(playerid, -1, "");
	}
}

stock Chat_ClearAll()
{
	for (new i = 0; i < 20; i++) {
		SendClientMessageToAll(-1, "");
	}
}

stock ProxDetector(playerid, Float:max_range, color, const string[], Float:max_ratio = 1.6)
{
	new
		Float:pos_x,
		Float:pos_y,
		Float:pos_z,
		Float:range,
		Float:range_ratio,
		Float:range_with_ratio,
		clr_r, clr_g, clr_b,
		Float:color_r, Float:color_g, Float:color_b;

	if (!GetPlayerPos(playerid, pos_x, pos_y, pos_z)) {
		return 0;
	}

	color_r = float(color >> 24 & 0xFF);
	color_g = float(color >> 16 & 0xFF);
	color_b = float(color >> 8 & 0xFF);
	range_with_ratio = max_range * max_ratio;

	foreach (new i : PlayerPlayersStream[playerid]) {
		range = GetPlayerDistanceFromPoint(i, pos_x, pos_y, pos_z);
		if (range > max_range) {
			continue;
		}

		range_ratio = (range_with_ratio - range) / range_with_ratio;
		clr_r = floatround(range_ratio * color_r);
		clr_g = floatround(range_ratio * color_g);
		clr_b = floatround(range_ratio * color_b);

		SendClientMessage(i, (color & 0xFF) | (clr_b << 8) | (clr_g << 16) | (clr_r << 24), string);
	}

	SendClientMessage(playerid, color, string);
	return 1;
}

/*

	Coords functions

*/

stock Float:GetDistanceBetweenPoints(Float:x1, Float:y1, Float:z1, Float:x2, Float:y2, Float:z2)
{
	return VectorSize(x1 - x2, y1 - y2, z1 - z2);
}

stock GetCoordsInFront(Float:x, Float:y, Float:a, Float:distance, &Float:res_x, &Float:res_y)
{
	res_x = x + (distance * floatsin(-a, degrees));
	res_y = y + (distance * floatcos(-a, degrees));
}

stock GetCoordsInFrontOfPlayer(playerid, Float:distance, &Float:x, &Float:y)
{
	new
		Float:x,
		Float:y,
		Float:z,
		Float:angle;

	GetPlayerPos(playerid, x, y, z);
	GetPlayerFacingAngle(playerid, angle);

	GetCoordsInFront(x, y, angle, distance, x, y);
}

stock GetVehicleCoordsInFrontOfPlayer(&Float:x, &Float:y, &Float:z, &Float:a, playerid, modelid = -1)
{
	new
		Float:wide, Float:long, Float:high,
		Float:wheel_x, Float:wheel_y, Float:wheel_z;

	if (modelid != -1) {
		GetVehicleModelInfo(modelid, VEHICLE_MODEL_INFO_SIZE, wide, long, high);
		GetVehicleModelInfo(modelid, VEHICLE_MODEL_INFO_WHEELSFRONT, wheel_x, wheel_y, wheel_z);
	} else {
		wide = 1.5;
		high = 1.0;
	}

	GetPlayerPos(playerid, x, y, z);
	GetPlayerFacingAngle(playerid, a);

	GetCoordsInFront(x, y, a, wide + 0.5, x, y);

	z += 0.1;
	if (wheel_z != 0.0) {
		z -= high / 2.0 + wheel_z;
	}

	a += 90.0;
	if (a > 360.0) {
		a -= 360.0;
	}
}

/*

	String functions

*/

stock StripNL(str[]) // by Y_Less
{
	new i = strlen(str);
	while (i-- && str[i] <= ' ') {
		str[i] = '\0';
	}
}

stock FixAscii(text[])
{
	for (new i = 0; text[i] != '\0'; i++) {
		text[i] &= 0xFF;
	}
}

stock FormatNumberToString(string[], const size, { _, Float, Text3D, Menu, Text, DB, DBResult, bool, File, hex, bit, bit_byte, Bit }:xVariable, iDecimals = -1, iThousandSeparator = DEFAULT_THOUSAND_SEP, iDecimalPoint = DEFAULT_DECIMAL_POINT, iTag = tagof( xVariable ) )
{
	string[0] = '\0';
	return strcat(string, FormatNumber(xVariable, iDecimals, iThousandSeparator, iDecimalPoint, iTag), size);
}

stock strcpy(result[], const source[], const size = sizeof(result))
{
	result[0] = 0;
	return strcat(result, source, size);
}

/*

	GenerateRandomString(result_str[], length, size = sizeof(result_str), alphabet[] = DEFAULT_ALPHABET, alphabet_size = sizeof(alphabet))

*/

#if !defined DEFAULT_ALPHABET
	#define DEFAULT_ALPHABET "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
#endif

stock GenerateRandomString(result_str[], length, const size = sizeof(result_str), const alphabet[] = DEFAULT_ALPHABET, const alphabet_size = sizeof(alphabet))
{
	result_str[0] = '\0';

	if (length >= size) {
		length = size - 1;
	}

	if (length < 1) {
		return 0;
	}

	for (new i = 0; i < length; i++) {
		result_str[i] = alphabet[ random(alphabet_size - 1) ];
	}

	return 1;
}

stock FormatPosition(position)
{
	static pos_string[] = {"st!", "nd!", "rd!", "th"};

	new
		pos_index,
		string[8];

	if (position < 1) {
		return string;
	}

	pos_index = position - 1;

	if (position > 3) {
		pos_index = sizeof(pos_string) - 1;
	}

	format(string, sizeof(string), "%d%s", position, pos_string[pos_index]);
	return string;
}

// deprecated
#if !defined ALLOWED_NICK_SYMBOLS
	#define ALLOWED_NICK_SYMBOLS 'a'..'z','A'..'Z','0'..'9','_','[',']','.','=','(',')'
#endif

stock NameCharCheck(const playername[MAX_PLAYER_NAME + 1])
{
	if (!strcmp(playername, "Unknown", true) || IsNumeric(playername)) {
		return 0;
	}

	for (new i = 0; playername[i] != '\0'; i++) {
		switch (playername[i]) {
			case ALLOWED_NICK_SYMBOLS: {
				continue;
			}
			default: {
				return 0;
			}
		}
	}

	if (IsIpAddress(playername)) {
		return 0;
	}

	return 1;
}

#if !defined ALLOWED_STRING_SYMBOLS
	#define ALLOWED_STRING_SYMBOLS 'a'..'z','A'..'Z','0'..'9','_','[',']','=','(',')',' '
#endif

stock IsValidChars(const string[])
{
	for (new i = 0; string[i] != '\0'; i++) {
		switch (string[i]) {
			case ALLOWED_STRING_SYMBOLS: {
				continue;
			}
			default: {
				return 0;
			}
		}
	}
	return 1;
}

stock IsNumeric(const string[])
{
	for (new i = 0; string[i] != '\0'; i++) {
		if (!isdigit(string[i])) {
			return 0;
		}
	}
	return 1;
}

stock IsIpAddress(const string[])
{
	new
		octet,
		j = 100;

	for (new i = 0; string[i] != '\0'; i++) {
		if (j != 0 && isdigit(string[i])) {
			octet += (string[i] - '0') * j;
			j /= 10;
		} else if (string[i] == '.') {
			if (0 <= octet <= 255) {
				j = 100;
				octet = 0;
			} else {
				return 0;
			}
		} else {
			return 0;
		}
	}
	return 1;
}

/*

	Math functions

*/

stock Float:frandom(Float:max)
{
	return floatdiv(float(random(0)), floatdiv(float(cellmax), max));
}

stock Float:mathfrandom(Float:min, Float:max)
{
	return floatadd(frandom(floatsub(max, min)), min);
}

stock mathrandom(min, max)
{
	return (random(max - min) + min);
}

stock pow(value, degree)
{
	new result = 1;
	while (degree--) {
		result *= value;
	}
	return result;
}

stock getmax(...)
{
	new value = getarg(0);
	for (new i = numargs() - 1; i != 0 ; i--) {
		value = max(getarg(i), value);
	}
	return value;
}

stock getmin(...)
{
	new value = getarg(0);
	for (new i = numargs() - 1; i != 0 ; i--) {
		value = min(getarg(i), value);
	}
	return value;
}

/*
	Float mod operators
*/

stock Float:operator%(Float:oper1, Float:oper2)
{
	new Float:sum2 = oper2;
	while (floatcmp(sum2, oper1) < 0) {
		sum2 = floatadd(sum2, oper2);
	}
	if (_:sum2 == _:oper1) {
		return 0.0;
	}
	return floatsub(oper1, floatsub(sum2, oper2));
}

stock Float:operator%(Float:oper1, oper2)
{
	new
		Float:oper2f = float(oper2),
		Float:sum2 = oper2f;
	while (floatcmp(sum2, oper1) < 0) {
		sum2 = floatadd(sum2, oper2f);
	}
	if (_:sum2 == _:oper1) {
		return 0.0;
	}
	return floatsub(oper1, floatsub(sum2, oper2f));
}

stock Float:operator%(oper1, Float:oper2)
{
	new
		Float:oper1f = float(oper1),
		Float:sum2 = oper2;
	while (floatcmp(sum2, oper1f) < 0) {
		sum2 = floatadd(sum2, oper2);
	}
	if (_:sum2 == _:oper1f) {
		return 0.0;
	}
	return floatsub(oper1f, floatsub(sum2, oper2));
}

/*
	Vehicle speed
*/

stock Float:GetVehicleSpeed(vehicleid)
{
	new
		Float:x,
		Float:y,
		Float:z;

	GetVehicleVelocity(vehicleid, x, y, z);

	return VectorSize(x, y, z) * 180.0;
}

/*
	SSCANF:weather
*/

SSCANF:weather(string[])
{
	if (!IsNumeric(string)) {
		return -1;
	}

	new
		weather;

	weather = strval(string);

	if (!(0 < weather < 255)) {
		return -1;
	}

	return weather;
}

/*
	SSCANF:ftime
*/

SSCANF:ftime(string[])
{
	new
		length,
		time_second,
		time_type;

	length = strlen(string);

	if (length < 1) {
		return -1;
	}

	if (!isdigit(string[length - 1])) {
		time_type = string[length - 1];

		strdel(string, length - 1, length);
	} else {
		return -1;
	}

	time_second = strval(string);
	if (time_second < 0) {
		return -1;
	}

	switch (time_type) {
		case 's', 'ñ': {

		}
		case 'm', 'ì': {
			time_second *= 60;
		}
		case 'h', '÷': {
			time_second *= 60 * 60;
		}
		case 'd', 'ä': {
			time_second *= 60 * 60 * 24;
		}
		case 'w', 'í': {
			time_second *= 60 * 60 * 24 * 7;
		}
		case 'y', 'ã': {
			time_second *= 60 * 60 * 24 * 365;
		}
	}

	return time_second;
}

stock isdigit(symbol)
{
	return '0' <= symbol <= '9';
}


/*
	insert_sort
*/

stock insert_sort(array[], const size = sizeof(array))
{
	new
		i,
		j,
		key;

	for (i = 1; i < size; i++) {
		key = array[i];

		for (j = i - 1; j >= 0 && array[j] > key; j--) {
			array[j + 1] = array[j];
		}

		array[j + 1] = key;
	}
}

/*

	For UpdateVehicleDamageStatus

*/

stock decode_panels(panels,&flp,&frp,&rlp,&rrp,&windshield,&front_bumper,&rear_bumper)
{
	flp = panels >> 0 & 0xF;
	frp = panels >> 4 & 0xF;
	rlp = panels >> 8 & 0xF;
	rrp = panels >> 12 & 0xF;
	windshield = panels >> 16 & 0xF;
	front_bumper = panels >> 20 & 0xF;
	rear_bumper = panels >> 24 & 0xF;
}

stock decode_doors(doors,&driver_door,&passenger_door,&bonnet,&boot)
{
	bonnet = doors & 0x7;
	boot = doors >> 8 & 0x7;
	driver_door = doors >> 16 & 0x7;
	passenger_door = doors >> 24 & 0x7;
}

stock decode_lights(lights,&light1,&light2,&light3,&light4)
{
	light1 = lights >> 0 & 0x1;
	light2 = lights >> 1 & 0x1;
	light3 = lights >> 2 & 0x1;
	light4 = lights >> 3 & 0x1;
}

stock decode_tires(tires,&tyre1,&tyre2,&tyre3,&tyre4)
{
	tyre1 = tires >> 0 & 0x1;
	tyre2 = tires >> 1 & 0x1;
	tyre3 = tires >> 2 & 0x1;
	tyre4 = tires >> 3 & 0x1;
}

stock encode_panels(flp, frp, rlp, rrp, windshield, front_bumper, rear_bumper)
{
	return flp | (frp << 4) | (rlp << 8) | (rrp << 12) | (windshield << 16) | (front_bumper << 20) | (rear_bumper << 24);
}

stock encode_doors(bonnet, boot, driver_door, passenger_door, behind_driver_door, behind_passenger_door)
{
	#pragma unused behind_driver_door
	#pragma unused behind_passenger_door
	return bonnet | (boot << 8) | (driver_door << 16) | (passenger_door << 24);
}

stock encode_lights(light1, light2, light3, light4)
{
	return light1 | (light2 << 1) | (light3 << 2) | (light4 << 3);
}

stock encode_tires(tire1, tire2, tire3, tire4)
{
	return tire1 | (tire2 << 1) | (tire3 << 2) | (tire4 << 3);
}

stock encode_tires_bike(rear, front)
{
	return rear | (front << 1);
}
