#include "Bitmask.h"

Bitmask::Bitmask()	
	: m_Bits(0)
{
}

void Bitmask::SetBitmask(Bitmask& other)
{
	m_Bits = other.GetMask();
}

uint32_t Bitmask::GetMask() const
{
	return m_Bits;
}

bool Bitmask::GetBit(int pos) const
{
	return (m_Bits & (1 << pos)) != 0;
}

void Bitmask::SetBit(int pos, bool on)
{
	if (on)
	{
		SetBit(pos);
	}
	else
	{
		ClearBit(pos);
	}
}

void Bitmask::SetBit(int pos)
{
	m_Bits = m_Bits | 1 << pos;
}

void Bitmask::ClearBit(int pos)
{
	m_Bits = m_Bits & ~(1 << pos);
}

void Bitmask::Clear()
{
	m_Bits = 0;
}
