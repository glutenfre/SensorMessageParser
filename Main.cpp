#include <iostream>
#include <string>
#include <sstream>

int GetBites(int num, int length) {
	// This function peeks certain amount bites from byte, but im not using it, since all the data comes in undivided bytes
	int m = 1;
	for (int i = 0; i < length; i++) {
		m *= 2;
	}
	m--;
	return (num & m);
}

std::string GetByteString(std::istringstream* s_stream) {
	char c;
	std::string res;
	int i = 0;
	while ((i < 2) && (s_stream->get(c).good())) {
		if ((c != ' ') && (c != '\n')) {
			res += c;
			i++;
		}
	}
	if (s_stream->get(c).good())
		if ((c != ' ') && (c != '\n'))
			s_stream->unget();
	return std::move(res);
}

unsigned long GetFourBytes(std::istringstream* s_stream) {
	unsigned long res;
	std::string bytes_s_1 = GetByteString(s_stream);
	bytes_s_1 += GetByteString(s_stream);
	std::string bytes_s_2 = GetByteString(s_stream);
	bytes_s_2 += GetByteString(s_stream);
	unsigned long tmp_l = std::stol(bytes_s_1, 0, 16);
	res = (tmp_l * 0x10000) + std::stol(bytes_s_2, 0, 16);
	return res;
}

int GetTwoBytes(std::istringstream* s_stream) {
	std::string bytes_s = GetByteString(s_stream);
	bytes_s += GetByteString(s_stream);
	return std::stoi(bytes_s, 0, 16);
}

int GetByte(std::istringstream* s_stream) {
	return std::stoi(GetByteString(s_stream), 0, 16);
}

bool SkipFrameHeader(std::istringstream* s_stream) {
	char c;
	int i = 0;
	while ((i < 8) && (s_stream->get(c).good())) {
		if ((c != ' ') && (c != '\n'))
			i++;
	}
	return true;
}

bool SkipBasicInfo(std::istringstream* s_stream, int* targets_num) {
	char c;
	int i = 0;
	std::string tmp_s;
	while ((i < 62) && (s_stream->get(c).good())) {
		if ((c != ' ') && (c != '\n'))
			i++;
		if (i == 33)
			tmp_s += c;
		if (i == 34)
			tmp_s += c;
	}
	*targets_num = std::stoi(tmp_s, 0, 16);
	return true;
}

bool SkipModuleData(std::istringstream* s_stream, int* targets_num) {
	char c;
	int i = 0;
	std::string tmp_s;
	while ((i < 420) && (s_stream->get(c).good())) {
		if ((c != ' ') && (c != '\n'))
			i++;
		if (i == 37)
			tmp_s += c;
		if (i == 38)
			tmp_s += c;
	}
	*targets_num = std::stoi(tmp_s, 0, 16);
	return true;
}

bool SkipRoadData(std::istringstream* s_stream, int* targets_num) {
	char c;
	int i = 0;
	std::string tmp_s;
	while ((i < 465) && (s_stream->get(c).good())) {
		if ((c != ' ') && (c != '\n'))
			i++;
		if (i == 37)
			tmp_s += c;
		if (i == 38)
			tmp_s += c;
	}
	*targets_num = std::stoi(tmp_s, 0, 16);
	return true;
}

// Main functions

bool ParseDataFrameHeader(const std::string char_buffer) {
	std::string reply;
	std::istringstream s_stream(char_buffer);
	if ((GetByteString(&s_stream) == "AB") && (GetByteString(&s_stream) == "CD")) {
		reply = "Data Frame Header\n";
		// Here im peeking two bytes instead of 6 bites, becuse the total size is the 251 bytes, not a 59 bytes
		int total_data_length = GetTwoBytes(&s_stream);
		reply += "Total data length: " + std::to_string(total_data_length) + " byte(s) \n";
		std::cout << reply;
		return true;
	}
	return false;
}

bool ParseBasicInformation(const std::string char_buffer) {
	std::string reply;
	std::istringstream s_stream(char_buffer);
	if (SkipFrameHeader(&s_stream)) {
		if ((GetByteString(&s_stream) == "4A") && (GetByteString(&s_stream) == "42")) {
			reply += "Basic Information\n";

			int module_data_length = GetTwoBytes(&s_stream);
			reply += "Module data length: " + std::to_string(module_data_length) + " byte(s)\n";

			int data_status = GetByte(&s_stream);
			reply += "Data status: ";
			if (data_status == 1)
				reply += "this frame data can be used.\n";
			 else if (data_status == 0)
				reply += "this frame data is not available.\n";
			 else 
				reply += "unknown status.\n";

			int time_year = GetByte(&s_stream);
			int time_month = GetByte(&s_stream);
			int time_date = GetByte(&s_stream);
			int time_hour = GetByte(&s_stream);
			int time_minute = GetByte(&s_stream);
			int time_second = GetByte(&s_stream);
			int time_milisecond = GetByte(&s_stream);
			reply += "Time: 200" +
				std::to_string(time_year) + ":" +
				std::to_string(time_month) + ":" +
				std::to_string(time_date) + " " +
				std::to_string(time_hour) + ":" +
				std::to_string(time_minute) + ":" +
				std::to_string(time_second) + "`" +
				std::to_string(time_milisecond) + "\n";

			int some_info = GetByte(&s_stream);
			reply += "Total number of targets: " + std::to_string(some_info) + "\n";

			some_info = GetByte(&s_stream);
			reply += "Information for each target: " + std::to_string(some_info) + " byte(s)\n";

			some_info = GetByte(&s_stream);
			reply += "Total number of lanes: " + std::to_string(some_info) + "\n";

			some_info = GetByte(&s_stream);
			reply += "Information for each lane: " + std::to_string(some_info) + " byte(s)\n";

			some_info = GetByte(&s_stream);
			reply += "Flow information output: ";
			if (some_info == 1)
				reply += "this frame has traffic.\n";
			else if (some_info == 0)
				reply += "no traffic in this frame.\n";
			else
				reply += " unknown status.\n";

			some_info = GetByte(&s_stream);
			reply += "Traffic per lane: " + std::to_string(some_info) + " byte(s)\n";

			// Here im peeking four bites, cause if not - a few bytes left unattended and the data became corrupted
			some_info = GetFourBytes(&s_stream);
			reply += "Traffic data reporting interval : " + std::to_string(some_info) + " seconds \n";

			some_info = GetByte(&s_stream);
			reply += "General Traffic conditions: ";
			if (some_info == 0)
				reply += "unblocked.\n";
			else if (some_info == 1)
				reply += "come to the congestion.\n";
			else if (some_info == 2) 
				reply += "going to the congestion.\n";
			else if (some_info == 3)
				reply += "two-way congestion.\n";
			else 
				reply += " unknown status.\n";

			some_info = GetByte(&s_stream);
			reply += "Event alarm: ";
			if (some_info == 0)
				reply += "no alarm in this frame.\n";
			else if (some_info == 1)
				reply += "this frame has an alarm.\n";
			else
				reply += " unknown status.\n";

			some_info = GetByte(&s_stream);
			reply += "Protocol version: " + std::to_string(some_info) + "\n";

			some_info = GetTwoBytes(&s_stream);
			reply += "Module verification: " + std::to_string(some_info) + "\n";

			std::cout << reply;
			return true;
		}
	}
	return false;
}

bool ParseModuleData(const std::string char_buffer) {
	std::string reply;
	std::istringstream s_stream(char_buffer);
	int targets_num;
	if (SkipBasicInfo(&s_stream, &targets_num)) {
		if ((GetByteString(&s_stream) == "4D") && (GetByteString(&s_stream) == "42")) {
			reply += "Data Module\n";
			int module_data_length = GetTwoBytes(&s_stream);
			reply += "Module data length: " + std::to_string(module_data_length) + " byte(s)\n";
			for (int i = 0; i < targets_num; i++) {
				int first_goal = GetByte(&s_stream);
				reply += "The " + std::to_string(i+1) + " goal: " + std::to_string(first_goal) + '\n';

				int some_info = GetTwoBytes(&s_stream);
				reply += "Vertical distance: " + std::to_string((some_info * 0.1)) + " m.\n";

				some_info = GetTwoBytes(&s_stream);
				reply += "Lateral distance: " + std::to_string((some_info * 0.1)) + " m.\n";

				some_info = GetTwoBytes(&s_stream);
				reply += "Speed (y direction): " + std::to_string((some_info * 0.1)) + " m.\n";

				some_info = GetByte(&s_stream);
				reply += "Target type: ";
				if (some_info == 0)
					reply += "small car.\n";
				else if (some_info == 1)
					reply += "pedestrian.\n";
				else if (some_info == 2)
					reply += "non-motor vehicle.\n";
				else if (some_info == 3)
					reply += "medium-sized car.\n";
				else if (some_info == 4)
					reply += "large cars.\n";
				else
					reply += "unknown status " + std::to_string(some_info) + ".\n";

				some_info = GetByte(&s_stream);
				reply += "Lane number: " + std::to_string(some_info) + '\n';

				some_info = GetTwoBytes(&s_stream);
				reply += "Front spacing: " + std::to_string((some_info * 0.1)) + " m.\n";

				some_info = GetTwoBytes(&s_stream);
				reply += "Front time interval: " + std::to_string((some_info * 0.1)) + " m.\n";

				some_info = GetTwoBytes(&s_stream);
				reply += "Speed (x direction): " + std::to_string((some_info * 0.1)) + " m.\n";

				some_info = GetTwoBytes(&s_stream);
				reply += "Heading angle: " + std::to_string(some_info) + "\n";

				some_info = GetByte(&s_stream);
				reply += "Incidents: ";
				if ((some_info & 1) == 1)
					reply += "unblocked parking; ";
				if ((some_info & 2) == 2)
					reply += "congested parking; ";
				if ((some_info & 4) == 4)
					reply += "vehicle overspeed; ";
				if ((some_info & 8) == 8)
					reply += "harbour parking; ";
				if ((some_info & 16) == 16)
					reply += "slow moving vehicles; ";
				if ((some_info & 32) == 32)
					reply += "pedestrian; ";
				if ((some_info & 64) == 64)
					reply += "retrograde vehicle; ";
				if ((some_info & 128) == 128)
					reply += "change of direction;";
				if (reply[reply.size() - 2] == ':')
					reply += "no event occurred.";

				unsigned long some_long_info = GetFourBytes(&s_stream);
				reply += "\nRadar network x-sit standard: " + std::to_string((some_long_info * 0.1)) + " m.\n";

				some_long_info = GetFourBytes(&s_stream);
				reply += "Radar network y-sit standard: " + std::to_string((some_long_info * 0.1)) + " m.\n";

				some_info = GetByte(&s_stream);
				reply += "Fill the blind mark: ";
				if (some_info == 0)
					reply += "main radar report.\n";
				else if (some_info == 1)
					reply += "blind radar report.\n";
				else
					reply += "unknown status " + std::to_string(some_info) + ".\n";

				some_info = GetByte(&s_stream);
				reply += "Car length: " + std::to_string((some_info * 0.1)) + " m.\n";

				some_info = GetByte(&s_stream);
				reply += "Car width: " + std::to_string((some_info * 0.1)) + " m.\n";
			}
			// Changed verification length to 8 bits, since thats the only amount left
			int ver = GetByte(&s_stream);
			reply += "Module verification: " + std::to_string(ver) + "\n";

			std::cout << reply;
			return true;
		}
	}
	return false;
}

bool ParseLaneData(const std::string char_buffer) {
	std::string reply;
	std::istringstream s_stream(char_buffer);
	int targets_num;
	if (SkipModuleData(&s_stream, &targets_num)) {
		if ((GetByteString(&s_stream) == "44") && (GetByteString(&s_stream) == "4C")) {
			reply += "Lane Data Module\n";
			int module_data_length = GetTwoBytes(&s_stream);
			reply += "Module data length: " + std::to_string(module_data_length) + " byte(s)\n";
			for (int i = 0; i < targets_num; i++) {
				int some_info = GetByte(&s_stream);
				reply += "Lane " + std::to_string(i+1) + ": " + std::to_string(some_info) + "\n";

				some_info = GetTwoBytes(&s_stream);
				reply += "Line up: " + std::to_string((some_info*0.1)) + " m.\n";
				
				some_info = GetTwoBytes(&s_stream);
				reply += "Congestion: ";
				if ((some_info & 1) == 1)
					reply += "no congestion; ";
				if ((some_info & 2) == 2)
					reply += "light congestion; ";
				if ((some_info & 4) == 4)
					reply += "moderate congestion; ";
				if ((some_info & 8) == 8)
					reply += "heavy congestion; ";
				if ((some_info & 16) == 16)
					reply += "Exceeding the congestion length; ";
				if (reply[reply.size() - 2] == ':')
					reply += "no event occurred.";
				reply += "\n";
			}
			int ver = GetByte(&s_stream);
			reply += "Module verification: " + std::to_string(ver) + "\n";

			std::cout << reply;
			return true;
		}
	}
	return false;
}

bool ParseTrafficInfo(const std::string char_buffer) {
	std::string reply;
	std::istringstream s_stream(char_buffer);
	int targets_num;
	if (SkipRoadData(&s_stream, &targets_num)) {
		if ((GetByteString(&s_stream) == "4C") && (GetByteString(&s_stream) == "4C")) {
			reply += "Lane Data Module\n";
			int module_data_length = GetTwoBytes(&s_stream);
			reply += "Module data length: " + std::to_string(module_data_length) + " byte(s)\n";
			for (int i = 0; i < targets_num; i++) {
				int some_info = GetByte(&s_stream);
				reply += "Lane " + std::to_string(i + 1) + ": " + std::to_string(some_info) + "\n";

				some_info = GetTwoBytes(&s_stream);
				reply += "Cross-sectional flow-large cars: " + std::to_string(some_info) + "\n";
				
				some_info = GetTwoBytes(&s_stream);
				reply += "Average Rate-Large cars: " + std::to_string(some_info) + "\n";
				
				some_info = GetTwoBytes(&s_stream);
				reply += "Cross-sectional flow-medium-sized car: " + std::to_string(some_info) + "\n";
				
				some_info = GetTwoBytes(&s_stream);
				reply += "Average rate-medium-sized car: " + std::to_string(some_info) + "\n";
				
				some_info = GetTwoBytes(&s_stream);
				reply += "Cross-sectional flow-small cars: " + std::to_string(some_info) + "\n";
				
				some_info = GetTwoBytes(&s_stream);
				reply += "Average Speed-small cars: " + std::to_string(some_info) + "\n";
				
				some_info = GetTwoBytes(&s_stream);
				reply += "Cross-section flow-locomotives (non-motor vehicles): " + std::to_string(some_info) + "\n";
				
				some_info = GetTwoBytes(&s_stream);
				reply += "Average speed-locomotives  (non-motor vehicles): " + std::to_string(some_info) + "\n";
				
				some_info = GetTwoBytes(&s_stream);
				reply += "Motor vehicles (large medium small: " + std::to_string(some_info) + "\n";
				
				some_info = GetTwoBytes(&s_stream);
				reply += "Lane share: " + std::to_string(some_info) + "\n";
				
				some_info = GetTwoBytes(&s_stream);
				reply += "Time share: " + std::to_string(some_info) + "\n";
				
				some_info = GetTwoBytes(&s_stream);
				reply += "Average driving time: " + std::to_string(some_info) + "\n";
				
				some_info = GetTwoBytes(&s_stream);
				reply += "Average driving distance: " + std::to_string(some_info) + "\n";
				
				some_info = GetTwoBytes(&s_stream);
				reply += "Average driving distance: " + std::to_string(some_info) + "\n";
			}
			int ver = GetByte(&s_stream);
			reply += "Module verification: " + std::to_string(ver) + "\n";

			std::cout << reply;
			return true;
		}
	}
	return false;
}

bool ParseVefifModule(const std::string char_buffer) {
	std::string reply;
	std::istringstream s_stream(char_buffer);
	int some_info = GetByte(&s_stream);
	reply += "Checksum: " + std::to_string(some_info) + "\n";
	std::cout << reply;
	return true;
}


int main() {
	std::string string_buf = "AB CD 00 FB 4A 42 00 1B 01 00 02 0C 0E 16 08 08 06 1D 08 05 00 1B 00 00 00 0C 00 00 01 08 4A 4D 42 00 B3 D4 03 67 FF A0\n"
		"FF 41 00 02 00 00 00 00 00 00 46 50 00 FF FF FF A0 00 00 03 67 00 2D 12 D6 03 A0 00 82 00 D7 00 08 00 00 00 00 00 00 00\n"
		"11 00 00 00 00 82 00 00 03 A0 00 31 12 D0 03 D2 FF 91 FE E1 00 02 00 00 00 00 FF FD 46 95 00 FF FF FF 91 00 00 03 D2 00\n"
		"2D 12 D5 04 34 00 67 00 FE 00 07 00 00 00 00 00 07 00 A9 00 00 00 00 67 00 00 04 34 00 2D 12 D2 07 4E 00 82 00 F2 00 08\n"
		"15 9D 00 EB 00 02 00 31 00 00 00 00 82 00 00 07 4E 00 2D 12 D3 08 58 FF B3 FE CE 00 03 00 00 00 00 00 02 46 22 00 FF FF \n"
		"FF B3 00 00 08 58 00 2D 12 1C 44 4C 00 2D 01 00 00 00 00 02 00 00 00 00 03 00 00 00 00 04 00 00 00 00 05 00 00 00 00 06 \n"
		"00 00 00 00 07 00 00 00 00 08 00 00 00 00 E1 8E ";

	ParseDataFrameHeader(string_buf);
	//ParseBasicInformation(string_buf);
	//ParseModuleData(string_buf);
	//ParseLaneData(string_buf);
	//ParseTrafficInfo(string_buf);
	//ParseVefifModule(string_buf);
}