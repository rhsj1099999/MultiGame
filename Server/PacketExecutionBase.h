#pragma once

class ExcuetionMessageBase
{
	public: ExcuetionMessageBase() {}
	public: virtual void ExecutePacket(PacketBase packet) = 0;
};

