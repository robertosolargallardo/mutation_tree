#include <node.hh>
node::node(void)
{
    this->_parent=nullptr;
    this->_number_of_mutations=0U;
    this->_references=0U;
    this->_repeats=0U;
    this->_id=(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())).count();
}
node::node(const node &_node)
{
    this->_id=_node._id;
    this->_references=_node._references;
    this->_parent=_node._parent;
    this->_children=_node._children;
    this->_repeats=_node._repeats;
    this->_point_mutations=_node._point_mutations;
    this->_number_of_mutations=_node._number_of_mutations;
}
node& node::operator=(const node &_node)
{
    this->_id=_node._id;
    this->_references=_node._references;
    this->_parent=_node._parent;
    this->_children=_node._children;
    this->_repeats=_node._repeats;
    this->_point_mutations=_node._point_mutations;
    this->_number_of_mutations=_node._number_of_mutations;
    return(*this);
}
node::~node(void)
{
    this->_parent=nullptr;
    this->_point_mutations.clear();
    this->_children.clear();
}
void node::child(std::shared_ptr<node> &_node)
{
    this->_children.push_back(_node);
}
void node::remove(std::shared_ptr<node> _node)
{
    if(_node->children().empty())
        {
            auto target=std::find(this->_children.begin(),this->_children.end(),_node);
            if(target==this->_children.end()) return;

            this->_children.erase(target);

            if(!this->references())
                {
                    if(this->children().empty())
                        this->parent()->remove(shared_from_this());
                    else if(this->children().size()==1U)
                        {
                            if(this->parent())
                                {
                                    this->children().back()->number_of_mutations(this->_number_of_mutations+this->children().back()->number_of_mutations());
                                    this->parent()->child(this->children().back());
                                    this->children().back()->parent(this->parent());
                                    this->children().pop_back();
                                    this->remove();
                                }
                            else
                                this->children().back()->_number_of_mutations=0U;
                        }
                }
        }
    else
        {
            for(auto& child : _node->children())
                {
                    child->parent(_node->parent());
                    this->children().push_back(child);
                }
            _node->children().clear();
            auto target=std::find(this->_children.begin(),this->_children.end(),_node);
            if(target==this->_children.end()) return;

            this->_children.erase(target);
        }
}
void node::remove(void)
{
    this->_parent->remove(shared_from_this());
}
void node::parent(std::shared_ptr<node> &_parent)
{
    this->_parent=_parent;
}
std::shared_ptr<node>& node::parent(void)
{
    return(this->_parent);
}
void node::increase(void)
{
    this->_references++;
}
uint32_t node::references(void) const
{
    return(this->_references);
}
void node::references(const uint32_t &_references)
{
    this->_references=_references;
}
void node::mutate(void)
{
    this->_number_of_mutations++;
}
std::vector<std::shared_ptr<node>>& node::children(void)
{
    return(this->_children);
}

json node::save(void)
{
    json document;

    document["id"]=this->id();
    document["references"]=this->references();
    document["mutations"]=this->number_of_mutations();

    if(!this->children().empty())
        {
            for(auto child : this->children())
                document["children"].push_back(child->save());
        }
    return(document);
}
uint32_t node::number_of_mutations(void) const
{
    return(this->_number_of_mutations);
}
void node::number_of_mutations(const uint32_t &_number_of_mutations)
{
    this->_number_of_mutations=_number_of_mutations;
}
void node::serialize(const std::string &_filename)
{
    std::ofstream output;
    output.open(_filename,std::ios::binary | std::ios::out);
    this->serialize(output);
    output.close();
}
void node::serialize(std::ofstream &_output)
{
    size_t number_of_children=this->_children.size();
    _output.write((char*)&this->_id,sizeof(uint32_t));
    _output.write((char*)&number_of_children,sizeof(size_t));
    _output.write((char*)&this->_references,sizeof(uint32_t));
    _output.write((char*)&this->_number_of_mutations,sizeof(uint32_t));

    if(this->_number_of_mutations==0U)
        {
            size_t length=this->_point_mutations.size();
            _output.write((char*)&length,sizeof(uint32_t));
            uint32_t *buffer=(uint32_t*)malloc(length*sizeof(uint32_t));
            for(uint32_t i=0U; i<this->_point_mutations.size(); i++)
                buffer[i]=this->_point_mutations[i];
            _output.write((char*)buffer,length*sizeof(uint32_t));
            free(buffer);
        }

    for(auto& child : this->_children) child->serialize(_output);
}
void node::unserialize(const std::string &_filename)
{
    std::ifstream input;
    input.open(_filename,std::ios::binary | std::ios::in);
    this->unserialize(input);
    input.close();
}
void node::unserialize(std::ifstream &_input)
{
    size_t number_of_children=0U;
    _input.read((char*)&this->_id,sizeof(uint32_t));
    _input.read((char*)&number_of_children,sizeof(size_t));
    _input.read((char*)&this->_references,sizeof(uint32_t));
    _input.read((char*)&this->_number_of_mutations,sizeof(size_t));

    if(this->_number_of_mutations==0U)
        {
            size_t length=this->_point_mutations.size();
            _input.read((char*)&length,sizeof(size_t));
            uint32_t *buffer=(uint32_t*)malloc(length*sizeof(uint32_t));
            _input.read((char*)buffer,length*sizeof(uint32_t));
            this->_point_mutations.assign(buffer,buffer+length);
            free(buffer);
        }

    for(uint32_t i=0U; i<number_of_children; i++)
        {
            this->_children.push_back(std::make_shared<node>());
            this->_children.back()->unserialize(_input);
        }
}
void node::id(const uint64_t &_id)
{
    this->_id=_id;
}

uint64_t node::id(void) const
{
    return(this->_id);
}
void node::srt(void)
{
    static thread_local std::mt19937 rng(time(0));
    std::binomial_distribution<uint32_t> binomial(this->_number_of_mutations,0.5);

    this->_repeats=binomial(rng);

    for(auto& child : this->children())
        child->srt();
}
void node::snp(const uint32_t &_length)
{
    static thread_local std::mt19937 rng(time(0));
    std::uniform_int_distribution<uint32_t> uniform(0U,_length-1U);

    for(uint32_t i=0; i<this->_number_of_mutations; ++i)
        this->_point_mutations.push_back(uniform(rng));

    for(auto& child : this->children())
        child->snp(_length);

    std::sort(this->_point_mutations.begin(),this->_point_mutations.end());
}
void node::path(std::vector<uint32_t> &_path)
{
    _path.insert(_path.begin(),this->id());
    if(this->parent()!=nullptr) this->parent()->path(_path);
}
std::vector<uint32_t> node::point_mutations(void)
{
    if(this->parent()!=nullptr)
        {
	         std::vector<uint32_t> ret;
	         std::merge(this->_point_mutations.begin(),this->_point_mutations.end(),this->parent()->point_mutations().begin(),this->parent()->point_mutations().end(),std::back_inserter(ret));
				auto it=std::unique(ret.begin(),ret.end());
            ret.resize(std::distance(ret.begin(),it));
            return(ret);
        }
    else return(this->_point_mutations);
}
uint32_t node::repeats(void)
{
    if(this->parent()!=nullptr)
        return(this->_repeats+this->parent()->repeats());
    else return(this->_repeats);
}
