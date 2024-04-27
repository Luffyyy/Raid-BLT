#include "SearchPattern.h"

SearchPattern::SearchPattern(std::string_view strPattern)
	: m_strPattern(strPattern)
{
	for (auto it : strPattern)
	{
		if (it >= '0' && it <= '9')
			m_nibbles.push_back({ static_cast<int8_t>(it - '0') });
		else if (it >= 'A' && it <= 'F')
			m_nibbles.push_back({ static_cast<int8_t>(it - 'A' + 0x0A) });
		else if (it >= 'a' && it <= 'f')
			m_nibbles.push_back({ static_cast<int8_t>(it - 'a' + 0x0A) });
		else if (it == '?')
			m_nibbles.push_back(Wildcard);
	}
}

SearchPattern::~SearchPattern()
{ }

SearchPattern::SearchPattern(const SearchPattern& old)
	: m_strPattern(old.m_strPattern), m_nibbles(old.m_nibbles)
{ }

SearchPattern::SearchPattern(SearchPattern&& old) noexcept
	: m_strPattern(std::move(old.m_strPattern)), m_nibbles(std::move(old.m_nibbles))
{ }

SearchPattern& SearchPattern::operator=(const SearchPattern& old)
{
	m_strPattern = old.m_strPattern;
	m_nibbles = old.m_nibbles;
	return (*this);
}

SearchPattern& SearchPattern::operator=(SearchPattern&& old) noexcept
{
	m_strPattern = std::move(old.m_strPattern);
	m_nibbles = std::move(old.m_nibbles);
	return (*this);
}

void* SearchPattern::Match(void* pOffset, size_t size)
{
	uint8_t* pCurrent = reinterpret_cast<uint8_t*>(pOffset);
	uint8_t* pEnd = pCurrent + size;

	while (pCurrent < pEnd)
	{
		auto pStart = pCurrent;

		int8_t rShift = 4;
		bool bFound = true;

		for (auto& it : m_nibbles)
		{
			if (it.Nibble != Wildcard.Nibble)
			{
				int8_t nibble = (*pStart >> rShift) & 0x0F;

				if (nibble != it.Nibble)
				{
					bFound = false;
					break;
				}
			}

			rShift ^= 4;
			pStart += (rShift >> 2);
		}

		if (bFound)
			return pCurrent;

		++pCurrent;
	}

	return nullptr;
}
