#include <node.hh>
node::node(void) {
    this->_parent=nullptr;
	 this->_counter=0U;
    this->_references=0U;
}
node::node(const node &_node) {
    this->_references=_node._references;
    this->_parent=_node._parent;
    this->_children=_node._children;
    this->_mutations=_node._mutations;
	 this->_counter=_node._counter;
}
node& node::operator=(const node &_node) {
    this->_references=_node._references;
    this->_parent=_node._parent;
    this->_children=_node._children;
    this->_mutations=_node._mutations;
	 this->_counter=_node._counter;
    return(*this);
}
node::~node(void) {
    this->_parent=nullptr;
    this->_mutations.clear();
	 this->_children.clear();
}
void node::child(std::shared_ptr<node> &_node) {
    this->_children.push_back(_node);
}
void node::remove(std::shared_ptr<node> _node) {
    if(_node->children().empty()) {
        auto target=std::find(this->_children.begin(),this->_children.end(),_node);
		  if(target==this->_children.end()) return;

        this->_children.erase(target);

        if(!this->references()) {
            if(this->children().empty())
                this->parent()->remove(shared_from_this());
            else if(this->children().size()==1U) {
                if(this->parent()) {
                    //this->children().back()->mutations().insert(this->children().back()->mutations().begin(),this->mutations().begin(),this->mutations().end());
                    this->children().back()->_counter+=this->_counter;
                    this->parent()->child(this->children().back());
                    this->children().back()->parent(this->parent());
                    this->children().pop_back();
                    this->remove();
                } else
                    this->children().back()->_counter=0U; //this->children().back()->mutations().clear();
            }
        }
    }
	 else{
        for(auto& child : _node->children()){
            child->parent(_node->parent());
			   this->children().push_back(child);
		  }
		  _node->children().clear();
		  auto target=std::find(this->_children.begin(),this->_children.end(),_node);
		  if(target==this->_children.end()) return;

        this->_children.erase(target);
	 }
}
void node::remove(void) {
    this->_parent->remove(shared_from_this());
}
void node::parent(std::shared_ptr<node> &_parent) {
    this->_parent=_parent;
}
std::shared_ptr<node>& node::parent(void) {
    return(this->_parent);
}
void node::increase(void) {
    this->_references++;
}
void node::clear(void) {
    this->_counter=0U;
    this->_references=0U;
    this->_mutations.clear();
}
uint32_t node::references(void) const {
    return(this->_references);
}
void node::references(const uint32_t &_references) {
    this->_references=_references;
}
void node::mutate(const uint32_t &_mutation) {
    this->_mutations.push_back(_mutation);
}
void node::mutate(void) {
    this->_counter++;
}
std::vector<std::shared_ptr<node>>& node::children(void) {
    return(this->_children);
}
std::vector<uint32_t>& node::mutations(void) {
    return(this->_mutations);
}
json node::serialize(void) {
    json document;

    document["references"]=this->references();
    document["mutations"]=this->mutations();

    if(!this->children().empty()) {
        for(auto child : this->children())
            document["children"].push_back(child->serialize());
    }
    return(document);
}
