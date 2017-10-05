#include <pop.hh>
pop::pop(void){

}
pop::pop(const pop &_pop){

}
pop::pop(const uint32_t &_population_size,const json &_profile){
	this->_population_size=_population_size;
	this->_number_of_genes=_profile["individual"]["genes"].size();
	this->_pool=std::make_unique<gene[]>(this->_number_of_genes);

	for(uint32_t i=0U;i<this->_number_of_genes;++i){
		this->_pool[i]._type=_profile["individual"]["genes"][i]["type"];
		this->_pool[i]._rate=_profile["individual"]["genes"][i]["mutation"]["rate"];
		this->_pool[i]._length=(_profile["individual"]["genes"][i].find("length")==_profile["individual"]["genes"][i].end())?0U:_profile["individual"]["genes"][i]["length"].get<uint32_t>();

		this->_pool[i]._root=std::make_shared<node>();
		this->_pool[i]._alleles.reserve(this->_population_size);

		for(uint32_t j=0U;j<this->_population_size;++j){
			this->_pool[i]._alleles.push_back(std::make_shared<node>());
			this->_pool[i]._alleles.back()->parent(this->_pool[i]._root);
			this->_pool[i]._root->child(this->_pool[i]._alleles.back());
		}
	}
	
	this->_individuals.reserve(this->_population_size);
	this->_index.reserve(this->_population_size);
   for(uint32_t i=0U;i<this->_population_size;++i){
		this->_index.push_back(i);
		this->_individuals.push_back(individual(this->_number_of_genes));
		for(uint32_t j=0U;j<this->_number_of_genes;++j)
			this->_individuals.back().set(j,this->_pool[j]._alleles[i]);
	}
}
pop& pop::operator=(const pop &_pop){
	return(*this);
}
pop::~pop(void){
	this->_individuals.clear();
}
void pop::drift(void){
	static thread_local std::mt19937 rng(time(0));
	std::uniform_int_distribution<uint32_t> uniform(0U,this->_population_size-1);

	/*TODO poner en una funcion*/
	uint32_t mutations[this->_number_of_genes],number_of_mutations=0U;
	uint32_t t=this->_population_size;
	for(uint32_t i=0U;i<this->_number_of_genes;++i){
		switch(this->_pool[i]._type){
			case SNP:{
				t=this->_population_size*this->_pool[i]._length;
				break;
			}
			case STR:{
				break;
			}
			default:{
				std::cerr << "error::unknown marker type: " << this->_pool[i]._type << std::endl;
				exit(EXIT_FAILURE);
			}
		}
		std::binomial_distribution<uint32_t> binomial(t,this->_pool[i]._rate);
		mutations[i]=binomial(rng);
		number_of_mutations+=mutations[i];
	}
	/*TODO poner en una funcion*/

	uint32_t j=0U,k=0U,m=0U;
	for(uint32_t i=0U;i<this->_population_size;++i){
		k=this->_index[uniform(rng)];

		if(i<number_of_mutations){
			individual mutant(this->_individuals[k]);
			mutant.references(0U);
			std::shared_ptr<node> allele=std::make_shared<node>();
			allele->parent(mutant._genes[j]);
			mutant._genes[j]->child(allele);

			allele->mutate();
			mutant.set(j,allele);
			mutant.increase();
			this->_pool[j]._alleles.push_back(allele);
			this->_individuals.push_back(mutant);
			
			++m;
			if(m==mutations[j]){
				++j;
				m=0U;
			}
		}
		else
			this->_individuals[k].increase();
	}
	this->rebuild();
}
void pop::rebuild(void){
	for(uint32_t i=0U;i<this->_number_of_genes;++i)
		this->_pool[i].contract();
	
	uint32_t k=0U;
	this->_index.clear();
	for(std::vector<individual>::iterator i=this->_individuals.begin();i!=this->_individuals.end();){
		if(i->references()==0U)
			this->_individuals.erase(i);
		else{
			for(uint32_t j=0U;j<i->_references;++j)
				this->_index.push_back(k);
			i->references(0U);

			++i;
			++k;
		}
	}
}
