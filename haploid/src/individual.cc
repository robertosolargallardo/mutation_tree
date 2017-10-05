#include <individual.hh>
individual::individual(void){
	this->_references=0U;
	this->_number_of_genes=0U;
}
individual::individual(const uint32_t &_number_of_genes){
	this->_references=0U;
	this->_number_of_genes=_number_of_genes;
	this->_genes=std::make_unique<std::shared_ptr<node>[]>(this->_number_of_genes);
}
individual::individual(const individual &_i){
	this->_references=_i._references;
	this->_number_of_genes=_i._number_of_genes;
	this->_genes=std::make_unique<std::shared_ptr<node>[]>(this->_number_of_genes);
	for(uint32_t i=0U;i<this->_number_of_genes;++i)
		this->_genes[i]=_i._genes[i];
}
individual& individual::operator=(const individual &_i){
	this->_references=_i._references;
	this->_number_of_genes=_i._number_of_genes;
	this->_genes=std::make_unique<std::shared_ptr<node>[]>(this->_number_of_genes);
	for(uint32_t i=0U;i<this->_number_of_genes;++i)
		this->_genes[i]=_i._genes[i];
	return(*this);
}
individual::~individual(void){
	;
}

void individual::increase(void){
	this->_references++;
	for(uint32_t gid=0U;gid<this->_number_of_genes;++gid)
		this->_genes[gid]->increase();
}
void individual::references(const uint32_t &_references){
	this->_references=_references;
}
void individual::set(const uint32_t &_position,const allele &_a){
	this->_genes[_position]=_a;
}

allele& individual::get(const uint32_t &_position) const{
	return(this->_genes[_position]);
}
uint32_t individual::references(void) const{
	return(this->_references);
}
