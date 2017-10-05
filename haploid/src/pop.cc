#include <pop.hh>
pop::pop(void){

}
pop::pop(const pop &_pop){

}
pop::pop(const uint32_t &_population_size,const json &_profile){
	this->_population_size=_population_size;
	this->_number_of_genes=_profile["individual"]["genes"].size();
	this->_pool=std::make_unique<gene[]>(this->_number_of_genes);

	for(uint32_t gid=0U;gid<this->_number_of_genes;++gid){
		this->_pool[gid].type(_profile["individual"]["genes"][gid]["type"]);
		this->_pool[gid].rate(_profile["individual"]["genes"][gid]["mutation"]["rate"]);
		this->_pool[gid].length((_profile["individual"]["genes"][gid].find("length")==_profile["individual"]["genes"][gid].end())?0U:_profile["individual"]["genes"][gid]["length"].get<uint32_t>());
		this->_pool[gid].create(this->_population_size);
	}
		
	this->_index.reserve(this->_population_size);
	this->_individuals.reserve(this->_population_size);

   for(uint32_t i=0U;i<this->_population_size;++i){
		this->_index.push_back(i);
		this->_individuals.push_back(individual(this->_number_of_genes));
		for(uint32_t gid=0U;gid<this->_number_of_genes;++gid)
			this->_individuals.back().set(gid,this->_pool[gid].get(i));
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
		switch(this->_pool[i].type()){
			case SNP:{
				t=this->_population_size*this->_pool[i].length();
				break;
			}
			case STR:{
				break;
			}
			default:{
				std::cerr << "error::unknown marker type: " << this->_pool[i].type() << std::endl;
				exit(EXIT_FAILURE);
			}
		}
		std::binomial_distribution<uint32_t> binomial(t,this->_pool[i].rate());
		mutations[i]=binomial(rng);
		number_of_mutations+=mutations[i];
	}
	/*TODO poner en una funcion*/

	uint32_t position=0U,k=0U,m=0U;
	for(uint32_t i=0U;i<this->_population_size;++i){
		k=this->_index[uniform(rng)];

		if(i<number_of_mutations){
			individual mutant(this->_individuals[k]);
			mutant.references(0U);
			std::shared_ptr<node> allele=std::make_shared<node>();
			allele->parent(mutant.get(position));
			mutant.get(position)->child(allele);

			allele->mutate();
			mutant.set(position,allele);
			mutant.increase();
			this->_pool[position].insert(allele);
			this->_individuals.push_back(mutant);
			
			++m;
			if(m==mutations[position]){
				++position;
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
			for(uint32_t j=0U;j<i->references();++j)
				this->_index.push_back(k);
			i->references(0U);

			++i;
			++k;
		}
	}
}
