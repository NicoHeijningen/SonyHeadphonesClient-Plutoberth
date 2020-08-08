#pragma once
#include <string>

inline constexpr auto XM3_UUID = "96CC203E-5068-46ad-B32D-E316F5E069BA";

class IBluetoothConnector
{
public:
	IBluetoothConnector() = default;
	virtual ~IBluetoothConnector() = default;

	IBluetoothConnector(const IBluetoothConnector&) = delete;
	IBluetoothConnector(IBluetoothConnector&&) = delete;

	IBluetoothConnector& operator=(const IBluetoothConnector&) = delete;
	IBluetoothConnector& operator=(IBluetoothConnector&&) = delete;

	//O: The number of bytes sent.
	virtual int send(char* buf, size_t length) noexcept(false) = 0;

	virtual void connect(const std::string& addrStr) noexcept(false) = 0;
};