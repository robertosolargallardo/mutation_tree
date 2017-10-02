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

    for(uint32_t i=0U; i<this->_population_size; i++) {
        std::array<std::shared_ptr<node>,N> individual;
		  for(int j=0;j<N;j++){
			  individual[j]=std::make_shared<node>();
           individual[j]->increase();
           individual[j]->parent(this->_root);
			  this->_root->insert(individual[j]);
		  }
		  this->_leafs.push_back(individual);
    }
	 std::random_shuffle(this->_leafs.begin(),this->_leafs.end());
}
geneset::~geneset(void) {
    this->_leafs.clear();
}
void geneset::drift(void) {
	 std::chrono::steady_clock::time_point start,end;
    start=std::chrono::steady_clock::now();

    std::uniform_int_distribution<uint32_t> choose(0U,(this->size()*N)-1);
    std::binomial_distribution<uint32_t> mutations(this->_population_size*this->_locus_length,this->_mutation_rate);
    uint32_t number_of_mutations=mutations(rng);
	
	 std::for_each(this->_leafs.begin(),this->_leafs.end(),[](std::array<std::shared_ptr<node>,N> &individual)->void{individual[0]->references(0U);individual[1]->references(0U);});
		
    std::array<uint32_t,N> k;
    std::array<std::shared_ptr<node>,N> homologous;
	 std::vector<std::array<std::shared_ptr<node>,N>> leafs;
	 std::uniform_int_distribution<uint32_t> coin(0U,1U);
	 std::uniform_int_distribution<uint32_t> mutation(0U,this->_locus_length-1U);

    for(uint32_t i=0; i<this->_population_size; i++){
		k={choose(rng),choose(rng)};
		homologous={this->_leafs[k[0]/N][k[0]%N],this->_leafs[k[1]/N][k[1]%N]};
	
		if(i<number_of_mutations){
			uint32_t m=coin(rng);
         std::shared_ptr<node> child=std::make_shared<node>();
         child->mutate(mutation(rng));
         homologous[m]->insert(child);
         child->parent(homologous[m]);
			homologous[m]=child;
		}

		homologous[0]->increase();
		homologous[1]->increase();
		leafs.push_back(homologous);
	 }
	 end=std::chrono::steady_clock::now();

    this->rebuild(leafs);

    std::cout << "simulation time::" << (std::chrono::duration_cast<std::chrono::nanoseconds>(end-start)).count() << " ";
	 std::cout << "number_of_mutations::" << number_of_mutations << std::endl;
}
void geneset::rebuild(std::vector<std::array<std::shared_ptr<node>,N>> &_leafs) {
	//std::chrono::steady_clock::time_point t1,t2,t3,t4,t5,t6;

   //t1=std::chrono::steady_clock::now();
	for(auto& individual : this->_leafs){
		if(individual[0]->references()==0U) individual[0]->remove();
		if(individual[1]->references()==0U) individual[1]->remove();
	}
	this->_leafs.clear();
   //t2=std::chrono::steady_clock::now();

   //t3=std::chrono::steady_clock::now();
	this->_leafs=_leafs;
   //t4=std::chrono::steady_clock::now();

	//std::cout << "remove  : " << (std::chrono::duration_cast<std::chrono::nanoseconds>(t2-t1)).count() << std::endl;
	//std::cout << "copy    : " << (std::chrono::duration_cast<std::chrono::nanoseconds>(t4-t3)).count() << std::endl;
}
uint32_t geneset::size(void) const {
    return(this->_leafs.size());
}
void geneset::save(const std::string &_filename) const{
	json document=this->_root->serialize();
	std::ofstream output(_filename);
   output << std::setw(4) << document << std::endl;
}
