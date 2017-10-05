#ifndef _POPSET_HH_
#define _POPSET_HH_
#include <pop.h>
#include <glob.h>

class popset{
	private:	std::vector<pop> _popset;

	public:	popset(void);
				popset(const json&);
				popset(const popset&);
				popset& operator=(const popset&);
				~popset(void);
};
#endif
