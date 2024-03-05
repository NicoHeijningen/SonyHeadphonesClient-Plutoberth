#pragma once
#include "Constants.h"
#include "ByteMagic.h"
#include <cstddef>
#include <vector>
#include <stdexcept>
#include "Exceptions.h"

constexpr int MINIMUM_VOICE_FOCUS_STEP = 2;
constexpr unsigned int ASM_LEVEL_DISABLED = -1;

namespace CommandSerializer
{

	//escape special chars

	Buffer _escapeSpecials(const Buffer& src);
	Buffer _unescapeSpecials(const Buffer& src);
	unsigned char _sumChecksum(const char* src, size_t size);
	unsigned char _sumChecksum(const Buffer& src);
	//Package a serialized command according to the protocol
	/*
	References:
	* DataType
	* CommandBluetoothSender.sendCommandWithRetries
	* BluetoothSenderWrapper.sendCommandViaBluetooth
	* 
	* Serialized data format: <START_MARKER>ESCAPE_SPECIALS(<DATA_TYPE><SEQ_NUMBER><BIG ENDIAN 4 BYTE SIZE OF UNESCAPED DATA><DATA><1 BYTE CHECKSUM>)<END_MARKER>
	*/
	Buffer packageDataForBt(const Buffer& src, DATA_TYPE dataType, unsigned int seqNumber);

	NC_DUAL_SINGLE_VALUE getDualSingleForAsmLevel(char asmLevel);
	Buffer serializeNcAndAsmSetting(NC_ASM_EFFECT ncAsmEffect, NC_ASM_SETTING_TYPE ncAsmSettingType, ASM_ID voicePassthrough, char asmLevel);
	Buffer serializeVPTSetting(VPT_INQUIRED_TYPE type, unsigned char preset);
	Buffer serializeVoiceGuidanceSetting(char volume);

	// POD Wrapper for any Buffer (of messages) that contains the command payload (which may also be size 0,i.e. ACKs)
	struct CommandMessage
	{
		Buffer messageBytes{};

		CommandMessage() = default;

		CommandMessage(Buffer const& buf) : messageBytes(buf) {};
		CommandMessage(Buffer&& buf) : messageBytes(buf) {};
		CommandMessage(DATA_TYPE dataType, Buffer const& buffer, unsigned char seqNumber) {
			messageBytes = CommandSerializer::packageDataForBt(buffer, dataType, seqNumber);
		}

		inline const DATA_TYPE getDataType() { return static_cast<DATA_TYPE>(messageBytes[1]); }
		inline const unsigned char getSeqNumber() { return messageBytes[2]; }
		inline const int getSize() { return bytesToIntBE(&messageBytes[3]); }
		inline const unsigned char getChkSum() { return messageBytes[7 + getSize()]; }

		inline Buffer::iterator begin() { return messageBytes.begin() + 7; }
		inline Buffer::iterator end() { return begin() + getSize(); }
		inline const char operator[](int i) { return *(begin() + i); }

		inline Buffer const& getMessage() { return messageBytes; }

		inline const unsigned char calcChkSum() { return CommandSerializer::_sumChecksum(messageBytes.data() + 1, messageBytes.size() - 3); }
		inline const bool verify() { return messageBytes.size() >= 7 && getChkSum() == calcChkSum(); }
	};
}

