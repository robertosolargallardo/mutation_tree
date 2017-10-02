#include <geneset.hh>
geneset::geneset(void) {
   this->_root=nullptr;
}
geneset::geneset(const uint32_t &_population_size,const uint32_t &_locus_length,const double &_mutation_rate) {
    this->_population_size=_population_size;
    this->_locus_length=_locus_length;
    this->_mutation_rate=_mutation_rate;
    this->_root=std::make_shared<node>();

    this->_leafs.reserve(this->_population_size);
    this->_index.reserve(this->_population_size);

    for(uint32_t i=0U; i<this->_population_size; i++) {
		  std::shared_ptr<node> individual=std::make_shared<node>();
        individual->increase();
        individual->parent(this->_root);
		  this->_root->insert(individual);
		  this->_leafs.push_back(individual);
		  this->_index.push_back(i);
    }
	 std::random_shuffle(this->_index.begin(),this->_index.end());
}
geneset::~geneset(void) {
    this->_leafs.clear();
}
void geneset::drift(void) {
    static thread_local std::mt19937 rng;
	 std::chrono::steady_clock::time_point start,end;
    start=std::chrono::steady_clock::now();

    std::uniform_int_distribution<uint32_t> choose(0U,this->size()-1);
    std::binomial_distribution<uint32_t> mutations(this->_population_size*this->_locus_length,this->_mutation_rate);
    uint32_t number_of_mutations=mutations(rng);
	 std::for_each(this->_leafs.begin(),this->_leafs.end(),[](std::shared_ptr<node> &individual)->void{individual->references(0U);});
	 std::uniform_int_distribution<uint32_t> mutation(0U,this->_locus_length-1U);
	
	 uint32_t k=0U;
    for(uint32_t i=0; i<this->_population_size; i++){
		k=choose(rng);
		
		if(i<number_of_mutations){
         std::shared_ptr<node> child=std::make_shared<node>();
         child->mutate(mutation(rng));
			child->increase();
         this->_leafs[this->_index[k]]->insert(child);
         child->parent(this->_leafs[this->_index[k]]);
			this->_leafs.push_back(child);
		}
		else
			this->_leafs[this->_index[k]]->increase();
	 }
	 end=std::chrono::steady_clock::now();

    this->rebuild();

    /*std::cout << "simulation time::" << (std::chrono::duration_cast<std::chrono::nanoseconds>(end-start)).count() << " ";
	 std::cout << "number_of_mutations::" << number_of_mutations << " ";
	 std::cout << "number_of_alleles::" << this->_leafs.size() << std::endl;*/
}
void geneset::rebuild(void){
	uint32_t k=0U;
	this->_index.clear();
	for(std::vector<std::shared_ptr<node>>::iterator i=this->_leafs.begin();i!=this->_leafs.end();){
		if((*i)->references()==0){
			(*i)->remove();
			this->_leafs.erase(i);
		}
		else{
			for(uint32_t j=0U;j<(*i)->references();j++)
				this->_index.push_back(k);
			++k;
			++i;
		}
	}
}
uint32_t geneset::size(void) const {
    return(this->_index.size());
}
void geneset::save(const std::string &_filename) const{
	json document=this->_root->serialize();
	std::ofstream output(_filename);
   output << std::setw(4) << document << std::endl;
}
