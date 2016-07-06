/*

	About: grand theft online standart library
	Author: Iain Gilbert
	Co-author: ziggi

*/

#if !defined _samp_included
	#error "Please include a_samp or a_npc before gtolib"
#endif

#if !defined _FOREACH_LOCAL_VERSION
	#error "Please include foreach before gtolib"
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

#define MODE_INFO_VERSION 1
#define MODE_INFO_NAME    2

// additional mxINI-like functions
#define ini_fileRemove	fremove
#define ini_fileExist	fexist

//#define IS_IN_RANGE(%0,%1,%2) (((%0) - ((%1) + cellmin)) < (((%2) + 1) - ((%1) + cellmin)))
#define IS_IN_RANGE(%0,%1,%2) ((%0) >= (%1) && (%0) <= (%2))

// useful states for vehicle windows
#define VEHICLE_WINDOW_CLOSED 1
#define VEHICLE_WINDOW_OPENED 0
#define VEHICLE_WINDOW_UNSET  -1

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

	Time functions

*/

#if defined _time_t_included

stock timestamp_to_format_date(unix_timestamp)
{
	new year, month, day, hour, minute, second;
	gmtime(unix_timestamp, year, month, day, hour, minute, second);

	new string[20];
	format(string, sizeof(string), "%04d-%02d-%02d %02d:%02d:%02d", year, month, day, hour, minute, second);
	return string;
}

#endif

stock GetTimeStringFromSeconds(input_seconds, string[], const size = sizeof(string))
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

	if (days != 0) {
		Declension_GetDays(days, str_days);
		Declension_GetHours(hours, str_hours);

		format(string, size, "%d %s, %d %s, ", days, str_days, hours, str_hours);
	} else if (hours != 0) {
		Declension_GetHours(hours, str_hours);

		format(string, size, "%d %s, ", hours, str_hours);
	} else {
		string[0] = '\0';
	}

	Declension_GetMinutes(minutes, str_minutes);
	Declension_GetSeconds(seconds, str_seconds);

	format(string, size, "%s%d %s, %d %s", string, minutes, str_minutes, seconds, str_seconds);
}

stock IsDateExpired(timestamp)
{
	if (getdate() > timestamp) {
		return 1;
	}

	return 0;
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

stock TeleportPlayerToPos(playerid, Float:x, Float:y, Float:z, Float:angle, interior = 0, world = 0, bool:with_vehicle = true)
{
	new vehicleid = GetPlayerVehicleID(playerid);

	if (vehicleid != 0 && with_vehicle) {
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
	} else {
		SetPlayerPos(playerid, x, y, z);
		SetPlayerFacingAngle(playerid, angle);
	}

	SetPlayerInterior(playerid, interior);
	SetPlayerVirtualWorld(playerid, world);
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

	x = x + random(2) - random(4);
	y = y + random(2) - random(4);

	// teleport
	TeleportPlayerToPos(playerid, x, y, z, angle, interior, world, with_vehicle);
}

stock SendMessageToNearPlayers(message[], Float:range, Float:x, Float:y, Float:z)
{
	foreach (new playerid : Player) {
		if (IsPlayerInRangeOfPoint(playerid, range, x, y, z)) {
			SendClientMessage(playerid, -1, message);
		}
	}
}

stock SendMessageToNearVehiclePlayers(message[], Float:range, vehicleid)
{
	new
		Float:x,
		Float:y,
		Float:z;

	GetVehiclePos(vehicleid, x, y, z);

	foreach (new playerid : Player) {
		if (IsPlayerInRangeOfPoint(playerid, range, x, y, z)) {
			SendClientMessage(playerid, -1, message);
		}
	}
}

stock SendMessageToNearPlayerPlayers(message[], Float:range, targetid)
{
	new
		Float:x,
		Float:y,
		Float:z;

	GetPlayerPos(targetid, x, y, z);

	foreach (new playerid : Player) {
		if (IsPlayerInRangeOfPoint(playerid, range, x, y, z)) {
			SendClientMessage(playerid, -1, message);
		}
	}
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

/*

	Coords functions

*/

stock Float:GetDistanceBetweenPoints(Float:x1, Float:y1, Float:z1, Float:x2, Float:y2, Float:z2)
{
	return VectorSize(x1 - x2, y1 - y2, z1 - z2);
}

stock GetCoordsBefore(Float:x, Float:y, Float:a, Float:distance, &Float:res_x, &Float:res_y)
{
	res_x = x + (distance * floatsin(-a, degrees));
	res_y = y + (distance * floatcos(-a, degrees));
}

stock GetCoordsBeforePlayer(playerid, Float:distance, &Float:x, &Float:y)
{
	new
		Float:x,
		Float:y,
		Float:z,
		Float:angle;

	GetPlayerPos(playerid, x, y, z);
	GetPlayerFacingAngle(playerid, angle);

	GetCoordsBefore(x, y, angle, distance, x, y);
}

stock GetVehicleCoordsBeforePlayer(&Float:x, &Float:y, &Float:z, &Float:a, playerid, modelid = -1)
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

	GetCoordsBefore(x, y, a, wide + 0.5, x, y);

	z += 0.1;
	if (wheel_z != 0.0) {
		z -= high / 2 + wheel_z;
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

stock InsertSpacesInString(string[], const size = sizeof(string), const between_pos = 3)
{
	for (new i = 1, length = strlen(string); i < length; i++) {
		if ((length - i) % between_pos == 0) {
			strins(string, " ", i, size);
			i++;
			length++;
		}
	}
}

stock InsertSpacesInInt(value, result[], const size = sizeof(result), const between_pos = 3)
{
	valstr(result, value);
	InsertSpacesInString(result, size, between_pos);
}

stock strcpy(result[], source[], const size = sizeof(result))
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

stock GenerateRandomString(result_str[], length, size = sizeof(result_str), alphabet[] = DEFAULT_ALPHABET, alphabet_size = sizeof(alphabet))
{
	result_str[0] = '\0';

	if (length > size) {
		length = size - 1;
	}

	if (length < 1) {
		return 0;
	}

	for (new i = 0; i < length; i++) {
		result_str[i] = alphabet[ random(alphabet_size) ];
	}

	return 1;
}

stock FormatPosition(position)
{
	static pos_string[] = {"st!", "nd!", "rd!", "th"};
	new string[8];

	valstr(string, position);

	if (IS_IN_RANGE(position, 1, 3)) {
		strcat(string, pos_string[position - 1]);
	} else if (position > 3) {
		strcat(string, pos_string[3]);
	}

	return string;
}

// deprecated
stock strcharsplit(const string[], &index, seperator=' ')
{
	new
		result[20],
		i = 0;

	if (index != 0 && string[index] != '\0') {
		index++;
	}

	while (string[index] && string[index] != seperator && string[index] != '\r' && string[index] != '\n') {
		result[i++] = string[index++];
	}

	return result;
}

#if !defined ALLOWED_NICK_SYMBOLS
	#define ALLOWED_NICK_SYMBOLS 'a'..'z','A'..'Z','0'..'9','_','[',']','.','=','(',')'
#endif

stock NameCharCheck(playername[MAX_PLAYER_NAME + 1])
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

stock IsValidChars(string[])
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

stock IsIpAddress(string[])
{
	new
		octet,
		j = 100;

	for (new i = 0; string[i] != '\0'; i++) {
		if (j != 0 && isdigit(string[i])) {
			octet += (string[i] - '0') * j;
			j /= 10;
		} else if (string[i] == '.') {
			if (IS_IN_RANGE(octet, 0, 255)) {
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

stock mathrandom(min, max)
{
	return (random(max - min + 1) + min);
}

stock pow(value, degree)
{
	new result = 1;
	while (degree--) {
		result *= value;
	}
	return result;
}

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
	}

	time_second = strval(string);

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
	if ('0' <= symbol <= '9') {
		return 1;
	}

	return 0;
}


/*
	insertSort
*/

stock insertSort(array[], const size = sizeof(array))
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
