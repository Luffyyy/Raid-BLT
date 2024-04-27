#ifndef __SEARCHPATTERN_H__
#define __SEARCHPATTERN_H__

#include <string_view>
#include <vector>

class SearchPattern
{
public:
	struct NibblePattern
	{
		int8_t Nibble;
	};

	static constexpr NibblePattern Wildcard = { static_cast<int8_t>(0xF0) };

	SearchPattern(std::string_view strPattern);
	~SearchPattern();

	SearchPattern(const SearchPattern&);
	SearchPattern(SearchPattern&&) noexcept;

	SearchPattern& operator=(const SearchPattern&);
	SearchPattern& operator=(SearchPattern&&) noexcept;

	inline std::string_view GetString() const { return m_strPattern; }

	inline const std::vector<NibblePattern> GetNibbles() const { return m_nibbles; }

	void* Match(void* pOffset, size_t size);

private:
	std::string_view m_strPattern;
	std::vector<NibblePattern> m_nibbles;
};

#endif
