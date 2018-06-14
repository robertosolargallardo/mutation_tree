#include <evlist.hh>
evlist::evlist(void) {
    this->_lvt=0U;
}
evlist::evlist(const evlist &_evlist) {
	this->_lvt=_evlist._lvt;
	this->_list=_evlist._list;
}
evlist& evlist::operator=(const evlist &_evlist) {
	this->_lvt=_evlist._lvt;
	this->_list=_evlist._list;
	return(*this);
}
evlist::evlist(const json &_fscenario) {
    this->_lvt=0U;
    for(auto& e : _fscenario["events"])
        this->_list.push(std::make_shared<event>(e["timestamp"],event_t(util::hash(e["type"])),e["params"]));
}
std::shared_ptr<event> evlist::top(void) {
    return(this->_list.empty()?nullptr:this->_list.top());
}
void evlist::pop(void) {
    if(!this->empty()) {
        this->_lvt=this->top()->timestamp();
        this->_list.pop();
    }
}
uint32_t evlist::lvt(void) {
    return(this->_lvt);
}
bool evlist::empty(void) {
    return(this->_list.empty());
}
evlist::~evlist(void) {
    while(!this->empty())
        this->pop();
}
