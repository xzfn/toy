#pragma once

namespace scopeutil {

class ScopedTrueToFalse {
public:
	ScopedTrueToFalse(bool& b)
	:m_bool(b)
	{
		m_bool = true;
	}

	~ScopedTrueToFalse() {
		m_bool = false;
	}

private:
	bool& m_bool;
};

}
