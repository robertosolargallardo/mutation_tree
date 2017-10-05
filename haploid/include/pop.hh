#ifndef _POP_HH_
#define _POP_HH_
#include <glob.hh>
#include <node.hh>
enum MarkerType {SNP=0U,STR=1U};

class pop{
	private:	class gene{
					public:  double _rate;
								uint32_t _length;
								MarkerType _type;

								std::shared_ptr<node> _root;
								std::vector<std::shared_ptr<node>> _alleles;

								gene(void){
									this->_root=nullptr;
								}
								gene(const gene &_g){
									this->_root=_g._root;
									this->_alleles=_g._alleles;
								}
								gene& operator=(const gene &_g){
									this->_root=_g._root;
									this->_alleles=_g._alleles;
									return(*this);
								}
								~gene(void){
									this->_alleles.clear();
								}
								void contract(void){
									for(std::vector<std::shared_ptr<node>>::iterator i=this->_alleles.begin();i!=this->_alleles.end();){
										if((*i)->references()==0U){
											(*i)->remove();
											this->_alleles.erase(i);
										}
										else
											++i;
									}
									for(auto& allele : this->_alleles)
											allele->references(0U);
								}
				};
				class individual{
					public:	uint32_t _references;
								uint32_t _number_of_genes;
								std::unique_ptr<std::shared_ptr<node>[]> _genes;

								individual(void){
									this->_references=0U;
								}
								individual(const uint32_t &_number_of_genes){
									this->_references=0U;
									this->_number_of_genes=_number_of_genes;
									this->_genes=std::make_unique<std::shared_ptr<node>[]>(this->_number_of_genes);
								}
								individual(const individual &_i){
									this->_references=_i._references;
									this->_number_of_genes=_i._number_of_genes;
									this->_genes=std::make_unique<std::shared_ptr<node>[]>(this->_number_of_genes);
									for(uint32_t i=0U;i<this->_number_of_genes;++i)
										this->_genes[i]=_i._genes[i];
								}
								individual& operator=(const individual &_i){
									this->_references=_i._references;
									this->_number_of_genes=_i._number_of_genes;
									this->_genes=std::make_unique<std::shared_ptr<node>[]>(this->_number_of_genes);
									for(uint32_t i=0U;i<this->_number_of_genes;++i)
										this->_genes[i]=_i._genes[i];
									return(*this);
								}
								void increase(void){
									this->_references++;
									for(uint32_t i=0U;i<this->_number_of_genes;++i)
										this->_genes[i]->increase();
					
								}
								~individual(void){
									;
								}
								uint32_t references(void) const{
									return(this->_references);
								}
								void references(const uint32_t &_references){
									this->_references=_references;
								}
								void set(const uint32_t &_index,const std::shared_ptr<node> &_gen){
									this->_genes[_index]=_gen;
								}
				};
			
				std::unique_ptr<gene[]> 	_pool;
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
