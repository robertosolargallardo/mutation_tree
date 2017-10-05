#ifndef _POP_HH_
#define _POP_HH_
#include <glob.hh>
#include <node.hh>
#include <gene.hh>
#include <individual.hh>

class pop{
	private: std::unique_ptr<gene[]> 	_pool;
				std::vector<individual> 	_individuals;
				std::vector<uint32_t> 		_index;
				uint32_t 						_population_size;
				uint32_t							_number_of_genes;

	public:	pop(void);
				pop(const pop&);
				pop(const uint32_t&,const json&);
				pop& operator=(const pop&);
				~pop(void);

				void drift(void);
				void rebuild(void);
};
#endif
