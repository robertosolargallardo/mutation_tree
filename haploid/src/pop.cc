#include <pop.hh>
pop::pop(void){
	this->_pool=nullptr;
	this->_population_size=0U;
	this->_number_of_genes=0U;
}
pop::pop(const pop &_p){
	this->_pool=_p._pool;
	this->_index=_p._index;
	this->_individuals=_p._individuals;
	this->_population_size=_p._population_size;
	this->_number_of_genes=_p._number_of_genes;
}
pop& pop::operator=(const pop &_p){
	this->_pool=_p._pool;
	this->_index=_p._index;
	this->_individuals=_p._individuals;
	this->_population_size=_p._population_size;
	this->_number_of_genes=_p._number_of_genes;
	return(*this);
}
pop::~pop(void){
	this->_index.clear();
	this->_individuals.clear();
}
pop::pop(const uint32_t &_id,const uint32_t &_population_size,const uint32_t &_number_of_genes,const std::shared_ptr<std::unique_ptr<gene[]>> &_pool){
	this->_id=this->_id;
	this->_pool=_pool;
	this->_population_size=_population_size;
	this->_number_of_genes=_number_of_genes;
	
	this->_index.reserve(this->_population_size);
	this->_individuals.reserve(this->_population_size);

	for(uint32_t i=0U;i<this->_population_size;++i){
		individual offspring(this->_number_of_genes);
		for(uint32_t position=0U;position<this->_number_of_genes;++position)
			offspring.set(position,(*this->_pool)[position].create());
		this->_individuals.push_back(offspring);
		this->_index.push_back(i);
	 }
}
uint32_t pop::id(void) const{
	return(this->_id);
}
std::tuple<uint32_t,uint32_t*> pop::mutations(void){
	static thread_local std::mt19937 rng(time(0));

	uint32_t number_of_mutations=0U;
	uint32_t *mutations_per_gene=new uint32_t[this->_number_of_genes];
	uint32_t t=this->_population_size;

	for(uint32_t i=0U;i<this->_number_of_genes;++i){
		switch((*this->_pool)[i].type()){
			case SNP:{
				t=this->_population_size*(*this->_pool)[i].length();
				break;
			}
			case STR:{
				break;
			}
			default:{
				std::cerr << "error::unknown marker type: " << (*this->_pool)[i].type() << std::endl;
				exit(EXIT_FAILURE);
			}
		}
		std::binomial_distribution<uint32_t> binomial(t,(*this->_pool)[i].rate());
		mutations_per_gene[i]=binomial(rng);
		number_of_mutations+=mutations_per_gene[i];
	}
	return(std::tuple<uint32_t,uint32_t*>(number_of_mutations,mutations_per_gene));
}
void pop::drift(void){
	static thread_local std::mt19937 rng(time(0));
	std::uniform_int_distribution<uint32_t> uniform(0U,this->_population_size-1U);

	uint32_t number_of_mutations=0U;
	uint32_t *mutations_per_gene=nullptr;

	std::tie<uint32_t,uint32_t*>(number_of_mutations,mutations_per_gene)=this->mutations();

	uint32_t id=0U,position=0U,mutations=0U;
	for(uint32_t i=0U;i<this->_population_size;++i){
		id=this->_index[uniform(rng)];

		if(i<number_of_mutations){
			individual mutant(this->_individuals[id]);
			mutant.references(0U);
			std::shared_ptr<node> allele=std::make_shared<node>();
			allele->parent(mutant.get(position));
			mutant.get(position)->child(allele);

			allele->mutate();
			mutant.set(position,allele);
			mutant.increase();
			(*this->_pool)[position].insert(allele);
			this->_individuals.push_back(mutant);
			
			++mutations;
			if(mutations==mutations_per_gene[position]){
				++position;
				mutations=0U;
			}
		}
		else
			this->_individuals[id].increase();
	}
	
	for(std::vector<individual>::iterator i=this->_individuals.begin();i!=this->_individuals.end();){
		if(i->references()==0U)
			this->_individuals.erase(i);
		else ++i;
	}
	delete[] mutations_per_gene;
}
void pop::flush(void){
	this->_index.clear();
	for(uint32_t i=0U;i<this->_individuals.size();++i){
		for(uint32_t j=0U;j<this->_individuals[i].references();++j)
			this->_index.push_back(i);
		this->_individuals[i].references(0U);
	}
}
