#include <gene.hh>
gene::gene(void){
	this->_rate=0.0;
	this->_length=0U;
	this->_root=nullptr;
	this->_type=UNKNOWN;
}
gene::gene(const gene &_g){
	this->_rate=_g._rate;
	this->_root=_g._root;
	this->_type=_g._type;
	this->_length=_g._length;
	this->_alleles=_g._alleles;
}
gene& gene::operator=(const gene &_g){
	this->_rate=_g._rate;
	this->_root=_g._root;
	this->_type=_g._type;
	this->_length=_g._length;
	this->_alleles=_g._alleles;
	return(*this);
}
gene::~gene(void){
	this->_alleles.clear();
}
		
void gene::rate(const double &_rate){
	this->_rate=_rate;
}
void gene::length(const uint32_t &_length){
	this->_length=_length;
}
void gene::type(const Type &_type){
	this->_type=_type;
}

double gene::rate(void) const{
	return(this->_rate);
}
uint32_t gene::length(void) const{
	return(this->_length);
}
Type gene::type(void) const{
	return(this->_type);
}

void gene::insert(const allele &_a){
	this->_alleles.push_back(_a);
}
void gene::contract(void){
	for(std::vector<std::shared_ptr<node>>::iterator i=this->_alleles.begin();i!=this->_alleles.end();){
		if((*i)->references()==0U){
			(*i)->remove();
			this->_alleles.erase(i);
		}
		else ++i;
	}
	for(auto& allele : this->_alleles) allele->references(0U);
}
allele gene::get(const uint32_t &_position) const{
	return(this->_alleles[_position]);
}
void gene::create(const uint32_t &_population_size){
	this->_root=std::make_shared<node>();
   this->_alleles.reserve(_population_size);

   for(uint32_t i=0U;i<_population_size;++i){
   	this->_alleles.push_back(std::make_shared<node>());
      this->_alleles.back()->parent(this->_root);
      this->_root->child(this->_alleles.back());
   }
}
