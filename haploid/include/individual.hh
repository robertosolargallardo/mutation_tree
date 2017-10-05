#ifndef _INDIVIDUAL_HH_
#define _INDIVIDUAL_HH_
#include <glob.hh>
#include <gene.hh>
class individual{
	private:	uint32_t _references;
				uint32_t _number_of_genes;
				std::unique_ptr<allele[]> _genes;

	public:	individual(void);
				individual(const uint32_t&);
				individual(const individual&);
				individual& operator=(const individual&);
				~individual(void);

				void increase(void);
				void references(const uint32_t&);
				void set(const uint32_t&,const allele&);

				allele& get(const uint32_t&) const;
				uint32_t references(void) const;
			
};
#endif
