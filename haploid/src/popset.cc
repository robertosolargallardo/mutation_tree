#include <popset.hh>
popset::popset(void){
	this->_number_of_genes=0U;
}
popset::popset(const json &_profile){
	this->_number_of_genes=_profile["individual"]["genes"].size();
	this->_pool=std::make_shared<std::unique_ptr<gene[]>>(std::make_unique<gene[]>(this->_number_of_genes));

	for(uint32_t gid=0U;gid<this->_number_of_genes;++gid){
		(*this->_pool)[gid].type(_profile["individual"]["genes"][gid]["type"]);
		(*this->_pool)[gid].rate(_profile["individual"]["genes"][gid]["mutation"]["rate"]);

		switch((*this->_pool)[gid].type()){
			case SNP:{
				(*this->_pool)[gid].length(_profile["individual"]["genes"][gid]["length"].get<uint32_t>());
				break;
			}
			case STR:{
				(*this->_pool)[gid].length(0U);
				break;
			}
			default:{
				std::cerr << "error::unknown marker type: " << (*this->_pool)[gid].type() << std::endl;
				exit(EXIT_FAILURE);
			}
		}
	}
}
popset::popset(const popset &_ps){
	this->_number_of_genes=_ps._number_of_genes;
	this->_popset=_ps._popset;
	this->_pool=_ps._pool;
}
popset& popset::operator=(const popset &_ps){
	this->_number_of_genes=_ps._number_of_genes;
	this->_popset=_ps._popset;
	this->_pool=_ps._pool;
	return(*this);
}
popset::~popset(void){
	this->_popset.clear();
}
uint32_t popset::create(const uint32_t &_population_size){
	pop p(this->_popset.empty()?0U:this->_popset.rbegin()->id()+1U,_population_size,this->_number_of_genes,this->_pool);
	this->_popset.push_back(p);
   return(p.id());
}
void popset::drift(void){
	for(auto& p : this->_popset)
		p.drift();
}
void popset::flush(void){
	for(auto& p : this->_popset)
		p.flush();

	for(uint32_t position=0U;position<this->_number_of_genes;++position){
		(*this->_pool)[position].contract();
		(*this->_pool)[position].flush();
	}
}
void popset::serialize(const std::string &_directory){
   for(uint32_t i=0;i<this->_number_of_genes;++i){
      std::string filename=_directory+"/"+"gene_"+std::to_string(i)+".bin";
      (*this->_pool)[i].serialize(filename);
   }
	for(auto& p : this->_popset){
      std::string filename=_directory+"/"+"pop_"+std::to_string(p.id())+".bin";
      p.serialize(filename);
   }
}

