#pragma once
#include <string>
#include <cstring>
#include <type_traits>

#include "enet/enet.h"
#include "Debug.h"

namespace une
{
	//Serialize a type with contiguous memory, make sure to not have any heap data
	template<typename T>
	[[nodiscard]] std::pair<char*, size_t> Serialize(T type)
	{
		static_assert(std::is_trivially_copyable_v<T>, "Default Serialize function must only be used with trivially copyable data types");

		//Create the buffer and copy the object to it
		char* buff = new char[sizeof(T)];
		memset(buff, 0, sizeof(T));
		memcpy(buff, &type, sizeof(T));

		return {buff, sizeof(T)};
	}

	//Deserialize a type with contiguous memory, make sure to not have any heap data
	template<typename T>
	std::pair<T, size_t> Deserialize(const char* data)
	{
		static_assert(std::is_trivially_copyable_v<T>, "Default Serialize function must only be used with trivially copyable data types");

		T result;
		memcpy(&result, data, sizeof(T));
		return {result, sizeof(T)};
	}

	//Special case for strings
	template<>
	[[nodiscard]] inline std::pair<char*, size_t> Serialize<std::string>(std::string type)
	{
		//Add a null terminator
		char* buff = new char[type.size() + 1];
		memset(buff, 0, type.size() + 1);
		memcpy(buff, type.data(), type.size());
		return {buff, type.size() + 1};
	}
	template<>
	std::pair<std::string, size_t> inline Deserialize<std::string>(const char* data)
	{
		//Include the null terminator
		size_t size = strlen((char*)data) + 1;
		std::string result((char*)data, size);
		return {result, size};
	}

	//A binary array of data typically used with ENet networking
	class Packet
	{
	public:
		enum Flag
		{
			Reliable = ENET_PACKET_FLAG_RELIABLE,
			Unreliable = ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT,
			Unsequenced = ENET_PACKET_FLAG_UNSEQUENCED
		};

		Packet() {};
		Packet(char* bytes, size_t size)
		{
			data.reserve(size);
			data.insert(data.end(), bytes, bytes + size);
		}

		//Deserialize an object and move the head, returns the object and it's size in bytes
		template<typename T>
		std::pair<T, size_t> Read()
		{
			std::pair<T, size_t> object = Deserialize<T>(data.data() + readHead);
			readHead += object.second;
			return object;
		}
		//Deserialize an object but don't move the head, returns the object and it's size in bytes
		template<typename T>
		std::pair<T, size_t> Peek()
		{
			std::pair<T, size_t> object = Deserialize<T>(data.data() + readHead);
			return object;
		}
		//Serialize an object to the end of the data
		template<typename T>
		void Write(T obj)
		{
			std::pair<char*, size_t> bytes = Serialize(obj);
			data.insert(data.end(), bytes.first, bytes.first + bytes.second);
			delete[] bytes.first;
		}
		//Read a numer of bytes and move the head
		[[nodiscard]] std::vector<char> ReadBytes(size_t num)
		{
			if (num > data.size() + readHead)
			{
				debug::LogWarning("Could not read " + std::to_string(num) + " bytes, eof!");
				return {};
			}
			std::vector<char> bytes(data.begin() + readHead, data.begin() + readHead + num);
			readHead += num;
			return bytes;
		}
		//Read a numer of bytes and move the head
		[[nodiscard]] std::vector<char> PeekBytes(size_t num)
		{
			if (num > data.size() + readHead)
			{
				debug::LogWarning("Could not read " + std::to_string(num) + " bytes, eof!");
				return {};
			}
			return std::vector<char>(data.begin() + readHead, data.begin() + readHead + num);
		}
		//Write a numer of bytes to the end of the data
		void WriteBytes(char* bytes, size_t num)
		{
			data.insert(data.end(), bytes, bytes + num);
		}

		//Create an enet packet from serialized data
		ENetPacket* ToENetPacket(Flag flags)
		{
			ENetPacket* packet = enet_packet_create(nullptr, data.size(), flags);
			memcpy(packet->data, data.data(), data.size());
			return packet;
		}

		std::vector<char> data;
		size_t readHead = 0;
		//These are used by enet implementation
		int peerID = -1;
		int chanelID = 0;
	};
}
